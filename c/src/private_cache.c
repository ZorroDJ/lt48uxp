#define _POSIX_C_SOURCE 200809L

#include <labtool48/labtool48.h>

#include "internal.h"

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

enum { LT48_CACHE_MAX_OBJECTS = 128 };

static const unsigned char idle_sha256[LT48_SHA256_SIZE] = {
    0x16,0x05,0xd6,0x10,0xbc,0x48,0xdc,0x5a,
    0x38,0x78,0xd0,0x63,0x3e,0xaa,0xf3,0x47,
    0x3b,0xc0,0x1c,0xed,0xbf,0xcf,0x1c,0xff,
    0x04,0x09,0xd3,0x3a,0xd0,0x8f,0x0b,0xab
};

static const unsigned char idle_source_exe_sha256[LT48_SHA256_SIZE] = {
    0x26,0xbb,0x09,0x5a,0x1c,0x69,0x1b,0x7c,
    0x07,0xdc,0x94,0x88,0x1b,0x2b,0x67,0xeb,
    0xc3,0xf7,0x53,0xf0,0x99,0x4d,0x91,0xeb,
    0x81,0xf5,0xa2,0xd1,0xdc,0x63,0x73,0xf3
};

typedef struct idle_source_profile {
    const char *name;
    const unsigned char *digest;
    long offset;
} idle_source_profile;
static const unsigned char dataman_1310_exe_sha256[LT48_SHA256_SIZE] = {
    0xf2,0xd4,0x9c,0x20,0x75,0x7e,0xb9,0x97,0x48,0xe9,0x1a,0x49,0x7f,0x58,0x49,0xe2,0x6b,0xf1,0x41,0x67,0x72,0x6f,0x77,0xa9,0xf3,0x13,0xa7,0x92,0x44,0x70,0x1c,0x12
};
static const unsigned char aec_1430_exe_sha256[LT48_SHA256_SIZE] = {
    0x7d,0xc3,0x96,0xcc,0xa1,0x1f,0xa3,0xd4,0x39,0xe3,0x53,0x32,0xce,0x39,0x03,0x77,0x3f,0xeb,0xd7,0x30,0x98,0x2b,0x4c,0xc9,0xf2,0xae,0x3e,0x08,0x7d,0x9f,0x5b,0xb3
};
static const unsigned char aec_1440_exe_sha256[LT48_SHA256_SIZE] = {
    0x2a,0x49,0x39,0x49,0xb6,0x29,0x08,0xc9,0x8c,0xb5,0xa9,0xe0,0x03,0x7c,0x9d,0x33,0xac,0x57,0x56,0x0b,0x8c,0x44,0xd6,0xae,0x2d,0x1f,0xf3,0xcf,0x75,0xb0,0x5f,0x2f
};
static const unsigned char aec_1440_variant_exe_sha256[LT48_SHA256_SIZE] = {
    0x25,0x8d,0xd0,0x8f,0x50,0xc6,0x5e,0x99,0x60,0xb1,0xfa,0xe6,0xc1,0x6c,0x6e,0x99,0x27,0x1d,0x0d,0x08,0x76,0x59,0xc2,0x75,0x19,0xfc,0x3d,0x71,0x7f,0x41,0xa4,0x68
};
static const idle_source_profile idle_sources[] = {
    {"idle-fpga.bin", idle_sha256, 0},
    {"WLT48UXP.exe", idle_source_exe_sha256, 925564},
    {"WLT48UXP.exe", aec_1430_exe_sha256, 6168776},
    {"WLT48UXP.EXE", aec_1430_exe_sha256, 6168776},
    {"WLT48UXP.exe", aec_1440_exe_sha256, 6169688},
    {"WLT48UXP.EXE", aec_1440_exe_sha256, 6169688},
    {"WLT48UXP.exe", aec_1440_variant_exe_sha256, 6169688},
    {"WLT48UXP.EXE", aec_1440_variant_exe_sha256, 6169688},
    {"WDATA48UXP.exe", dataman_1310_exe_sha256, 6132712},
    {"WDATA48UXP.EXE", dataman_1310_exe_sha256, 6132712}
};

static char *join_path(const char *left, const char *right) {
    size_t left_size;
    size_t right_size;
    char *result;
    if (left == NULL || right == NULL) return NULL;
    left_size = strlen(left);
    right_size = strlen(right);
    if (left_size > SIZE_MAX - right_size - 2u) return NULL;
    result = malloc(left_size + right_size + 2u);
    if (result == NULL) return NULL;
    snprintf(result, left_size + right_size + 2u, "%s/%s", left, right);
    return result;
}

static void digest_hex(
    const unsigned char digest[LT48_SHA256_SIZE],
    char output[LT48_SHA256_SIZE * 2u + 1u]
) {
    static const char digits[] = "0123456789abcdef";
    for (size_t index = 0u; index < LT48_SHA256_SIZE; ++index) {
        output[index * 2u] = digits[digest[index] >> 4];
        output[index * 2u + 1u] = digits[digest[index] & 15u];
    }
    output[LT48_SHA256_SIZE * 2u] = '\0';
}

static int parse_digest_hex(
    const char *text, unsigned char digest[LT48_SHA256_SIZE]
) {
    if (text == NULL || strlen(text) != LT48_SHA256_SIZE * 2u) return 0;
    for (size_t index = 0u; index < LT48_SHA256_SIZE; ++index) {
        int high;
        int low;
        unsigned char a = (unsigned char)text[index * 2u];
        unsigned char b = (unsigned char)text[index * 2u + 1u];
        high = a >= '0' && a <= '9' ? a - '0' :
               a >= 'a' && a <= 'f' ? a - 'a' + 10 : -1;
        low = b >= '0' && b <= '9' ? b - '0' :
              b >= 'a' && b <= 'f' ? b - 'a' + 10 : -1;
        if (high < 0 || low < 0) return 0;
        digest[index] = (unsigned char)((high << 4) | low);
    }
    return 1;
}

lt48_status lt48_hash_file_internal(
    const char *path, unsigned char digest[LT48_SHA256_SIZE], size_t *out_size
) {
    unsigned char buffer[8192];
    lt48_sha256_context hash;
    size_t total = 0u;
    FILE *file;
    if (path == NULL || digest == NULL) return LT48_INVALID_ARGUMENT;
    file = fopen(path, "rb");
    if (file == NULL) return LT48_FILE_ERROR;
    lt48_sha256_init(&hash);
    for (;;) {
        size_t size = fread(buffer, 1u, sizeof(buffer), file);
        if (size > SIZE_MAX - total) {
            fclose(file);
            return LT48_FORMAT_ERROR;
        }
        if (size != 0u) {
            lt48_sha256_update(&hash, buffer, size);
            total += size;
        }
        if (size != sizeof(buffer)) {
            if (ferror(file)) {
                fclose(file);
                return LT48_FILE_ERROR;
            }
            break;
        }
    }
    if (fclose(file) != 0) return LT48_FILE_ERROR;
    lt48_sha256_final(&hash, digest);
    if (out_size != NULL) *out_size = total;
    return LT48_OK;
}

static lt48_status read_exact_file(
    const char *path, size_t expected_size, unsigned char **out_data
) {
    FILE *file;
    unsigned char *data;
    int trailing;
    if (path == NULL || out_data == NULL) return LT48_INVALID_ARGUMENT;
    *out_data = NULL;
    file = fopen(path, "rb");
    if (file == NULL) return LT48_FILE_ERROR;
    data = malloc(expected_size == 0u ? 1u : expected_size);
    if (data == NULL) {
        fclose(file);
        return LT48_NO_MEMORY;
    }
    if (fread(data, 1u, expected_size, file) != expected_size) {
        free(data);
        fclose(file);
        return LT48_FILE_ERROR;
    }
    trailing = fgetc(file);
    if (trailing != EOF || ferror(file) || fclose(file) != 0) {
        free(data);
        return trailing == EOF ? LT48_FILE_ERROR : LT48_FORMAT_ERROR;
    }
    *out_data = data;
    return LT48_OK;
}

static lt48_status load_verified_idle_with_source(
    const char *root, unsigned char **out_data, size_t *out_size,
    const idle_source_profile **out_source
) {
    enum { idle_size = 2778 };
    const idle_source_profile *source = NULL;
    unsigned char digest[LT48_SHA256_SIZE];
    unsigned char *idle = NULL;
    char *path;
    FILE *file;
    lt48_status status;
    if (root == NULL || out_data == NULL || out_size == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    *out_data = NULL;
    *out_size = 0u;
    if (out_source) *out_source = NULL;

    path = join_path(root, "idle-fpga.bin");
    if (path == NULL) return LT48_NO_MEMORY;
    if (access(path, F_OK) == 0) {
        status = read_exact_file(path, idle_size, &idle);
        free(path);
        if (status != LT48_OK) return status;
        lt48_sha256(idle, idle_size, digest);
        if (memcmp(digest, idle_sha256, sizeof(digest)) != 0) {
            free(idle);
            return LT48_SAFETY_REJECTED;
        }
        *out_data = idle;
        *out_size = idle_size;
        if (out_source) *out_source = &idle_sources[0];
        return LT48_OK;
    }
    free(path);

    int found_candidate = 0;
    for (size_t i = 1; i < sizeof(idle_sources)/sizeof(idle_sources[0]); ++i) {
        path = join_path(root, idle_sources[i].name);
        if (!path) return LT48_NO_MEMORY;
        if (access(path, F_OK) == 0) {
            found_candidate = 1;
            status = lt48_hash_file_internal(path, digest, NULL);
            if (status != LT48_OK) { free(path); return status; }
            if (memcmp(digest, idle_sources[i].digest, sizeof(digest)) == 0) {
                source = &idle_sources[i];
                break;
            }
        }
        free(path);
        path = NULL;
    }
    if (!source) return found_candidate ? LT48_SAFETY_REJECTED : LT48_FILE_ERROR;
    file = fopen(path, "rb");
    free(path);
    if (file == NULL) return LT48_FILE_ERROR;
    if (fseek(file, source->offset, SEEK_SET) != 0) {
        fclose(file);
        return LT48_FILE_ERROR;
    }
    idle = malloc(idle_size);
    if (idle == NULL) {
        fclose(file);
        return LT48_NO_MEMORY;
    }
    if (fread(idle, 1u, idle_size, file) != idle_size || fclose(file) != 0) {
        free(idle);
        return LT48_FILE_ERROR;
    }
    lt48_sha256(idle, idle_size, digest);
    if (memcmp(digest, idle_sha256, sizeof(digest)) != 0) {
        free(idle);
        return LT48_SAFETY_REJECTED;
    }
    *out_data = idle;
    *out_size = idle_size;
    if (out_source) *out_source = source;
    return LT48_OK;
}

lt48_status lt48_load_verified_idle_fpga_internal(
    const char *root, unsigned char **out_data, size_t *out_size
) {
    return load_verified_idle_with_source(root, out_data, out_size, NULL);
}

static int safe_logical_name(const char *name) {
    size_t length;
    if (name == NULL) return 0;
    length = strlen(name);
    if (length == 0u || length > 96u || name[0] == '.') return 0;
    for (size_t index = 0u; index < length; ++index) {
        unsigned char value = (unsigned char)name[index];
        if (!(isalnum(value) || value == '_' || value == '-' || value == '.')) {
            return 0;
        }
    }
    return 1;
}

static lt48_status verify_one_cache_object(
    const char *root, const char *name, const char *hex, size_t expected_size
) {
    unsigned char expected[LT48_SHA256_SIZE];
    unsigned char actual[LT48_SHA256_SIZE];
    char object_name[8u + LT48_SHA256_SIZE * 2u + 1u];
    char *object_path = NULL;
    char *logical_path = NULL;
    size_t actual_size;
    lt48_status status;
    if (!safe_logical_name(name) || !parse_digest_hex(hex, expected)) {
        return LT48_FORMAT_ERROR;
    }
    snprintf(object_name, sizeof(object_name), "objects/%s", hex);
    object_path = join_path(root, object_name);
    logical_path = join_path(root, name);
    if (object_path == NULL || logical_path == NULL) {
        free(object_path);
        free(logical_path);
        return LT48_NO_MEMORY;
    }
    status = lt48_hash_file_internal(object_path, actual, &actual_size);
    if (status == LT48_OK &&
        (actual_size != expected_size ||
         memcmp(actual, expected, sizeof(actual)) != 0)) {
        status = LT48_SAFETY_REJECTED;
    }
    if (status == LT48_OK) {
        status = lt48_hash_file_internal(logical_path, actual, &actual_size);
        if (status == LT48_OK &&
            (actual_size != expected_size ||
             memcmp(actual, expected, sizeof(actual)) != 0)) {
            status = LT48_SAFETY_REJECTED;
        }
    }
    free(object_path);
    free(logical_path);
    return status;
}

lt48_status lt48_private_cache_verify(
    const char *path, lt48_private_cache_report *out_report
) {
    char *manifest_path = NULL;
    char *target = NULL;
    char *line = NULL;
    size_t line_capacity = 0u;
    ssize_t line_size;
    struct stat link_info;
    FILE *manifest = NULL;
    unsigned char expected_manifest[LT48_SHA256_SIZE];
    unsigned char actual_manifest[LT48_SHA256_SIZE];
    size_t actual_manifest_size;
    size_t entry_count = 0u;
    size_t derived_count = 0u;
    lt48_status status = LT48_OK;
    lt48_private_cache_report report;
    if (path == NULL || out_report == NULL) return LT48_INVALID_ARGUMENT;
    memset(&report, 0, sizeof(report));
    manifest_path = join_path(path, "manifest.lt48");
    if (manifest_path == NULL) return LT48_NO_MEMORY;
    if (lstat(manifest_path, &link_info) != 0 || !S_ISLNK(link_info.st_mode) ||
        link_info.st_size <= 0 || link_info.st_size > 96) {
        free(manifest_path);
        return LT48_FORMAT_ERROR;
    }
    target = malloc((size_t)link_info.st_size + 1u);
    if (target == NULL) {
        free(manifest_path);
        return LT48_NO_MEMORY;
    }
    line_size = readlink(manifest_path, target, (size_t)link_info.st_size + 1u);
    if (line_size != link_info.st_size) {
        status = LT48_FILE_ERROR;
        goto cleanup;
    }
    target[line_size] = '\0';
    if (strncmp(target, "objects/", 8u) != 0 ||
        !parse_digest_hex(target + 8u, expected_manifest)) {
        status = LT48_FORMAT_ERROR;
        goto cleanup;
    }
    status = lt48_hash_file_internal(
        manifest_path, actual_manifest, &actual_manifest_size
    );
    if (status != LT48_OK) goto cleanup;
    if (actual_manifest_size == 0u ||
        memcmp(actual_manifest, expected_manifest, sizeof(actual_manifest)) != 0) {
        status = LT48_SAFETY_REJECTED;
        goto cleanup;
    }
    memcpy(report.cache_manifest_sha256, actual_manifest,
           sizeof(actual_manifest));
    report.object_count = 1u;
    report.verified_object_count = 1u;
    report.total_bytes = actual_manifest_size;
    manifest = fopen(manifest_path, "rb");
    if (manifest == NULL) {
        status = LT48_FILE_ERROR;
        goto cleanup;
    }
    line_size = getline(&line, &line_capacity, manifest);
    if (line_size < 0 || strcmp(line, "LT48-PRIVATE-CACHE\t1\n") != 0) {
        status = LT48_FORMAT_ERROR;
        goto cleanup;
    }
    while ((line_size = getline(&line, &line_capacity, manifest)) >= 0) {
        char *save = NULL;
        char *role;
        char *name;
        char *hex;
        char *size_text;
        char *extra;
        char *end = NULL;
        unsigned long long parsed_size;
        if (line_size == 0 || line[line_size - 1] != '\n') {
            status = LT48_FORMAT_ERROR;
            goto cleanup;
        }
        line[line_size - 1] = '\0';
        role = strtok_r(line, "\t", &save);
        name = strtok_r(NULL, "\t", &save);
        hex = strtok_r(NULL, "\t", &save);
        size_text = strtok_r(NULL, "\t", &save);
        extra = strtok_r(NULL, "\t", &save);
        if (role != NULL && strcmp(role, "derived") == 0) {
            unsigned char source_digest[LT48_SHA256_SIZE];
            char *length_text = strtok_r(NULL, "\t", &save);
            char *derived_extra = strtok_r(NULL, "\t", &save);
            int known_source = 0;
            if (hex && size_text && extra && parse_digest_hex(size_text, source_digest)) {
                for (size_t i = 0; i < sizeof(idle_sources)/sizeof(idle_sources[0]); ++i) {
                    char offset_text[32];
                    snprintf(offset_text, sizeof(offset_text), "%ld", idle_sources[i].offset);
                    if (strcmp(hex, idle_sources[i].name) == 0 &&
                        memcmp(source_digest, idle_sources[i].digest, LT48_SHA256_SIZE) == 0 &&
                        strcmp(extra, offset_text) == 0) known_source = 1;
                }
            }
            if (name == NULL || hex == NULL || size_text == NULL ||
                extra == NULL || length_text == NULL ||
                derived_extra != NULL ||
                strcmp(name, "idle-fpga.bin") != 0 ||
                !known_source ||
                strcmp(length_text, "2778") != 0 || derived_count != 0u) {
                status = LT48_FORMAT_ERROR;
                goto cleanup;
            }
            ++derived_count;
            continue;
        }
        if (role == NULL || name == NULL || hex == NULL ||
            size_text == NULL || extra != NULL ||
            (strcmp(role, "catalogue") != 0 &&
             strcmp(role, "definition") != 0 &&
             strcmp(role, "runtime") != 0)) {
            status = LT48_FORMAT_ERROR;
            goto cleanup;
        }
        errno = 0;
        parsed_size = strtoull(size_text, &end, 10);
        if (errno != 0 || end == size_text || *end != '\0' ||
            parsed_size > SIZE_MAX || entry_count >= LT48_CACHE_MAX_OBJECTS) {
            status = LT48_FORMAT_ERROR;
            goto cleanup;
        }
        status = verify_one_cache_object(
            path, name, hex, (size_t)parsed_size
        );
        if (status != LT48_OK) goto cleanup;
        ++entry_count;
        ++report.verified_object_count;
        report.total_bytes += (size_t)parsed_size;
        if (strcmp(role, "runtime") != 0) ++report.source_file_count;
    }
    if (ferror(manifest) || entry_count == 0u || derived_count != 1u)
        status = LT48_FORMAT_ERROR;
    if (status == LT48_OK) report.object_count = entry_count + 1u;

cleanup:
    if (manifest != NULL && fclose(manifest) != 0 && status == LT48_OK) {
        status = LT48_FILE_ERROR;
    }
    free(line);
    free(target);
    free(manifest_path);
    if (status == LT48_OK) *out_report = report;
    return status;
}

static lt48_status copy_to_object(
    const char *source, const char *object_path,
    const unsigned char expected[LT48_SHA256_SIZE], size_t expected_size,
    int *out_created
) {
    unsigned char buffer[8192];
    unsigned char actual[LT48_SHA256_SIZE];
    size_t actual_size;
    int input = -1;
    int output = -1;
    lt48_status status = LT48_OK;
    *out_created = 0;
    output = open(object_path, O_WRONLY | O_CREAT | O_EXCL, 0600);
    if (output < 0 && errno == EEXIST) {
        status = lt48_hash_file_internal(object_path, actual, &actual_size);
        if (status == LT48_OK &&
            (actual_size != expected_size ||
             memcmp(actual, expected, sizeof(actual)) != 0)) {
            status = LT48_SAFETY_REJECTED;
        }
        return status;
    }
    if (output < 0) return LT48_FILE_ERROR;
    input = open(source, O_RDONLY);
    if (input < 0) {
        close(output);
        unlink(object_path);
        return LT48_FILE_ERROR;
    }
    for (;;) {
        ssize_t count = read(input, buffer, sizeof(buffer));
        if (count < 0) {
            if (errno == EINTR) continue;
            status = LT48_FILE_ERROR;
            break;
        }
        if (count == 0) break;
        for (ssize_t offset = 0; offset < count;) {
            ssize_t written = write(output, buffer + offset,
                                    (size_t)(count - offset));
            if (written < 0) {
                if (errno == EINTR) continue;
                status = LT48_FILE_ERROR;
                break;
            }
            offset += written;
        }
        if (status != LT48_OK) break;
    }
    if (close(input) != 0 && status == LT48_OK) status = LT48_FILE_ERROR;
    if (fsync(output) != 0 && status == LT48_OK) status = LT48_FILE_ERROR;
    if (close(output) != 0 && status == LT48_OK) status = LT48_FILE_ERROR;
    if (status == LT48_OK) {
        status = lt48_hash_file_internal(object_path, actual, &actual_size);
        if (status == LT48_OK &&
            (actual_size != expected_size ||
             memcmp(actual, expected, sizeof(actual)) != 0)) {
            status = LT48_SAFETY_REJECTED;
        }
    }
    if (status != LT48_OK) unlink(object_path);
    else *out_created = 1;
    return status;
}

static lt48_status write_bytes_object(
    const char *object_path, const unsigned char *data, size_t size,
    const unsigned char expected[LT48_SHA256_SIZE], int *out_created
) {
    unsigned char actual[LT48_SHA256_SIZE];
    size_t actual_size;
    int output;
    size_t offset = 0u;
    lt48_status status = LT48_OK;
    *out_created = 0;
    output = open(object_path, O_WRONLY | O_CREAT | O_EXCL, 0600);
    if (output < 0 && errno == EEXIST) {
        status = lt48_hash_file_internal(object_path, actual, &actual_size);
        if (status == LT48_OK &&
            (actual_size != size || memcmp(actual, expected, sizeof(actual)) != 0))
            status = LT48_SAFETY_REJECTED;
        return status;
    }
    if (output < 0) return LT48_FILE_ERROR;
    while (offset < size) {
        ssize_t written = write(output, data + offset, size - offset);
        if (written < 0) {
            if (errno == EINTR) continue;
            status = LT48_FILE_ERROR;
            break;
        }
        offset += (size_t)written;
    }
    if (fsync(output) != 0 && status == LT48_OK) status = LT48_FILE_ERROR;
    if (close(output) != 0 && status == LT48_OK) status = LT48_FILE_ERROR;
    if (status != LT48_OK) unlink(object_path);
    else *out_created = 1;
    return status;
}

static lt48_status add_source_object(
    const char *source_root, const char *cache_root, const char *role,
    const char *name, FILE *manifest, lt48_private_cache_report *report
) {
    unsigned char digest[LT48_SHA256_SIZE];
    char hex[LT48_SHA256_SIZE * 2u + 1u];
    char object_name[8u + LT48_SHA256_SIZE * 2u + 1u];
    char target[8u + LT48_SHA256_SIZE * 2u + 1u];
    char *source = NULL;
    char *object = NULL;
    char *logical = NULL;
    size_t size;
    int created;
    lt48_status status;
    source = join_path(source_root, name);
    if (source == NULL) return LT48_NO_MEMORY;
    status = lt48_hash_file_internal(source, digest, &size);
    if (status != LT48_OK) goto cleanup;
    digest_hex(digest, hex);
    snprintf(object_name, sizeof(object_name), "objects/%s", hex);
    snprintf(target, sizeof(target), "objects/%s", hex);
    object = join_path(cache_root, object_name);
    logical = join_path(cache_root, name);
    if (object == NULL || logical == NULL) {
        status = LT48_NO_MEMORY;
        goto cleanup;
    }
    status = copy_to_object(source, object, digest, size, &created);
    if (status != LT48_OK) goto cleanup;
    if (symlink(target, logical) != 0) {
        status = LT48_FILE_ERROR;
        goto cleanup;
    }
    if (fprintf(manifest, "%s\t%s\t%s\t%zu\n", role, name, hex, size) < 0) {
        status = LT48_FILE_ERROR;
        goto cleanup;
    }
    ++report->source_file_count;
    if (created) {
        ++report->object_count;
        report->total_bytes += size;
    }

cleanup:
    free(source);
    free(object);
    free(logical);
    return status;
}

static lt48_status add_runtime_object(
    const char *cache_root, const char *name, const unsigned char *data,
    size_t size, FILE *manifest, lt48_private_cache_report *report
) {
    unsigned char digest[LT48_SHA256_SIZE];
    char hex[LT48_SHA256_SIZE * 2u + 1u];
    char object_name[8u + LT48_SHA256_SIZE * 2u + 1u];
    char *object = NULL;
    char *logical = NULL;
    int created;
    lt48_status status;
    lt48_sha256(data, size, digest);
    digest_hex(digest, hex);
    snprintf(object_name, sizeof(object_name), "objects/%s", hex);
    object = join_path(cache_root, object_name);
    logical = join_path(cache_root, name);
    if (object == NULL || logical == NULL) {
        status = LT48_NO_MEMORY;
        goto cleanup;
    }
    status = write_bytes_object(object, data, size, digest, &created);
    if (status != LT48_OK) goto cleanup;
    if (symlink(object_name, logical) != 0) {
        status = LT48_FILE_ERROR;
        goto cleanup;
    }
    if (fprintf(manifest, "runtime\t%s\t%s\t%zu\n", name, hex, size) < 0) {
        status = LT48_FILE_ERROR;
        goto cleanup;
    }
    if (created) {
        ++report->object_count;
        report->total_bytes += size;
    }

cleanup:
    free(object);
    free(logical);
    return status;
}

static void remove_tree(const char *path) {
    DIR *directory = opendir(path);
    struct dirent *entry;
    if (directory == NULL) {
        unlink(path);
        return;
    }
    while ((entry = readdir(directory)) != NULL) {
        char *child;
        struct stat info;
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) continue;
        child = join_path(path, entry->d_name);
        if (child == NULL) continue;
        if (lstat(child, &info) == 0 && S_ISDIR(info.st_mode)) remove_tree(child);
        else unlink(child);
        free(child);
    }
    closedir(directory);
    rmdir(path);
}

lt48_status lt48_private_cache_import(
    const char *source_path, const char *destination_path,
    lt48_private_cache_report *out_report
) {
    return lt48_private_cache_import_with_runtime(
        source_path, source_path, destination_path, out_report
    );
}

lt48_status lt48_private_cache_import_with_runtime(
    const char *definition_source_path, const char *runtime_source_path,
    const char *destination_path, lt48_private_cache_report *out_report
) {
    enum { FIRST_CLASS = 2000, MAX_CLASS_COUNT = 49 };
    int used[MAX_CLASS_COUNT] = {0};
    size_t catalogue_numeric_column_count = 0u;
    size_t class_count;
    struct stat existing;
    lt48_installation *installation = NULL;
    lt48_installation *cached_installation = NULL;
    lt48_installation_audit audit;
    lt48_installation_audit cached_audit;
    lt48_private_cache_report report;
    lt48_private_cache_report verified;
    unsigned char *idle = NULL;
    unsigned char *pof_bundle = NULL;
    size_t pof_bundle_size = 0;
    const idle_source_profile *idle_source = NULL;
    size_t idle_size = 0u;
    char *temporary = NULL;
    char *objects = NULL;
    char *pending_manifest = NULL;
    char *manifest_object = NULL;
    char *manifest_link = NULL;
    FILE *manifest = NULL;
    unsigned char manifest_digest[LT48_SHA256_SIZE];
    char manifest_hex[LT48_SHA256_SIZE * 2u + 1u];
    char manifest_target[8u + LT48_SHA256_SIZE * 2u + 1u];
    size_t ignored_size;
    lt48_status status;
    if (definition_source_path == NULL || runtime_source_path == NULL ||
        destination_path == NULL || out_report == NULL ||
        definition_source_path[0] == '\0' || runtime_source_path[0] == '\0' ||
        destination_path[0] == '\0') {
        return LT48_INVALID_ARGUMENT;
    }
    memset(&report, 0, sizeof(report));
    if (lstat(destination_path, &existing) == 0 || errno != ENOENT) {
        return LT48_FILE_ERROR;
    }
    status = lt48_installation_open(definition_source_path, &installation);
    if (status != LT48_OK) goto cleanup;
    status = lt48_installation_audit_all(installation, &audit);
    if (status != LT48_OK) goto cleanup;
    memcpy(report.installation_manifest_sha256, audit.manifest_sha256,
           sizeof(audit.manifest_sha256));
    for (size_t index = 0u;
         index < lt48_installation_device_count(installation); ++index) {
        lt48_catalogue_entry entry;
        status = lt48_installation_get_device(installation, index, &entry);
        if (status != LT48_OK) goto cleanup;
        if (catalogue_numeric_column_count == 0u)
            catalogue_numeric_column_count = entry.numeric_column_count;
        else if (catalogue_numeric_column_count != entry.numeric_column_count) {
            status = LT48_FORMAT_ERROR;
            goto cleanup;
        }
        class_count = catalogue_numeric_column_count >= 15u ? 49u : 48u;
        if (entry.device_class < FIRST_CLASS ||
            entry.device_class >= FIRST_CLASS + class_count) {
            status = LT48_UNSUPPORTED;
            goto cleanup;
        }
        used[entry.device_class - FIRST_CLASS] = 1;
    }
    status = load_verified_idle_with_source(
        runtime_source_path, &idle, &idle_size, &idle_source
    );
    if (status != LT48_OK) goto cleanup;
    /* Optional POF runtime. Older installations remain usable without it;
     * a present but corrupted candidate is not silently replaced. */
    {
        lt48_pof_trs_source *pof=NULL;
        status=lt48_pof_trs_source_open_internal(runtime_source_path,&pof);
        if(status==LT48_OK)status=lt48_pof_source_bundle_internal(pof,&pof_bundle,&pof_bundle_size);
        lt48_pof_trs_source_close_internal(pof);
        if(status==LT48_NOT_FOUND)status=LT48_OK;
        if(status!=LT48_OK)goto cleanup;
    }
    if (strlen(destination_path) > SIZE_MAX - 12u) {
        status = LT48_INVALID_ARGUMENT;
        goto cleanup;
    }
    temporary = malloc(strlen(destination_path) + 12u);
    if (temporary == NULL) {
        status = LT48_NO_MEMORY;
        goto cleanup;
    }
    snprintf(temporary, strlen(destination_path) + 12u,
             "%s.tmp.XXXXXX", destination_path);
    if (mkdtemp(temporary) == NULL) {
        status = LT48_FILE_ERROR;
        goto cleanup;
    }
    if (chmod(temporary, 0700) != 0) {
        status = LT48_FILE_ERROR;
        goto cleanup;
    }
    objects = join_path(temporary, "objects");
    pending_manifest = join_path(temporary, ".manifest.pending");
    if (objects == NULL || pending_manifest == NULL) {
        status = LT48_NO_MEMORY;
        goto cleanup;
    }
    if (mkdir(objects, 0700) != 0) {
        status = LT48_FILE_ERROR;
        goto cleanup;
    }
    manifest = fopen(pending_manifest, "wb");
    if (manifest == NULL ||
        fputs("LT48-PRIVATE-CACHE\t1\n", manifest) == EOF) {
        status = LT48_FILE_ERROR;
        goto cleanup;
    }
    status = add_source_object(
        definition_source_path, temporary, "catalogue", "dev48uxp.rez",
        manifest, &report
    );
    if (status != LT48_OK) goto cleanup;
    class_count = catalogue_numeric_column_count >= 15u ? 49u : 48u;
    for (size_t offset = 0u; offset < class_count; ++offset) {
        const char *stem;
        char filename[16];
        if (!used[offset]) continue;
        stem = lt48_definition_stem_for_class_format(
            (uint16_t)(FIRST_CLASS + offset), catalogue_numeric_column_count
        );
        if (stem == NULL || snprintf(filename, sizeof(filename), "%s.rez", stem) < 0) {
            status = LT48_FORMAT_ERROR;
            goto cleanup;
        }
        status = add_source_object(
            definition_source_path, temporary, "definition", filename,
            manifest, &report
        );
        if (status != LT48_OK) goto cleanup;
    }
    status = add_runtime_object(
        temporary, "idle-fpga.bin", idle, idle_size, manifest, &report
    );
    if (status != LT48_OK) goto cleanup;
    {
        char source_digest[LT48_SHA256_SIZE * 2u + 1u];
        digest_hex(idle_source->digest, source_digest);
        if (fprintf(
                manifest,
                "derived\tidle-fpga.bin\t%s\t%s\t%ld\t2778\n",
                idle_source->name, source_digest, idle_source->offset
            ) < 0) {
            status = LT48_FILE_ERROR;
            goto cleanup;
        }
    }
    if(pof_bundle) {
        status=add_runtime_object(temporary,"pof-runtime-13.10.bin",pof_bundle,pof_bundle_size,manifest,&report);
        if(status!=LT48_OK)goto cleanup;
    }
    if (fflush(manifest) != 0 || fsync(fileno(manifest)) != 0 ||
        fclose(manifest) != 0) {
        manifest = NULL;
        status = LT48_FILE_ERROR;
        goto cleanup;
    }
    manifest = NULL;
    status = lt48_hash_file_internal(
        pending_manifest, manifest_digest, &ignored_size
    );
    if (status != LT48_OK) goto cleanup;
    digest_hex(manifest_digest, manifest_hex);
    snprintf(manifest_target, sizeof(manifest_target), "objects/%s", manifest_hex);
    manifest_object = join_path(temporary, manifest_target);
    manifest_link = join_path(temporary, "manifest.lt48");
    if (manifest_object == NULL || manifest_link == NULL) {
        status = LT48_NO_MEMORY;
        goto cleanup;
    }
    if (rename(pending_manifest, manifest_object) != 0 ||
        symlink(manifest_target, manifest_link) != 0) {
        status = LT48_FILE_ERROR;
        goto cleanup;
    }
    ++report.object_count;
    report.total_bytes += ignored_size;
    memcpy(report.cache_manifest_sha256, manifest_digest,
           sizeof(manifest_digest));
    status = lt48_private_cache_verify(temporary, &verified);
    if (status != LT48_OK) goto cleanup;
    status = lt48_installation_open(temporary, &cached_installation);
    if (status == LT48_OK) {
        status = lt48_installation_audit_all(
            cached_installation, &cached_audit
        );
    }
    if (status != LT48_OK ||
        memcmp(cached_audit.manifest_sha256, audit.manifest_sha256,
               LT48_SHA256_SIZE) != 0) {
        if (status == LT48_OK) status = LT48_SAFETY_REJECTED;
        goto cleanup;
    }
    if (lstat(destination_path, &existing) == 0 || errno != ENOENT ||
        rename(temporary, destination_path) != 0) {
        status = LT48_FILE_ERROR;
        goto cleanup;
    }
    free(temporary);
    temporary = NULL;
    report.verified_object_count = verified.verified_object_count;
    *out_report = report;
    status = LT48_OK;

cleanup:
    if (manifest != NULL) fclose(manifest);
    if (temporary != NULL) remove_tree(temporary);
    free(idle);
    free(pof_bundle);
    free(manifest_link);
    free(manifest_object);
    free(pending_manifest);
    free(objects);
    free(temporary);
    lt48_installation_close(cached_installation);
    lt48_installation_close(installation);
    return status;
}
