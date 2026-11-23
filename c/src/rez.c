#include <labtool48/labtool48.h>

#include <stdio.h>
#include <stdint.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>

void lt48_rez_digest_normalize(unsigned char digest[LT48_SHA256_SIZE]);

typedef struct rez_resource_owned {
    char *name;
    char *type_name;
    uint32_t offset;
    uint32_t size;
    uint32_t body_offset;
    uint32_t body_size;
} rez_resource_owned;

struct lt48_rez {
    atomic_uint references;
    unsigned char *data;
    size_t size;
    uint32_t declared_size;
    uint32_t table_offset;
    size_t resource_count;
    rez_resource_owned *resources;
};

/* An explicitly scoped, thread-local immutable view for offline catalogue
 * evaluation. Cache ownership is independent of returned handles; a catalogue
 * handle may outlive the scope or be transferred to the GUI thread. */
typedef struct rez_snapshot_entry {
    char *path;
    lt48_rez *rez;
    unsigned char digest[LT48_SHA256_SIZE];
    struct rez_snapshot_entry *next;
} rez_snapshot_entry;
static _Thread_local rez_snapshot_entry *snapshot_entries;
static _Thread_local unsigned snapshot_depth;

void lt48_rez_snapshot_begin(void) { ++snapshot_depth; }

void lt48_rez_snapshot_end(void) {
    if (!snapshot_depth || --snapshot_depth) return;
    while (snapshot_entries) {
        rez_snapshot_entry *entry = snapshot_entries;
        snapshot_entries = entry->next;
        lt48_rez_close(entry->rez);
        free(entry->path);
        free(entry);
    }
}

static uint32_t read_u32(const unsigned char *data) {
    return (uint32_t)data[0] | (uint32_t)data[1] << 8 |
           (uint32_t)data[2] << 16 | (uint32_t)data[3] << 24;
}

static uint16_t read_u16(const unsigned char *data) {
    return (uint16_t)((uint16_t)data[0] | (uint16_t)data[1] << 8);
}

static int range_valid(size_t size, size_t offset, size_t length) {
    return offset <= size && length <= size - offset;
}

static lt48_status require_atom(
    const lt48_rez *rez,
    size_t index,
    const char *type_name,
    const rez_resource_owned **out_resource,
    const unsigned char **out_body
) {
    if (rez == NULL || out_resource == NULL || out_body == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    if (index >= rez->resource_count) {
        return LT48_NOT_FOUND;
    }
    *out_resource = &rez->resources[index];
    if (strcmp((*out_resource)->type_name, type_name) != 0) {
        return LT48_NOT_FOUND;
    }
    *out_body = rez->data + (*out_resource)->body_offset;
    return LT48_OK;
}

static char *copy_pascal(
    const unsigned char *data,
    size_t size,
    size_t *position
) {
    size_t length;
    char *result;
    if (*position >= size) {
        return NULL;
    }
    length = data[(*position)++];
    if (!range_valid(size, *position, length)) {
        return NULL;
    }
    result = malloc(length + 1);
    if (result == NULL) {
        return NULL;
    }
    memcpy(result, data + *position, length);
    result[length] = '\0';
    *position += length;
    return result;
}

void lt48_rez_close(lt48_rez *rez) {
    if (rez == NULL) {
        return;
    }
    if (atomic_fetch_sub(&rez->references, 1u) != 1u) return;
    for (size_t index = 0; index < rez->resource_count; ++index) {
        free(rez->resources[index].name);
        free(rez->resources[index].type_name);
    }
    free(rez->resources);
    free(rez->data);
    free(rez);
}

lt48_status lt48_rez_open(const char *path, lt48_rez **out_rez) {
    FILE *file = NULL;
    lt48_rez *rez = NULL;
    long file_size;
    size_t position;
    char *collection_type = NULL;
    uint32_t count;
    size_t bytes_read;
    int close_result;

    if (path == NULL || out_rez == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    *out_rez = NULL;
    if (snapshot_depth) {
        for (rez_snapshot_entry *entry = snapshot_entries; entry; entry = entry->next) {
            if (!strcmp(entry->path, path)) {
                atomic_fetch_add(&entry->rez->references, 1u);
                *out_rez = entry->rez;
                return LT48_OK;
            }
        }
    }
    file = fopen(path, "rb");
    if (file == NULL || fseek(file, 0, SEEK_END) != 0 ||
        (file_size = ftell(file)) < 0 || fseek(file, 0, SEEK_SET) != 0) {
        if (file != NULL) {
            fclose(file);
        }
        return LT48_FILE_ERROR;
    }
    rez = calloc(1, sizeof(*rez));
    if (rez == NULL) {
        fclose(file);
        return LT48_NO_MEMORY;
    }
    atomic_init(&rez->references, 1u);
    rez->size = (size_t)file_size;
    rez->data = malloc(rez->size == 0 ? 1 : rez->size);
    if (rez->data == NULL) {
        fclose(file);
        lt48_rez_close(rez);
        return LT48_NO_MEMORY;
    }
    bytes_read = fread(rez->data, 1, rez->size, file);
    close_result = fclose(file);
    if (bytes_read != rez->size || close_result != 0) {
        lt48_rez_close(rez);
        return LT48_FILE_ERROR;
    }
    if (rez->size < 12 || memcmp(rez->data, "FBPR", 4) != 0) {
        lt48_rez_close(rez);
        return LT48_FORMAT_ERROR;
    }
    rez->declared_size = read_u32(rez->data + 4);
    rez->table_offset = read_u32(rez->data + 8);
    position = rez->table_offset;
    if (!range_valid(rez->size, position, 2) || rez->data[position] != 0x02 ||
        rez->data[position + 1] != 0x5b) {
        lt48_rez_close(rez);
        return LT48_FORMAT_ERROR;
    }
    position += 2;
    collection_type = copy_pascal(rez->data, rez->size, &position);
    if (collection_type == NULL || strcmp(collection_type, "TResourceCollection") != 0 ||
        !range_valid(rez->size, position, 12)) {
        free(collection_type);
        lt48_rez_close(rez);
        return LT48_FORMAT_ERROR;
    }
    free(collection_type);
    count = read_u32(rez->data + position);
    position += 12;
    if (count > rez->size / 9) {
        lt48_rez_close(rez);
        return LT48_FORMAT_ERROR;
    }
    rez->resources = calloc(count == 0 ? 1 : count, sizeof(*rez->resources));
    if (rez->resources == NULL) {
        lt48_rez_close(rez);
        return LT48_NO_MEMORY;
    }
    rez->resource_count = count;
    for (size_t index = 0; index < count; ++index) {
        rez_resource_owned *resource = &rez->resources[index];
        size_t atom_position;
        if (!range_valid(rez->size, position, 8)) {
            lt48_rez_close(rez);
            return LT48_FORMAT_ERROR;
        }
        resource->offset = read_u32(rez->data + position);
        resource->size = read_u32(rez->data + position + 4);
        position += 8;
        resource->name = copy_pascal(rez->data, rez->size, &position);
        if (resource->name == NULL ||
            !range_valid(rez->size, resource->offset, resource->size) ||
            resource->size < 3 || rez->data[resource->offset] != 0x02 ||
            rez->data[resource->offset + 1] != 0x5b) {
            lt48_rez_close(rez);
            return LT48_FORMAT_ERROR;
        }
        atom_position = (size_t)resource->offset + 2;
        resource->type_name = copy_pascal(
            rez->data, (size_t)resource->offset + resource->size, &atom_position
        );
        if (resource->type_name == NULL) {
            lt48_rez_close(rez);
            return LT48_FORMAT_ERROR;
        }
        if (atom_position > (size_t)resource->offset + resource->size) {
            lt48_rez_close(rez);
            return LT48_FORMAT_ERROR;
        }
        if (atom_position > UINT32_MAX ||
            (size_t)resource->offset + resource->size - atom_position >
                UINT32_MAX) {
            lt48_rez_close(rez);
            return LT48_FORMAT_ERROR;
        }
        resource->body_offset = (uint32_t)atom_position;
        resource->body_size = (uint32_t)((size_t)resource->offset +
                                         resource->size - atom_position);
        if (resource->body_size != 0 &&
            rez->data[resource->body_offset + resource->body_size - 1] == 0x5d) {
            --resource->body_size;
        }
    }
    if (snapshot_depth) {
        rez_snapshot_entry *entry = calloc(1, sizeof(*entry));
        if (entry) {
            entry->path = malloc(strlen(path) + 1u);
            if (!entry->path) free(entry);
            else {
                strcpy(entry->path, path);
                entry->rez = rez;
                lt48_sha256(rez->data, rez->size, entry->digest);
                lt48_rez_digest_normalize(entry->digest);
                atomic_fetch_add(&rez->references, 1u);
                entry->next = snapshot_entries;
                snapshot_entries = entry;
            }
        }
    }
    *out_rez = rez;
    return LT48_OK;
}

/* AEC 14.40 rebuilt most resource headers and some payloads while retaining
 * earlier engine resources. Map recognized 14.40 digests to their audited
 * 13.10 equivalents so existing exact-hash engine checks remain meaningful. */
static const struct { const char *variant; const char *canonical; } aec1440_compat[] = {
    {"3f9e5fe3cebc6709328a8a26844d355c4262febda1bf01a7b1b684d52c72f2ee", "3163ffcfd04a7cdc2c1853110175fae42fe9e399a8cce9fbc85e23d007b2e7d2"}, /* GAAA___.rez */
    {"ca549c5dd5b536eff2123cd93ba01e2507cf7e1238c8af188bea207e9b377d2e", "cb8e52932d7e04b06eb43530a95ea3533311cc49904a2e4924fd47837318b695"}, /* GAABA__.rez */
    {"0446228f1d40cdc05a383034b94bb21d326705dccb5eacb92b185f923000a7b7", "175b86105f89b58c3e9e40d169e742f3bc193be9af9a3a3c5b7b998138ab182b"}, /* GAABB__.rez */
    {"e17b35d8cdcfb4e64822967deee15a0f2faf929c64afde6a9ff379ab1a1c362d", "884d761b7deca2ccbaa49e0b26c3b515e1381033c6b9af6fd496136135c47bdf"}, /* GAAB___.rez */
    {"30fdc4100b12367a79dc4c988ba42e443faf95742f68625d10bc933b22f39849", "a5b774bb0fada8081d9a158f710586df4dafb71c298d4c049c625627dde173c4"}, /* GAAC___.rez */
    {"e28bd9ebae1bf883bc41c413cee5c7a27e7b170a6ba46e504c26b22fc6fe2bed", "e282aa2c1a8fbcc13070fee62dae551fe9db3809639a90529b5d423f99c90180"}, /* GAAD___.rez */
    {"dd9f7d517d84ee390ff46107fa3a65faefdbd762987f8105174ae8ca573f0628", "4d97d960939a13b84a36d664def2823698b680ecbd97514be0b7b55ecc244e05"}, /* GAAE1__.rez */
    {"6e8f48b3482cccac6817e7caabb2755d3f8843d44e19c4def994b9b5522b8ae6", "3a9cc5ad13d26c607a5d2bd5774c5cc54315b642aea14864a12da92f86765117"}, /* GAAE___.rez */
    {"347d3a09910ff169f68425cc8a18a9f85a0e9a3785585e6d37ce12ad54afe974", "9fdc0c7a88bb96aaead0314fb4f85c0800ab987abf370908000493cc272d13f6"}, /* GAAF___.rez */
    {"b567bcd4dd7172384099a11496ad38f9ac6a11c1d990acd037326f15d553b694", "2e6261c867d6857786933c0d093e5bd669f70355cc218ff006c88c4fb919636b"}, /* GAA____.rez */
    {"d8ee0b1379b9dabc8e7816590e4a786931d5ce36ac3cffd1f6899bfd1f725854", "72881a986aa39f0a7258f153f6ea30eb74368350229a89cdcde7867f4219c2f9"}, /* GAB____.rez */
    {"a8e2391da37525e0b4f4d4b8900a0ce4167be93ee79c67e8fbb4484faa8d62c6", "bbc71e51baba29f24c9485678eb31b5bd5634789cd1acc503334c70f950a842f"}, /* GAC____.rez */
    {"91f4fb885f78d6638612ea924aefb8a4e1091faba044887ee8af4511deb2cc17", "aca05831121f0e30654601df133b298cec1d33e7ffb94626c5d62634e690c452"}, /* GADA___.rez */
    {"1a6bd49cd281d2a8390d952f043fafe3bcc867a76f9f0186a0f59e6c1e05ccf5", "a9a45cce0fb16ce810a5f884b82a8e46afc7b0ccbd80ab6ab25b5a77613c3a3b"}, /* GADB___.rez */
    {"ce421a50ba02b55313e90a3b29c8ab10c2eb679e82c5bbdd1e19fba78a80f27d", "24b8320da4cfd700ee436ff06bd55d9dfbd472c53a021ad527c9bbcb3c804dec"}, /* GADC___.rez */
    {"6206d10fb6db041303eb8a2bde77cffb2180643c9138708ee01071819c8e2969", "aa366e3c14e1e088730223f8be3e1343a16c551039f95fcdf7d101bc45c46706"}, /* GAD____.rez */
    {"f72c78b5b462bf95f891116ffc6fcfdb23e2a54c8a4d2c317de219eb380474af", "7c4cdca4c1dd5bfd3c405001054e555d12e1925815f52ae8fb8db1cd034d9c90"}, /* GAE____.rez */
    {"a9390992046cc2efab4777ade1433dd128cc9ce2b02abb7131bc69c8f7e2b2d6", "126dcd7dd7252b5af78e151d34106f0c17b5e52575b9606a80b9f615f912c123"}, /* GAF____.rez */
    {"87483163c76421e5dcac252df5232d5bc835a385604e010ca5d5cdbb5baedd10", "a286d6799117d4e188067376cab75843fbb9299f4990091d91ad27d8abab99ad"}, /* GA_____.rez */
    {"4763ec977a7a5b2f658d6ce75c88156e3415e99308895607d870c8f9e93ea830", "2b82c3e23dde22e68484fa6de0bf93f4ac957486158ab2d0d91a160a3416a380"}, /* GBA____.rez */
    {"93232803e59db0ce81a6449a599bdf2b59447cc351b31e967dcdbff0dbc7feb8", "c9c29035544d72ea92a281b7a8def753b035889635f82fdb1f521fc0330f8c76"}, /* GBBA___.rez */
    {"024d6dd3bdbdc3280acd9480f227bf2d8ec5db2578820c35365a9880efc636d6", "85ca2609c5d2691cd435b8edd8550523d7bac5c0e8107b599f189692baeeafa7"}, /* GBB____.rez */
    {"cde95ccc66128627d8c428fef9c568ee1a0033b08f4474e48c57060188574ea8", "65753e664d77d1cfab853ae51f6b58de07fc98ab73f88e618a31d0261fd1d846"}, /* GBC____.rez */
    {"07a05dbd80acabf5a6fdede9178873a04f55179ff2436cf6ae01daa9859de4f0", "88cd45163f6ffac436959a4659a6f782aea294d0add67c87bf1f4336fb5d07f1"}, /* GBD____.rez */
    {"26d3a3169fad22e54b5e7fbcaa2885f4dc34d68e6372dc2de3ceafcc651f54e4", "a7d97ec76f79911804983d3faee0bc0fc8ecfb1bcc2fb137305de06022b5556a"}, /* GBE____.rez */
    {"17de5e40a537ddccd3fb17530091e5f369398854b717d332f6dce951cd19394a", "913000514454bd9ee191bc4167efa52aff3969ff46e84081d9ddfbe2635c6f7f"}, /* GB_____.rez */
    {"a801588dbab3329ddfd62e3dd77bd4fcb34a88e26a25f311199996607a654988", "fe960eb458355e267848f82bd9d86dd05c54d9979c40a80036c3d3a57a7c4b32"}, /* GCA____.rez */
    {"619a4e42c883e28b3e54e8a57cb6a84ea38890c2d28d3234d5da353b7d2bf4d5", "9bd0f6c9cd3e5e6888a23f8750c6acc6da7fafb64501f477e064db3c8bb74b12"}, /* GCB____.rez */
    {"daa20c8959ca87075547d063d8d222aae51b2c9a30900459b94bd9b042fa9ed5", "fc6529b7b6c6443dc9a962052b5f18e7f2928ae1bdfc3248d373a9a9fec32c20"}, /* GC_____.rez */
    {"50b21f3689f9b58d2be91a8876046a4532767526437304ad6293a31e11948f33", "98e1cfe07d11ae94d93a71b49e90197370df36694b5ac880523b6012efcbcbde"}, /* GDAAA__.rez */
    {"d4342c691ecd4307c5be7cd6bab865e81cb2632a682489ad61937cfa355bec7f", "6f780f677ae8e3f57c49d951f45509baead7bbfd4b678488e73383fc7d841471"}, /* GDAA___.rez */
    {"e754944d0f66b09a1d9713ad2c16f88d719786ce08ce637ab9b5979b683a639b", "77e939e536a8af7d76fa8d661927e7f1e712eb7b1441fe6e7c86ddbeefdb51ec"}, /* GDABA__.rez */
    {"6525d96a1a69cab6cd6c3899a7365d9173eb9de60667a4ffd1b2d7545932d053", "55d48e0a8888665c93b12551e9ddb3307abe819d088f8e164ac1c623b7ad0d44"}, /* GDAB___.rez */
    {"d502c6196d04ae846c4cc4b3e7327c1af4180d064ee3238fbbc00a58a2e90c1c", "4abfb9024179d1cf38f17bc43ebfdaabaca4694b95e76e73edfdbbe7a0f46710"}, /* GDAC___.rez */
    {"2627d159e0be4198e163d9f25c5d406cf2943d3bb20cf4dffac139f680305f91", "9a9afa4870cdcd5cc9d431ad060311b1f6f0c8d653ec3e157c8aa888b8d66e36"}, /* GDAE___.rez */
    {"9f6eb4bf296fcf9bb02e5baeed2ecd3718651fb31644e235cd36404e833348b9", "b889a62b78b8d63028e47b3c4c008b86f245fa96322c4f73336a5f882f19fe28"}, /* GDBAA__.rez */
    {"6b88516336b159f041b4672919ba41d4d6670e3af9f2dbc181b8c417031aa971", "4129e4261b9eeb7a4b7f4272d99b3e8d4ec45e41f9150a86baa57f909e8fc7ff"}, /* GDBA___.rez */
    {"a1eaf80cdd90e46b7bb197320577e6a9d4a6b78f3b114764d838388d68973b70", "d2c4b6b43cff23dcfca67c92e4341a9e630322bb41850236ce83937228f5fab0"}, /* GDBB___.rez */
    {"a913a0b10ef4001ccc110a7d0eecc3a954eba3980707f0192807c2615d590794", "2439a3725457dfc077c67ea7420701829f4d91d62155ffd070921c767d322ac8"}, /* GDCAA__.rez */
    {"702936863173924e260be39d597cd7ebbc35a8d5c811fbb7561234191a5f9835", "5b391bb2789baa1496eb5db708c3d867b8e25ec2b01dc7ea030eb9c9a84e5a39"}, /* GDCA___.rez */
    {"f63f318c0a4f84528b3a7ac7901db7fa1c1a17d21407a0ed0604e21b39e300f7", "84d3f2c830be58a084b5c763687d8a7c3a6217d440d4a96fb63fb2cf842a38fa"}, /* GDCBA__.rez */
    {"3783c20e7d491803a4a369759af2b59d46ffe6e8498c6e30979e5acf674daf3a", "c0e90a55e97a3ac12d73006eb9da1139e3d5688aef85242b0001bc406cab4640"}, /* GDCC___.rez */
    {"199263b340c19eee9513594162c7c6b00949fdf4a420f0ea0ffa5630de5162af", "a1f539dee05f744739a739c69e61a552c50bfbd94e9e42dc774c8b89db00234f"}, /* dev48uxp.rez */
    {"199263b340c19eee9513594162c7c6b00949fdf4a420f0ea0ffa5630de5162af", "a1f539dee05f744739a739c69e61a552c50bfbd94e9e42dc774c8b89db00234f"}, /* dev48uxp.rez */
};

static unsigned char hex_nibble_compat(char c) {
    return c >= '0' && c <= '9' ? (unsigned char)(c - '0') :
           (unsigned char)(c - 'a' + 10);
}

void lt48_rez_digest_normalize(unsigned char digest[LT48_SHA256_SIZE]) {
    char hex[LT48_SHA256_SIZE * 2u + 1u];
    for (size_t i = 0; i < LT48_SHA256_SIZE; ++i)
        (void)snprintf(hex + 2u*i, 3u, "%02x", digest[i]);
    for (size_t i = 0; i < sizeof(aec1440_compat)/sizeof(*aec1440_compat); ++i) {
        if (strcmp(hex, aec1440_compat[i].variant)) continue;
        for (size_t j = 0; j < LT48_SHA256_SIZE; ++j)
            digest[j] = (unsigned char)((hex_nibble_compat(aec1440_compat[i].canonical[2u*j]) << 4) |
                                        hex_nibble_compat(aec1440_compat[i].canonical[2u*j+1u]));
        return;
    }
}

lt48_status lt48_rez_sha256(
    const lt48_rez *rez, unsigned char digest[LT48_SHA256_SIZE]
) {
    if (rez == NULL || digest == NULL) return LT48_INVALID_ARGUMENT;
    for (rez_snapshot_entry *entry = snapshot_entries; entry; entry = entry->next) {
        if (entry->rez == rez) {
            memcpy(digest, entry->digest, LT48_SHA256_SIZE);
            return LT48_OK;
        }
    }
    lt48_sha256(rez->data, rez->size, digest);
    lt48_rez_digest_normalize(digest);
    return LT48_OK;
}

static lt48_status device_kind(
    const char *type_name,
    lt48_device_definition_kind *out_kind,
    int *out_is_xp
) {
    static const struct {
        const char *name;
        lt48_device_definition_kind kind;
    } types[] = {
        {"TMemDev", LT48_DEVICE_DEFINITION_MEMORY},
        {"TLogDev", LT48_DEVICE_DEFINITION_LOGIC},
        {"TPofDev", LT48_DEVICE_DEFINITION_POF},
        {"TPofFuseDev", LT48_DEVICE_DEFINITION_POF_FUSE},
        {"TXilinxMemDev", LT48_DEVICE_DEFINITION_XILINX_MEMORY},
        {"TDblBufMemDev", LT48_DEVICE_DEFINITION_DOUBLE_BUFFER_MEMORY},
        {"TAddTabMemDev", LT48_DEVICE_DEFINITION_ADDRESS_TABLE_MEMORY},
        {"TXPMemDev", LT48_DEVICE_DEFINITION_XP_MEMORY}
    };
    for (size_t index = 0; index < sizeof(types) / sizeof(types[0]); ++index) {
        if (strcmp(type_name, types[index].name) == 0) {
            *out_kind = types[index].kind;
            *out_is_xp = strcmp(type_name, "TXPMemDev") == 0;
            return LT48_OK;
        }
    }
    return LT48_NOT_FOUND;
}

lt48_status lt48_rez_get_device_definition(
    const lt48_rez *rez,
    size_t index,
    lt48_device_definition *out_definition
) {
    const rez_resource_owned *resource;
    const unsigned char *body;
    int is_xp;
    lt48_status status;
    if (rez == NULL || out_definition == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    if (index >= rez->resource_count) {
        return LT48_NOT_FOUND;
    }
    resource = &rez->resources[index];
    memset(out_definition, 0, sizeof(*out_definition));
    status = device_kind(resource->type_name, &out_definition->kind, &is_xp);
    if (status != LT48_OK) {
        return status;
    }
    if (resource->body_size < (is_xp ? 13U : 7U)) {
        return LT48_FORMAT_ERROR;
    }
    body = rez->data + resource->body_offset;
    out_definition->resource_name = resource->name;
    out_definition->type_name = resource->type_name;
    out_definition->family_code = read_u16(body);
    out_definition->pinmap = read_u16(body + 2);
    out_definition->value2 = read_u16(body + 4);
    if (is_xp) {
        out_definition->value3 = body[6];
        out_definition->device_size = read_u32(body + 7);
        out_definition->element_count = out_definition->device_size;
        out_definition->element_bits = read_u16(body + 11);
        out_definition->has_element_geometry = 1;
        out_definition->trailing_data = body + 11;
        out_definition->trailing_data_size = resource->body_size - 11;
        out_definition->has_device_size = 1;
    } else {
        /* The Windows deserializer at 00404490 reads this field as a byte.
         * Consuming a uint16 here used to steal the first derived byte. */
        out_definition->value3 = body[6];
        out_definition->trailing_data = body + 7;
        out_definition->trailing_data_size = resource->body_size - 7;
        if ((out_definition->kind == LT48_DEVICE_DEFINITION_MEMORY ||
             out_definition->kind == LT48_DEVICE_DEFINITION_XILINX_MEMORY ||
             out_definition->kind == LT48_DEVICE_DEFINITION_ADDRESS_TABLE_MEMORY ||
             out_definition->kind ==
                 LT48_DEVICE_DEFINITION_DOUBLE_BUFFER_MEMORY) &&
            out_definition->trailing_data_size >= 10U) {
            out_definition->element_count = read_u32(body + 7);
            out_definition->element_bits = read_u16(body + 11);
            out_definition->auxiliary_value = read_u32(body + 13);
            out_definition->has_element_geometry = 1;
            if (out_definition->kind ==
                    LT48_DEVICE_DEFINITION_DOUBLE_BUFFER_MEMORY) {
                if (out_definition->trailing_data_size != 16U)
                    return LT48_FORMAT_ERROR;
                out_definition->secondary_element_count =
                    read_u32(body + 17);
                out_definition->secondary_element_bits =
                    read_u16(body + 21);
                out_definition->has_secondary_element_geometry = 1;
            }
        }
    }
    return LT48_OK;
}

lt48_status lt48_rez_get_logic_fuse_codes(
    const lt48_rez *rez, size_t definition_index,
    lt48_logic_fuse_codes *out_codes
) {
    const rez_resource_owned *resource;
    const unsigned char *body;
    size_t count;
    size_t required;
    size_t cursor;
    if (rez == NULL || out_codes == NULL)
        return LT48_INVALID_ARGUMENT;
    memset(out_codes, 0, sizeof(*out_codes));
    if (definition_index >= rez->resource_count) return LT48_NOT_FOUND;
    resource = &rez->resources[definition_index];
    if (strcmp(resource->type_name, "TLogDev") != 0)
        return LT48_NOT_FOUND;
    /* TDev base is three words plus one byte. TLogDev then serializes a word
     * count followed by that many TFuseCode word identifiers. */
    if (resource->body_size < 9u) return LT48_FORMAT_ERROR;
    body = rez->data + resource->body_offset;
    count = read_u16(body + 7u);
    if (count > LT48_LOGIC_MAX_FUSE_CODES ||
        count > (SIZE_MAX - 9u) / 2u)
        return LT48_FORMAT_ERROR;
    required = 9u + count * 2u;
    if (required > resource->body_size) return LT48_FORMAT_ERROR;
    out_codes->count = count;
    for (size_t index = 0u; index < count; ++index)
        out_codes->identifiers[index] = read_u16(body + 9u + index * 2u);
    cursor = required;
    /* TLogDev continues with two optional auxiliary fuse contexts. Each
     * starts with a 32-bit editable-value count and carries a TFuseCode
     * identifier only when that count is nonzero. A referenced layout may
     * contain an additional family control fuse. The final 32-bit value is
     * the primary fuse count. This conditional layout is reproduced at
     * 00404f80 and accounts for the 12/14/16-byte tails in the catalogue. */
    if (resource->body_size - cursor < 4u) return LT48_FORMAT_ERROR;
    out_codes->e_field_value_count = read_u32(body + cursor);
    cursor += 4u;
    if (out_codes->e_field_value_count != 0u) {
        if (resource->body_size - cursor < 2u) return LT48_FORMAT_ERROR;
        out_codes->e_field_identifier = read_u16(body + cursor);
        out_codes->has_e_field = 1;
        cursor += 2u;
    }
    if (resource->body_size - cursor < 4u) return LT48_FORMAT_ERROR;
    out_codes->user_code_value_count = read_u32(body + cursor);
    cursor += 4u;
    if (out_codes->user_code_value_count != 0u) {
        if (resource->body_size - cursor < 2u) return LT48_FORMAT_ERROR;
        out_codes->user_code_identifier = read_u16(body + cursor);
        out_codes->has_user_code = 1;
        cursor += 2u;
    }
    if (resource->body_size - cursor != 4u) return LT48_FORMAT_ERROR;
    out_codes->primary_fuse_count = read_u32(body + cursor);
    return LT48_OK;
}

lt48_status lt48_rez_get_fuse_code(
    const lt48_rez *rez, size_t resource_index,
    lt48_fuse_code *out_code
) {
    const rez_resource_owned *resource;
    const unsigned char *body;
    size_t bytecode_size;
    if (rez == NULL || out_code == NULL) return LT48_INVALID_ARGUMENT;
    memset(out_code, 0, sizeof(*out_code));
    if (resource_index >= rez->resource_count) return LT48_NOT_FOUND;
    resource = &rez->resources[resource_index];
    if (strcmp(resource->type_name, "TFuseCode") != 0)
        return LT48_NOT_FOUND;
    if (resource->body_size < 16u) return LT48_FORMAT_ERROR;
    body = rez->data + resource->body_offset;
    out_code->instruction_count = read_u16(body);
    bytecode_size = (size_t)out_code->instruction_count * 3u;
    if (bytecode_size != resource->body_size - 16u)
        return LT48_FORMAT_ERROR;
    out_code->dimensions[0] = read_u16(body + 2u);
    out_code->dimensions[1] = read_u16(body + 4u);
    out_code->dimensions[2] = read_u16(body + 6u);
    /* The executable reads object +48 before object +46. Preserve serialized
     * order here: body offsets 8 and 10. */
    out_code->dimensions[3] = read_u16(body + 8u);
    out_code->dimensions[4] = read_u16(body + 10u);
    out_code->total_fuse_count = read_u32(body + 12u);
    out_code->bytecode = body + 16u;
    out_code->bytecode_size = bytecode_size;
    return LT48_OK;
}

lt48_status lt48_rez_get_catalogue_entry(
    const lt48_rez *rez,
    size_t resource_index,
    size_t device_index,
    lt48_catalogue_entry *out_entry,
    size_t *out_device_count
) {
    const rez_resource_owned *resource;
    const unsigned char *body;
    const unsigned char *strings;
    size_t count;
    size_t strings_size;
    size_t numeric_size;
    size_t numeric_column_count;
    size_t name_index = 0;
    size_t position = 0;
    const unsigned char *name = NULL;
    size_t name_length = 0;
    if (rez == NULL || out_entry == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    if (resource_index >= rez->resource_count) {
        return LT48_NOT_FOUND;
    }
    resource = &rez->resources[resource_index];
    if (strcmp(resource->type_name, "TDevPool") != 0) {
        return LT48_NOT_FOUND;
    }
    if (resource->body_size < 4) {
        return LT48_FORMAT_ERROR;
    }
    body = rez->data + resource->body_offset;
    count = read_u16(body);
    strings_size = read_u16(body + 2);
    if (strings_size > resource->body_size - 4) {
        return LT48_FORMAT_ERROR;
    }
    numeric_size = resource->body_size - 4 - strings_size;
    if (count == 0) {
        if (numeric_size != 0) return LT48_FORMAT_ERROR;
        numeric_column_count = 0;
    } else {
        size_t column_size;
        if (count > SIZE_MAX / 2) return LT48_FORMAT_ERROR;
        column_size = count * 2;
        if (numeric_size % column_size != 0) return LT48_FORMAT_ERROR;
        numeric_column_count = numeric_size / column_size;
    }
    if ((count != 0 && (numeric_column_count < 10 ||
                        numeric_column_count > UINT16_MAX)) ||
        numeric_size > resource->body_size - 4) {
        return LT48_FORMAT_ERROR;
    }
    if (out_device_count != NULL) {
        *out_device_count = count;
    }
    if (device_index >= count) {
        return LT48_NOT_FOUND;
    }
    strings = body + 4 + numeric_size;
    while (position < strings_size && name_index <= device_index) {
        size_t start = position;
        while (position < strings_size && strings[position] != 0) {
            ++position;
        }
        if (position != start) {
            if (name_index == device_index) {
                name = strings + start;
                name_length = position - start;
                break;
            }
            ++name_index;
        }
        if (position < strings_size) {
            ++position;
        }
    }
    memset(out_entry, 0, sizeof(*out_entry));
    out_entry->vendor = resource->name;
    out_entry->device_name = name;
    out_entry->device_name_length = name_length;
    out_entry->numeric_column_count = (uint16_t)numeric_column_count;
    {
        uint16_t *fields[] = {
            &out_entry->table_index, &out_entry->device_class,
            &out_entry->device_type, &out_entry->unknown3,
            &out_entry->manufacturer_id, &out_entry->device_id,
            &out_entry->unknown6, &out_entry->unknown7,
            &out_entry->unknown8, &out_entry->unknown9
        };
        for (size_t column = 0; column < 10; ++column) {
            *fields[column] = read_u16(
                body + 4 + column * count * 2 + device_index * 2
            );
        }
    }
    return LT48_OK;
}

const char *lt48_definition_stem_for_class(uint16_t device_class) {
    static const char *const stems[] = {
        "GA_____", "GAA____", "GAAA___", "GAAB___", "GAABA__", "GAAC___",
        "GAAD___", "GAAE___", "GAAE1__", "GAAF___", "GAB____", "GAC____",
        "GAD____", "GADA___", "GADB___", "GADC___", "GADD___", "GAE____",
        "GAF____", "GB_____", "GBA____", "GBB____", "GBBA___", "GBC____",
        "GBD____", "GBE____", "GC_____", "GCA____", "GCB____", "GAABB__",
        "GDAA___", "GDAAA__", "GDAB___", "GDABA__", "GDAC___", "GDACA__",
        "GDAD___", "GDADA__", "GDAE___", "GDAEA__", "GDBA___", "GDBAA__",
        "GDBB___", "GDCA___", "GDCAA__", "GDCB___", "GDCBA__", "GDCC___"
    };
    size_t index;
    if (device_class < 2000) {
        return NULL;
    }
    index = (size_t)device_class - 2000;
    return index < sizeof(stems) / sizeof(stems[0]) ? stems[index] : NULL;
}

const char *lt48_definition_stem_for_class_format(
    uint16_t device_class, size_t numeric_column_count
) {
    static const char *const extended_stems[] = {
        "GA_____", "GAA____", "GAAA___", "GAAB___", "GAABA__", "GAAC___",
        "GAAD___", "GAAE___", "GAAE1__", "GAAF___", "GAB____", "GAC____",
        "GAD____", "GADA___", "GADB___", "GADC___", "GADD___", "GAE____",
        "GAF____", "GB_____", "GBA____", "GBB____", "GBBA___", "GBC____",
        "GBD____", "GBE____", "GC_____", "GCA____", "GCB____", "GAABB__",
        "GDAA___", "GDAAA__", "GDAB___", "GDABA__", "GDAC___", "GDACA__",
        "GDAD___", "GDADA__", "GDAE___", "GDAEA__", "GDAF___", "GDBA___",
        "GDBAA__", "GDBB___", "GDCA___", "GDCAA__", "GDCB___", "GDCBA__",
        "GDCC___"
    };
    size_t index;
    if (numeric_column_count < 15u)
        return lt48_definition_stem_for_class(device_class);
    if (device_class < 2000) return NULL;
    index = (size_t)device_class - 2000;
    return index < sizeof(extended_stems) / sizeof(extended_stems[0]) ?
           extended_stems[index] : NULL;
}

lt48_status lt48_rez_get_family_info(
    const lt48_rez *rez, size_t index, lt48_family_info *out_info
) {
    const rez_resource_owned *resource;
    const unsigned char *body;
    size_t used;
    lt48_status status;
    if (out_info == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    status = require_atom(rez, index, "TFamCode", &resource, &body);
    if (status != LT48_OK) {
        return status;
    }
    if (resource->body_size < 6) {
        return LT48_FORMAT_ERROR;
    }
    out_info->operation_count = read_u16(body);
    out_info->variable_count = read_u16(body + 2);
    /* 00403580 stores this third word at TFamCode object +0x0c. 004055d0
     * publishes it as the selected Configuration map key; 1..9999 select the
     * compact map and 10000+ select the rich map. Optional fixed-size records
     * are separately preceded by their own presence words. */
    out_info->configuration_set = read_u16(body + 4);
    out_info->header_value = out_info->configuration_set;
    used = 6 + (size_t)out_info->operation_count * 4 +
           (size_t)out_info->variable_count * 10;
    if (used > resource->body_size) {
        return LT48_FORMAT_ERROR;
    }
    out_info->trailing_data_size = resource->body_size - used;
    return LT48_OK;
}

lt48_status lt48_rez_get_family_operation_options(
    const lt48_rez *rez, size_t index,
    lt48_family_operation_options *out_options
) {
    const rez_resource_owned *resource;
    const unsigned char *body;
    const unsigned char *record;
    lt48_family_info info;
    lt48_status status;
    size_t offset;
    size_t number;
    if (out_options == NULL) return LT48_INVALID_ARGUMENT;
    memset(out_options, 0, sizeof(*out_options));
    status = lt48_rez_get_family_info(rez, index, &info);
    if (status != LT48_OK) return status;
    status = require_atom(rez, index, "TFamCode", &resource, &body);
    if (status != LT48_OK) return status;
    offset = 6u + (size_t)info.operation_count * 4u +
             (size_t)info.variable_count * 10u;
    if (!range_valid(resource->body_size, offset, 2u))
        return LT48_FORMAT_ERROR;
    if (read_u16(body + offset) == 0u) return LT48_NOT_FOUND;
    if (!range_valid(resource->body_size, offset + 2u, 0xd4u))
        return LT48_FORMAT_ERROR;
    record = body + offset + 2u;
    out_options->engine_flags = read_u32(record);
    out_options->checkbox_presence = read_u32(record + 4u);
    out_options->choice_presence = read_u32(record + 8u);
    out_options->number_presence = read_u32(record + 12u);
    out_options->checkbox_values = read_u32(record + 16u);
    for (number = 0u; number < 32u; ++number)
        out_options->number_values[number] =
            read_u32(record + 20u + number * 4u);
    for (number = 0u; number < 32u; ++number)
        out_options->choice_values[number] =
            read_u16(record + 148u + number * 2u);
    return LT48_OK;
}

lt48_status lt48_rez_get_family_operation(
    const lt48_rez *rez, size_t index, size_t operation_index,
    lt48_family_operation *out_operation
) {
    const rez_resource_owned *resource;
    const unsigned char *body;
    lt48_family_info info;
    lt48_status status;
    if (out_operation == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    status = lt48_rez_get_family_info(rez, index, &info);
    if (status != LT48_OK) {
        return status;
    }
    if (operation_index >= info.operation_count) {
        return LT48_NOT_FOUND;
    }
    status = require_atom(rez, index, "TFamCode", &resource, &body);
    if (status != LT48_OK) {
        return status;
    }
    (void)resource;
    body += 6 + operation_index * 4;
    out_operation->operation_id = read_u16(body);
    out_operation->wave = read_u16(body + 2);
    return LT48_OK;
}

lt48_status lt48_rez_get_family_variable(
    const lt48_rez *rez, size_t index, size_t variable_index,
    lt48_family_variable *out_variable
) {
    const rez_resource_owned *resource;
    const unsigned char *body;
    lt48_family_info info;
    lt48_status status;
    if (out_variable == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    status = lt48_rez_get_family_info(rez, index, &info);
    if (status != LT48_OK) {
        return status;
    }
    if (variable_index >= info.variable_count) {
        return LT48_NOT_FOUND;
    }
    status = require_atom(rez, index, "TFamCode", &resource, &body);
    if (status != LT48_OK) {
        return status;
    }
    (void)resource;
    body += 6 + (size_t)info.operation_count * 4 + variable_index * 10;
    for (size_t value = 0; value < 5; ++value) {
        out_variable->values[value] = read_u16(body + value * 2);
    }
    return LT48_OK;
}

static lt48_status defined_name_at(
    const unsigned char *body,
    size_t body_size,
    size_t wanted_index,
    size_t count,
    lt48_defined_name *out_name,
    size_t *out_end,
    lt48_defined_name *all_names
) {
    size_t position = 12;
    for (size_t current = 0; current < count; ++current) {
        size_t kind_length;
        size_t name_length;
        const unsigned char *name;
        uint16_t identifier;
        if (!range_valid(body_size, position, 3) ||
            body[position] != 0x02 || body[position + 1] != 0x5b) {
            return LT48_FORMAT_ERROR;
        }
        position += 2;
        kind_length = body[position++];
        if (kind_length != 3 || !range_valid(body_size, position, 3) ||
            memcmp(body + position, "TDN", 3) != 0) {
            return LT48_FORMAT_ERROR;
        }
        position += 3;
        if (!range_valid(body_size, position, 1)) {
            return LT48_FORMAT_ERROR;
        }
        name_length = body[position++];
        if (!range_valid(body_size, position, name_length + 3)) {
            return LT48_FORMAT_ERROR;
        }
        name = body + position;
        position += name_length;
        identifier = read_u16(body + position);
        position += 2;
        if (body[position++] != 0x5d) {
            return LT48_FORMAT_ERROR;
        }
        if (all_names != NULL) {
            all_names[current].name = name;
            all_names[current].name_length = name_length;
            all_names[current].identifier = identifier;
        }
        if (current == wanted_index && out_name != NULL) {
            out_name->name = name;
            out_name->name_length = name_length;
            out_name->identifier = identifier;
        }
    }
    if (out_end != NULL) {
        *out_end = position;
    }
    return wanted_index < count || wanted_index == SIZE_MAX ?
           LT48_OK : LT48_NOT_FOUND;
}

lt48_status lt48_rez_get_defined_name_collection_info(
    const lt48_rez *rez, size_t index,
    lt48_defined_name_collection_info *out_info
) {
    const rez_resource_owned *resource;
    const unsigned char *body;
    size_t end;
    lt48_status status;
    if (out_info == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    status = require_atom(rez, index, "TDNCol", &resource, &body);
    if (status != LT48_OK) {
        return status;
    }
    if (resource->body_size < 12) {
        return LT48_FORMAT_ERROR;
    }
    memset(out_info, 0, sizeof(*out_info));
    out_info->count = read_u32(body);
    out_info->value2 = read_u32(body + 4);
    out_info->value3 = read_u32(body + 8);
    status = defined_name_at(
        body, resource->body_size, SIZE_MAX, out_info->count, NULL, &end, NULL
    );
    if (status != LT48_OK || end > resource->body_size) {
        return LT48_FORMAT_ERROR;
    }
    out_info->trailing_data = body + end;
    out_info->trailing_data_size = resource->body_size - end;
    return LT48_OK;
}

lt48_status lt48_rez_get_defined_name(
    const lt48_rez *rez, size_t index, size_t name_index,
    lt48_defined_name *out_name
) {
    const rez_resource_owned *resource;
    const unsigned char *body;
    lt48_defined_name_collection_info info;
    lt48_status status;
    if (out_name == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    status = lt48_rez_get_defined_name_collection_info(rez, index, &info);
    if (status != LT48_OK) {
        return status;
    }
    if (name_index >= info.count) {
        return LT48_NOT_FOUND;
    }
    status = require_atom(rez, index, "TDNCol", &resource, &body);
    if (status != LT48_OK) {
        return status;
    }
    return defined_name_at(
        body, resource->body_size, name_index, info.count, out_name, NULL, NULL
    );
}

/* Decode a collection in one pass instead of rescanning it per name. */
lt48_status lt48_rez_get_defined_names(const lt48_rez *rez, size_t index,
    lt48_defined_name *names, size_t capacity, size_t *count) {
    const rez_resource_owned *resource;
    const unsigned char *body;
    lt48_status status;
    if (!count) return LT48_INVALID_ARGUMENT;
    *count = 0;
    status = require_atom(rez, index, "TDNCol", &resource, &body);
    if (status != LT48_OK) return status;
    if (resource->body_size < 12) return LT48_FORMAT_ERROR;
    *count = read_u32(body);
    /* Every serialized TDN occupies at least ten bytes. */
    if (*count > (resource->body_size - 12u) / 10u) return LT48_FORMAT_ERROR;
    if (capacity < *count) return LT48_BUFFER_TOO_SMALL;
    if (*count && !names) return LT48_INVALID_ARGUMENT;
    return defined_name_at(body, resource->body_size, SIZE_MAX,
        *count, NULL, NULL, names);
}

lt48_status lt48_rez_get_pin_code(
    const lt48_rez *rez, size_t index, lt48_pin_code *out_pin_code
) {
    const rez_resource_owned *resource;
    const unsigned char *body;
    lt48_status status;
    if (out_pin_code == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    status = require_atom(rez, index, "TPinCode", &resource, &body);
    if (status != LT48_OK) {
        return status;
    }
    if (resource->body_size < 1) {
        return LT48_FORMAT_ERROR;
    }
    out_pin_code->pin_count = body[0];
    out_pin_code->data = body + 1;
    out_pin_code->data_size = resource->body_size - 1;
    return LT48_OK;
}

lt48_status lt48_rez_get_wave_code(
    const lt48_rez *rez, size_t index, lt48_wave_code *out_wave_code
) {
    const rez_resource_owned *resource;
    const unsigned char *body;
    lt48_status status;
    if (out_wave_code == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    status = require_atom(rez, index, "TWaveCode", &resource, &body);
    if (status != LT48_OK) {
        return status;
    }
    if (resource->body_size < 4) {
        return LT48_FORMAT_ERROR;
    }
    memset(out_wave_code, 0, sizeof(*out_wave_code));
    out_wave_code->wave_type = read_u32(body);
    if (out_wave_code->wave_type == 6) {
        if (resource->body_size < 10) {
            return LT48_FORMAT_ERROR;
        }
        out_wave_code->length1 = read_u16(body + 4);
        out_wave_code->load_address = read_u16(body + 6);
        out_wave_code->length2 = read_u16(body + 8);
        out_wave_code->data = body + 10;
        out_wave_code->data_size = resource->body_size - 10;
        out_wave_code->has_applet_header = 1;
    } else {
        out_wave_code->data = body + 4;
        out_wave_code->data_size = resource->body_size - 4;
    }
    return LT48_OK;
}

lt48_status lt48_rez_get_variable_name(
    const lt48_rez *rez, size_t index, size_t name_index,
    const unsigned char **out_name, size_t *out_name_length,
    size_t *out_name_count
) {
    const rez_resource_owned *resource;
    const unsigned char *body;
    size_t count;
    size_t position = 2;
    lt48_status status;
    if (out_name == NULL || out_name_length == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    status = require_atom(rez, index, "TVarName", &resource, &body);
    if (status != LT48_OK) {
        return status;
    }
    if (resource->body_size < 2) {
        return LT48_FORMAT_ERROR;
    }
    count = read_u16(body);
    if (out_name_count != NULL) {
        *out_name_count = count;
    }
    for (size_t current = 0; current < count; ++current) {
        size_t length;
        if (position >= resource->body_size) {
            return LT48_FORMAT_ERROR;
        }
        length = body[position++];
        if (length > resource->body_size - position) {
            return LT48_FORMAT_ERROR;
        }
        if (current == name_index) {
            *out_name = body + position;
            *out_name_length = length;
            return LT48_OK;
        }
        position += length;
    }
    return LT48_NOT_FOUND;
}

lt48_status lt48_rez_get_info(const lt48_rez *rez, lt48_rez_info *out_info) {
    if (rez == NULL || out_info == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    out_info->declared_size = rez->declared_size;
    out_info->table_offset = rez->table_offset;
    out_info->actual_size = rez->size;
    out_info->resource_count = rez->resource_count;
    return LT48_OK;
}

lt48_status lt48_rez_get_resource(
    const lt48_rez *rez,
    size_t index,
    lt48_rez_resource *out_resource
) {
    const rez_resource_owned *resource;
    if (rez == NULL || out_resource == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    if (index >= rez->resource_count) {
        return LT48_NOT_FOUND;
    }
    resource = &rez->resources[index];
    out_resource->name = resource->name;
    out_resource->type_name = resource->type_name;
    out_resource->offset = resource->offset;
    out_resource->size = resource->size;
    return LT48_OK;
}

lt48_status lt48_rez_find_resource(
    const lt48_rez *rez,
    const char *name,
    size_t *out_index
) {
    if (rez == NULL || name == NULL || out_index == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    for (size_t index = 0; index < rez->resource_count; ++index) {
        if (strcmp(rez->resources[index].name, name) == 0) {
            *out_index = index;
            return LT48_OK;
        }
    }
    return LT48_NOT_FOUND;
}

lt48_status lt48_rez_get_kernel_buffer_count(
    const lt48_rez *rez, size_t index, uint16_t *out_count
) {
    const rez_resource_owned *resource;
    const unsigned char *body;
    lt48_status status;
    if (!out_count) return LT48_INVALID_ARGUMENT;
    status = require_atom(rez, index, "TKernel", &resource, &body);
    if (status != LT48_OK) return status;
    if (resource->body_size < 4u) return LT48_FORMAT_ERROR;
    *out_count = read_u16(body + 2u);
    return LT48_OK;
}

lt48_status lt48_rez_get_kernel_configuration_value(
    const lt48_rez *rez, size_t index, unsigned category, unsigned field,
    uint32_t *out_value
) {
    const rez_resource_owned *resource;
    const unsigned char *body;
    if (!out_value || category > 2u || field >= 32u) return LT48_INVALID_ARGUMENT;
    lt48_status status = require_atom(rez, index, "TKernel", &resource, &body);
    if (status != LT48_OK) return status;
    if (resource->body_size != 0xf0u) return LT48_FORMAT_ERROR;
    /* Original interpreter 00442177 selects Kernel+20h; getters
     * 00443d90 select +0Ch bits, +90h WORDs, or +10h DWORDs. */
    uint32_t value = category == 0u ? (read_u32(body + 0x2cu) >> field) & 1u :
        category == 1u ? read_u16(body + 0xb0u + 2u * field) :
        read_u32(body + 0x30u + 4u * field);
    *out_value = value;
    return LT48_OK;
}

lt48_status lt48_rez_get_family_frame_profile(
    const lt48_rez *rez, size_t family_index, lt48_runtime_frame_profile *out_profile
) {
    lt48_runtime_frame_profile profile = {0};
    lt48_defined_name_collection_info names;
    lt48_family_info family;
    size_t index, names_index;
    uint16_t count;
    int found = 0;
    lt48_status status;
    if (!rez || !out_profile) return LT48_INVALID_ARGUMENT;
#define FRAME_TRY(call) do { status = (call); if (status != LT48_OK) return status; } while (0)
    FRAME_TRY(lt48_rez_get_family_info(rez, family_index, &family));
    FRAME_TRY(lt48_rez_find_resource(rez, "Kernel", &index));
    FRAME_TRY(lt48_rez_get_kernel_buffer_count(rez, index, &count));
    if (!count || count > 76) return LT48_UNSUPPORTED;
    profile.buffer_count = (uint8_t)count;
    profile.reset_mask = count >= 32 ? UINT32_MAX : (UINT32_C(1) << count) - 1u;
    FRAME_TRY(lt48_rez_find_resource(rez, "DefineName", &names_index));
    FRAME_TRY(lt48_rez_get_defined_name_collection_info(rez, names_index, &names));
    for (size_t item = 0; item < names.count; ++item) {
        lt48_defined_name name;
        FRAME_TRY(lt48_rez_get_defined_name(rez, names_index, item, &name));
        if (name.name_length == 9 && !memcmp(name.name, "_BUF_Mode", 9)) {
            if (found || name.identifier >= count) return LT48_FORMAT_ERROR;
            profile.mode_buffer = (uint8_t)name.identifier;
            found = 1;
        }
    }
    if (!found) return LT48_NOT_FOUND;
    for (size_t item = 0; item < family.operation_count; ++item) {
        lt48_family_operation operation;
        lt48_wave_code code;
        lt48_compact_wave *wave = NULL;
        char name[16];
        FRAME_TRY(lt48_rez_get_family_operation(rez, family_index, item, &operation));
        snprintf(name, sizeof(name), "W%x", operation.wave);
        FRAME_TRY(lt48_rez_find_resource(rez, name, &index));
        FRAME_TRY(lt48_rez_get_wave_code(rez, index, &code));
        if (code.wave_type != 0) continue;
        FRAME_TRY(lt48_compact_decode(code.data, code.data_size, &wave));
        for (size_t instruction = 0; instruction < lt48_compact_instruction_count(wave); ++instruction) {
            lt48_compact_instruction compact;
            status = lt48_compact_get_instruction(wave, instruction, &compact);
            if (status != LT48_OK) break;
            if (compact.opcode == 0x8e) {
                if (compact.operand_size != 1 || compact.operands[0] >= count) {
                    status = LT48_FORMAT_ERROR; break;
                }
                /* High-buffer preservation needs an explicit wider profile;
                 * reject it rather than aliasing its bit onto a low buffer. */
                if (compact.operands[0] >= 32) { status = LT48_UNSUPPORTED; break; }
                profile.reset_mask &= ~(UINT32_C(1) << compact.operands[0]);
            }
        }
        lt48_compact_wave_close(wave);
        if (status != LT48_OK) return status;
    }
    *out_profile = profile;
    return LT48_OK;
#undef FRAME_TRY
}

lt48_status lt48_rez_get_pof_fuse_core(const lt48_rez *rez,
    size_t definition_index, lt48_pof_fuse_core *core) {
    lt48_device_definition d; const rez_resource_owned *r;
    const unsigned char *body; size_t index, at; char name[16];
    if (!rez || !core) return LT48_INVALID_ARGUMENT;
    lt48_status status=lt48_rez_get_device_definition(rez,definition_index,&d);
    if(status!=LT48_OK)return status;
    if(d.kind!=LT48_DEVICE_DEFINITION_POF_FUSE || d.trailing_data_size!=4)return LT48_UNSUPPORTED;
    snprintf(name,sizeof(name),"A%x",read_u16(d.trailing_data));
    status=lt48_rez_find_resource(rez,name,&index);
    if(status!=LT48_OK)return status;
    status=require_atom(rez,index,"TPOFCoreCode",&r,&body);
    if(status!=LT48_OK)return status;
    static const unsigned char collection[]={2,'[',14,'T','P','O','F','C','o','l','l','e','c','t','i','o','n'};
    static const unsigned char unit[]={2,'[',8,'T','P','O','F','U','n','i','t'};
    if(r->body_size<sizeof(collection)+12 || memcmp(body,collection,sizeof(collection)))return LT48_FORMAT_ERROR;
    at=sizeof(collection);if(read_u32(body+at)!=3)return LT48_UNSUPPORTED;at+=12;
    lt48_pof_fuse_core out={0};unsigned seen=0;
    for(unsigned i=0;i<3;i++) {
        if(!range_valid(r->body_size,at,sizeof(unit)+8) || memcmp(body+at,unit,sizeof(unit)))return LT48_FORMAT_ERROR;
        at+=sizeof(unit);unsigned tag=read_u16(body+at);uint32_t value=read_u32(body+at+2);size_t n=read_u16(body+at+6);at+=8;
        if(!range_valid(r->body_size,at,n+1) || body[at+n]!=']')return LT48_FORMAT_ERROR;
        unsigned flag=tag==2?1:tag==17?2:tag==8?4:0;
        if(!flag || (seen&flag))return LT48_UNSUPPORTED;
        seen|=flag;
        if(tag==2){if(!n || n>256 || value!=n || body[at+n-1])return LT48_FORMAT_ERROR;out.device_name=body+at;out.device_name_size=n;}
        if(tag==17){if(n!=12 || value<12)return LT48_FORMAT_ERROR;memcpy(out.header,body+at,12);out.packed_size=value-12;out.fuse_count=read_u32(body+at+6);}
        if(tag==8 && (n!=2 || value!=2))return LT48_FORMAT_ERROR;
        at+=n+1;
    }
    if(!range_valid(r->body_size,at,7) || body[at]!=']' || read_u32(body+at+1)!=out.packed_size || read_u16(body+at+5)!=2)return LT48_FORMAT_ERROR;
    if(read_u32(out.header)!=0 || read_u16(out.header+4)>1 || read_u16(out.header+10)!=1 || !out.fuse_count || out.packed_size!=(out.fuse_count+7)/8)return LT48_UNSUPPORTED;
    out.fuse_identifier=read_u16(d.trailing_data+2);*core=out;return LT48_OK;
}

/* 00477430 deserialization and 0045de40 byte spans. The container is parsed
 * once before any operation. Never publish a partial or inconsistent layout. */
static lt48_status pof_unit_at(const unsigned char *body,size_t size,
    size_t *at,lt48_pof_core_unit *out) {
    static const unsigned char mark[]={2,'[',8,'T','P','O','F','U','n','i','t'};
    if(!range_valid(size,*at,sizeof(mark)+8) || memcmp(body+*at,mark,sizeof(mark)))return LT48_FORMAT_ERROR;
    size_t start=*at+sizeof(mark);lt48_pof_core_unit unit={0};
    unit.tag=read_u16(body+start);unit.value=read_u32(body+start+2);
    unit.payload_size=read_u16(body+start+6);start+=8;
    if(!range_valid(size,start,unit.payload_size+1) || body[start+unit.payload_size]!=']')return LT48_FORMAT_ERROR;
    unit.payload=body+start;*at=start+unit.payload_size+1;*out=unit;return LT48_OK;
}
lt48_status lt48_pof_core_parse_internal(const unsigned char *body,size_t size,lt48_pof_core *out) {
    static const unsigned char collection[]={2,'[',14,'T','P','O','F','C','o','l','l','e','c','t','i','o','n'};
    if(!body||!out)return LT48_INVALID_ARGUMENT;
    memset(out,0,sizeof(*out));
    if(size<sizeof(collection)+19 || memcmp(body,collection,sizeof(collection)))return LT48_FORMAT_ERROR;
    size_t at=sizeof(collection);uint32_t count=read_u32(body+at);at+=12;
    if(!count || count>10000)return LT48_FORMAT_ERROR;
    lt48_pof_core core={0};core.units=body+at;core.unit_count=count;
    size_t first=at;uint64_t total=0;unsigned metadata=0;
    for(uint32_t i=0;i<count;i++) {
        lt48_pof_core_unit u;lt48_status status=pof_unit_at(body,size,&at,&u);
        if(status!=LT48_OK)return status;
        if(u.tag==7) {
            if(u.payload_size!=12)return LT48_FORMAT_ERROR;
            uint32_t rows=read_u16(u.payload+4),columns=read_u16(u.payload+6);
            total+=(uint64_t)rows*columns*(1u+(read_u16(u.payload+10)&1u));
        } else if(u.tag==17) {
            if(u.payload_size!=12 || u.value<12)return LT48_FORMAT_ERROR;
            uint32_t span=u.value-12,limit=read_u32(u.payload+6);if(span>limit)span=limit;
            total+=span;
        } else if(u.tag==2) {
            if((metadata&1) || !u.payload_size || u.value!=u.payload_size || u.payload[u.payload_size-1]!=0)return LT48_FORMAT_ERROR;
            metadata|=1;
        } else if(u.tag==8) {
            if((metadata&2) || u.payload_size!=2 || u.value!=2)return LT48_FORMAT_ERROR;
            metadata|=2;
        } else return LT48_UNSUPPORTED;
        if(total>UINT32_MAX)return LT48_FORMAT_ERROR;
    }
    core.units_size=at-first;
    if(size-at!=7 || body[at]!=']' || metadata!=3)return LT48_FORMAT_ERROR;
    core.total_bytes=read_u32(body+at+1);core.format=read_u16(body+at+5);
    if(!total || total!=core.total_bytes || (core.format!=1 && core.format!=2))return LT48_FORMAT_ERROR;
    *out=core;return LT48_OK;
}
lt48_status lt48_pof_core_get_unit(const lt48_pof_core *core,size_t index,lt48_pof_core_unit *out) {
    if(!core||!out||!core->units)return LT48_INVALID_ARGUMENT;
    if(index>=core->unit_count)return LT48_NOT_FOUND;
    size_t at=0;lt48_pof_core_unit u;
    for(size_t i=0;i<=index;i++) {lt48_status status=pof_unit_at(core->units,core->units_size,&at,&u);if(status!=LT48_OK)return status;}
    *out=u;return LT48_OK;
}
lt48_status lt48_rez_get_pof_core(const lt48_rez *rez,size_t definition_index,lt48_pof_core *out) {
    if(!rez||!out)return LT48_INVALID_ARGUMENT;
    memset(out,0,sizeof(*out));lt48_device_definition d;
    lt48_status status=lt48_rez_get_device_definition(rez,definition_index,&d);
    if(status!=LT48_OK)return status;
    if(d.kind!=LT48_DEVICE_DEFINITION_POF || d.trailing_data_size!=2)return LT48_UNSUPPORTED;
    char name[16];snprintf(name,sizeof(name),"A%x",read_u16(d.trailing_data));size_t index;
    status=lt48_rez_find_resource(rez,name,&index);if(status!=LT48_OK)return status;
    const rez_resource_owned *r;const unsigned char *body;
    status=require_atom(rez,index,"TPOFCoreCode",&r,&body);if(status!=LT48_OK)return status;
    return lt48_pof_core_parse_internal(body,r->body_size,out);
}

/* Class 2017 sparse address/mask collection. Borrowed immutable atom body. */
lt48_status lt48_rez_xilinx_table_internal(const lt48_rez *rez,size_t index,
    const unsigned char **data,size_t *size) {
    const rez_resource_owned *r;const unsigned char *p;
    if(!data||!size)return LT48_INVALID_ARGUMENT;
    lt48_status st=require_atom(rez,index,"TXilinxAddrData",&r,&p);
    if(st!=LT48_OK)return st;
    *data=p;*size=r->body_size;return LT48_OK;
}
