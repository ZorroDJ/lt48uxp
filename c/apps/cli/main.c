#define _POSIX_C_SOURCE 200809L

#include <labtool48/labtool48.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <unistd.h>

typedef struct cli_cancel_scope {
    struct sigaction previous;
    int installed;
} cli_cancel_scope;

static volatile sig_atomic_t cli_cancel_requested;

static void cli_cancel_signal_handler(int signal_number) {
    (void)signal_number;
    cli_cancel_requested = 1;
}

static int cli_cancel_scope_begin(cli_cancel_scope *scope) {
    struct sigaction action;
    if (scope == NULL) return 0;
    memset(scope, 0, sizeof(*scope));
    memset(&action, 0, sizeof(action));
    action.sa_handler = cli_cancel_signal_handler;
    sigemptyset(&action.sa_mask);
    cli_cancel_requested = 0;
    if (sigaction(SIGINT, &action, &scope->previous) != 0) return 0;
    scope->installed = 1;
    return 1;
}

static void cli_cancel_scope_end(cli_cancel_scope *scope) {
    if (scope != NULL && scope->installed) {
        sigaction(SIGINT, &scope->previous, NULL);
        scope->installed = 0;
    }
}

static int cli_cancel_check(void *context) {
    (void)context;
    return cli_cancel_requested != 0;
}

static int component_long_status_exit(lt48_status status) {
    if (status == LT48_OK) return 0;
    if (status == LT48_CANCELLED) return 9;
    if (status == LT48_NO_MEMORY) return 6;
    return 2;
}

static void print_json_string(const char *text);
static void hex_string(
    const unsigned char *data, size_t size, char *output
);
static int catalogue_plan_compare(
    const char *path, const char *selector, uint32_t start, uint32_t length,
    const char *image_path, const char *report_path,
    lt48_operation_kind requested_operation, int json
);
static int catalogue_plan_verify_margins(
    const char *path, const char *selector, uint32_t start, uint32_t length,
    const char *image_path, const char *report_path,
    lt48_verify_margin_selection margin, unsigned margin_type, int json
);
static int catalogue_plan_program(
    const char *path, const char *selector, uint32_t start, uint32_t length,
    const char *image_path, const char *report_path, int json
);
static int programmer_read_m37(int argc, char **argv);

static void usage(FILE *stream, const char *program) {
    fputs("       labtool48 new-device-lib list|validate [ROOT] [--json]\n",stream);
    fprintf(stream,
            "usage: %s version [--json]\n"
            "       %s cache import DEFINITIONS PRIVATE-CACHE"
            " [--runtime-source OWNED-INSTALLATION] [--json]\n"
            "       %s cache verify PRIVATE-CACHE [--json]\n"
            "       %s component resolve BUNDLE.json OBJECT-ID [--json]\n"
            "       %s component init OUTPUT.json ID VERSION AUTHOR LICENSE"
            " YYYY-MM-DD [--json]\n"
            "       %s component add-object INPUT.json OUTPUT.json NEW-VERSION"
            " YYYY-MM-DD OBJECT.json [--json]\n"
            "       %s component check-semantics BUNDLE.json [--json]\n"
            "       %s component compile-plan BUNDLE.json COMBINATION-INDEX"
            " OPERATION [--json]\n"
            "       %s component plan-matrix BUNDLE.json [--json]\n"
            "       labtool48 component evidence validate RECORD.json [--json]\n"
            "       labtool48 component evidence verify BUNDLE.json RECORD.json"
            " ARTIFACT [--json]\n"
            "       labtool48 component evidence import BUNDLE.json RECORD.json"
            " ARTIFACT STORE [--json]\n"
            "       labtool48 component evidence audit BUNDLE.json STORE"
            " [--json]\n"
            "       labtool48 component release-check BUNDLE.json STORE"
            " [--json]\n"
            "       %s diagnostics exit-codes [--json]\n"
            "       %s report inspect FILE [--json]\n"
            "       %s operation plan INSTALLATION VENDOR:DEVICE OPERATION"
            " [--start N --length N] [--input FILE|--image FILE]"
            " [--margin nominal|low|high|all] [--margin-type 0|1]"
            " [--report FILE] [--json]\n"
            "       %s programmer list [--json]\n"
            "       %s programmer probe [--json] [--bus N --address N]\n"
            "       %s programmer initialize [--json] [--bus N --address N]\n"
            "       %s programmer load-fx2 FIRMWARE.hex --bus N --address N"
            " [--execute --confirm SHA256] [--json]\n"
            "       %s programmer read-m37 INSTALLATION VENDOR:DEVICE"
            " START LENGTH OUTPUT [--bus N --address N] [--timeout MS]"
            " [--report FILE] [--execute --confirm SHA256] [--json]\n"
            "       %s resources inspect FILE [--json]\n"
            "       %s resources devices FILE [--json]\n"
            "       %s resources resolve FILE RESOURCE [--json]\n"
            "       %s resources compact-audit FILE [--json]\n"
            "       %s resources pinmaps FILE [--json]\n"
            "       %s resources compile FILE DEFINITION OPERATION"
            " [--resident-noop-default-pins|--analysis-default-pins]"
            " [--json]\n"
            "       %s resources interpret FILE DEFINITION OPERATION MODE"
            " [--address N] [--parameter-hex HEX]"
            " [--capture-hex HEX]"
            " [--initial-voltage A,B,C,D] [--resumable]"
            " [--protocol-json|--json]\n"
            "       %s catalogue search INSTALLATION QUERY [--json]\n"
            "       %s catalogue verify-installation INSTALLATION [--json]\n"
            "       %s catalogue show INSTALLATION VENDOR:DEVICE [--json]\n"
            "       %s catalogue support INSTALLATION VENDOR:DEVICE [--json]\n"
            "       %s catalogue identify INSTALLATION"
            " [--manufacturer-id N] [--device-id N] [--pins N]"
            " [--class N] [--json]\n"
            "       %s catalogue audit-m37 INSTALLATION VENDOR:DEVICE [--json]\n"
            "       %s catalogue plan-read INSTALLATION VENDOR:DEVICE"
            " START LENGTH [--report FILE] [--json]\n"
            "       %s catalogue plan-blank INSTALLATION VENDOR:DEVICE"
            " START LENGTH [--report FILE] [--json]\n"
            "       %s catalogue plan-verify INSTALLATION VENDOR:DEVICE"
            " START LENGTH IMAGE [--report FILE] [--json]\n"
            "       %s buffer bind FILE SIZE START LENGTH [--json]\n"
            "       %s buffer checksum FILE SIZE [START LENGTH] [--json]\n"
            "       %s buffer compare ACTUAL EXPECTED SIZE [START LENGTH]"
            " [--json]\n"
            "       %s buffer import INPUT SIZE OUTPUT [--pad VALUE]"
            " [--truncate] [--word-size 1|2|4|8]"
            " [--reverse-word-bytes] [--json]\n"
            "       %s buffer create SIZE FILL OUTPUT [--json]\n"
            "       %s buffer fill INPUT SIZE START LENGTH VALUE OUTPUT"
            " [--json]\n"
            "       %s buffer edit INPUT SIZE OFFSET VALUE OUTPUT [--json]\n"
            "       %s buffer word-read FILE SIZE OFFSET WIDTH little|big"
            " [--json]\n"
            "       %s buffer word-edit INPUT SIZE OFFSET WIDTH little|big"
            " VALUE OUTPUT [--json]\n"
            "       %s buffer extract INPUT SIZE START LENGTH OUTPUT"
            " [--json]\n"
            "       %s jtag plan-scan ir|dr TDI-HEX BITS TCK-HZ MAX-TCK-HZ"
            " reset|idle [--json]\n"
            "       %s jtag evaluate TDO-HEX BITS [--expected HEX] [--mask HEX]"
            " [--json]\n"
            "       %s jtag decode-chain TDO-HEX BITS ELEMENTS [--json]\n"
            "       %s jtag validate-profile ADAPTER REVISION"
            " TCK,TMS,TDI,TDO,GND,VREF[,RESET[,POWER]]"
            " external|adapter|programmer VREF-MIN-MV VREF-MAX-MV MAX-TCK-HZ"
            " present|not-required present|not-required none|hold|released"
            " high-z|high-z-power-off [--json]\n",
            program, program, program, program, program, program, program,
            program, program,
            program, program, program, program, program,
            program, program,
            program, program, program, program, program, program, program, program,
            program, program, program, program, program, program, program, program,
            program, program,
            program, program, program, program, program, program, program,
            program, program, program, program, program, program);
    fputs("       programmer initialize also accepts --fx2 OWNER.hex\n", stream);
    fputs("       labtool48 programmer leds --state off|busy|error|good [--bus N --address N] [--fx2 OWNER.hex] [--json]\n", stream);
    fprintf(stream,
            "       %s buffer image-inspect INPUT SIZE"
            " [--format auto|bin|hex|srec|straight-hex|mos|tek|hpabs] [--base ADDRESS]"
            " [--fill-gaps VALUE] [--json]\n"
            "       %s buffer image-import INPUT SIZE OUTPUT"
            " [--format auto|bin|hex|srec|straight-hex|mos|tek|hpabs] [--base ADDRESS]"
            " [--fill-gaps VALUE] [--json]\n",
            program, program);
    fputs("       labtool48 component validate BUNDLE.json [--all-variants] "
          "[--json]\n"
          "       labtool48 component add-family INPUT.json OUTPUT.json "
          "NEW-VERSION YYYY-MM-DD FAMILY.json [--json]\n"
          "       labtool48 component add-variant INPUT.json OUTPUT.json "
          "NEW-VERSION YYYY-MM-DD VARIANT.json [--json]\n"
          "       labtool48 component add-package INPUT.json OUTPUT.json "
          "NEW-VERSION YYYY-MM-DD PACKAGE.json [--json]\n"
          "       labtool48 adapter init INPUT.json OUTPUT.json NEW-VERSION "
          "YYYY-MM-DD ADAPTER.json [--json]\n"
          "       labtool48 adapter test-plan BUNDLE.json ADAPTER-ID [--json]\n"
          "       labtool48 component diff BUNDLE.json BASE-ID VARIANT-ID "
          "[--json]\n", stream);
    fputs("       labtool48 component build-payloads BUNDLE.json [--json]\n",
          stream);
    fputs("       labtool48 component verify-payload-inputs BUNDLE.json ROOT "
          "[--json]\n", stream);
    fputs("       labtool48 component execute-payload-builds BUNDLE.json ROOT "
          "--confirm SHA256 [--json]\n", stream);
    fprintf(stream,
            "       %s catalogue plan-compare INSTALLATION VENDOR:DEVICE"
            " START LENGTH IMAGE [--report FILE] [--json]\n", program);
    fputs("       labtool48 auto-id eprom-plan [--pins N] [--json]\n"
          "       labtool48 auto-id prepare-eprom --bus N --address N "
          "(--probe-only|--signature) [--pins N] [--report PATH] [--json]\n"
          "       labtool48 auto-id decode-socket RAW24HEX [--json]\n",
          stream);
    fputs("       labtool48 programmer info [--json] [--bus N --address N]\n",
          stream);
    fputs("       labtool48 diagnostics self-test [INSTALLATION] [--json]\n",
          stream);
    fputs("       labtool48 diagnostics progress-schema [--json]\n", stream);
    fputs("       labtool48 trace inspect TRACE.json [--json]\n"
          "       labtool48 trace compare LEFT.json RIGHT.json [--json]\n",
          stream);
    fputs("       labtool48 devices list INSTALLATION [--json]\n"
          "       labtool48 devices search INSTALLATION QUERY [--json]\n"
          "       labtool48 devices show INSTALLATION VENDOR:DEVICE [--json]\n"
          "       labtool48 devices guide INSTALLATION VENDOR:DEVICE [--json]\n"
          "       labtool48 support report INSTALLATION VENDOR:DEVICE [--json]\n"
          "       labtool48 support matrix INSTALLATION [--json]\n"
          "       labtool48 support export INSTALLATION [--snapshot] --json\n"
          "       labtool48 identify INSTALLATION [--manufacturer-id N]"
          " [--device-id N] [--pins N] [--class N] [--json]\n"
          "       labtool48 plan INSTALLATION VENDOR:DEVICE OPERATION"
          " [operation options] [--json]\n"
          "       labtool48 read INSTALLATION VENDOR:DEVICE START LENGTH OUTPUT"
          " [--bus N --address N] [--timeout MS] [--report FILE]"
          " [--execute --confirm SHA256] [--json]\n"
          "       labtool48 blank-check INSTALLATION VENDOR:DEVICE"
          " [--start N --length N] [--report FILE] [--json]\n"
          "       labtool48 verify INSTALLATION VENDOR:DEVICE"
          " [--start N --length N] --input FILE"
          " [--margin nominal|low|high|all] [--margin-type 0|1]"
          " [--report FILE] [--json]\n"
          "       labtool48 program|over-program|compare|erase|secure|"
          "memory-protect|configure|functional-test|auto-id|mass-production|"
          "jtag-isp INSTALLATION VENDOR:DEVICE [operation options] [--json]\n"
          "       labtool48 protect INSTALLATION VENDOR:DEVICE"
          " secure|memory-protect [operation options] [--json]\n",
          stream);
}

static int cache_import_command(
    const char *source, const char *runtime_source,
    const char *destination, int json
) {
    lt48_private_cache_report report;
    char installation_digest[LT48_SHA256_SIZE * 2u + 1u];
    char cache_digest[LT48_SHA256_SIZE * 2u + 1u];
    lt48_status status = lt48_private_cache_import_with_runtime(
        source, runtime_source, destination, &report
    );
    if (status != LT48_OK) {
        fprintf(stderr, "labtool48: private-cache import failed: %s\n",
                lt48_status_string(status));
        return status == LT48_NO_MEMORY ? 6 : 2;
    }
    hex_string(report.installation_manifest_sha256, LT48_SHA256_SIZE,
               installation_digest);
    hex_string(report.cache_manifest_sha256, LT48_SHA256_SIZE, cache_digest);
    if (json) {
        fputs("{\"schema\":1,\"operation\":\"cache-import\",\"source\":",
              stdout);
        print_json_string(source);
        fputs(",\"destination\":", stdout);
        print_json_string(destination);
        fputs(",\"runtime_source\":", stdout);
        print_json_string(runtime_source);
        printf(",\"valid\":true,\"usb_opened\":false,"
               "\"source_file_count\":%zu,\"object_count\":%zu,"
               "\"verified_object_count\":%zu,\"total_bytes\":%zu,"
               "\"installation_manifest_sha256\":",
               report.source_file_count, report.object_count,
               report.verified_object_count, report.total_bytes);
        print_json_string(installation_digest);
        fputs(",\"cache_manifest_sha256\":", stdout);
        print_json_string(cache_digest);
        puts("}");
    } else {
        printf("private cache created: %s\n", destination);
        printf("  definitions: %s\n  verified runtime source: %s\n",
               source, runtime_source);
        printf("  %zu owned source files; %zu content-addressed objects; "
               "%zu bytes\n", report.source_file_count,
               report.object_count, report.total_bytes);
        printf("  installation manifest SHA-256: %s\n", installation_digest);
        printf("  cache manifest SHA-256: %s\n", cache_digest);
        puts("  USB opened: no; original executable copied: no");
    }
    return 0;
}

static int cache_verify_command(const char *path, int json) {
    lt48_private_cache_report report;
    lt48_installation *installation = NULL;
    lt48_installation_audit audit;
    char installation_digest[LT48_SHA256_SIZE * 2u + 1u];
    char cache_digest[LT48_SHA256_SIZE * 2u + 1u];
    lt48_status status = lt48_private_cache_verify(path, &report);
    if (status == LT48_OK) status = lt48_installation_open(path, &installation);
    if (status == LT48_OK) status = lt48_installation_audit_all(installation, &audit);
    if (status != LT48_OK) {
        lt48_installation_close(installation);
        if (json) {
            fputs("{\"schema\":1,\"operation\":\"cache-verify\",\"path\":",
                  stdout);
            print_json_string(path);
            fputs(",\"valid\":false,\"usb_opened\":false,\"status\":",
                  stdout);
            print_json_string(lt48_status_string(status));
            puts("}");
            return 5;
        }
        fprintf(stderr, "labtool48: private-cache verification failed: %s\n",
                lt48_status_string(status));
        return 5;
    }
    memcpy(report.installation_manifest_sha256, audit.manifest_sha256,
           LT48_SHA256_SIZE);
    hex_string(report.installation_manifest_sha256, LT48_SHA256_SIZE,
               installation_digest);
    hex_string(report.cache_manifest_sha256, LT48_SHA256_SIZE, cache_digest);
    if (json) {
        fputs("{\"schema\":1,\"operation\":\"cache-verify\",\"path\":",
              stdout);
        print_json_string(path);
        printf(",\"valid\":true,\"usb_opened\":false,"
               "\"source_file_count\":%zu,\"object_count\":%zu,"
               "\"verified_object_count\":%zu,\"total_bytes\":%zu,"
               "\"resolved_device_count\":%zu,"
               "\"installation_manifest_sha256\":",
               report.source_file_count, report.object_count,
               report.verified_object_count, report.total_bytes,
               audit.resolved_device_count);
        print_json_string(installation_digest);
        fputs(",\"cache_manifest_sha256\":", stdout);
        print_json_string(cache_digest);
        puts("}");
    } else {
        printf("private cache %s: valid\n", path);
        printf("  %zu objects verified; %zu devices resolved; USB opened: no\n",
               report.verified_object_count, audit.resolved_device_count);
        printf("  cache manifest SHA-256: %s\n", cache_digest);
    }
    lt48_installation_close(installation);
    return 0;
}

static int parse_u8(const char *text, uint8_t *value) {
    char *end = NULL;
    unsigned long parsed = strtoul(text, &end, 0);
    if (text[0] == '\0' || end == NULL || *end != '\0' || parsed > 255) {
        return 0;
    }
    *value = (uint8_t)parsed;
    return 1;
}

static int parse_u16(const char *text, uint16_t *value) {
    char *end = NULL;
    unsigned long parsed = strtoul(text, &end, 0);
    if (text[0] == '\0' || end == NULL || *end != '\0' || parsed > 0xffff) {
        return 0;
    }
    *value = (uint16_t)parsed;
    return 1;
}

static int parse_u32(const char *text, uint32_t *value) {
    char *end = NULL;
    unsigned long parsed = strtoul(text, &end, 0);
    if (text[0] == '\0' || end == NULL || *end != '\0' ||
        parsed > UINT32_MAX) {
        return 0;
    }
    *value = (uint32_t)parsed;
    return 1;
}

static int parse_u64(const char *text, uint64_t *value) {
    char *end = NULL;
    unsigned long long parsed;
    if (text == NULL || text[0] == '\0' || text[0] == '-') return 0;
    errno = 0;
    parsed = strtoull(text, &end, 0);
    if (errno == ERANGE || end == NULL || *end != '\0') return 0;
    *value = (uint64_t)parsed;
    return 1;
}

static int parse_buffer_byte_order(
    const char *text, lt48_buffer_byte_order *out_order
) {
    if (strcmp(text, "little") == 0) {
        *out_order = LT48_BUFFER_LITTLE_ENDIAN;
        return 1;
    }
    if (strcmp(text, "big") == 0) {
        *out_order = LT48_BUFFER_BIG_ENDIAN;
        return 1;
    }
    return 0;
}

static int parse_image_format(
    const char *text, lt48_image_format *out_format
) {
    if (strcmp(text, "auto") == 0) *out_format = LT48_IMAGE_FORMAT_AUTO;
    else if (strcmp(text, "bin") == 0 || strcmp(text, "raw") == 0)
        *out_format = LT48_IMAGE_FORMAT_RAW_BINARY;
    else if (strcmp(text, "hex") == 0 || strcmp(text, "ihex") == 0)
        *out_format = LT48_IMAGE_FORMAT_INTEL_HEX;
    else if (strcmp(text, "srec") == 0 || strcmp(text, "s-record") == 0)
        *out_format = LT48_IMAGE_FORMAT_MOTOROLA_S_RECORD;
    else if (strcmp(text, "straight-hex") == 0 ||
             strcmp(text, "shx") == 0)
        *out_format = LT48_IMAGE_FORMAT_STRAIGHT_HEX;
    else if (strcmp(text, "mos") == 0 ||
             strcmp(text, "mos-technology") == 0)
        *out_format = LT48_IMAGE_FORMAT_MOS_TECHNOLOGY;
    else if (strcmp(text, "tek") == 0 || strcmp(text, "tekhex") == 0 ||
             strcmp(text, "tektronix") == 0)
        *out_format = LT48_IMAGE_FORMAT_TEKTRONIX_HEX;
    else if (strcmp(text, "hpabs") == 0 ||
             strcmp(text, "hp64000") == 0 ||
             strcmp(text, "hp64000-absolute") == 0)
        *out_format = LT48_IMAGE_FORMAT_HP64000_ABSOLUTE;
    else return 0;
    return 1;
}

static int parse_hex_buffer(const char *text, unsigned char *data,
                            size_t capacity, size_t *out_size) {
    size_t length = strlen(text);
    size_t index;
    if ((length & 1u) != 0u || length / 2u > capacity) return 0;
    for (index = 0; index < length / 2u; ++index) {
        unsigned value;
        if (sscanf(text + index * 2u, "%2x", &value) != 1) return 0;
        if (!isxdigit((unsigned char)text[index * 2u]) ||
            !isxdigit((unsigned char)text[index * 2u + 1u])) return 0;
        data[index] = (unsigned char)value;
    }
    *out_size = length / 2u;
    return 1;
}

static int parse_initial_voltage(const char *text, uint32_t values[4]) {
    char trailing;
    return sscanf(text, "%u,%u,%u,%u%c", &values[0], &values[1],
                  &values[2], &values[3], &trailing) == 4;
}

static void print_json_bytes(const unsigned char *text, size_t length) {
    putchar('"');
    for (size_t index = 0; index < length; ++index) {
        unsigned char value = text[index];
        switch (value) {
        case '"': fputs("\\\"", stdout); break;
        case '\\': fputs("\\\\", stdout); break;
        case '\b': fputs("\\b", stdout); break;
        case '\f': fputs("\\f", stdout); break;
        case '\n': fputs("\\n", stdout); break;
        case '\r': fputs("\\r", stdout); break;
        case '\t': fputs("\\t", stdout); break;
        default:
            if (value < 0x20 || value >= 0x80) {
                printf("\\u%04x", value);
            } else {
                putchar(value);
            }
            break;
        }
    }
    putchar('"');
}

static void print_json_string(const char *text) {
    if(text==NULL){fputs("null",stdout);return;}
    print_json_bytes((const unsigned char *)text, strlen(text));
}

static int argument_requests_json(int argc, char **argv) {
    for (int index = 1; index < argc; ++index) {
        if (strcmp(argv[index], "--json") == 0 ||
            strcmp(argv[index], "--protocol-json") == 0)
            return 1;
    }
    return 0;
}

static const char *exit_status_meaning(int status) {
    static const char *meanings[] = {
        "success",
        "comparison-difference",
        "usage-definition-input-or-filesystem-error",
        "operation-unsupported",
        "safety-rejection",
        "diagnostic-failure",
        "programmer-communication-failure",
        "device-identity-mismatch",
        "verification-failure",
        "operation-cancelled"
    };
    if (status < 0 || (size_t)status >= sizeof(meanings) / sizeof(meanings[0]))
        return "unclassified-failure";
    return meanings[status];
}

static char *captured_diagnostic(FILE *stream) {
    const size_t limit = 16384u;
    char *text;
    size_t size;
    size_t start = 0u;
    if (stream == NULL || fflush(stream) != 0 || fseek(stream, 0, SEEK_SET) != 0)
        return NULL;
    text = malloc(limit + 1u);
    if (text == NULL) return NULL;
    size = fread(text, 1u, limit, stream);
    while (start < size && isspace((unsigned char)text[start])) ++start;
    while (size > start && isspace((unsigned char)text[size - 1u])) --size;
    if (size == start) {
        free(text);
        return NULL;
    }
    if (start != 0u) memmove(text, text + start, size - start);
    size -= start;
    text[size] = '\0';
    if (strncmp(text, "labtool48: ", 11u) == 0)
        memmove(text, text + 11u, size - 10u);
    if (strncmp(text, "usage:", 6u) == 0)
        strcpy(text, "invalid command line; run without --json for usage");
    return text;
}

static int copy_captured(FILE *source, FILE *destination) {
    unsigned char buffer[4096];
    size_t size;
    if (fflush(source) != 0 || fseek(source, 0, SEEK_SET) != 0) return 0;
    while ((size = fread(buffer, 1u, sizeof(buffer), source)) != 0u) {
        if (fwrite(buffer, 1u, size, destination) != size) return 0;
    }
    return !ferror(source);
}

static int captured_looks_like_complete_json(FILE *stream) {
    long end;
    int first = EOF;
    int last = EOF;
    int value;
    if (stream == NULL || fflush(stream) != 0 ||
        fseek(stream, 0, SEEK_END) != 0 || (end = ftell(stream)) <= 0 ||
        fseek(stream, 0, SEEK_SET) != 0) return 0;
    while ((value = fgetc(stream)) != EOF) {
        if (!isspace((unsigned char)value)) {
            first = value;
            break;
        }
    }
    while (end > 0) {
        if (fseek(stream, --end, SEEK_SET) != 0) return 0;
        value = fgetc(stream);
        if (value != EOF && !isspace((unsigned char)value)) {
            last = value;
            break;
        }
    }
    return (first == '{' && last == '}') || (first == '[' && last == ']');
}

static void print_hex(const unsigned char *data, size_t size) {
    for (size_t index = 0; index < size; ++index) printf("%02x", data[index]);
}

static void fprint_hex(FILE *stream, const unsigned char *data, size_t size) {
    for (size_t index = 0; index < size; ++index)
        fprintf(stream, "%02x", data[index]);
}

static void hex_string(
    const unsigned char *data, size_t size, char *out_text
) {
    static const char digits[] = "0123456789abcdef";
    for (size_t index = 0u; index < size; ++index) {
        out_text[index * 2u] = digits[data[index] >> 4];
        out_text[index * 2u + 1u] = digits[data[index] & 0x0fu];
    }
    out_text[size * 2u] = '\0';
}

static int digest_text_equals(
    const char *text, const unsigned char digest[LT48_SHA256_SIZE]
) {
    char expected[LT48_SHA256_SIZE * 2u + 1u];
    if (text == NULL || strlen(text) != LT48_SHA256_SIZE * 2u) return 0;
    hex_string(digest, LT48_SHA256_SIZE, expected);
    for (size_t index = 0u; index < LT48_SHA256_SIZE * 2u; ++index) {
        if (tolower((unsigned char)text[index]) != expected[index]) return 0;
    }
    return 1;
}

static lt48_status write_new_file(
    const char *path, const unsigned char *data, size_t size
) {
    int descriptor;
    size_t offset = 0u;
    if (path == NULL || (size != 0u && data == NULL)) {
        return LT48_INVALID_ARGUMENT;
    }
    descriptor = open(path, O_WRONLY | O_CREAT | O_EXCL, 0600);
    if (descriptor < 0) return LT48_FILE_ERROR;
    while (offset < size) {
        ssize_t written = write(descriptor, data + offset, size - offset);
        if (written < 0 && errno == EINTR) continue;
        if (written <= 0) {
            (void)close(descriptor);
            (void)unlink(path);
            return LT48_FILE_ERROR;
        }
        offset += (size_t)written;
    }
    if (close(descriptor) != 0) {
        (void)unlink(path);
        return LT48_FILE_ERROR;
    }
    return LT48_OK;
}

static lt48_status read_exact_file(
    const char *path, size_t expected_size, unsigned char **out_data
) {
    FILE *file;
    long size;
    unsigned char *data;
    if (path == NULL || out_data == NULL || expected_size > LONG_MAX) {
        return LT48_INVALID_ARGUMENT;
    }
    *out_data = NULL;
    file = fopen(path, "rb");
    if (file == NULL) return LT48_FILE_ERROR;
    if (fseek(file, 0, SEEK_END) != 0 || (size = ftell(file)) < 0 ||
        (size_t)size != expected_size || fseek(file, 0, SEEK_SET) != 0) {
        fclose(file);
        return LT48_FILE_ERROR;
    }
    data = malloc(expected_size == 0u ? 1u : expected_size);
    if (data == NULL) {
        fclose(file);
        return LT48_NO_MEMORY;
    }
    if (fread(data, 1u, expected_size, file) != expected_size ||
        fclose(file) != 0) {
        free(data);
        return LT48_FILE_ERROR;
    }
    *out_data = data;
    return LT48_OK;
}

static lt48_status write_m37_planned_report(
    const char *report_path, const lt48_installation *installation,
    size_t selected_device, const lt48_m37_resources *resources,
    const lt48_protocol_plan *protocol_plan, lt48_m37_operation operation,
    uint32_t address_start, uint32_t address_length,
    const unsigned char *input_image_sha256
) {
    if (report_path == NULL) return LT48_OK;
    return lt48_m37_operation_report_write_planned(
        report_path, installation, selected_device, resources, protocol_plan,
        operation, address_start, address_length, input_image_sha256
    );
}

static int ascii_contains(const unsigned char *text, size_t text_length,
                          const char *query) {
    size_t query_length = strlen(query);
    if (query_length == 0) {
        return 1;
    }
    if (query_length > text_length) {
        return 0;
    }
    for (size_t offset = 0; offset <= text_length - query_length; ++offset) {
        size_t index;
        for (index = 0; index < query_length; ++index) {
            unsigned char left = text[offset + index];
            unsigned char right = (unsigned char)query[index];
            if (left < 0x80) {
                left = (unsigned char)tolower(left);
            }
            if (right < 0x80) {
                right = (unsigned char)tolower(right);
            }
            if (left != right) {
                break;
            }
        }
        if (index == query_length) {
            return 1;
        }
    }
    return 0;
}

static int catalogue_key_equals(const lt48_catalogue_entry *entry,
                                const char *selector) {
    size_t vendor_length = strlen(entry->vendor);
    size_t selector_length = strlen(selector);
    size_t key_length = vendor_length + 1 + entry->device_name_length;
    if (selector_length != key_length) {
        return 0;
    }
    for (size_t index = 0; index < key_length; ++index) {
        unsigned char left;
        unsigned char right = (unsigned char)selector[index];
        if (index < vendor_length) {
            left = (unsigned char)entry->vendor[index];
        } else if (index == vendor_length) {
            left = ':';
        } else {
            left = entry->device_name[index - vendor_length - 1];
        }
        if (left < 0x80) {
            left = (unsigned char)tolower(left);
        }
        if (right < 0x80) {
            right = (unsigned char)tolower(right);
        }
        if (left != right) {
            return 0;
        }
    }
    return 1;
}

static int list_programmers(int json) {
    lt48_context *context = NULL;
    lt48_programmer_info *items = NULL;
    lt48_status status;
    size_t count = 0;

    status = lt48_context_create(&context);
    if (status != LT48_OK) {
        fprintf(stderr, "labtool48: %s\n", lt48_status_string(status));
        return 6;
    }
    status = lt48_programmer_list(context, NULL, 0, &count);
    if (status != LT48_OK && status != LT48_BUFFER_TOO_SMALL) {
        fprintf(stderr, "labtool48: %s\n", lt48_status_string(status));
        lt48_context_destroy(context);
        return 6;
    }
    if (count != 0) {
        items = calloc(count, sizeof(*items));
        if (items == NULL) {
            fprintf(stderr, "labtool48: out of memory\n");
            lt48_context_destroy(context);
            return 6;
        }
        status = lt48_programmer_list(context, items, count, &count);
        if (status != LT48_OK) {
            fprintf(stderr, "labtool48: %s\n", lt48_status_string(status));
            free(items);
            lt48_context_destroy(context);
            return 6;
        }
    }

    if (json) {
        fputs("{\"schema\":1,\"programmers\":[", stdout);
    }
    for (size_t index = 0; index < count; ++index) {
        const lt48_programmer_info *item = &items[index];
        if (json) {
            printf("%s{\"bus\":%u,\"address\":%u,\"vendor_id\":%u,"
                   "\"product_id\":%u,\"usb_version_bcd\":%u}",
                   index == 0 ? "" : ",",
                   item->bus_number, item->device_address,
                   item->vendor_id, item->product_id, item->usb_version_bcd);
        } else {
            printf("%03u:%03u %04x:%04x USB %x.%02x\n",
                   item->bus_number, item->device_address,
                   item->vendor_id, item->product_id,
                   item->usb_version_bcd >> 8, item->usb_version_bcd & 0xff);
        }
    }
    if (json) {
        fputs("]}\n", stdout);
    } else if (count == 0) {
        puts("No supported LabTool programmer found.");
    }

    free(items);
    lt48_context_destroy(context);
    return 0;
}

static int programmer_info(int argc, char **argv) {
    lt48_context *context = NULL;
    lt48_programmer_info *items = NULL;
    lt48_programmer_info selected;
    lt48_status status;
    size_t count = 0u;
    uint8_t bus = 0u;
    uint8_t address = 0u;
    int have_bus = 0;
    int have_address = 0;
    int json = 0;
    int result = 6;

    for (int index = 3; index < argc; ++index) {
        if (strcmp(argv[index], "--json") == 0 && !json) {
            json = 1;
        } else if (strcmp(argv[index], "--bus") == 0 &&
                   index + 1 < argc && !have_bus) {
            have_bus = parse_u8(argv[++index], &bus);
            if (!have_bus) return 2;
        } else if (strcmp(argv[index], "--address") == 0 &&
                   index + 1 < argc && !have_address) {
            have_address = parse_u8(argv[++index], &address);
            if (!have_address) return 2;
        } else {
            return 2;
        }
    }
    if (have_bus != have_address) return 2;

    status = lt48_context_create(&context);
    if (status != LT48_OK) {
        fprintf(stderr, "labtool48: %s\n", lt48_status_string(status));
        return 6;
    }
    status = lt48_programmer_list(context, NULL, 0u, &count);
    if (status != LT48_OK && status != LT48_BUFFER_TOO_SMALL) goto done;
    if (count != 0u) {
        items = calloc(count, sizeof(*items));
        if (items == NULL) {
            status = LT48_NO_MEMORY;
            goto done;
        }
        status = lt48_programmer_list(context, items, count, &count);
        if (status != LT48_OK) goto done;
    }
    status = lt48_programmer_info_select(
        items, count, have_bus, bus, address, &selected
    );
    if (status != LT48_OK) {
        if (status == LT48_BUSY) {
            fputs("labtool48: multiple programmers found; select --bus and --address\n",
                  stderr);
        } else if (status == LT48_NOT_FOUND) {
            fputs("labtool48: programmer not found\n", stderr);
        } else {
            fprintf(stderr, "labtool48: %s\n", lt48_status_string(status));
        }
        result = (status == LT48_BUSY || status == LT48_NOT_FOUND) ? 7 : 6;
        goto cleanup;
    }
    if (json) {
        printf("{\"schema\":1,\"bus\":%u,\"address\":%u,"
               "\"vendor_id\":%u,\"product_id\":%u,"
               "\"usb_version_bcd\":%u,\"descriptor_role\":",
               selected.bus_number, selected.device_address,
               selected.vendor_id, selected.product_id,
               selected.usb_version_bcd);
        print_json_string(lt48_programmer_descriptor_role(selected.product_id));
        fputs(",\"usb_opened\":false,\"interface_claimed\":false,"
              "\"endpoint_readiness_proven\":false}\n", stdout);
    } else {
        printf("%03u:%03u %04x:%04x USB %x.%02x\n"
               "Descriptor role: %s\n"
               "USB opened: no; interface claimed: no; endpoint readiness proven: no\n",
               selected.bus_number, selected.device_address,
               selected.vendor_id, selected.product_id,
               selected.usb_version_bcd >> 8,
               selected.usb_version_bcd & 0xff,
               lt48_programmer_descriptor_role(selected.product_id));
    }
    result = 0;
    goto cleanup;

done:
    fprintf(stderr, "labtool48: %s\n", lt48_status_string(status));
cleanup:
    free(items);
    lt48_context_destroy(context);
    return result;
}

static int open_programmer(int argc, char **argv, int initialize) {
    lt48_context *context = NULL;
    lt48_programmer *programmer = NULL;
    lt48_programmer_info item;
    lt48_status status;
    size_t count = 0;
    uint8_t bus = 0;
    uint8_t address = 0;
    int have_bus = 0;
    int have_address = 0;
    int json = 0;
    lt48_transport_info transport_info = {0};
    const char *fx2_path = getenv("LABTOOL48_FX2_FIRMWARE");
    char default_fx2[4096];
    int indicator = -1;
    if (!fx2_path || !*fx2_path) {
        const char *config = getenv("XDG_CONFIG_HOME"), *home = getenv("HOME");
        int n = config && *config ? snprintf(default_fx2, sizeof(default_fx2), "%s/labtool48uxp/fx2-firmware.hex", config) :
            home && *home ? snprintf(default_fx2, sizeof(default_fx2), "%s/.config/labtool48uxp/fx2-firmware.hex", home) : -1;
        fx2_path = n > 0 && (size_t)n < sizeof(default_fx2) ? default_fx2 : NULL;
    }

    for (int index = 3; index < argc; ++index) {
        if (strcmp(argv[index], "--json") == 0) {
            json = 1;
        } else if (strcmp(argv[index], "--bus") == 0 && index + 1 < argc) {
            have_bus = parse_u8(argv[++index], &bus);
            if (!have_bus) {
                return 2;
            }
        } else if (strcmp(argv[index], "--address") == 0 && index + 1 < argc) {
            have_address = parse_u8(argv[++index], &address);
            if (!have_address) {
                return 2;
            }
        } else if (initialize && strcmp(argv[index], "--fx2") == 0 && index + 1 < argc) {
            fx2_path = argv[++index];
        } else if (initialize == 2 && strcmp(argv[index], "--state") == 0 && index + 1 < argc) {
            const char *value = argv[++index];
            indicator = !strcmp(value,"off") ? 0 : !strcmp(value,"busy") ? 1 : !strcmp(value,"error") ? 2 : !strcmp(value,"good") ? 4 : -1;
            if (indicator < 0) return 2;
        } else {
            return 2;
        }
    }
    if (initialize == 2 && indicator < 0) return 2;
    if (have_bus != have_address) {
        return 2;
    }
    status = lt48_context_create(&context);
    if (status != LT48_OK) {
        fprintf(stderr, "labtool48: %s\n", lt48_status_string(status));
        return 6;
    }
    if (!have_bus) {
        status = lt48_programmer_list(context, &item, 1, &count);
        if (status == LT48_BUFFER_TOO_SMALL || count > 1) {
            fprintf(stderr, "labtool48: multiple programmers found; select --bus and --address\n");
            lt48_context_destroy(context);
            return 2;
        }
        if (status != LT48_OK || count == 0) {
            fprintf(stderr, "labtool48: programmer not found\n");
            lt48_context_destroy(context);
            return 6;
        }
        bus = item.bus_number;
        address = item.device_address;
    }
    status = lt48_programmer_open(context, bus, address, &programmer);
    if (status != LT48_OK) {
        fprintf(stderr, "labtool48: %s\n", lt48_status_string(status));
        lt48_context_destroy(context);
        return 6;
    }
    if (initialize) {
        status = lt48_programmer_transport_initialize(
            programmer, 2000, &transport_info
        );
        if (status != LT48_OK) {
            lt48_programmer_close(programmer); programmer = NULL;
            lt48_fx2_firmware *firmware = NULL; lt48_fx2_load_preparation preparation;
            lt48_fx2_load_result load;
            lt48_status loaded = fx2_path ? lt48_fx2_firmware_open_hex(fx2_path, &firmware) : LT48_FILE_ERROR;
            if (loaded == LT48_OK) loaded = lt48_fx2_prepare_load(firmware, bus, address, &preparation);
            if (loaded == LT48_OK) loaded = lt48_programmer_load_fx2(context, bus, address, firmware, LT48_FX2_LOAD_HARDWARE_AUTHORIZATION, &load);
            lt48_fx2_firmware_close(firmware); lt48_context_destroy(context);
            if (loaded == LT48_OK) {
                if (json) puts("{\"schema\":1,\"firmware_loaded\":true,\"transport_initialized\":false,\"reenumeration_required\":true}");
                else puts("Volatile FX2 firmware loaded. Re-run programmer list and initialize using its re-enumerated USB address; the requested operation has not run yet.");
                return 0;
            }
            fprintf(stderr, "labtool48: transport initialization failed: %s; FX2 load: %s. Use --fx2 OWNER.hex or import a verified image in the GUI. Unknown images require an audit; use load-fx2 without --execute to inspect one.\n", lt48_status_string(status), lt48_status_string(loaded));
            return 6;
        }
    }
    if (indicator >= 0) {
        status = lt48_programmer_set_indicators(programmer, (unsigned)indicator, 2000);
        if (status != LT48_OK) {
            fprintf(stderr,"labtool48: LED command failed: %s\n",lt48_status_string(status));
            lt48_programmer_close(programmer);lt48_context_destroy(context);return 6;
        }
        if (!json) printf("Front-panel indicator mask set to %d (socket pins unchanged).\n",indicator);
    }
    if (json) {
        printf("{\"schema\":1,\"bus\":%u,\"address\":%u,"
               "\"claimed_interface\":0,\"alternate_setting\":1,"
               "\"transport_initialized\":%s,\"packet_size_ack\":%u,"
               "\"mode_ack\":%u}\n", bus, address,
               initialize ? "true" : "false",
               transport_info.packet_size_ack, transport_info.mode_ack);
    } else {
        printf("%03u:%03u opened, interface 0 claimed, alternate setting 1",
               bus, address);
        if (initialize) {
            printf(", transport initialized (acks %02x/%02x)\n",
                   transport_info.packet_size_ack, transport_info.mode_ack);
        } else {
            puts("; no payload sent");
        }
    }
    lt48_programmer_close(programmer);
    lt48_context_destroy(context);
    return 0;
}

static int programmer_load_fx2(int argc, char **argv) {
    const char *path = argv[3];
    const char *confirmation = NULL;
    uint8_t bus = 0u;
    uint8_t address = 0u;
    int have_bus = 0;
    int have_address = 0;
    int execute = 0;
    int json = 0;
    lt48_fx2_firmware *firmware = NULL;
    lt48_fx2_firmware_info info;
    lt48_fx2_load_preparation preparation;
    lt48_fx2_load_result load = {0};
    lt48_context *context = NULL;
    lt48_programmer_info devices[8];
    size_t device_count = 0u;
    int found_cold = 0;
    lt48_status status;
    int result = 2;

    for (int index = 4; index < argc; ++index) {
        if (strcmp(argv[index], "--json") == 0 && !json) {
            json = 1;
        } else if (strcmp(argv[index], "--execute") == 0 && !execute) {
            execute = 1;
        } else if (strcmp(argv[index], "--bus") == 0 &&
                   index + 1 < argc && !have_bus) {
            have_bus = parse_u8(argv[++index], &bus);
            if (!have_bus) return 2;
        } else if (strcmp(argv[index], "--address") == 0 &&
                   index + 1 < argc && !have_address) {
            have_address = parse_u8(argv[++index], &address);
            if (!have_address) return 2;
        } else if (strcmp(argv[index], "--confirm") == 0 &&
                   index + 1 < argc && confirmation == NULL) {
            confirmation = argv[++index];
        } else {
            return 2;
        }
    }
    if (!have_bus || !have_address || (!execute && confirmation != NULL) ||
        (execute && confirmation == NULL)) return 2;

    status = lt48_fx2_firmware_open_hex(path, &firmware);
    if (status != LT48_OK) goto fail;
    status = lt48_fx2_firmware_info_get(firmware, &info);
    if (status != LT48_OK) goto fail;
    status = lt48_fx2_prepare_load(firmware, bus, address, &preparation);
    if (status != LT48_OK) {
        fprintf(stderr,
                "labtool48: unrecognized FX2 image: records=%zu payload=%zu "
                "image_size=%zu image_sha256=",
                info.record_count, info.payload_size, info.image_size);
        fprint_hex(stderr, info.image_sha256, LT48_SHA256_SIZE);
        fputc('\n', stderr);
        goto fail;
    }
    status = lt48_context_create(&context);
    if (status != LT48_OK) goto fail;
    status = lt48_programmer_list(
        context, devices, sizeof(devices) / sizeof(devices[0]), &device_count
    );
    if (status != LT48_OK) goto fail;
    for (size_t index = 0u; index < device_count; ++index) {
        if (devices[index].bus_number == bus &&
            devices[index].device_address == address &&
            devices[index].vendor_id == LT48_USB_VENDOR_ID &&
            devices[index].product_id == LT48_USB_PRODUCT_ID_COLD) {
            found_cold = 1;
        }
    }
    if (!found_cold) {
        status = LT48_NOT_FOUND;
        goto fail;
    }
    if (!execute) {
        if (json) {
            printf("{\"schema\":1,\"mode\":\"dry-run\",\"bus\":%u,"
                   "\"address\":%u,\"firmware\":", bus, address);
            print_json_string(path);
            printf(",\"records\":%zu,\"payload_bytes\":%zu,"
                   "\"image_size\":%zu,\"verified_owner_image\":true,"
                   "\"file_sha256\":\"", info.record_count,
                   info.payload_size, info.image_size);
            print_hex(info.file_sha256, LT48_SHA256_SIZE);
            fputs("\",\"image_sha256\":\"", stdout);
            print_hex(info.image_sha256, LT48_SHA256_SIZE);
            fputs("\",\"confirmation_sha256\":\"", stdout);
            print_hex(preparation.confirmation_sha256, LT48_SHA256_SIZE);
            fputs("\",\"usb_opened\":false}\n", stdout);
        } else {
            printf("DRY RUN: load volatile FX2 firmware into %03u:%03u\n",
                   bus, address);
            printf("%zu records, %zu payload bytes, %zu-byte sparse image\n",
                   info.record_count, info.payload_size, info.image_size);
            fputs("file SHA-256:   ", stdout);
            print_hex(info.file_sha256, LT48_SHA256_SIZE);
            fputs("\nimage SHA-256:  ", stdout);
            print_hex(info.image_sha256, LT48_SHA256_SIZE);
            fputs("\nconfirmation:   ", stdout);
            print_hex(preparation.confirmation_sha256, LT48_SHA256_SIZE);
            puts("\nNo USB device opened. To execute, repeat this exact command"
                 " with --execute --confirm <confirmation>.");
        }
        result = 0;
        goto cleanup;
    }
    if (!digest_text_equals(confirmation, preparation.confirmation_sha256)) {
        fprintf(stderr,
                "labtool48: confirmation does not match this exact FX2 load\n");
        status = LT48_SAFETY_REJECTED;
        result = 4;
        goto cleanup;
    }
    status = lt48_programmer_load_fx2(
        context, bus, address, firmware,
        LT48_FX2_LOAD_HARDWARE_AUTHORIZATION, &load
    );
    if (status != LT48_OK) {
        fprintf(stderr,
                "labtool48: FX2 load failed: %s; halted=%s records=%zu "
                "bytes=%zu released=%s\n",
                lt48_status_string(status), load.cpu_halted ? "yes" : "no",
                load.records_written, load.bytes_written,
                load.cpu_released ? "yes" : "no");
        if (load.cpu_halted && !load.cpu_released) {
            fputs("labtool48: FX2 CPU remains halted; power-cycle the programmer "
                  "before retrying\n", stderr);
        }
        result = 6;
        goto cleanup;
    }
    if (json) {
        printf("{\"schema\":1,\"mode\":\"executed\",\"bus\":%u,"
               "\"address\":%u,\"records_written\":%zu,"
               "\"bytes_written\":%zu,\"cpu_released\":true}\n",
               bus, address, load.records_written, load.bytes_written);
    } else {
        printf("Loaded %zu FX2 records (%zu bytes) into %03u:%03u and "
               "released the CPU.\n",
               load.records_written, load.bytes_written, bus, address);
    }
    result = 0;
    goto cleanup;

fail:
    fprintf(stderr, "labtool48: cannot prepare FX2 load: %s\n",
            lt48_status_string(status));
    result = status == LT48_SAFETY_REJECTED ? 4 : 2;

cleanup:
    lt48_context_destroy(context);
    lt48_fx2_firmware_close(firmware);
    return result;
}

static int inspect_resources(const char *path, int json) {
    lt48_rez *rez = NULL;
    lt48_rez_info info;
    lt48_status status = lt48_rez_open(path, &rez);
    if (status != LT48_OK) {
        fprintf(stderr, "labtool48: %s: %s\n", path, lt48_status_string(status));
        return 2;
    }
    status = lt48_rez_get_info(rez, &info);
    if (status != LT48_OK) {
        lt48_rez_close(rez);
        return 2;
    }
    if (json) {
        printf("{\"schema\":1,\"actual_size\":%zu,\"declared_size\":%u,"
               "\"table_offset\":%u,\"resource_count\":%zu}\n",
               info.actual_size, info.declared_size, info.table_offset,
               info.resource_count);
    } else {
        printf("FBPR resources: %zu; size: %zu; declared: %u; table: 0x%x\n",
               info.resource_count, info.actual_size, info.declared_size,
               info.table_offset);
    }
    lt48_rez_close(rez);
    return 0;
}

static int list_device_definitions(const char *path, int json) {
    lt48_rez *rez = NULL;
    lt48_rez_info info;
    lt48_status status = lt48_rez_open(path, &rez);
    size_t emitted = 0;
    if (status != LT48_OK) {
        fprintf(stderr, "labtool48: %s: %s\n", path, lt48_status_string(status));
        return 2;
    }
    status = lt48_rez_get_info(rez, &info);
    if (status != LT48_OK) {
        lt48_rez_close(rez);
        return 2;
    }
    if (json) {
        fputs("{\"schema\":1,\"definitions\":[", stdout);
    }
    for (size_t index = 0; index < info.resource_count; ++index) {
        lt48_device_definition definition;
        status = lt48_rez_get_device_definition(rez, index, &definition);
        if (status == LT48_NOT_FOUND) {
            continue;
        }
        if (status != LT48_OK) {
            fprintf(stderr, "labtool48: malformed device definition at resource %zu\n",
                    index);
            lt48_rez_close(rez);
            return 2;
        }
        if (json) {
            printf("%s{\"resource\":", emitted == 0 ? "" : ",");
            print_json_string(definition.resource_name);
            fputs(",\"type\":", stdout);
            print_json_string(definition.type_name);
            printf(",\"family_code\":%u,\"pinmap\":%u,\"value2\":%u,"
                   "\"value3\":%u,", definition.family_code,
                   definition.pinmap, definition.value2, definition.value3);
            if (definition.has_device_size) {
                printf("\"device_size\":%u,", definition.device_size);
            }
            if (definition.has_element_geometry) {
                printf("\"element_count\":%u,\"element_bits\":%u,",
                       definition.element_count, definition.element_bits);
            }
            if (definition.has_secondary_element_geometry) {
                printf("\"secondary_element_count\":%u,"
                       "\"secondary_element_bits\":%u,",
                       definition.secondary_element_count,
                       definition.secondary_element_bits);
            }
            printf("\"trailing_data_size\":%zu}", definition.trailing_data_size);
        } else {
            printf("%-8s %-16s family=%u pinmap=%u values=%u/%u",
                   definition.resource_name, definition.type_name,
                   definition.family_code, definition.pinmap,
                   definition.value2, definition.value3);
            if (definition.has_device_size) {
                printf(" size=%u", definition.device_size);
            }
            printf(" trailing=%zu\n", definition.trailing_data_size);
        }
        ++emitted;
    }
    if (json) {
        printf("],\"definition_count\":%zu}\n", emitted);
    } else {
        printf("%zu device definitions\n", emitted);
    }
    lt48_rez_close(rez);
    return 0;
}

static int catalogue_search(const char *installation, const char *query, int json) {
    lt48_installation *model = NULL;
    size_t emitted = 0;
    lt48_status status = lt48_installation_open(installation, &model);
    if (status != LT48_OK) {
        fprintf(stderr, "labtool48: %s: %s\n", installation,
                lt48_status_string(status));
        return 2;
    }
    if (json) {
        fputs("{\"schema\":1,\"devices\":[", stdout);
    }
    for (size_t device_index = 0;
         device_index < lt48_installation_device_count(model); ++device_index) {
            lt48_catalogue_entry entry;
            const char *stem;
            size_t vendor_length;
            size_t key_length;
            unsigned char *key;
            status = lt48_installation_get_device(model, device_index, &entry);
            if (status != LT48_OK) {
                fprintf(stderr, "labtool48: malformed catalogue entry\n");
                lt48_installation_close(model);
                return 2;
            }
            vendor_length = strlen(entry.vendor);
            key_length = vendor_length + 1 + entry.device_name_length;
            key = malloc(key_length == 0 ? 1 : key_length);
            if (key == NULL) {
                fprintf(stderr, "labtool48: out of memory\n");
                lt48_installation_close(model);
                return 6;
            }
            memcpy(key, entry.vendor, vendor_length);
            key[vendor_length] = ':';
            if (entry.device_name_length != 0) {
                memcpy(key + vendor_length + 1, entry.device_name,
                       entry.device_name_length);
            }
            if (!ascii_contains(key, key_length, query)) {
                free(key);
                continue;
            }
            free(key);
            stem = lt48_definition_stem_for_class_format(
                entry.device_class, entry.numeric_column_count
            );
            if (json) {
                printf("%s{\"vendor\":", emitted == 0 ? "" : ",");
                print_json_string(entry.vendor);
                fputs(",\"name\":", stdout);
                if (entry.device_name != NULL) {
                    print_json_bytes(entry.device_name, entry.device_name_length);
                } else {
                    fputs("null", stdout);
                }
                printf(",\"table_index\":%u,\"device_class\":%u,"
                       "\"device_type\":%u,\"manufacturer_id\":%u,"
                       "\"device_id\":%u,\"definition_file\":",
                       entry.table_index, entry.device_class, entry.device_type,
                       entry.manufacturer_id, entry.device_id);
                if (stem != NULL) {
                    printf("\"%s.rez\"", stem);
                } else {
                    fputs("null", stdout);
                }
                printf(",\"definition_resource\":\"D%x\"}", entry.device_type);
            } else {
                printf("%s:%.*s class=%u type=%u -> ", entry.vendor,
                       (int)entry.device_name_length,
                       entry.device_name == NULL ? "" :
                           (const char *)entry.device_name,
                       entry.device_class, entry.device_type);
                if (stem != NULL) {
                    printf("%s.rez:D%x\n", stem, entry.device_type);
                } else {
                    printf("unsupported-class:D%x\n", entry.device_type);
                }
            }
            ++emitted;
    }
    if (json) {
        printf("],\"match_count\":%zu}\n", emitted);
    } else {
        printf("%zu matches\n", emitted);
    }
    lt48_installation_close(model);
    return 0;
}

static int catalogue_verify_installation(const char *path, int json) {
    lt48_installation *installation = NULL;
    lt48_installation_audit audit;
    lt48_status status = lt48_installation_open(path, &installation);
    char digest[LT48_SHA256_SIZE * 2u + 1u];
    if (status != LT48_OK) {
        fprintf(stderr, "labtool48: %s: %s\n", path, lt48_status_string(status));
        return 2;
    }
    status = lt48_installation_audit_all(installation, &audit);
    hex_string(audit.manifest_sha256, LT48_SHA256_SIZE, digest);
    if (json) {
        printf("{\"schema\":1,\"operation\":\"verify-installation\","
               "\"path\":");
        print_json_string(path);
        printf(",\"valid\":%s,\"usb_opened\":false,"
               "\"catalogue_device_count\":%zu,"
               "\"supported_device_count\":%zu,"
               "\"unsupported_class_count\":%zu,"
               "\"definition_file_count\":%zu,"
               "\"resolved_device_count\":%zu,"
               "\"failed_device_count\":%zu,"
               "\"zero_pin_device_count\":%zu,"
               "\"first_failed_device_index\":",
               status == LT48_OK ? "true" : "false",
               audit.catalogue_device_count, audit.supported_device_count,
               audit.unsupported_class_count, audit.definition_file_count,
               audit.resolved_device_count, audit.failed_device_count,
               audit.zero_pin_device_count);
        if (audit.first_failed_device_index == SIZE_MAX) fputs("null", stdout);
        else printf("%zu", audit.first_failed_device_index);
        fputs(",\"first_failure\":", stdout);
        if (audit.first_failed_device_index == SIZE_MAX) fputs("null", stdout);
        else print_json_string(lt48_status_string(audit.first_failure));
        fputs(",\"manifest_sha256\":", stdout);
        print_json_string(digest);
        puts("}");
    } else {
        printf("installation %s: %s\n", path,
               status == LT48_OK ? "valid" : "INVALID");
        printf("  %zu catalogue devices; %zu resolved; %zu failed; "
               "%zu unsupported classes\n",
               audit.catalogue_device_count, audit.resolved_device_count,
               audit.failed_device_count, audit.unsupported_class_count);
        printf("  %zu definition files; %zu zero-pin catalogue devices\n",
               audit.definition_file_count, audit.zero_pin_device_count);
        printf("  manifest SHA-256: %s\n", digest);
        if (status != LT48_OK)
            fprintf(stderr, "labtool48: first failure at device %zu: %s\n",
                    audit.first_failed_device_index,
                    lt48_status_string(audit.first_failure));
    }
    lt48_installation_close(installation);
    return status == LT48_OK ? 0 : 5;
}

static lt48_status find_defined_function(
    const lt48_rez *rez,
    size_t names_index,
    uint16_t identifier,
    lt48_defined_name *out_name
) {
    lt48_defined_name_collection_info info;
    lt48_defined_name match = {0};
    int found = 0;
    lt48_status status = lt48_rez_get_defined_name_collection_info(
        rez, names_index, &info
    );
    if (status != LT48_OK) return status;
    for (size_t index = 0u; index < info.count; ++index) {
        lt48_defined_name candidate;
        status = lt48_rez_get_defined_name(
            rez, names_index, index, &candidate
        );
        if (status != LT48_OK) return status;
        if (candidate.identifier != identifier || candidate.name_length < 5u ||
            memcmp(candidate.name, "_FUN_", 5u) != 0) continue;
        if (found) return LT48_FORMAT_ERROR;
        match = candidate;
        found = 1;
    }
    if (!found) return LT48_NOT_FOUND;
    *out_name = match;
    return LT48_OK;
}

static lt48_status find_defined_name_exact(
    const lt48_rez *rez, size_t names_index, const char *name,
    lt48_defined_name *out_name
) {
    lt48_defined_name_collection_info info;
    size_t name_length;
    int found = 0;
    lt48_status status;
    if (rez == NULL || name == NULL || out_name == NULL)
        return LT48_INVALID_ARGUMENT;
    name_length = strlen(name);
    status = lt48_rez_get_defined_name_collection_info(
        rez, names_index, &info
    );
    if (status != LT48_OK) return status;
    for (size_t index = 0u; index < info.count; ++index) {
        lt48_defined_name candidate;
        status = lt48_rez_get_defined_name(
            rez, names_index, index, &candidate
        );
        if (status != LT48_OK) return status;
        if (candidate.name_length != name_length ||
            memcmp(candidate.name, name, name_length) != 0) continue;
        if (found) return LT48_FORMAT_ERROR;
        *out_name = candidate;
        found = 1;
    }
    return found ? LT48_OK : LT48_NOT_FOUND;
}

static int resolve_definition(const char *path, const char *resource_name, int json) {
    lt48_rez *rez = NULL;
    lt48_device_definition definition;
    lt48_family_info family;
    lt48_pin_code pin;
    lt48_status status;
    size_t definition_index;
    size_t family_index;
    size_t pin_index;
    size_t var_names_index = 0;
    size_t defined_names_index = 0;
    int have_var_names;
    int have_defined_names;
    char name[32];
    uint32_t capacity = 0;

    status = lt48_rez_open(path, &rez);
    if (status == LT48_OK) {
        status = lt48_rez_find_resource(rez, resource_name, &definition_index);
    }
    if (status == LT48_OK) {
        status = lt48_rez_get_device_definition(rez, definition_index, &definition);
    }
    if (status != LT48_OK) {
        fprintf(stderr, "labtool48: cannot resolve %s:%s: %s\n", path,
                resource_name, lt48_status_string(status));
        lt48_rez_close(rez);
        return 2;
    }
    snprintf(name, sizeof(name), "M%x", definition.family_code);
    status = lt48_rez_find_resource(rez, name, &family_index);
    if (status == LT48_OK) {
        status = lt48_rez_get_family_info(rez, family_index, &family);
    }
    if (status != LT48_OK) {
        fprintf(stderr, "labtool48: missing or malformed family %s\n", name);
        lt48_rez_close(rez);
        return 2;
    }
    snprintf(name, sizeof(name), "P%x", definition.pinmap);
    status = lt48_rez_find_resource(rez, name, &pin_index);
    if (status == LT48_OK) {
        status = lt48_rez_get_pin_code(rez, pin_index, &pin);
    }
    if (status != LT48_OK) {
        fprintf(stderr, "labtool48: missing or malformed pin map %s\n", name);
        lt48_rez_close(rez);
        return 2;
    }
    have_var_names = lt48_rez_find_resource(rez, "VarName", &var_names_index) == LT48_OK;
    have_defined_names = lt48_rez_find_resource(
        rez, "DefineName", &defined_names_index
    ) == LT48_OK;
    if (definition.has_device_size) {
        capacity = definition.device_size;
    } else if (definition.has_element_geometry &&
               definition.element_bits > 0u &&
               definition.element_count <=
                   UINT32_MAX / ((definition.element_bits + 7u) / 8u)) {
        capacity = definition.element_count *
                   ((definition.element_bits + 7u) / 8u);
    } else if (definition.kind == LT48_DEVICE_DEFINITION_MEMORY &&
               definition.trailing_data_size >= 2) {
        capacity = ((uint32_t)definition.trailing_data[0] |
                    (uint32_t)definition.trailing_data[1] << 8) * 256;
    }

    if (json) {
        fputs("{\"schema\":1,\"resource\":", stdout);
        print_json_string(definition.resource_name);
        fputs(",\"type\":", stdout);
        print_json_string(definition.type_name);
        printf(",\"family_code\":%u,\"pinmap\":%u,\"pin_count\":%u,"
               "\"capacity_bytes\":%u,\"operations\":[",
               definition.family_code, definition.pinmap, pin.pin_count, capacity);
    } else {
        printf("%s %s: family M%x, pin map P%x (%u pins), capacity %u bytes\n",
               definition.resource_name, definition.type_name,
               definition.family_code, definition.pinmap, pin.pin_count, capacity);
        puts("Operations:");
    }
    for (size_t operation_index = 0; operation_index < family.operation_count;
         ++operation_index) {
        lt48_family_operation operation;
        lt48_wave_code wave;
        size_t wave_index;
        int have_wave;
        lt48_defined_name operation_name;
        int have_operation_name;
        status = lt48_rez_get_family_operation(
            rez, family_index, operation_index, &operation
        );
        if (status != LT48_OK) {
            lt48_rez_close(rez);
            return 2;
        }
        snprintf(name, sizeof(name), "W%x", operation.wave);
        have_wave = lt48_rez_find_resource(rez, name, &wave_index) == LT48_OK &&
                    lt48_rez_get_wave_code(rez, wave_index, &wave) == LT48_OK;
        have_operation_name = have_defined_names &&
            find_defined_function(
                rez, defined_names_index, operation.operation_id,
                &operation_name
            ) == LT48_OK;
        if (json) {
            printf("%s{\"operation_id\":%u,\"symbol\":",
                   operation_index == 0 ? "" : ",", operation.operation_id);
            if (have_operation_name) {
                print_json_bytes(operation_name.name, operation_name.name_length);
            } else {
                fputs("null", stdout);
            }
            printf(",\"wave\":%u,\"wave_type\":", operation.wave);
            if (have_wave) {
                printf("%u,\"stored_data_size\":%zu,\"load_address\":",
                       wave.wave_type, wave.data_size);
                if (wave.has_applet_header) {
                    printf("%u", wave.load_address);
                } else {
                    fputs("null", stdout);
                }
            } else {
                fputs("null,\"stored_data_size\":null,\"load_address\":null", stdout);
            }
            putchar('}');
        } else if (have_wave) {
            printf("  id=%u", operation.operation_id);
            if (have_operation_name) {
                printf(" %.*s", (int)operation_name.name_length,
                       (const char *)operation_name.name);
            }
            printf(" W%x type=%u stored-data=%zu%s",
                   operation.wave, wave.wave_type,
                   wave.data_size, wave.has_applet_header ? " applet" : "");
            if (wave.has_applet_header) {
                printf(" load=0x%x", wave.load_address);
            }
            putchar('\n');
        } else {
            printf("  id=%u W%x MISSING\n", operation.operation_id, operation.wave);
        }
    }
    if (json) {
        fputs("],\"variables\":[", stdout);
    } else {
        puts("Variables:");
    }
    for (size_t variable_index = 0; variable_index < family.variable_count;
         ++variable_index) {
        lt48_family_variable variable;
        const unsigned char *variable_name = NULL;
        size_t variable_name_length = 0;
        status = lt48_rez_get_family_variable(
            rez, family_index, variable_index, &variable
        );
        if (status != LT48_OK) {
            lt48_rez_close(rez);
            return 2;
        }
        if (have_var_names) {
            (void)lt48_rez_get_variable_name(
                rez, var_names_index, variable.values[1], &variable_name,
                &variable_name_length, NULL
            );
        }
        if (json) {
            printf("%s{\"values\":[%u,%u,%u,%u,%u],\"name\":",
                   variable_index == 0 ? "" : ",", variable.values[0],
                   variable.values[1], variable.values[2], variable.values[3],
                   variable.values[4]);
            if (variable_name != NULL) {
                print_json_bytes(variable_name, variable_name_length);
            } else {
                fputs("null", stdout);
            }
            putchar('}');
        } else {
            printf("  [%u,%u,%u,%u,%u] ", variable.values[0], variable.values[1],
                   variable.values[2], variable.values[3], variable.values[4]);
            if (variable_name != NULL) {
                printf("%.*s", (int)variable_name_length,
                       (const char *)variable_name);
            }
            putchar('\n');
        }
    }
    if (json) {
        printf("],\"configuration_set\":%u,\"family_header_value\":%u}\n",
               family.configuration_set, family.header_value);
    }
    lt48_rez_close(rez);
    return 0;
}

static int catalogue_show(const char *path, const char *selector, int json) {
    lt48_installation *installation = NULL;
    lt48_catalogue_entry entry;
    lt48_device_spec spec;
    lt48_device_guidance guidance;
    lt48_device_compile_evidence compile_evidence;
    lt48_device_operation_binding *operation_bindings = NULL;
    size_t binding_count = 0u;
    lt48_status status = lt48_installation_open(path, &installation);
    size_t match_index = 0;
    size_t matches = 0;
    if (status != LT48_OK) {
        fprintf(stderr, "labtool48: %s: %s\n", path, lt48_status_string(status));
        return 2;
    }
    for (size_t index = 0; index < lt48_installation_device_count(installation);
         ++index) {
        status = lt48_installation_get_device(installation, index, &entry);
        if (status != LT48_OK) {
            lt48_installation_close(installation);
            return 2;
        }
        if (catalogue_key_equals(&entry, selector)) {
            match_index = index;
            ++matches;
        }
    }
    if (matches != 1) {
        fprintf(stderr, "labtool48: exact selector matched %zu devices\n", matches);
        lt48_installation_close(installation);
        return 2;
    }
    status = lt48_installation_resolve_device(installation, match_index, &spec);
    if (status != LT48_OK) {
        fprintf(stderr, "labtool48: cannot resolve %s: %s\n", selector,
                lt48_status_string(status));
        lt48_installation_close(installation);
        return 2;
    }
    status = lt48_installation_device_guidance(
        installation, match_index, &guidance
    );
    if (status != LT48_OK) {
        fprintf(stderr, "labtool48: cannot build guidance for %s: %s\n",
                selector, lt48_status_string(status));
        lt48_installation_close(installation);
        return 2;
    }
    status = lt48_installation_device_compile_evidence(
        installation, match_index, &compile_evidence
    );
    if (status != LT48_OK) {
        fprintf(stderr, "labtool48: cannot compile workflow evidence for %s: %s\n",
                selector, lt48_status_string(status));
        lt48_installation_close(installation);
        return 2;
    }
    status = lt48_installation_device_operations(
        installation, match_index, NULL, 0u, &binding_count
    );
    if (status == LT48_BUFFER_TOO_SMALL && binding_count != 0u) {
        operation_bindings = calloc(binding_count, sizeof(*operation_bindings));
        if (operation_bindings == NULL) status = LT48_NO_MEMORY;
        else status = lt48_installation_device_operations(
            installation, match_index, operation_bindings, binding_count,
            &binding_count
        );
    }
    if (status != LT48_OK) {
        fprintf(stderr, "labtool48: cannot resolve operation resources for %s: %s\n",
                selector, lt48_status_string(status));
        free(operation_bindings);
        lt48_installation_close(installation);
        return 2;
    }
    if (json) {
        fputs("{\"schema\":1,\"vendor\":", stdout);
        print_json_string(spec.catalogue.vendor);
        fputs(",\"name\":", stdout);
        print_json_bytes(spec.catalogue.device_name,
                         spec.catalogue.device_name_length);
        printf(",\"device_class\":%u,\"device_type\":%u,"
               "\"definition_file\":\"%s.rez\","
               "\"definition_resource\":\"D%x\","
               "\"family_code\":%u,\"pinmap\":%u,\"pin_count\":%u,"
               "\"capacity_bytes\":%u,\"element_count\":",
               spec.catalogue.device_class, spec.catalogue.device_type,
               spec.definition_stem, spec.catalogue.device_type,
               spec.family_code, spec.pinmap, spec.pin_count,
               spec.capacity_bytes);
        if (spec.element_count != 0U)
            printf("%u", spec.element_count);
        else
            fputs("null", stdout);
        fputs(",\"element_bits\":", stdout);
        if (spec.element_bits != 0U)
            printf("%u", spec.element_bits);
        else
            fputs("null", stdout);
        fputs(",\"secondary_element_count\":", stdout);
        if (spec.secondary_element_count != 0U)
            printf("%u", spec.secondary_element_count);
        else
            fputs("null", stdout);
        fputs(",\"secondary_element_bits\":", stdout);
        if (spec.secondary_element_bits != 0U)
            printf("%u", spec.secondary_element_bits);
        else
            fputs("null", stdout);
        printf(",\"operation_count\":%u,"
               "\"variable_count\":%u,\"configuration_set\":%u,"
               "\"family_header_value\":%u,"
               "\"guidance\":{\"definition_pin_count\":%u,"
               "\"catalogue_package_hint\":",
               spec.operation_count, spec.variable_count,
               spec.configuration_set,
               spec.family_header_value, guidance.definition_pin_count);
        if (guidance.has_catalogue_package_hint) {
            printf("{\"pin_count\":%u,\"code\":",
                   guidance.catalogue_package_pin_count);
            print_json_string(guidance.catalogue_package_code);
            putchar('}');
        } else {
            fputs("null", stdout);
        }
        printf(",\"adapter_requirement\":\"%s\",\"socket_span\":",
               guidance.adapter_requirement == LT48_ADAPTER_REQUIRED ?
                   "required" :
               guidance.adapter_requirement == LT48_ADAPTER_POSSIBLY_DIRECT ?
                   "possibly-direct" : "unknown");
        if (guidance.has_socket_span) {
            printf("{\"first_position\":%u,\"last_position\":%u,"
                   "\"basis\":\"TPinCode compiler centering rule\"}",
                   guidance.first_socket_position,
                   guidance.last_socket_position);
        } else {
            fputs("null", stdout);
        }
        const char *wiring = lt48_installation_device_adapter_wiring_note(
            installation, match_index);
        printf(",\"pin_count_conflict\":%s,"
               "\"orientation\":\"not-inferred\",\"adapter_wiring\":",
               guidance.package_pin_count_conflict ? "true" : "false");
        print_json_string(wiring == NULL ? "unknown" : wiring);
        printf("},"
               "\"compile_evidence\":{\"operation_count\":%zu,"
               "\"strict_compact_count\":%zu,"
               "\"resident_noop_default_count\":%zu,"
               "\"unresolved_default_count\":%zu,"
               "\"rejected_compact_count\":%zu,"
               "\"local_resource_count\":%zu},"
               "\"definition_operations\":[",
               compile_evidence.operation_count,
               compile_evidence.strict_compact_count,
               compile_evidence.resident_noop_default_count,
               compile_evidence.unresolved_default_count,
               compile_evidence.rejected_compact_count,
               compile_evidence.local_resource_count);
        for (size_t index = 0u; index < binding_count; ++index) {
            const lt48_device_operation_binding *binding =
                &operation_bindings[index];
            printf("%s{\"operation_id\":%u,\"symbol\":",
                   index == 0u ? "" : ",", binding->operation_id);
            if (binding->has_symbol)
                print_json_string(binding->symbol);
            else
                fputs("null", stdout);
            printf(",\"wave\":%u,\"wave_type\":", binding->wave);
            if (binding->has_wave) printf("%u", binding->wave_type);
            else fputs("null", stdout);
            fputs(",\"load_address\":", stdout);
            if (binding->has_applet_header)
                printf("%u", binding->load_address);
            else
                fputs("null", stdout);
            printf(",\"stored_data_size\":%zu}",
                   binding->stored_data_size);
        }
        puts("]}");
    } else {
        printf("%s:%.*s\n  %s.rez:D%x; class %u; family M%x; pin map P%x\n"
               "  %u pins; %u bytes; %u operations; %u variables\n",
               spec.catalogue.vendor, (int)spec.catalogue.device_name_length,
               (const char *)spec.catalogue.device_name, spec.definition_stem,
               spec.catalogue.device_type, spec.catalogue.device_class,
               spec.family_code, spec.pinmap, spec.pin_count, spec.capacity_bytes,
               spec.operation_count, spec.variable_count);
        if (spec.element_count != 0U && spec.element_bits != 0U)
            printf("  geometry: %u elements x %u bits\n",
                   spec.element_count, spec.element_bits);
        if (spec.configuration_set != 0u)
            printf("  Configuration set %u\n", spec.configuration_set);
        if (guidance.has_socket_span)
            printf("  socket contacts %u..%u, bottom-centered by the recovered "
                   "TPinCode rule\n", guidance.first_socket_position,
                   guidance.last_socket_position);
        else
            puts("  automatic socket span unavailable");
        printf("  adapter: %s; orientation: not inferred\n",
               guidance.adapter_requirement == LT48_ADAPTER_REQUIRED ?
                   "required" :
               guidance.adapter_requirement == LT48_ADAPTER_POSSIBLY_DIRECT ?
                   "possibly direct" : "unknown");
        const char *wiring = lt48_installation_device_adapter_wiring_note(
            installation, match_index);
        if (wiring != NULL) printf("  adapter wiring: %s\n", wiring);
        if (guidance.has_catalogue_package_hint)
            printf("  catalogue package hint: %u%s%s\n",
                   guidance.catalogue_package_pin_count,
                   guidance.catalogue_package_code,
                   guidance.package_pin_count_conflict ?
                       " (conflicts with definition; do not auto-place)" : "");
        printf("  compile evidence: %zu strict compact, %zu proved resident "
               "no-op default, %zu unresolved default, %zu rejected compact, "
               "%zu local opaque resource%s\n",
               compile_evidence.strict_compact_count,
               compile_evidence.resident_noop_default_count,
               compile_evidence.unresolved_default_count,
               compile_evidence.rejected_compact_count,
               compile_evidence.local_resource_count,
               compile_evidence.local_resource_count == 1u ? "" : "s");
        puts("  definition callbacks:");
        for (size_t index = 0u; index < binding_count; ++index) {
            const lt48_device_operation_binding *binding =
                &operation_bindings[index];
            printf("    %s%sW%x type=%u stored-data=%zu%s",
                   binding->has_symbol ? binding->symbol : "id ",
                   binding->has_symbol ? " -> " : "",
                   binding->wave, binding->wave_type,
                   binding->stored_data_size,
                   binding->has_applet_header ? " applet" : "");
            if (binding->has_applet_header)
                printf(" load=0x%x", binding->load_address);
            putchar('\n');
        }
    }
    free(operation_bindings);
    lt48_installation_close(installation);
    return 0;
}

static const char *availability_name(lt48_operation_availability value) {
    switch (value) {
    case LT48_OPERATION_PLAN_ONLY: return "plan-only";
    case LT48_OPERATION_EXECUTABLE: return "executable";
    case LT48_OPERATION_HOST_ONLY: return "host-only";
    default: return "unavailable";
    }
}

static int catalogue_support(const char *path, const char *selector, int json) {
    lt48_installation *installation = NULL;
    lt48_catalogue_entry entry;
    lt48_operation_support items[LT48_OPERATION_KIND_COUNT];
    size_t match_index = 0u;
    size_t matches = 0u;
    size_t count = 0u;
    lt48_status status = lt48_installation_open(path, &installation);
    if (status != LT48_OK) {
        fprintf(stderr, "labtool48: %s: %s\n", path, lt48_status_string(status));
        return 2;
    }
    for (size_t index = 0u;
         index < lt48_installation_device_count(installation); ++index) {
        status = lt48_installation_get_device(installation, index, &entry);
        if (status != LT48_OK) {
            lt48_installation_close(installation);
            return 2;
        }
        if (catalogue_key_equals(&entry, selector)) {
            match_index = index;
            ++matches;
        }
    }
    if (matches != 1u) {
        fprintf(stderr, "labtool48: exact selector matched %zu devices\n", matches);
        lt48_installation_close(installation);
        return 2;
    }
    status = lt48_installation_get_device(installation, match_index, &entry);
    if (status != LT48_OK) {
        lt48_installation_close(installation);
        return 2;
    }
    status = lt48_installation_operation_support(
        installation, match_index, items,
        sizeof(items) / sizeof(items[0]), &count
    );
    if (status != LT48_OK) {
        fprintf(stderr, "labtool48: cannot determine operation support: %s\n",
                lt48_status_string(status));
        lt48_installation_close(installation);
        return 2;
    }
    if (json) {
        fputs("{\"schema\":1,\"selector\":", stdout);
        print_json_string(selector);
        fputs(",\"new_device_library_sha256\":",stdout);
        print_json_string(lt48_installation_new_device_lib_sha256(installation));
        fputs(",\"complete_operation_profile\":", stdout);
        print_json_string(lt48_installation_complete_operation_profile(installation, match_index));
        printf(",\"complete_supported\":%s",
               lt48_installation_device_complete_support(
                   installation, match_index) ? "true" : "false");
        fputs(",\"operations\":[", stdout);
        for (size_t index = 0u; index < count; ++index) {
            const lt48_operation_support *item = &items[index];
            printf("%s{\"operation_id\":%u,\"name\":",
                   index == 0u ? "" : ",", (unsigned)item->operation);
            print_json_string(item->name);
            fputs(",\"availability\":", stdout);
            print_json_string(availability_name(item->availability));
            fputs(",\"support_level\":", stdout);
            print_json_string(item->support_level);
            printf(",\"mutates_device\":%s,\"reason\":",
                   item->mutates_device ? "true" : "false");
            print_json_string(item->reason);
            putchar('}');
        }
        puts("]}");
    } else {
        printf("%s operation support (local inspection; no USB):\n", selector);
        printf("  complete support: %s (%s)\n",
               lt48_installation_device_complete_support(
                   installation, match_index) ? "yes" : "no",
               lt48_installation_complete_operation_profile(installation, match_index));
        for (size_t index = 0u; index < count; ++index)
            printf("  %-17s %-11s %-25s %s%s\n", items[index].name,
                   availability_name(items[index].availability),
                   items[index].support_level, items[index].reason,
                   items[index].mutates_device ? " [mutating]" : "");
    }
    lt48_installation_close(installation);
    return 0;
}

/* One installation, one evaluation per row. No per-device child processes. */
static int catalogue_support_export(const char *path, int snapshot) {
    lt48_installation *installation = NULL;
    lt48_status status = lt48_installation_open(path, &installation);
    if (status != LT48_OK) {
        fprintf(stderr, "labtool48: %s: %s\n", path, lt48_status_string(status));
        return 2;
    }
    /* Explicit offline scope only. Operation owners retain live revalidation.
     * Collectors must reject changes to source files across this snapshot. */
    if (snapshot) lt48_rez_snapshot_begin();
    size_t devices = lt48_installation_device_count(installation), complete_count = 0;
    fputs("{\"schema\":1,\"usb_opened\":false,\"installation\":", stdout);
    print_json_string(path);
    fputs(",\"new_device_library_sha256\":",stdout);
    print_json_string(lt48_installation_new_device_lib_sha256(installation));
    printf(",\"device_count\":%zu,\"devices\":[", devices);
    for (size_t i = 0; i < devices; i++) {
        lt48_catalogue_entry entry;
        lt48_operation_support items[LT48_OPERATION_KIND_COUNT];
        size_t count = 0;
        const char *profile;
        int complete;
        status = lt48_installation_get_device(installation, i, &entry);
        if (status == LT48_OK) status = lt48_installation_support_summary(
            installation, i, items, LT48_OPERATION_KIND_COUNT, &count, &profile, &complete);
        if (status != LT48_OK) {
            fprintf(stderr, "labtool48: export row %zu: %s\n", i, lt48_status_string(status));
            lt48_installation_close(installation);
            if (snapshot) lt48_rez_snapshot_end();
            return 2; /* Deliberately incomplete JSON; callers must reject failure. */
        }
        complete_count += complete;
        printf("%s{\"index\":%zu,\"vendor\":", i ? "," : "", i);
        print_json_string(entry.vendor);
        fputs(",\"name\":", stdout);
        print_json_bytes(entry.device_name, entry.device_name_length);
        printf(",\"device_class\":%u,\"device_type\":%u,\"complete_supported\":%s,\"complete_operation_profile\":",
            entry.device_class, entry.device_type, complete ? "true" : "false");
        print_json_string(profile);
        const lt48_new_device_record *authored=lt48_installation_new_device_record(installation,i);
        fputs(",\"new_device_definition\":",stdout);
        print_json_string(authored?authored->id:NULL);
        fputs(",\"new_device_definition_sha256\":",stdout);
        print_json_string(authored?authored->source_sha256:NULL);
        fputs(",\"operations\":[", stdout);
        for (size_t j = 0; j < count; j++) {
            const lt48_operation_support *item = items + j;
            printf("%s{\"operation_id\":%u,\"name\":", j ? "," : "", (unsigned)item->operation);
            print_json_string(item->name);
            fputs(",\"availability\":", stdout); print_json_string(availability_name(item->availability));
            fputs(",\"support_level\":", stdout); print_json_string(item->support_level);
            printf(",\"mutates_device\":%s,\"definition_bound\":%s,\"reason\":",
                item->mutates_device ? "true" : "false",
                item->definition_callback_present || item->availability != LT48_OPERATION_UNAVAILABLE ? "true" : "false");
            print_json_string(item->reason);
            putchar('}');
        }
        fputs("]}", stdout);
    }
    printf("],\"complete_supported_device_count\":%zu}\n", complete_count);
    lt48_installation_close(installation);
    if (snapshot) lt48_rez_snapshot_end();
    return ferror(stdout) ? 2 : 0;
}

static int catalogue_support_matrix(const char *path, int json) {
#define MAX_SUPPORT_REASONS 128u
    lt48_installation *installation = NULL;
    lt48_operation_support support[LT48_OPERATION_KIND_COUNT];
    size_t totals[LT48_OPERATION_KIND_COUNT][4] = {{0}};
    size_t definition_totals[LT48_OPERATION_KIND_COUNT] = {0};
    size_t definition_blocked_totals[LT48_OPERATION_KIND_COUNT] = {0};
    unsigned char *states = NULL;
    unsigned char *definition_states = NULL;
    uint16_t *reason_codes = NULL;
    const char *reason_table[MAX_SUPPORT_REASONS];
    size_t reason_count = 0u;
    size_t device_count;
    size_t complete_device_count = 0u;
    lt48_status status = lt48_installation_open(path, &installation);
    if (status != LT48_OK) {
        fprintf(stderr, "labtool48: %s: %s\n", path,
                lt48_status_string(status));
        return 2;
    }
    device_count = lt48_installation_device_count(installation);
    if (device_count >
        SIZE_MAX / (2u * LT48_OPERATION_KIND_COUNT)) {
        lt48_installation_close(installation);
        return 6;
    }
    states = malloc(
        device_count * 2u * LT48_OPERATION_KIND_COUNT);
    if (states == NULL && device_count != 0u) {
        lt48_installation_close(installation);
        return 6;
    }
    definition_states = states == NULL ? NULL :
        states + device_count * LT48_OPERATION_KIND_COUNT;
    reason_codes = calloc(
        device_count * LT48_OPERATION_KIND_COUNT, sizeof(*reason_codes));
    if (reason_codes == NULL && device_count != 0u) {
        free(states);
        lt48_installation_close(installation);
        return 6;
    }
    for (size_t device = 0u; device < device_count; ++device) {
        size_t count = 0u;
        lt48_catalogue_entry complete_entry;
        status = lt48_installation_operation_support(
            installation, device, support, LT48_OPERATION_KIND_COUNT, &count);
        if (status != LT48_OK || count != LT48_OPERATION_KIND_COUNT) {
            fprintf(stderr,
                    "labtool48: cannot determine support for device %zu: %s\n",
                    device, lt48_status_string(status));
            free(reason_codes);
            free(states);
            lt48_installation_close(installation);
            return 2;
        }
        for (size_t index = 0u; index < count; ++index) {
            unsigned operation = (unsigned)support[index].operation;
            unsigned availability = (unsigned)support[index].availability;
            if (operation == 0u || operation > LT48_OPERATION_KIND_COUNT ||
                availability > LT48_OPERATION_HOST_ONLY) {
                free(reason_codes);
                free(states);
                lt48_installation_close(installation);
                return 2;
            }
            states[device * LT48_OPERATION_KIND_COUNT + operation - 1u] =
                (unsigned char)availability;
            definition_states[
                device * LT48_OPERATION_KIND_COUNT + operation - 1u] =
                support[index].definition_callback_present ||
                availability != LT48_OPERATION_UNAVAILABLE ? 1u : 0u;
            ++totals[operation - 1u][availability];
            if (definition_states[
                    device * LT48_OPERATION_KIND_COUNT + operation - 1u]) {
                ++definition_totals[operation - 1u];
                if (availability != LT48_OPERATION_EXECUTABLE &&
                    availability != LT48_OPERATION_HOST_ONLY)
                    ++definition_blocked_totals[operation - 1u];
            }
            {
                size_t reason_index;
                for (reason_index = 0u; reason_index < reason_count;
                     ++reason_index) {
                    if (strcmp(reason_table[reason_index],
                               support[index].reason) == 0) break;
                }
                if (reason_index == reason_count) {
                    if (reason_count == MAX_SUPPORT_REASONS) {
                        free(reason_codes);
                        free(states);
                        lt48_installation_close(installation);
                        return 2;
                    }
                    reason_table[reason_count++] = support[index].reason;
                }
                reason_codes[
                    device * LT48_OPERATION_KIND_COUNT + operation - 1u] =
                    (uint16_t)reason_index;
            }
        }
        if (lt48_installation_get_device(
                installation, device, &complete_entry) != LT48_OK) {
            free(reason_codes);
            free(states);
            lt48_installation_close(installation);
            return 2;
        }
        if (lt48_installation_device_complete_support(
                installation, device))
            ++complete_device_count;
    }
    if (json) {
        fputs("{\"schema\":1,\"installation\":", stdout);
        print_json_string(path);
        printf(",\"device_count\":%zu,\"operation_order\":[", device_count);
        for (unsigned operation = 1u;
             operation <= LT48_OPERATION_KIND_COUNT; ++operation) {
            if (operation != 1u) putchar(',');
            print_json_string(lt48_operation_kind_name(
                (lt48_operation_kind)operation));
        }
        fputs("],\"reason_table\":[", stdout);
        for (size_t reason = 0u; reason < reason_count; ++reason) {
            if (reason != 0u) putchar(',');
            print_json_string(reason_table[reason]);
        }
        fputs("],\"devices\":[", stdout);
        for (size_t device = 0u; device < device_count; ++device) {
            lt48_catalogue_entry entry;
            lt48_host_engine_info engine;
            lt48_status engine_status;
            status = lt48_installation_get_device(
                installation, device, &entry);
            if (status != LT48_OK) {
                free(reason_codes);
                free(states);
                lt48_installation_close(installation);
                return 2;
            }
            printf("%s{\"index\":%zu,\"vendor\":",
                   device == 0u ? "" : ",", device);
            print_json_string(entry.vendor);
            fputs(",\"name\":", stdout);
            print_json_bytes(entry.device_name, entry.device_name_length);
            printf(",\"device_class\":%u,\"device_type\":%u",
                   entry.device_class, entry.device_type);
            printf(",\"complete_operation_profile\":");
            print_json_string(lt48_installation_complete_operation_profile(installation, device));
            printf(",\"complete_supported\":%s,\"host_engine\":",
                   lt48_installation_device_complete_support(
                       installation, device) ?
                   "true" : "false");
            engine_status = lt48_host_engine_for_class(
                entry.device_class, &engine);
            if (engine_status == LT48_OK) {
                fputs("{\"primary_engine\":", stdout);
                print_json_string(engine.primary_engine);
                fputs(",\"semantic_family\":", stdout);
                print_json_string(engine.semantic_family);
                fputs(",\"semantic_status\":", stdout);
                print_json_string(engine.semantic_status);
                fputs(",\"normalized_profile\":", stdout);
                print_json_string(engine.normalized_engine_profile);
                printf(",\"constructor_address\":%u,"
                       "\"vtable_address\":%u,"
                       "\"primary_entry_address\":%u}",
                       engine.constructor_address, engine.vtable_address,
                       engine.primary_entry_address);
            } else {
                fputs("null", stdout);
            }
            fputs(",\"availability\":[", stdout);
            for (size_t operation = 0u;
                 operation < LT48_OPERATION_KIND_COUNT; ++operation) {
                if (operation != 0u) putchar(',');
                print_json_string(availability_name(
                    (lt48_operation_availability)
                    states[device * LT48_OPERATION_KIND_COUNT + operation]));
            }
            fputs("],\"definition_bound\":[", stdout);
            for (size_t operation = 0u;
                 operation < LT48_OPERATION_KIND_COUNT; ++operation) {
                if (operation != 0u) putchar(',');
                fputs(definition_states[
                    device * LT48_OPERATION_KIND_COUNT + operation] ?
                    "true" : "false", stdout);
            }
            fputs("],\"reason_code\":[", stdout);
            for (size_t operation = 0u;
                 operation < LT48_OPERATION_KIND_COUNT; ++operation) {
                if (operation != 0u) putchar(',');
                printf("%u", (unsigned)reason_codes[
                    device * LT48_OPERATION_KIND_COUNT + operation]);
            }
            fputs("]}", stdout);
        }
        fputs("],\"totals\":[", stdout);
        for (size_t operation = 0u;
             operation < LT48_OPERATION_KIND_COUNT; ++operation) {
            printf("%s{\"operation\":", operation == 0u ? "" : ",");
            print_json_string(lt48_operation_kind_name(
                (lt48_operation_kind)(operation + 1u)));
            printf(",\"unavailable\":%zu,\"plan_only\":%zu,"
                   "\"executable\":%zu,\"host_only\":%zu,"
                   "\"definition_bound\":%zu,"
                   "\"definition_bound_blocked\":%zu}",
                   totals[operation][LT48_OPERATION_UNAVAILABLE],
                   totals[operation][LT48_OPERATION_PLAN_ONLY],
                   totals[operation][LT48_OPERATION_EXECUTABLE],
                   totals[operation][LT48_OPERATION_HOST_ONLY],
                   definition_totals[operation],
                   definition_blocked_totals[operation]);
        }
        printf("],\"complete_supported_device_count\":%zu,"
               "\"complete_support_rule\":\"all applicable operations "
               "must be executable; partial operation rows are excluded\","
               "\"usb_opened\":false}\n", complete_device_count);
    } else {
        printf("Operation support matrix for %zu exact device definitions "
               "(local inspection; no USB):\n", device_count);
        for (size_t operation = 0u;
             operation < LT48_OPERATION_KIND_COUNT; ++operation)
            printf("  %-17s unavailable %zu; plan-only %zu; executable %zu; "
                   "host-only %zu; definition-bound %zu; "
                   "definition-bound-blocked %zu\n",
                   lt48_operation_kind_name(
                       (lt48_operation_kind)(operation + 1u)),
                   totals[operation][LT48_OPERATION_UNAVAILABLE],
                   totals[operation][LT48_OPERATION_PLAN_ONLY],
                   totals[operation][LT48_OPERATION_EXECUTABLE],
                   totals[operation][LT48_OPERATION_HOST_ONLY],
                   definition_totals[operation],
                   definition_blocked_totals[operation]);
        printf("Complete applicable-operation support: %zu/%zu devices "
               "(partial operation rows excluded).\n",
               complete_device_count, device_count);
        puts("Use --json to emit the per-device matrix in operation_order.");
    }
    free(reason_codes);
    free(states);
    lt48_installation_close(installation);
#undef MAX_SUPPORT_REASONS
    return 0;
}

static int catalogue_identify(
    const char *path, const lt48_auto_id_observation *observation, int json
) {
    lt48_installation *installation = NULL;
    lt48_auto_id_candidate *candidates = NULL;
    size_t count = 0u;
    lt48_status status = lt48_installation_open(path, &installation);
    if (status != LT48_OK) {
        fprintf(stderr, "labtool48: %s: %s\n", path, lt48_status_string(status));
        return 2;
    }
    status = lt48_installation_rank_auto_id(
        installation, observation, NULL, 0u, &count
    );
    if (status != LT48_OK && status != LT48_BUFFER_TOO_SMALL) {
        fprintf(stderr, "labtool48: cannot rank Auto-ID observations: %s\n",
                lt48_status_string(status));
        lt48_installation_close(installation);
        return 2;
    }
    if (count != 0u) {
        candidates = calloc(count, sizeof(*candidates));
        if (candidates == NULL) {
            lt48_installation_close(installation);
            return 6;
        }
        status = lt48_installation_rank_auto_id(
            installation, observation, candidates, count, &count
        );
        if (status != LT48_OK) {
            fprintf(stderr, "labtool48: cannot rank Auto-ID observations: %s\n",
                    lt48_status_string(status));
            free(candidates);
            lt48_installation_close(installation);
            return 2;
        }
    }
    if (json) {
        fputs("{\"schema\":1,\"mode\":\"offline-observation-ranking\","
              "\"usb_opened\":false,\"observation\":{"
              "\"manufacturer_id\":", stdout);
        if (observation->has_manufacturer_id)
            printf("%u", observation->manufacturer_id);
        else fputs("null", stdout);
        fputs(",\"device_id\":", stdout);
        if (observation->has_device_id) printf("%u", observation->device_id);
        else fputs("null", stdout);
        fputs(",\"pin_count_hint\":", stdout);
        if (observation->has_pin_count_hint)
            printf("%u", observation->pin_count_hint);
        else fputs("null", stdout);
        fputs(",\"device_class_hint\":", stdout);
        if (observation->has_device_class_hint)
            printf("%u", observation->device_class_hint);
        else fputs("null", stdout);
        fputs("},\"candidates\":[", stdout);
        for (size_t index = 0u; index < count; ++index) {
            const lt48_auto_id_candidate *candidate = &candidates[index];
            printf("%s{\"rank\":%zu,\"device_index\":%zu,\"vendor\":",
                   index == 0u ? "" : ",", index + 1u,
                   candidate->device_index);
            print_json_string(candidate->catalogue.vendor);
            fputs(",\"name\":", stdout);
            print_json_bytes(candidate->catalogue.device_name,
                             candidate->catalogue.device_name_length);
            printf(",\"manufacturer_id\":%u,\"device_id\":%u,"
                   "\"device_class\":%u,\"pin_count\":",
                   candidate->catalogue.manufacturer_id,
                   candidate->catalogue.device_id,
                   candidate->catalogue.device_class);
            if (candidate->has_resolved_pin_count)
                printf("%u", candidate->resolved_pin_count);
            else fputs("null", stdout);
            printf(",\"score\":%u,\"evidence\":{"
                   "\"manufacturer_id_match\":%s,"
                   "\"device_id_match\":%s,\"pin_count_match\":%s,"
                   "\"device_class_match\":%s}}",
                   candidate->score,
                   candidate->manufacturer_id_match ? "true" : "false",
                   candidate->device_id_match ? "true" : "false",
                   candidate->pin_count_match ? "true" : "false",
                   candidate->device_class_match ? "true" : "false");
        }
        printf("],\"candidate_count\":%zu}\n", count);
    } else {
        printf("%zu ranked candidate%s; offline only, no USB opened\n", count,
               count == 1u ? "" : "s");
        for (size_t index = 0u; index < count; ++index)
            printf("  %zu. %s:%.*s score=%u class=%u%s\n", index + 1u,
                   candidates[index].catalogue.vendor,
                   (int)candidates[index].catalogue.device_name_length,
                   (const char *)candidates[index].catalogue.device_name,
                   candidates[index].score,
                   candidates[index].catalogue.device_class,
                   candidates[index].pin_count_match ? " pin-count-match" : "");
    }
    free(candidates);
    lt48_installation_close(installation);
    return 0;
}

static int identify_command(
    int argc, char **argv, int path_index, int option_start
) {
    lt48_auto_id_observation observation = {0};
    int json = 0;
    for (int index = option_start; index < argc; ++index) {
        uint16_t value;
        if (strcmp(argv[index], "--json") == 0 && !json) {
            json = 1;
        } else if (strcmp(argv[index], "--manufacturer-id") == 0 &&
                   index + 1 < argc &&
                   !observation.has_manufacturer_id &&
                   parse_u16(argv[++index], &value)) {
            observation.has_manufacturer_id = 1;
            observation.manufacturer_id = value;
        } else if (strcmp(argv[index], "--device-id") == 0 &&
                   index + 1 < argc && !observation.has_device_id &&
                   parse_u16(argv[++index], &value)) {
            observation.has_device_id = 1;
            observation.device_id = value;
        } else if (strcmp(argv[index], "--pins") == 0 &&
                   index + 1 < argc && !observation.has_pin_count_hint &&
                   parse_u16(argv[++index], &value) && value <= UINT8_MAX) {
            observation.has_pin_count_hint = 1;
            observation.pin_count_hint = value;
        } else if (strcmp(argv[index], "--class") == 0 &&
                   index + 1 < argc &&
                   !observation.has_device_class_hint &&
                   parse_u16(argv[++index], &value)) {
            observation.has_device_class_hint = 1;
            observation.device_class_hint = value;
        } else {
            return 2;
        }
    }
    if (!observation.has_manufacturer_id && !observation.has_device_id &&
        !observation.has_pin_count_hint &&
        !observation.has_device_class_hint) return 2;
    return catalogue_identify(argv[path_index], &observation, json);
}

static int catalogue_audit_m37(
    const char *path, const char *selector, int json
) {
    lt48_installation *installation = NULL;
    lt48_catalogue_entry entry;
    lt48_m37_resource_audit audit;
    lt48_m37_plan_evidence read_evidence = {0};
    lt48_m37_cleanup_phase cleanup[12];
    size_t cleanup_count = 0u;
    uint64_t read_issues = 0u;
    int authoritative_read_gate = 0;
    lt48_m37_resources *snapshot = NULL;
    size_t snapshot_variable_count = 0u;
    size_t snapshot_applet_count = 0u;
    size_t snapshot_fpga_size = 0u;
    unsigned snapshot_compiled_count = 0u;
    size_t snapshot_read_transactions = 0u;
    unsigned char snapshot_read_sha256[LT48_SHA256_SIZE] = {0};
    int snapshot_read_digest_valid = 0;
    size_t recovery_transactions[3] = {0u,0u,0u};
    size_t recovery_phase_count[3] = {0u,0u,0u};
    lt48_m37_cleanup_phase recovery_phases[3][12] = {{0}};
    size_t recovery_phase_first[3][12] = {{0}};
    size_t recovery_phase_transactions[3][12] = {{0}};
    unsigned char recovery_sha256[3][LT48_SHA256_SIZE] = {{0}};
    int recovery_valid[3] = {0,0,0};
    int snapshot_ready = 0;
    lt48_status status = lt48_installation_open(path, &installation);
    size_t match_index = 0u;
    size_t matches = 0u;
    if (status != LT48_OK) {
        fprintf(stderr, "labtool48: %s: %s\n", path, lt48_status_string(status));
        return 2;
    }
    for (size_t index = 0u;
         index < lt48_installation_device_count(installation); ++index) {
        status = lt48_installation_get_device(installation, index, &entry);
        if (status != LT48_OK) {
            lt48_installation_close(installation);
            return 2;
        }
        if (catalogue_key_equals(&entry, selector)) {
            match_index = index;
            ++matches;
        }
    }
    if (matches != 1u) {
        fprintf(stderr, "labtool48: exact selector matched %zu devices\n", matches);
        lt48_installation_close(installation);
        return 2;
    }
    status = lt48_installation_audit_m37(installation, match_index, &audit);
    if (status != LT48_OK) {
        fprintf(stderr, "labtool48: cannot audit %s: %s\n", selector,
                lt48_status_string(status));
        lt48_installation_close(installation);
        return 2;
    }
    if (lt48_m37_cleanup_recipe(
            1, 1, cleanup, sizeof(cleanup) / sizeof(cleanup[0]),
            &cleanup_count) == LT48_OK) {
        read_evidence.operation = LT48_M37_READ;
        read_evidence.start = 0u;
        read_evidence.length = audit.capacity_bytes;
        read_evidence.chunk_size = 163u;
        read_evidence.cleanup_phases = cleanup;
        read_evidence.cleanup_phase_count = cleanup_count;
        authoritative_read_gate =
            lt48_installation_validate_m37_plan(
                installation, match_index, &read_evidence, &read_issues
            ) == LT48_OK;
    }
    if (lt48_m37_resources_open(
            installation, match_index, &snapshot) == LT48_OK) {
        const unsigned char *fpga = NULL;
        (void)lt48_m37_resources_variables(
            snapshot, &snapshot_variable_count
        );
        snapshot_applet_count = lt48_m37_resources_applet_count(snapshot);
        if (lt48_m37_resources_active_fpga(
                snapshot, &fpga, &snapshot_fpga_size) == LT48_OK &&
            fpga != NULL) {
            for (uint16_t operation = 0u; operation <= 4u; ++operation) {
                uint16_t wave;
                const lt48_compiled_wave *compiled;
                if (lt48_m37_resources_compiled_wave(
                        snapshot, operation, &wave, &compiled) == LT48_OK &&
                    compiled != NULL) {
                    ++snapshot_compiled_count;
                }
            }
            snapshot_ready = snapshot_compiled_count == 5u;
            if (snapshot_ready) {
                lt48_protocol_plan *read_plan = NULL;
                if (lt48_m37_resources_build_offline_read_plan(
                        snapshot, 0u, audit.capacity_bytes,
                        &read_plan) == LT48_OK) {
                    snapshot_read_transactions =
                        lt48_protocol_plan_transaction_count(read_plan);
                    snapshot_read_digest_valid =
                        lt48_protocol_plan_sha256(
                            read_plan, snapshot_read_sha256
                        ) == LT48_OK;
                }
                lt48_protocol_plan_close(read_plan);
                for (int state = LT48_M37_RECOVERY_UNOPENED;
                     state <= LT48_M37_RECOVERY_POWERED; ++state) {
                    lt48_protocol_plan *recovery_plan = NULL;
                    if (lt48_m37_resources_build_recovery_plan(
                            snapshot, (lt48_m37_recovery_state)state,
                            &recovery_plan) == LT48_OK) {
                        recovery_transactions[state] =
                            lt48_protocol_plan_transaction_count(recovery_plan);
                        recovery_phase_count[state] =
                            lt48_protocol_plan_recovery_phase_count(
                                recovery_plan
                            );
                        for (size_t phase = 0u;
                             phase < recovery_phase_count[state] &&
                             phase < 12u; ++phase) {
                            if (lt48_protocol_plan_get_recovery_phase(
                                    recovery_plan, phase,
                                    &recovery_phases[state][phase],
                                    &recovery_phase_first[state][phase],
                                    &recovery_phase_transactions[state][phase]
                                ) != LT48_OK) {
                                recovery_phase_count[state] = 0u;
                                break;
                            }
                        }
                        recovery_valid[state] = lt48_protocol_plan_sha256(
                            recovery_plan, recovery_sha256[state]
                        ) == LT48_OK;
                    }
                    lt48_protocol_plan_close(recovery_plan);
                }
            }
        }
    }
    if (json) {
        printf("{\"schema\":1,\"selector\":");
        print_json_string(selector);
        printf(",\"device_class\":%u,\"device_type\":%u,"
               "\"family_code\":%u,\"pinmap\":%u,\"pin_count\":%u,"
               "\"capacity_bytes\":%u,\"operation_count\":%u,"
               "\"variable_count\":%u,\"nominal_vcc_centivolts\":%u,"
               "\"expected_vcc_selector\":%u,\"compact_wave_count\":%u,"
               "\"applet_count\":%u,\"active_fpga_count\":%u,"
               "\"exact_engine_envelope\":%s,\"strict_pin_mapping\":%s,"
               "\"required_compact_waves_strict\":%s,"
               "\"applet_metadata_complete\":%s,"
               "\"applet_ranges_nonoverlapping\":%s,"
               "\"active_fpga_exactly_one\":%s,"
               "\"authoritative_read_gate\":%s,\"read_issue_mask\":%llu,"
               "\"immutable_snapshot_ready\":%s,"
               "\"snapshot_compiled_count\":%u,"
               "\"snapshot_variable_count\":%zu,"
               "\"snapshot_applet_count\":%zu,"
               "\"snapshot_fpga_size\":%zu,"
               "\"snapshot_read_transactions\":%zu,"
               "\"snapshot_read_sha256\":",
               audit.device_class, audit.device_type, audit.family_code,
               audit.pinmap, audit.pin_count, audit.capacity_bytes,
               audit.operation_count, audit.variable_count,
               audit.nominal_vcc_centivolts, audit.expected_vcc_selector,
               audit.compact_wave_count, audit.applet_count,
               audit.active_fpga_count,
               audit.exact_engine_envelope ? "true" : "false",
               audit.strict_pin_mapping ? "true" : "false",
               audit.required_compact_waves_strict ? "true" : "false",
               audit.applet_metadata_complete ? "true" : "false",
               audit.applet_ranges_nonoverlapping ? "true" : "false",
               audit.active_fpga_exactly_one ? "true" : "false",
               authoritative_read_gate ? "true" : "false",
               (unsigned long long)read_issues,
               snapshot_ready ? "true" : "false",
               snapshot_compiled_count, snapshot_variable_count,
               snapshot_applet_count, snapshot_fpga_size,
               snapshot_read_transactions);
        if (snapshot_read_digest_valid) {
            putchar('"');
            print_hex(snapshot_read_sha256, LT48_SHA256_SIZE);
            putchar('"');
        } else {
            fputs("null", stdout);
        }
        fputs(",\"recovery_plans\":[", stdout);
        for (size_t state = 0u; state < 3u; ++state) {
            printf("%s{\"state\":%zu,\"transactions\":%zu,\"sha256\":",
                   state == 0u ? "" : ",", state,
                   recovery_transactions[state]);
            if (recovery_valid[state]) {
                putchar('"');
                print_hex(recovery_sha256[state], LT48_SHA256_SIZE);
                putchar('"');
            } else {
                fputs("null", stdout);
            }
            fputs(",\"phases\":[", stdout);
            for (size_t phase = 0u;
                 phase < recovery_phase_count[state]; ++phase) {
                printf("%s{\"name\":", phase == 0u ? "" : ",");
                print_json_string(lt48_m37_cleanup_phase_name(
                    recovery_phases[state][phase]
                ));
                printf(",\"first_transaction\":%zu,\"transactions\":%zu}",
                       recovery_phase_first[state][phase],
                       recovery_phase_transactions[state][phase]);
            }
            fputs("]}", stdout);
        }
        putchar(']');
        fputs(",\"definition_sha256\":\"", stdout);
        print_hex(audit.definition_sha256, LT48_SHA256_SIZE);
        fputs("\",\"resource_manifest_sha256\":\"", stdout);
        print_hex(audit.resource_manifest_sha256, LT48_SHA256_SIZE);
        fputs("\"}\n", stdout);
    } else {
        printf("%s: M%x/P%x, %u pins, %u bytes, VCC %.2f V (selector %02x)\n",
               selector, audit.family_code, audit.pinmap, audit.pin_count,
               audit.capacity_bytes,
               (double)audit.nominal_vcc_centivolts / 100.0,
               audit.expected_vcc_selector);
        printf("  waves: %u compact, %u applets, %u active FPGA\n",
               audit.compact_wave_count, audit.applet_count,
               audit.active_fpga_count);
        printf("  checks: envelope=%s pin-map=%s compact=%s applets=%s "
               "non-overlap=%s FPGA=%s read-gate=%s (issues 0x%llx)\n",
               audit.exact_engine_envelope ? "pass" : "FAIL",
               audit.strict_pin_mapping ? "pass" : "FAIL",
               audit.required_compact_waves_strict ? "pass" : "FAIL",
               audit.applet_metadata_complete ? "pass" : "FAIL",
               audit.applet_ranges_nonoverlapping ? "pass" : "FAIL",
               audit.active_fpga_exactly_one ? "pass" : "FAIL",
               authoritative_read_gate ? "pass" : "FAIL",
               (unsigned long long)read_issues);
        printf("  immutable snapshot: %s; %u compiled, %zu variables, "
               "%zu applets, %zu FPGA bytes, %zu read transactions\n",
               snapshot_ready ? "ready" : "unavailable",
               snapshot_compiled_count, snapshot_variable_count,
               snapshot_applet_count, snapshot_fpga_size,
               snapshot_read_transactions);
        if (snapshot_read_digest_valid) {
            fputs("  read-plan SHA-256:  ", stdout);
            print_hex(snapshot_read_sha256, LT48_SHA256_SIZE);
            putchar('\n');
        }
        for (size_t state = 0u; state < 3u; ++state) {
            printf("  recovery state %zu: %zu transactions in %zu phases",
                   state, recovery_transactions[state],
                   recovery_phase_count[state]);
            if (recovery_valid[state]) {
                fputs(", SHA-256 ", stdout);
                print_hex(recovery_sha256[state], LT48_SHA256_SIZE);
            }
            putchar('\n');
        }
        fputs("  definition SHA-256: ", stdout);
        print_hex(audit.definition_sha256, LT48_SHA256_SIZE);
        fputs("\n  manifest SHA-256:   ", stdout);
        print_hex(audit.resource_manifest_sha256, LT48_SHA256_SIZE);
        putchar('\n');
    }
    lt48_m37_resources_close(snapshot);
    lt48_installation_close(installation);
    return 0;
}

static int catalogue_plan_read(
    const char *path, const char *selector, uint32_t start, uint32_t length,
    const char *report_path, int json
) {
    lt48_installation *installation = NULL;
    lt48_m37_resources *resources = NULL;
    lt48_protocol_plan *plan = NULL;
    lt48_catalogue_entry entry;
    unsigned char digest[LT48_SHA256_SIZE];
    size_t selected = 0u;
    size_t matches = 0u;
    size_t chunks = 0u;
    size_t bytes = 0u;
    size_t transactions;
    lt48_status status = lt48_installation_open(path, &installation);
    if (status != LT48_OK) {
        fprintf(stderr, "labtool48: %s: %s\n", path, lt48_status_string(status));
        return 2;
    }
    for (size_t index = 0u;
         index < lt48_installation_device_count(installation); ++index) {
        status = lt48_installation_get_device(installation, index, &entry);
        if (status != LT48_OK) goto fail;
        if (catalogue_key_equals(&entry, selector)) {
            selected = index;
            ++matches;
        }
    }
    if (matches != 1u) {
        fprintf(stderr, "labtool48: exact selector matched %zu devices\n", matches);
        status = LT48_NOT_FOUND;
        goto fail;
    }
    status = lt48_m37_resources_open(installation, selected, &resources);
    if (status == LT48_OK) {
        status = lt48_m37_resources_build_offline_read_plan(
            resources, start, length, &plan
        );
    }
    if (status != LT48_OK) goto fail;
    transactions = lt48_protocol_plan_transaction_count(plan);
    for (size_t index = 0u; index < transactions; ++index) {
        lt48_protocol_transaction transaction;
        status = lt48_protocol_plan_get_transaction(plan, index, &transaction);
        if (status != LT48_OK) goto fail;
        if (transaction.reads_device_data) {
            ++chunks;
            bytes += transaction.length;
        }
    }
    status = lt48_protocol_plan_sha256(plan, digest);
    if (status != LT48_OK) goto fail;
    status = write_m37_planned_report(
        report_path, installation, selected, resources, plan,
        LT48_M37_READ, start, length, NULL
    );
    if (status != LT48_OK) goto fail;
    if (json) {
        fputs("{\"schema\":1,\"operation\":\"read\",\"selector\":", stdout);
        print_json_string(selector);
        printf(",\"start\":%u,\"length\":%u,\"transactions\":%zu,"
               "\"read_chunks\":%zu,\"read_bytes\":%zu,"
               "\"plan_sha256\":\"", start, length, transactions,
               chunks, bytes);
        print_hex(digest, sizeof(digest));
        fputs("\",\"report\":", stdout);
        if (report_path == NULL) fputs("null", stdout);
        else print_json_string(report_path);
        fputs(",\"offline\":true,\"usb_opened\":false}\n", stdout);
    } else {
        printf("%s read 0x%x..0x%x: %zu transactions, %zu chunks, %zu bytes\n",
               selector, start, start + length, transactions, chunks, bytes);
        fputs("plan SHA-256: ", stdout);
        print_hex(digest, sizeof(digest));
        puts("\noffline plan only; no programmer opened");
        if (report_path != NULL) printf("report: %s\n", report_path);
    }
    lt48_protocol_plan_close(plan);
    lt48_m37_resources_close(resources);
    lt48_installation_close(installation);
    return 0;

fail:
    fprintf(stderr, "labtool48: cannot plan read: %s\n",
            lt48_status_string(status));
    lt48_protocol_plan_close(plan);
    lt48_m37_resources_close(resources);
    lt48_installation_close(installation);
    return status == LT48_SAFETY_REJECTED ? 4 : 2;
}

static int operation_plan_gate(
    const char *path, const char *selector, lt48_operation_kind operation,
    int have_start, uint32_t start, int have_length, uint32_t length,
    const char *image_path, const char *report_path,
    lt48_verify_margin_selection margin, unsigned margin_type,
    int have_margin_option, int json
) {
    lt48_installation *installation = NULL;
    lt48_catalogue_entry entry;
    lt48_operation_support support;
    size_t selected = 0u;
    size_t matches = 0u;
    lt48_status status = lt48_installation_open(path, &installation);
    if (status != LT48_OK) {
        fprintf(stderr, "labtool48: %s: %s\n", path,
                lt48_status_string(status));
        return 2;
    }
    for (size_t index = 0u;
         index < lt48_installation_device_count(installation); ++index) {
        status = lt48_installation_get_device(installation, index, &entry);
        if (status != LT48_OK) break;
        if (catalogue_key_equals(&entry, selector)) {
            selected = index;
            ++matches;
        }
    }
    if (status == LT48_OK && matches != 1u) status = LT48_NOT_FOUND;
    if (status == LT48_OK) {
        status = lt48_installation_operation_support_one(
            installation, selected, operation, &support
        );
    }
    lt48_installation_close(installation);
    if (status != LT48_OK) {
        fprintf(stderr, "labtool48: cannot resolve operation support: %s\n",
                lt48_status_string(status));
        return 2;
    }

    if (support.availability == LT48_OPERATION_EXECUTABLE ||
        support.availability == LT48_OPERATION_PLAN_ONLY) {
        if (!have_start || !have_length || length == 0u) {
            fprintf(stderr, "labtool48: %s requires --start and nonzero --length\n",
                    support.name);
            return 2;
        }
        if (operation == LT48_OPERATION_READ) {
            if (image_path != NULL || have_margin_option) return 2;
            return catalogue_plan_read(
                path, selector, start, length, report_path, json
            );
        }
        if (operation == LT48_OPERATION_BLANK_CHECK) {
            if (image_path != NULL || have_margin_option) return 2;
            return catalogue_plan_compare(
                path, selector, start, length, NULL, report_path,
                LT48_OPERATION_BLANK_CHECK, json
            );
        }
        if (operation == LT48_OPERATION_VERIFY) {
            if (image_path == NULL) {
                fprintf(stderr, "labtool48: verify requires --image FILE\n");
                return 2;
            }
            return catalogue_plan_verify_margins(
                path, selector, start, length, image_path, report_path,
                margin, margin_type, json
            );
        }
        if (operation == LT48_OPERATION_COMPARE) {
            if (image_path == NULL || have_margin_option) {
                fprintf(stderr, "labtool48: compare requires --image FILE\n");
                return 2;
            }
            return catalogue_plan_compare(
                path, selector, start, length, image_path, report_path,
                LT48_OPERATION_COMPARE, json
            );
        }
        if (operation == LT48_OPERATION_PROGRAM ||
            operation == LT48_OPERATION_OVER_PROGRAM) {
            if (image_path == NULL || have_margin_option) {
                fprintf(stderr, "labtool48: program requires --image FILE\n");
                return 2;
            }
            return catalogue_plan_program(
                path, selector, start, length, image_path, report_path, json
            );
        }
    }

    if (json) {
        fputs("{\"schema\":1,\"operation\":\"operation-plan\","
              "\"selector\":", stdout);
        print_json_string(selector);
        fputs(",\"requested_operation\":", stdout);
        print_json_string(support.name);
        fputs(",\"availability\":", stdout);
        print_json_string(availability_name(support.availability));
        fputs(",\"support_level\":", stdout);
        print_json_string(support.support_level);
        fputs(",\"reason\":", stdout);
        print_json_string(support.reason);
        printf(",\"mutates_device\":%s,"
               "\"definition_callback_present\":%s,"
               "\"definition_callback\":",
               support.mutates_device ? "true" : "false",
               support.definition_callback_present ? "true" : "false");
        if (support.definition_callback_present) {
            print_json_string(support.definition_callback);
            printf(",\"definition_wave\":%u", support.definition_wave);
        } else {
            fputs("null,\"definition_wave\":null", stdout);
        }
        printf(",\"plan_available\":false,"
               "\"executable\":false,\"usb_opened\":false}\n");
    } else {
        printf("%s %s: %s (%s)\n  %s\n",
               selector, support.name,
               availability_name(support.availability), support.support_level,
               support.reason);
        if (support.availability == LT48_OPERATION_HOST_ONLY)
            puts("  Use the corresponding host-only command; no USB opened.");
        else
            puts("  No safe native plan is available; no USB opened.");
    }
    return support.availability == LT48_OPERATION_HOST_ONLY ? 0 : 3;
}

static int operation_plan_command(
    int argc, char **argv, int installation_index, int selector_index,
    int operation_index, int option_index
) {
    lt48_operation_kind operation;
    uint32_t start = 0u;
    uint32_t length = 0u;
    int have_start = 0;
    int have_length = 0;
    int json = 0;
    lt48_verify_margin_selection margin = LT48_VERIFY_MARGIN_ALL;
    unsigned margin_type = 0u;
    int have_margin = 0;
    int have_margin_type = 0;
    const char *image_path = NULL;
    const char *report_path = NULL;
    if (lt48_operation_kind_parse(argv[operation_index], &operation) !=
        LT48_OK) return 2;
    for (int index = option_index; index < argc; ++index) {
        if (strcmp(argv[index], "--json") == 0 && !json) {
            json = 1;
        } else if (strcmp(argv[index], "--start") == 0 &&
                   index + 1 < argc && !have_start &&
                   parse_u32(argv[++index], &start)) {
            have_start = 1;
        } else if (strcmp(argv[index], "--length") == 0 &&
                   index + 1 < argc && !have_length &&
                   parse_u32(argv[++index], &length)) {
            have_length = 1;
        } else if ((strcmp(argv[index], "--image") == 0 ||
                    strcmp(argv[index], "--input") == 0) &&
                   index + 1 < argc && image_path == NULL) {
            image_path = argv[++index];
        } else if (strcmp(argv[index], "--report") == 0 &&
                   index + 1 < argc && report_path == NULL) {
            report_path = argv[++index];
        } else if (strcmp(argv[index], "--margin") == 0 &&
                   index + 1 < argc && !have_margin) {
            const char *value = argv[++index];
            have_margin = 1;
            if (strcmp(value, "nominal") == 0)
                margin = LT48_VERIFY_MARGIN_NOMINAL;
            else if (strcmp(value, "low") == 0)
                margin = LT48_VERIFY_MARGIN_LOW;
            else if (strcmp(value, "high") == 0)
                margin = LT48_VERIFY_MARGIN_HIGH;
            else if (strcmp(value, "all") == 0)
                margin = LT48_VERIFY_MARGIN_ALL;
            else return 2;
        } else if (strcmp(argv[index], "--margin-type") == 0 &&
                   index + 1 < argc && !have_margin_type) {
            uint32_t parsed;
            have_margin_type = 1;
            if (!parse_u32(argv[++index], &parsed) || parsed > 1u) return 2;
            margin_type = (unsigned)parsed;
        } else {
            return 2;
        }
    }
    return operation_plan_gate(
        argv[installation_index], argv[selector_index], operation,
        have_start, start, have_length, length, image_path, report_path,
        margin, margin_type, have_margin || have_margin_type, json
    );
}

static int top_level_plan_command(
    int argc, char **argv, const char *operation_name, int option_start
) {
    char **adapted;
    int adapted_argc;
    int result;
    int execute_requested = 0;
    if (argc < option_start || operation_name == NULL) return 2;
    for (int index = option_start; index < argc; ++index) {
        if (strcmp(argv[index], "--execute") == 0) {
            if (execute_requested) return 2;
            execute_requested = 1;
        }
    }
    adapted_argc = argc - option_start + 5 - execute_requested;
    adapted = calloc((size_t)adapted_argc, sizeof(*adapted));
    if (adapted == NULL) return 6;
    adapted[0] = argv[0];
    adapted[1] = (char *)"plan";
    adapted[2] = argv[2];
    adapted[3] = argv[3];
    adapted[4] = (char *)operation_name;
    for (int source = option_start, target = 5; source < argc; ++source) {
        if (strcmp(argv[source], "--execute") == 0) continue;
        adapted[target] = strcmp(argv[source], "--input") == 0 ?
            (char *)"--image" : argv[source];
        ++target;
    }
    result = operation_plan_command(
        adapted_argc, adapted, 2, 3, 4, 5
    );
    free(adapted);
    return execute_requested && result == 0 ? 3 : result;
}

static int top_level_read_command(int argc, char **argv) {
    char **adapted;
    int result;
    if (argc < 7) return 2;
    adapted = calloc((size_t)argc + 1u, sizeof(*adapted));
    if (adapted == NULL) return 6;
    adapted[0] = argv[0];
    adapted[1] = (char *)"programmer";
    adapted[2] = (char *)"read-m37";
    for (int index = 2; index < argc; ++index)
        adapted[index + 1] = argv[index];
    result = programmer_read_m37(argc + 1, adapted);
    free(adapted);
    return result;
}

static int catalogue_plan_verify_margins(
    const char *path, const char *selector, uint32_t start, uint32_t length,
    const char *image_path, const char *report_path,
    lt48_verify_margin_selection margin, unsigned margin_type, int json
) {
    static const char *const margin_names[] = {
        "nominal", "low", "high", "all"
    };
    lt48_installation *installation = NULL;
    lt48_m37_resources *resources = NULL;
    lt48_m37_verify_plan_set *set = NULL;
    lt48_catalogue_entry entry;
    lt48_verify_voltage_profile profile;
    unsigned char *image = NULL;
    unsigned char image_digest[LT48_SHA256_SIZE];
    unsigned char combined_digest[LT48_SHA256_SIZE];
    uint16_t voltages[3];
    size_t voltage_count = 0u;
    size_t selected = 0u;
    size_t matches = 0u;
    lt48_status status;
    status = read_exact_file(image_path, length, &image);
    if (status != LT48_OK) {
        fprintf(stderr,
                "labtool48: verify image must contain exactly %u bytes: %s\n",
                length, lt48_status_string(status));
        return 2;
    }
    lt48_sha256(image, length, image_digest);
    status = lt48_installation_open(path, &installation);
    if (status != LT48_OK) goto fail;
    for (size_t index = 0u;
         index < lt48_installation_device_count(installation); ++index) {
        status = lt48_installation_get_device(installation, index, &entry);
        if (status != LT48_OK) goto fail;
        if (catalogue_key_equals(&entry, selector)) {
            selected = index;
            ++matches;
        }
    }
    if (matches != 1u) {
        status = LT48_NOT_FOUND;
        goto fail;
    }
    status = lt48_installation_verify_voltage_profile(
        installation, selected, margin_type, &profile
    );
    if (status == LT48_OK)
        status = lt48_verify_voltage_profile_select(
            &profile, margin, voltages,
            sizeof(voltages) / sizeof(voltages[0]), &voltage_count
        );
    if (status == LT48_OK)
        status = lt48_m37_resources_open(installation, selected, &resources);
    if (status == LT48_OK)
        status = lt48_m37_verify_plan_set_create(
            resources, start, image, length, voltages, voltage_count, &set
        );
    if (status == LT48_OK)
        status = lt48_m37_verify_plan_set_sha256(set, combined_digest);
    if (status == LT48_OK && report_path != NULL)
        status = lt48_m37_operation_report_write_verify_plan_set(
            report_path, installation, selected, resources, set,
            start, length, image_digest
        );
    if (status != LT48_OK) goto fail;
    if (json) {
        fputs("{\"schema\":1,\"operation\":\"verify\",\"selector\":",
              stdout);
        print_json_string(selector);
        printf(",\"start\":%u,\"length\":%u,\"margin\":",
               start, length);
        print_json_string(margin_names[margin]);
        printf(",\"margin_type\":%u,\"voltage_profile\":{"
               "\"nominal_centivolts\":%u,\"low_centivolts\":%u,"
               "\"high_centivolts\":%u,\"explicit_values_centivolts\":[",
               margin_type, profile.nominal_centivolts,
               profile.low_centivolts, profile.high_centivolts);
        for (size_t index = 0u; index < profile.explicit_value_count; ++index)
            printf("%s%u", index == 0u ? "" : ",",
                   profile.explicit_values_centivolts[index]);
        printf("]},\"pass_count\":%zu,\"passes\":[", voltage_count);
        for (size_t index = 0u; index < voltage_count; ++index) {
            const lt48_protocol_plan *pass;
            unsigned char digest[LT48_SHA256_SIZE];
            uint16_t voltage;
            (void)lt48_m37_verify_plan_set_get(set, index, &voltage, &pass);
            (void)lt48_protocol_plan_sha256(pass, digest);
            printf("%s{\"voltage_centivolts\":%u,\"transactions\":%zu,"
                   "\"plan_sha256\":\"", index == 0u ? "" : ",",
                   voltage, lt48_protocol_plan_transaction_count(pass));
            print_hex(digest, LT48_SHA256_SIZE);
            fputs("\"}", stdout);
        }
        fputs("],\"image\":", stdout);
        print_json_string(image_path);
        fputs(",\"image_sha256\":\"", stdout);
        print_hex(image_digest, LT48_SHA256_SIZE);
        fputs("\",\"combined_plan_sha256\":\"", stdout);
        print_hex(combined_digest, LT48_SHA256_SIZE);
        fputs("\",\"support_level\":\"planned\",\"executable\":false,"
              "\"report\":", stdout);
        if (report_path == NULL) fputs("null", stdout);
        else print_json_string(report_path);
        fputs(",\"offline\":true,\"usb_opened\":false}\n", stdout);
    } else {
        printf("%s verify 0x%x..0x%x: %zu independent %s-margin pass%s\n",
               selector, start, start + length, voltage_count,
               margin_names[margin], voltage_count == 1u ? "" : "es");
        for (size_t index = 0u; index < voltage_count; ++index) {
            const lt48_protocol_plan *pass;
            unsigned char digest[LT48_SHA256_SIZE];
            uint16_t voltage;
            (void)lt48_m37_verify_plan_set_get(set, index, &voltage, &pass);
            (void)lt48_protocol_plan_sha256(pass, digest);
            printf("  %u.%02u V: %zu transactions; SHA-256 ",
                   voltage / 100u, voltage % 100u,
                   lt48_protocol_plan_transaction_count(pass));
            print_hex(digest, LT48_SHA256_SIZE);
            putchar('\n');
        }
        fputs("combined plan SHA-256: ", stdout);
        print_hex(combined_digest, LT48_SHA256_SIZE);
        puts("\nplanned support only; execution disabled; no USB opened");
        if (report_path != NULL) printf("report: %s\n", report_path);
    }
    lt48_m37_verify_plan_set_close(set);
    lt48_m37_resources_close(resources);
    lt48_installation_close(installation);
    free(image);
    return 0;

fail:
    fprintf(stderr, "labtool48: cannot plan verify margins: %s\n",
            lt48_status_string(status));
    lt48_m37_verify_plan_set_close(set);
    lt48_m37_resources_close(resources);
    lt48_installation_close(installation);
    free(image);
    return status == LT48_SAFETY_REJECTED ? 4 : 2;
}

static int catalogue_plan_program(
    const char *path, const char *selector, uint32_t start, uint32_t length,
    const char *image_path, const char *report_path, int json
) {
    static const char *const phase_names[] = {
        "blank-precheck", "program", "mandatory-readback"
    };
    lt48_installation *installation = NULL;
    lt48_m37_resources *resources = NULL;
    lt48_m37_program_plan_set *set = NULL;
    lt48_catalogue_entry entry;
    unsigned char *image = NULL;
    unsigned char image_digest[LT48_SHA256_SIZE];
    unsigned char combined_digest[LT48_SHA256_SIZE];
    size_t selected = 0u;
    size_t matches = 0u;
    lt48_status status;
    status = read_exact_file(image_path, length, &image);
    if (status != LT48_OK) {
        fprintf(stderr,
                "labtool48: program image must contain exactly %u bytes: %s\n",
                length, lt48_status_string(status));
        return 2;
    }
    lt48_sha256(image, length, image_digest);
    status = lt48_installation_open(path, &installation);
    if (status != LT48_OK) goto fail;
    for (size_t index = 0u;
         index < lt48_installation_device_count(installation); ++index) {
        status = lt48_installation_get_device(installation, index, &entry);
        if (status != LT48_OK) goto fail;
        if (catalogue_key_equals(&entry, selector)) {
            selected = index;
            ++matches;
        }
    }
    if (matches != 1u) {
        status = LT48_NOT_FOUND;
        goto fail;
    }
    status = lt48_m37_resources_open(installation, selected, &resources);
    if (status == LT48_OK)
        status = lt48_m37_program_plan_set_create(
            resources, start, image, length, &set
        );
    if (status == LT48_OK)
        status = lt48_m37_program_plan_set_sha256(set, combined_digest);
    if (status == LT48_OK && report_path != NULL)
        status = lt48_m37_operation_report_write_program_plan_set(
            report_path, installation, selected, resources, set,
            start, length, image_digest
        );
    if (status != LT48_OK) goto fail;
    if (json) {
        fputs("{\"schema\":1,\"operation\":\"program\",\"selector\":",
              stdout);
        print_json_string(selector);
        printf(",\"start\":%u,\"length\":%u,\"precheck\":\"blank\","
               "\"selected_device_interlock\":\"required\","
               "\"post_verify\":\"mandatory\",\"phase_count\":%zu,"
               "\"phases\":[", start, length, lt48_m37_program_plan_set_count(set));
        for (size_t index = 0u; index < lt48_m37_program_plan_set_count(set); ++index) {
            const lt48_protocol_plan *phase = NULL;
            lt48_m37_operation operation;
            unsigned char digest[LT48_SHA256_SIZE];
            (void)lt48_m37_program_plan_set_get(
                set, index, &operation, &phase
            );
            (void)operation;
            (void)lt48_protocol_plan_sha256(phase, digest);
            printf("%s{\"name\":", index == 0u ? "" : ",");
            print_json_string((index==lt48_m37_program_plan_set_count(set)-1 ? phase_names[2] : index==2 ? "program-verify" : phase_names[index]));
            printf(",\"transactions\":%zu,\"plan_sha256\":\"",
                   lt48_protocol_plan_transaction_count(phase));
            print_hex(digest, LT48_SHA256_SIZE);
            fputs("\"}", stdout);
        }
        fputs("],\"image\":", stdout);
        print_json_string(image_path);
        fputs(",\"image_sha256\":\"", stdout);
        print_hex(image_digest, LT48_SHA256_SIZE);
        fputs("\",\"combined_plan_sha256\":\"", stdout);
        print_hex(combined_digest, LT48_SHA256_SIZE);
        fputs("\",\"support_level\":\"planned\",\"executable\":false,"
              "\"report\":", stdout);
        if (report_path == NULL) fputs("null", stdout);
        else print_json_string(report_path);
        fputs(",\"offline\":true,\"usb_opened\":false}\n", stdout);
    } else {
        printf("%s program 0x%x..0x%x: blank precheck, mode-2 program, "
               "mandatory nominal readback\n", selector, start,
               start + length);
        for (size_t index = 0u; index < lt48_m37_program_plan_set_count(set); ++index) {
            const lt48_protocol_plan *phase = NULL;
            lt48_m37_operation operation;
            unsigned char digest[LT48_SHA256_SIZE];
            (void)lt48_m37_program_plan_set_get(
                set, index, &operation, &phase
            );
            (void)operation;
            (void)lt48_protocol_plan_sha256(phase, digest);
            printf("  %s: %zu transactions; SHA-256 ",
                   (index==lt48_m37_program_plan_set_count(set)-1 ? phase_names[2] : index==2 ? "program-verify" : phase_names[index]),
                   lt48_protocol_plan_transaction_count(phase));
            print_hex(digest, LT48_SHA256_SIZE);
            putchar('\n');
        }
        fputs("combined plan SHA-256: ", stdout);
        print_hex(combined_digest, LT48_SHA256_SIZE);
        puts("\nselected-device interlock required; execution disabled; "
             "no USB opened");
        if (report_path != NULL) printf("report: %s\n", report_path);
    }
    lt48_m37_program_plan_set_close(set);
    lt48_m37_resources_close(resources);
    lt48_installation_close(installation);
    free(image);
    return 0;

fail:
    fprintf(stderr, "labtool48: cannot plan program: %s\n",
            lt48_status_string(status));
    lt48_m37_program_plan_set_close(set);
    lt48_m37_resources_close(resources);
    lt48_installation_close(installation);
    free(image);
    return status == LT48_SAFETY_REJECTED ? 4 : 2;
}

static int catalogue_plan_compare(
    const char *path, const char *selector, uint32_t start, uint32_t length,
    const char *image_path, const char *report_path,
    lt48_operation_kind requested_operation, int json
) {
    const int verify = requested_operation == LT48_OPERATION_VERIFY;
    const int device_compare = requested_operation == LT48_OPERATION_COMPARE;
    const int has_image = verify || device_compare;
    const char *operation = device_compare ? "compare" :
                            verify ? "verify" : "blank-check";
    lt48_installation *installation = NULL;
    lt48_m37_resources *resources = NULL;
    lt48_protocol_plan *plan = NULL;
    lt48_catalogue_entry entry;
    unsigned char *image = NULL;
    unsigned char image_digest[LT48_SHA256_SIZE] = {0};
    unsigned char plan_digest[LT48_SHA256_SIZE];
    size_t selected = 0u;
    size_t matches = 0u;
    size_t transactions;
    lt48_status status;
    if (has_image) {
        status = read_exact_file(image_path, length, &image);
        if (status != LT48_OK) {
            fprintf(stderr,
                    "labtool48: %s image must contain exactly %u bytes: %s\n",
                    operation, length, lt48_status_string(status));
            return 2;
        }
        lt48_sha256(image, length, image_digest);
    }
    status = lt48_installation_open(path, &installation);
    if (status != LT48_OK) goto fail;
    for (size_t index = 0u;
         index < lt48_installation_device_count(installation); ++index) {
        status = lt48_installation_get_device(installation, index, &entry);
        if (status != LT48_OK) goto fail;
        if (catalogue_key_equals(&entry, selector)) {
            selected = index;
            ++matches;
        }
    }
    if (matches != 1u) {
        status = LT48_NOT_FOUND;
        goto fail;
    }
    status = lt48_m37_resources_open(installation, selected, &resources);
    if (status == LT48_OK) {
        status = device_compare ? lt48_m37_resources_build_compare_plan(
                                      resources, start, image, length, &plan) :
                 verify ? lt48_m37_resources_build_verify_plan(
                              resources, start, image, length, &plan) :
                          lt48_m37_resources_build_blank_check_plan(
                              resources, start, length, &plan);
    }
    if (status != LT48_OK) goto fail;
    transactions = lt48_protocol_plan_transaction_count(plan);
    status = lt48_protocol_plan_sha256(plan, plan_digest);
    if (status != LT48_OK) goto fail;
    status = write_m37_planned_report(
        report_path, installation, selected, resources, plan,
        device_compare ? LT48_M37_COMPARE :
        verify ? LT48_M37_VERIFY : LT48_M37_BLANK_CHECK,
        start, length,
        has_image ? image_digest : NULL
    );
    if (status != LT48_OK) goto fail;
    if (json) {
        fputs("{\"schema\":1,\"operation\":", stdout);
        print_json_string(operation);
        fputs(",\"selector\":", stdout);
        print_json_string(selector);
        printf(",\"start\":%u,\"length\":%u,\"transactions\":%zu",
               start, length, transactions);
        if (has_image) {
            fputs(",\"image\":", stdout);
            print_json_string(image_path);
            fputs(",\"image_sha256\":\"", stdout);
            print_hex(image_digest, LT48_SHA256_SIZE);
            putchar('"');
        } else {
            fputs(",\"blank_value\":255", stdout);
        }
        fputs(",\"plan_sha256\":\"", stdout);
        print_hex(plan_digest, LT48_SHA256_SIZE);
        fputs("\",\"support_level\":\"planned\",\"executable\":false,"
              "\"report\":", stdout);
        if (report_path == NULL) fputs("null", stdout);
        else print_json_string(report_path);
        fputs(",\"offline\":true,\"usb_opened\":false}\n", stdout);
    } else {
        printf("%s %s 0x%x..0x%x: %zu transactions\n",
               selector, operation, start, start + length, transactions);
        if (has_image) {
            fputs("image SHA-256: ", stdout);
            print_hex(image_digest, LT48_SHA256_SIZE);
            putchar('\n');
        } else {
            puts("definition-backed M37 blank value: 0xff");
        }
        fputs("plan SHA-256:  ", stdout);
        print_hex(plan_digest, LT48_SHA256_SIZE);
        puts("\nplanned support only; execution disabled; no USB opened");
        if (report_path != NULL) printf("report: %s\n", report_path);
    }
    free(image);
    lt48_protocol_plan_close(plan);
    lt48_m37_resources_close(resources);
    lt48_installation_close(installation);
    return 0;

fail:
    fprintf(stderr, "labtool48: cannot plan %s: %s\n", operation,
            lt48_status_string(status));
    free(image);
    lt48_protocol_plan_close(plan);
    lt48_m37_resources_close(resources);
    lt48_installation_close(installation);
    return status == LT48_SAFETY_REJECTED ? 4 : 2;
}

static lt48_status prepare_read_execution_report(
    const char *path, const char *selector, uint32_t start, uint32_t length,
    const unsigned char plan_sha256[LT48_SHA256_SIZE]
) {
    lt48_operation_report *report = NULL;
    lt48_operation_report_info info;
    lt48_status status;
    int matches;
    if (access(path, F_OK) != 0) {
        return errno == ENOENT ? LT48_OK : LT48_FILE_ERROR;
    }
    status = lt48_operation_report_open(path, &report);
    if (status == LT48_OK)
        status = lt48_operation_report_get_info(report, &info);
    matches = status == LT48_OK && info.status != NULL &&
        strcmp(info.status, "planned") == 0 && info.operation != NULL &&
        strcmp(info.operation, "read") == 0 && info.device_key != NULL &&
        strcmp(info.device_key, selector) == 0 && info.has_address_range &&
        info.address_start == start && info.address_length == length &&
        info.plan_sha256 != NULL &&
        digest_text_equals(info.plan_sha256, plan_sha256);
    lt48_operation_report_close(report);
    if (!matches) return LT48_FILE_ERROR;
    return remove(path) == 0 ? LT48_OK : LT48_FILE_ERROR;
}

static int programmer_read_m37(int argc, char **argv) {
    const char *installation_path = argv[3];
    const char *selector = argv[4];
    const char *output_path = argv[7];
    const char *confirmation = NULL;
    const char *report_path = NULL;
    uint32_t start;
    uint32_t length;
    uint32_t parsed_timeout = 2000u;
    uint8_t bus = 0u;
    uint8_t address = 0u;
    int have_bus = 0;
    int have_address = 0;
    int execute = 0;
    int json = 0;
    lt48_installation *installation = NULL;
    lt48_m37_resources *resources = NULL;
    lt48_protocol_plan *plan = NULL;
    lt48_protocol_plan *recovery[3] = {NULL,NULL,NULL};
    lt48_m37_read_collector *collector = NULL;
    lt48_context *context = NULL;
    lt48_programmer *programmer = NULL;
    lt48_catalogue_entry entry;
    const lt48_m37_resource_audit *audit;
    lt48_cleanup_result finalization[12];
    lt48_m37_snapshot_read_execution_result execution = {0};
    lt48_m37_read_preparation preparation;
    unsigned char plan_digest[LT48_SHA256_SIZE];
    unsigned char confirmation_digest[LT48_SHA256_SIZE] = {0};
    unsigned char recovery_digest[3][LT48_SHA256_SIZE] = {{0}};
    size_t recovery_transactions[3] = {0u,0u,0u};
    size_t selected = 0u;
    size_t matches = 0u;
    size_t chunks = 0u;
    size_t bytes = 0u;
    size_t transactions;
    const unsigned char *read_data = NULL;
    size_t read_size = 0u;
    uint32_t read_start = 0u;
    lt48_status status;
    int result = 2;
    int execution_authorized = 0;
    int execution_succeeded = 0;

    if (!parse_u32(argv[5], &start) || !parse_u32(argv[6], &length)) {
        return 2;
    }
    for (int index = 8; index < argc; ++index) {
        if (strcmp(argv[index], "--json") == 0 && !json) {
            json = 1;
        } else if (strcmp(argv[index], "--execute") == 0 && !execute) {
            execute = 1;
        } else if (strcmp(argv[index], "--bus") == 0 &&
                   index + 1 < argc && !have_bus) {
            have_bus = parse_u8(argv[++index], &bus);
            if (!have_bus) return 2;
        } else if (strcmp(argv[index], "--address") == 0 &&
                   index + 1 < argc && !have_address) {
            have_address = parse_u8(argv[++index], &address);
            if (!have_address) return 2;
        } else if (strcmp(argv[index], "--timeout") == 0 &&
                   index + 1 < argc) {
            if (!parse_u32(argv[++index], &parsed_timeout) ||
                parsed_timeout == 0u) return 2;
        } else if (strcmp(argv[index], "--confirm") == 0 &&
                   index + 1 < argc && confirmation == NULL) {
            confirmation = argv[++index];
        } else if (strcmp(argv[index], "--report") == 0 &&
                   index + 1 < argc && report_path == NULL) {
            report_path = argv[++index];
        } else {
            return 2;
        }
    }
    if (have_bus != have_address || (!execute && confirmation != NULL) ||
        (execute && (!have_bus || confirmation == NULL ||
                     report_path == NULL))) return 2;

    status = lt48_installation_open(installation_path, &installation);
    if (status != LT48_OK) goto fail;
    for (size_t index = 0u;
         index < lt48_installation_device_count(installation); ++index) {
        status = lt48_installation_get_device(installation, index, &entry);
        if (status != LT48_OK) goto fail;
        if (catalogue_key_equals(&entry, selector)) {
            selected = index;
            ++matches;
        }
    }
    if (matches != 1u) {
        status = LT48_NOT_FOUND;
        goto fail;
    }
    status = lt48_m37_resources_open(installation, selected, &resources);
    if (status != LT48_OK) goto fail;
    audit = lt48_m37_resources_audit(resources);
    if (audit == NULL) {
        status = LT48_SAFETY_REJECTED;
        goto fail;
    }
    if (!audit->read_behavior_approved) {
        status = LT48_UNSUPPORTED;
        goto fail;
    }
    status = lt48_m37_resources_build_read_plan(
        resources, start, length, &plan
    );
    if (status != LT48_OK) goto fail;
    transactions = lt48_protocol_plan_transaction_count(plan);
    for (size_t index = 0u; index < transactions; ++index) {
        lt48_protocol_transaction transaction;
        status = lt48_protocol_plan_get_transaction(plan, index, &transaction);
        if (status != LT48_OK) goto fail;
        if (transaction.reads_device_data) {
            ++chunks;
            bytes += transaction.length;
        }
    }
    status = lt48_protocol_plan_sha256(plan, plan_digest);
    if (status != LT48_OK) goto fail;
    for (size_t state = 0u; state < 3u; ++state) {
        status = lt48_m37_resources_build_recovery_plan(
            resources, (lt48_m37_recovery_state)state, &recovery[state]
        );
        if (status != LT48_OK) goto fail;
        recovery_transactions[state] =
            lt48_protocol_plan_transaction_count(recovery[state]);
        status = lt48_protocol_plan_sha256(
            recovery[state], recovery_digest[state]
        );
        if (status != LT48_OK) goto fail;
    }
    if (have_bus) {
        status = lt48_m37_prepare_read(
            resources, plan, selector, output_path, bus, address,
            start, length, parsed_timeout, &preparation
        );
        if (status != LT48_OK) goto fail;
        memcpy(confirmation_digest, preparation.confirmation_sha256,
               LT48_SHA256_SIZE);
    }

    if (!execute) {
        status = write_m37_planned_report(
            report_path, installation, selected, resources, plan,
            LT48_M37_READ, start, length, NULL
        );
        if (status != LT48_OK) goto fail;
        if (json) {
            fputs("{\"schema\":1,\"mode\":\"dry-run\",\"selector\":", stdout);
            print_json_string(selector);
            printf(",\"start\":%u,\"length\":%u,\"output\":", start, length);
            print_json_string(output_path);
            printf(",\"bus\":");
            if (have_bus) printf("%u", bus); else fputs("null", stdout);
            fputs(",\"address\":", stdout);
            if (have_address) printf("%u", address); else fputs("null", stdout);
            printf(",\"timeout_ms\":%u,\"transactions\":%zu,"
                   "\"read_chunks\":%zu,\"read_bytes\":%zu,"
                   "\"plan_sha256\":\"", parsed_timeout, transactions,
                   chunks, bytes);
            print_hex(plan_digest, LT48_SHA256_SIZE);
            fputs("\",\"manifest_sha256\":\"", stdout);
            print_hex(audit->resource_manifest_sha256, LT48_SHA256_SIZE);
            fputs("\",\"definition_sha256\":\"", stdout);
            print_hex(audit->definition_sha256, LT48_SHA256_SIZE);
            fputs("\",\"recovery_plans\":[", stdout);
            for (size_t state = 0u; state < 3u; ++state) {
                printf("%s{\"state\":%zu,\"transactions\":%zu,"
                       "\"sha256\":\"", state == 0u ? "" : ",",
                       state, recovery_transactions[state]);
                print_hex(recovery_digest[state], LT48_SHA256_SIZE);
                fputs("\"}", stdout);
            }
            fputs("],\"confirmation_sha256\":", stdout);
            if (have_bus) {
                putchar('"');
                print_hex(confirmation_digest, LT48_SHA256_SIZE);
                putchar('"');
            } else {
                fputs("null", stdout);
            }
            fputs(",\"report\":", stdout);
            if (report_path == NULL) fputs("null", stdout);
            else print_json_string(report_path);
            printf(",\"live_ready\":%s,\"usb_opened\":false}\n",
                   have_bus ? "true" : "false");
        } else {
            printf("DRY RUN: %s read 0x%x..0x%x to %s\n",
                   selector, start, start + length, output_path);
            printf("%zu transactions, %zu chunks, %zu bytes; timeout %u ms\n",
                   transactions, chunks, bytes, parsed_timeout);
            fputs("plan SHA-256:     ", stdout);
            print_hex(plan_digest, LT48_SHA256_SIZE);
            fputs("\nmanifest SHA-256: ", stdout);
            print_hex(audit->resource_manifest_sha256, LT48_SHA256_SIZE);
            if (have_bus) {
                printf("\ntarget: %03u:%03u\nconfirmation: ", bus, address);
                print_hex(confirmation_digest, LT48_SHA256_SIZE);
                puts("\nNo USB device opened. To execute, repeat this exact command"
                     " with --execute --confirm <confirmation>.");
            } else {
                puts("\nNo target selected and no USB device opened. Add --bus and"
                     " --address to produce an executable confirmation token.");
            }
            if (report_path != NULL) printf("report: %s\n", report_path);
        }
        result = 0;
        goto cleanup;
    }

    if (!digest_text_equals(confirmation, confirmation_digest)) {
        fprintf(stderr, "labtool48: confirmation does not match this exact read\n");
        status = LT48_SAFETY_REJECTED;
        result = 4;
        goto cleanup;
    }
    if (access(output_path, F_OK) == 0 || errno != ENOENT) {
        fprintf(stderr, "labtool48: output already exists or cannot be checked: %s\n",
                output_path);
        status = LT48_FILE_ERROR;
        goto fail;
    }
    status = prepare_read_execution_report(
        report_path, selector, start, length, plan_digest
    );
    if (status != LT48_OK) {
        fprintf(stderr,
                "labtool48: report exists but is not the matching planned "
                "report, or cannot be prepared: %s\n",
                report_path);
        goto fail;
    }
    execution_authorized = 1;
    status = lt48_m37_read_collector_create(plan, &collector);
    if (status != LT48_OK) goto fail;
    status = lt48_context_create(&context);
    if (status != LT48_OK) goto hardware_fail;
    status = lt48_programmer_open(context, bus, address, &programmer);
    if (status != LT48_OK) goto hardware_fail;
    {
        lt48_transport_info ready_info = {0};
        status = lt48_programmer_transport_initialize(
            programmer, (unsigned)parsed_timeout, &ready_info
        );
        if (status != LT48_OK) goto hardware_fail;
    }
    status = lt48_programmer_arm_m37_read(
        programmer, resources, LT48_M37_READ_HARDWARE_AUTHORIZATION
    );
    if (status != LT48_OK) goto hardware_fail;
    status = lt48_programmer_execute_m37_read(
        programmer, resources, plan, collector, (unsigned)parsed_timeout,
        finalization, sizeof(finalization) / sizeof(finalization[0]),
        &execution
    );
    if (status != LT48_OK) goto hardware_fail;
    status = lt48_m37_read_collector_data(
        collector, &read_start, &read_data, &read_size
    );
    if (status != LT48_OK || read_start != start || read_size != length) {
        status = status == LT48_OK ? LT48_PROTOCOL_ERROR : status;
        goto hardware_fail;
    }
    status = write_new_file(output_path, read_data, read_size);
    if (status != LT48_OK) goto fail;
    execution_succeeded = 1;
    result = 0;
    goto cleanup;

hardware_fail:
    fprintf(stderr, "labtool48: hardware read failed: %s\n",
            lt48_status_string(status));
    if (execution.operation_status != LT48_OK ||
        execution.finalization_phase_count != 0u) {
        fprintf(stderr,
                "labtool48: operation=%s completed=%zu; "
                "finalization=%s recovery=%s state=%d completed=%zu "
                "phases=%zu failures=%zu\n",
                lt48_status_string(execution.operation_status),
                execution.operation_completed_transactions,
                lt48_status_string(execution.finalization_status),
                execution.used_recovery_plan ? "yes" : "no",
                (int)execution.recovery_state,
                execution.recovery_completed_transactions,
                execution.finalization_phase_count,
                execution.finalization_failure_count);
        for (size_t phase = 0u;
             phase < execution.finalization_phase_count &&
             phase < sizeof(finalization) / sizeof(finalization[0]); ++phase) {
            fprintf(stderr, "labtool48: finalization[%zu] %s: %s\n",
                    phase,
                    finalization[phase].name == NULL ?
                        "unnamed" : finalization[phase].name,
                    lt48_status_string(finalization[phase].status));
        }
    }
    result = (status == LT48_SAFETY_REJECTED ||
              status == LT48_FIRMWARE_REQUIRED) ? 4 : 6;
    goto cleanup;

fail:
    fprintf(stderr, "labtool48: cannot prepare read: %s\n",
            lt48_status_string(status));
    result = status == LT48_UNSUPPORTED ? 3 :
             status == LT48_SAFETY_REJECTED ? 4 : 2;

cleanup:
    if (execution_authorized) {
        lt48_status report_status;
        lt48_status command_status = result == 0 ? LT48_OK : status;
        if (execution.finalization_phase_count == 0u &&
            execution.operation_status == LT48_OK &&
            command_status != LT48_OK)
            execution.operation_status = command_status;
        report_status = lt48_m37_operation_report_write_execution(
            report_path, installation, selected, resources, plan,
            start, length, command_status, &execution, finalization,
            execution.finalization_phase_count,
            read_data, read_data == NULL ? 0u : read_size
        );
        if (report_status != LT48_OK) {
            fprintf(stderr, "labtool48: execution report was not written: %s\n",
                    lt48_status_string(report_status));
            if (result == 0) result = 2;
        }
    }
    if (execution_succeeded && result == 0) {
        if (json) {
            fputs("{\"schema\":1,\"mode\":\"executed\",\"selector\":", stdout);
            print_json_string(selector);
            printf(",\"bus\":%u,\"address\":%u,\"start\":%u,"
                   "\"length\":%u,\"output\":", bus, address,
                   start, length);
            print_json_string(output_path);
            printf(",\"bytes_written\":%zu,\"operation_status\":%d,"
                   "\"finalization_status\":%d,\"recovery_used\":%s,"
                   "\"report\":",
                   read_size, execution.operation_status,
                   execution.finalization_status,
                   execution.used_recovery_plan ? "true" : "false");
            print_json_string(report_path);
            puts("}");
        } else {
            printf("Read %zu bytes from %s through %03u:%03u into %s\n",
                   read_size, selector, bus, address, output_path);
            printf("report: %s\n", report_path);
        }
    }
    lt48_programmer_close(programmer);
    lt48_context_destroy(context);
    lt48_m37_read_collector_close(collector);
    for (size_t state = 0u; state < 3u; ++state)
        lt48_protocol_plan_close(recovery[state]);
    lt48_protocol_plan_close(plan);
    lt48_m37_resources_close(resources);
    lt48_installation_close(installation);
    return result;
}

static int compact_audit(const char *path, int json) {
    lt48_rez *rez = NULL;
    lt48_rez_info info;
    lt48_status status = lt48_rez_open(path, &rez);
    size_t wave_count = 0;
    size_t instruction_count = 0;
    size_t opcode_counts[0xa5] = {0};
    if (status == LT48_OK) {
        status = lt48_rez_get_info(rez, &info);
    }
    if (status != LT48_OK) {
        fprintf(stderr, "labtool48: %s: %s\n", path, lt48_status_string(status));
        lt48_rez_close(rez);
        return 2;
    }
    for (size_t resource_index = 0; resource_index < info.resource_count;
         ++resource_index) {
        lt48_rez_resource resource;
        lt48_wave_code wave_code;
        lt48_compact_wave *wave = NULL;
        status = lt48_rez_get_resource(rez, resource_index, &resource);
        if (status != LT48_OK || strcmp(resource.type_name, "TWaveCode") != 0) {
            continue;
        }
        status = lt48_rez_get_wave_code(rez, resource_index, &wave_code);
        if (status != LT48_OK) {
            fprintf(stderr, "labtool48: malformed wave %s\n", resource.name);
            lt48_rez_close(rez);
            return 2;
        }
        if (wave_code.wave_type != 0) {
            continue;
        }
        status = lt48_compact_decode(wave_code.data, wave_code.data_size, &wave);
        if (status != LT48_OK) {
            fprintf(stderr, "labtool48: malformed compact wave %s\n", resource.name);
            lt48_rez_close(rez);
            return 2;
        }
        ++wave_count;
        instruction_count += lt48_compact_instruction_count(wave);
        for (size_t index = 0; index < lt48_compact_instruction_count(wave);
             ++index) {
            lt48_compact_instruction instruction;
            status = lt48_compact_get_instruction(wave, index, &instruction);
            if (status != LT48_OK || instruction.opcode >= 0xa5) {
                lt48_compact_wave_close(wave);
                lt48_rez_close(rez);
                return 2;
            }
            ++opcode_counts[instruction.opcode];
        }
        lt48_compact_wave_close(wave);
    }
    if (json) {
        printf("{\"schema\":1,\"wave_count\":%zu,"
               "\"instruction_count\":%zu,\"opcode_counts\":{",
               wave_count, instruction_count);
        {
            size_t emitted = 0;
            for (size_t opcode = 0; opcode < 0xa5; ++opcode) {
                if (opcode_counts[opcode] != 0) {
                    printf("%s\"0x%02zx\":%zu", emitted == 0 ? "" : ",",
                           opcode, opcode_counts[opcode]);
                    ++emitted;
                }
            }
        }
        fputs("}}\n", stdout);
    } else {
        printf("%zu compact waves; %zu instructions\n", wave_count,
               instruction_count);
        for (size_t opcode = 0; opcode < 0xa5; ++opcode) {
            if (opcode_counts[opcode] != 0) {
                printf("  0x%02zx: %zu\n", opcode, opcode_counts[opcode]);
            }
        }
    }
    lt48_rez_close(rez);
    return 0;
}

static int list_pin_maps(const char *path, int json) {
    lt48_rez *rez = NULL;
    lt48_rez_info info;
    lt48_status status = lt48_rez_open(path, &rez);
    size_t emitted = 0;
    if (status == LT48_OK) {
        status = lt48_rez_get_info(rez, &info);
    }
    if (status != LT48_OK) {
        fprintf(stderr, "labtool48: %s: %s\n", path, lt48_status_string(status));
        lt48_rez_close(rez);
        return 2;
    }
    if (json) {
        fputs("{\"schema\":1,\"pin_maps\":[", stdout);
    }
    for (size_t resource_index = 0; resource_index < info.resource_count;
         ++resource_index) {
        lt48_rez_resource resource;
        lt48_pin_code code;
        lt48_pin_map *map = NULL;
        status = lt48_rez_get_resource(rez, resource_index, &resource);
        if (status != LT48_OK || strcmp(resource.type_name, "TPinCode") != 0) {
            continue;
        }
        status = lt48_rez_get_pin_code(rez, resource_index, &code);
        if (status == LT48_OK) {
            status = lt48_pin_map_parse(&code, &map);
        }
        if (status != LT48_OK) {
            fprintf(stderr, "labtool48: malformed pin map %s\n", resource.name);
            lt48_pin_map_close(map);
            lt48_rez_close(rez);
            return 2;
        }
        if (json) {
            printf("%s{\"resource\":", emitted == 0 ? "" : ",");
            print_json_string(resource.name);
            printf(",\"pin_count\":%u,\"groups\":[",
                   lt48_pin_map_package_pin_count(map));
            for (size_t group_index = 0;
                 group_index < lt48_pin_map_group_count(map); ++group_index) {
                lt48_pin_group group;
                (void)lt48_pin_map_get_group(map, group_index, &group);
                printf("%s{\"kind\":%u,\"pins\":[",
                       group_index == 0 ? "" : ",", group.kind);
                for (size_t pin = 0; pin < group.pin_count; ++pin) {
                    printf("%s%u", pin == 0 ? "" : ",", group.pins[pin]);
                }
                fputs("]}", stdout);
            }
            fputs("],\"logical\":[", stdout);
            for (size_t logical_index = 0;
                 logical_index < lt48_pin_map_logical_count(map); ++logical_index) {
                lt48_logical_pin logical;
                (void)lt48_pin_map_get_logical(map, logical_index, &logical);
                printf("%s{\"number\":%u,\"engine_pin\":%u}",
                       logical_index == 0 ? "" : ",", logical.number,
                       logical.engine_pin);
            }
            fputs("]}", stdout);
        } else {
            printf("%s: %u package pins, %zu groups, %zu logical pins\n",
                   resource.name, lt48_pin_map_package_pin_count(map),
                   lt48_pin_map_group_count(map),
                   lt48_pin_map_logical_count(map));
        }
        ++emitted;
        lt48_pin_map_close(map);
    }
    if (json) {
        printf("],\"pin_map_count\":%zu}\n", emitted);
    } else {
        printf("%zu pin maps\n", emitted);
    }
    lt48_rez_close(rez);
    return 0;
}

static int compile_definition_operation(
    const char *path, const char *definition_name, uint16_t operation_id,
    lt48_pin_fallback_policy pin_policy, int json,
    const lt48_runtime_options *runtime_options, int protocol_json,
    int resumable
) {
    lt48_rez *rez = NULL;
    lt48_device_definition definition;
    lt48_family_info family;
    lt48_family_operation operation = {0};
    lt48_pin_code pin_code;
    lt48_pin_map *pin_map = NULL;
    lt48_wave_code wave_code;
    lt48_compact_wave *wave = NULL;
    lt48_compiled_wave *compiled = NULL;
    lt48_compile_variable *variables = NULL;
    lt48_status status;
    size_t index;
    size_t family_index;
    char name[32];
    size_t operation_matches = 0;
    int result = 2;

    status = lt48_rez_open(path, &rez);
    if (status == LT48_OK) {
        status = lt48_rez_find_resource(rez, definition_name, &index);
    }
    if (status == LT48_OK) {
        status = lt48_rez_get_device_definition(rez, index, &definition);
    }
    if (status != LT48_OK) {
        fprintf(stderr, "labtool48: cannot resolve definition: %s\n",
                lt48_status_string(status));
        goto cleanup;
    }
    snprintf(name, sizeof(name), "M%x", definition.family_code);
    status = lt48_rez_find_resource(rez, name, &family_index);
    if (status == LT48_OK) {
        status = lt48_rez_get_family_info(rez, family_index, &family);
    }
    if (status != LT48_OK) {
        fprintf(stderr, "labtool48: cannot resolve family %s\n", name);
        goto cleanup;
    }
    for (size_t operation_index = 0; operation_index < family.operation_count;
         ++operation_index) {
        lt48_family_operation candidate;
        status = lt48_rez_get_family_operation(
            rez, family_index, operation_index, &candidate
        );
        if (status != LT48_OK) {
            goto cleanup;
        }
        if (candidate.operation_id == operation_id) {
            operation = candidate;
            ++operation_matches;
        }
    }
    /* The original loader replaces earlier bindings for the same slot. */
    if (operation_matches == 0) {
        fprintf(stderr, "labtool48: operation %u has %zu bindings\n",
                operation_id, operation_matches);
        goto cleanup;
    }
    snprintf(name, sizeof(name), "P%x", definition.pinmap);
    status = lt48_rez_find_resource(rez, name, &index);
    if (status == LT48_OK) {
        status = lt48_rez_get_pin_code(rez, index, &pin_code);
    }
    if (status == LT48_OK) {
        status = lt48_pin_map_parse(&pin_code, &pin_map);
    }
    if (status != LT48_OK) {
        fprintf(stderr, "labtool48: cannot resolve pin map %s\n", name);
        goto cleanup;
    }
    snprintf(name, sizeof(name), "W%x", operation.wave);
    status = lt48_rez_find_resource(rez, name, &index);
    if (status == LT48_OK) {
        status = lt48_rez_get_wave_code(rez, index, &wave_code);
    }
    if (status != LT48_OK || wave_code.wave_type != 0) {
        fprintf(stderr, "labtool48: operation does not reference a compact wave\n");
        goto cleanup;
    }
    status = lt48_compact_decode(wave_code.data, wave_code.data_size, &wave);
    if (status != LT48_OK) {
        fprintf(stderr, "labtool48: malformed compact wave %s\n", name);
        goto cleanup;
    }
    variables = calloc(family.variable_count == 0 ? 1 : family.variable_count,
                       sizeof(*variables));
    if (variables == NULL) {
        result = 6;
        goto cleanup;
    }
    for (size_t variable_index = 0; variable_index < family.variable_count;
         ++variable_index) {
        lt48_family_variable variable;
        status = lt48_rez_get_family_variable(
            rez, family_index, variable_index, &variable
        );
        if (status != LT48_OK) {
            goto cleanup;
        }
        variables[variable_index] = (lt48_compile_variable){
            variable.values[0], variable.values[1], variable.values[2],
            variable.values[3], variable.values[4]
        };
    }
    status = lt48_compact_compile_with_pin_policy(
        wave, pin_map, variables, family.variable_count, 1,
        pin_policy, &compiled
    );
    if (status != LT48_OK) {
        fprintf(stderr, "labtool48: %s\n", lt48_status_string(status));
        result = status == LT48_UNSUPPORTED ? 3 : 2;
        goto cleanup;
    }
    if (runtime_options) {
        lt48_runtime_options resolved_options = *runtime_options;
        const lt48_runtime_options *effective_options = &resolved_options;
        lt48_runtime_buffer_seed size_seed;
        unsigned char size_seed_data[4];
        size_t names_index = 0u;
        lt48_defined_name address_name;
        lt48_defined_name parameter_name;
        lt48_defined_name mode_name;
        lt48_defined_name size_name;
        lt48_defined_name size_variable_name;
        int have_names;
        lt48_runtime_result *runtime = NULL;
        lt48_protocol_plan *protocol = NULL;
        lt48_applet_address *applets = NULL;
        size_t applet_count = 0;
        size_t event_count;
        have_names = lt48_rez_find_resource(
            rez, "DefineName", &names_index) == LT48_OK;
        if (have_names) {
            const char *names[] = {"_BUF_Address", "_BUF_DataIn", "_BUF_Mode"};
            lt48_defined_name *records[] = {&address_name, &parameter_name, &mode_name};
            uint8_t *destinations[] = {&resolved_options.address_buffer,
                &resolved_options.parameter_buffer_id, &resolved_options.mode_buffer};
            if (!resolved_options.use_explicit_buffer_ids) {
                resolved_options.address_buffer = 0u;
                resolved_options.parameter_buffer_id = 1u;
                resolved_options.mode_buffer = 2u;
            }
            for (size_t binding = 0u; binding < 3u; ++binding) {
                lt48_status binding_status = find_defined_name_exact(
                    rez, names_index, names[binding], records[binding]);
                if (binding_status == LT48_NOT_FOUND) continue;
                if (binding_status != LT48_OK || records[binding]->identifier >= 32u) {
                    status = LT48_FORMAT_ERROR;
                    goto runtime_failed;
                }
                *destinations[binding] = (uint8_t)records[binding]->identifier;
                resolved_options.use_explicit_buffer_ids = 1;
            }
        }
        if (have_names &&
            find_defined_name_exact(rez, names_index, "_BUF_Size",
                                    &size_name) == LT48_OK &&
            find_defined_name_exact(rez, names_index, "_VAR_Size",
                                    &size_variable_name) == LT48_OK) {
            uint64_t size_value = 0u;
            int have_value = 0;
            for (size_t variable_index = 0u;
                 variable_index < family.variable_count; ++variable_index) {
                if (variables[variable_index].variable_id !=
                    size_variable_name.identifier) continue;
                if (variables[variable_index].kind == 2u) {
                    size_value = variables[variable_index].value;
                    have_value = 1;
                } else if (variables[variable_index].kind == 1u &&
                           variables[variable_index].value < 3u) {
                    static const uint32_t scales[] = {1u, 100u, 100000u};
                    size_value = (uint64_t)variables[variable_index].channel *
                                 scales[variables[variable_index].value];
                    have_value = 1;
                }
            }
            if (have_value && size_value <= UINT32_MAX &&
                size_name.identifier <= UINT8_MAX) {
                size_seed_data[0] = (unsigned char)size_value;
                size_seed_data[1] = (unsigned char)(size_value >> 8);
                size_seed_data[2] = (unsigned char)(size_value >> 16);
                size_seed_data[3] = (unsigned char)(size_value >> 24);
                size_seed.buffer = (uint8_t)size_name.identifier;
                size_seed.data = size_seed_data;
                size_seed.size = sizeof(size_seed_data);
                resolved_options.initial_buffers = &size_seed;
                resolved_options.initial_buffer_count = 1u;
            }
        }
        if (resumable) {
            lt48_runtime_options machine_options = *effective_options;
            lt48_runtime_machine *machine = NULL;
            lt48_runtime_machine_state machine_state;
            size_t response_index = 0u;
            machine_options.capture_response_bytes = NULL;
            machine_options.capture_response_size = 0u;
            machine_options.require_capture_responses = 0;
            status = lt48_runtime_machine_create(
                compiled, pin_map, variables, family.variable_count,
                &machine_options, &machine
            );
            while (status == LT48_OK) {
                status = lt48_runtime_machine_advance(machine, &machine_state);
                if (status != LT48_OK) break;
                if (machine_state == LT48_RUNTIME_MACHINE_FINISHED) {
                    if (response_index != effective_options->capture_response_size) {
                        status = LT48_RUNTIME_ERROR;
                    } else {
                        status = lt48_runtime_machine_take_result(machine,
                                                                  &runtime);
                    }
                    break;
                }
                {
                    uint8_t capture_buffer;
                    size_t capture_length;
                    status = lt48_runtime_machine_capture_request(
                        machine, &capture_buffer, &capture_length
                    );
                    (void)capture_buffer;
                    if (status != LT48_OK) break;
                    if (response_index > effective_options->capture_response_size ||
                        capture_length > effective_options->capture_response_size -
                            response_index) {
                        status = LT48_RUNTIME_ERROR;
                        break;
                    }
                    status = lt48_runtime_machine_supply_capture_bytes(
                        machine,
                        effective_options->capture_response_bytes + response_index,
                        capture_length
                    );
                    if (status == LT48_OK) response_index += capture_length;
                }
            }
            lt48_runtime_machine_close(machine);
        } else {
            status = lt48_runtime_interpret(
                compiled, pin_map, variables, family.variable_count,
                effective_options, &runtime
            );
        }
runtime_failed:
        if (status != LT48_OK) {
            fprintf(stderr, "labtool48: %s\n", lt48_status_string(status));
            result = 2;
            goto cleanup;
        }
        if (protocol_json) {
            applets = calloc(family.operation_count ? family.operation_count : 1,
                             sizeof(*applets));
            if (!applets) {
                lt48_runtime_result_close(runtime);
                result = 6;
                goto cleanup;
            }
            for (size_t operation_index = 0;
                 operation_index < family.operation_count; ++operation_index) {
                lt48_family_operation candidate;
                lt48_wave_code candidate_wave;
                status = lt48_rez_get_family_operation(
                    rez, family_index, operation_index, &candidate
                );
                if (status != LT48_OK) break;
                snprintf(name, sizeof(name), "W%x", candidate.wave);
                status = lt48_rez_find_resource(rez, name, &index);
                if (status == LT48_OK) {
                    status = lt48_rez_get_wave_code(rez, index, &candidate_wave);
                }
                if (status != LT48_OK) break;
                if (candidate_wave.wave_type == 6u) {
                    applets[applet_count++] = (lt48_applet_address){
                        candidate.operation_id, candidate_wave.load_address
                    };
                }
            }
            if (status == LT48_OK) status = lt48_protocol_plan_create(&protocol);
            if (status == LT48_OK) {
                status = lt48_protocol_plan_apply_runtime(
                    protocol, runtime, applets, applet_count
                );
            }
            if (status != LT48_OK) {
                fprintf(stderr, "labtool48: protocol encoding: %s\n",
                        lt48_status_string(status));
                lt48_protocol_plan_close(protocol);
                free(applets);
                lt48_runtime_result_close(runtime);
                result = status == LT48_UNSUPPORTED ? 3 : 2;
                goto cleanup;
            }
        }
        event_count = lt48_runtime_event_count(runtime);
        if (json) {
            fputs("{\"schema\":1,\"definition\":", stdout);
            print_json_string(definition_name);
            printf(",\"operation_id\":%u,\"mode\":%u,\"events\":[",
                   operation_id, effective_options->mode);
            for (size_t event_index = 0; event_index < event_count;
                 ++event_index) {
                lt48_runtime_event event;
                (void)lt48_runtime_get_event(runtime, event_index, &event);
                printf("%s{\"kind\":%u,\"arguments\":[",
                       event_index ? "," : "", event.kind);
                for (size_t argument = 0; argument < event.argument_count;
                     ++argument) {
                    printf("%s%llu", argument ? "," : "",
                           (unsigned long long)event.arguments[argument]);
                }
                fputs("]}", stdout);
            }
            fputs("],\"voltages\":[", stdout);
            for (uint8_t channel = 0; channel < 4; ++channel) {
                uint32_t voltage = 0;
                (void)lt48_runtime_get_voltage(runtime, channel, &voltage);
                printf("%s%u", channel ? "," : "", voltage);
            }
            fputc(']', stdout);
            if (protocol_json) {
                fputs(",\"transactions\":[", stdout);
                for (size_t transaction_index = 0;
                     transaction_index <
                         lt48_protocol_plan_transaction_count(protocol);
                     ++transaction_index) {
                    lt48_protocol_transaction transaction;
                    (void)lt48_protocol_plan_get_transaction(
                        protocol, transaction_index, &transaction
                    );
                    printf("%s{\"direction\":%u,\"endpoint\":%u,"
                           "\"length\":%zu",
                           transaction_index ? "," : "",
                           transaction.direction, transaction.endpoint,
                           transaction.length);
                    if (transaction.direction == LT48_TRANSFER_OUT) {
                        fputs(",\"data_hex\":\"", stdout);
                        print_hex(transaction.data, transaction.length);
                        fputc('"', stdout);
                    }
                    if (transaction.captures_runtime_byte) {
                        printf(",\"capture\":{\"buffer\":%u,"
                               "\"response_offset\":%zu,\"length\":%zu}",
                               transaction.runtime_buffer,
                               transaction.response_offset,
                               transaction.runtime_capture_length);
                    }
                    fputc('}', stdout);
                }
                fputc(']', stdout);
            }
            fputs("}\n", stdout);
        } else {
            printf("%s operation %u mode %u: %zu runtime events\n",
                   definition_name, operation_id, effective_options->mode,
                   event_count);
        }
        lt48_protocol_plan_close(protocol);
        free(applets);
        lt48_runtime_result_close(runtime);
    } else {
        size_t word_count;
        const uint16_t *words = lt48_compiled_wave_words(compiled, &word_count);
        if (json) {
            printf("{\"schema\":1,\"definition\":");
            print_json_string(definition_name);
            printf(",\"operation_id\":%u,\"wave\":%u,\"words\":[",
                   operation_id, operation.wave);
            for (size_t word = 0; word < word_count; ++word) {
                printf("%s%u", word == 0 ? "" : ",", words[word]);
            }
            printf("],\"expansion_count\":%zu,\"defaulted_pin_count\":%zu,"
                   "\"defaulted_pins\":[",
                   lt48_compiled_wave_expansion_count(compiled),
                   lt48_compiled_wave_defaulted_pin_count(compiled));
            for (size_t pin_index = 0;
                 pin_index < lt48_compiled_wave_defaulted_pin_count(compiled);
                 ++pin_index) {
                lt48_defaulted_logical_pin defaulted;
                (void)lt48_compiled_wave_get_defaulted_pin(
                    compiled, pin_index, &defaulted
                );
                printf("%s{\"logical_pin\":%u,\"compact_offset\":%zu,"
                       "\"compact_opcode\":%u,\"classification\":\"%s\"}",
                       pin_index == 0 ? "" : ",", defaulted.logical_pin,
                       defaulted.compact_offset, defaulted.compact_opcode,
                       defaulted.classification ==
                               LT48_DEFAULTED_PIN_RESIDENT_NOOP ?
                           "resident-noop-default" : "unresolved-default");
            }
            fputs("],\"preserved_buffers\":[", stdout);
            for (size_t buffer_index = 0;
                 buffer_index < lt48_compiled_wave_preserved_buffer_count(compiled);
                 ++buffer_index) {
                uint8_t buffer;
                (void)lt48_compiled_wave_get_preserved_buffer(
                    compiled, buffer_index, &buffer
                );
                printf("%s%u", buffer_index == 0 ? "" : ",", buffer);
            }
            fputs("],\"completion_message_index\":", stdout);
            {
                uint16_t message_index;
                if (lt48_compiled_wave_completion_message(
                        compiled, &message_index) == LT48_OK) {
                    printf("%u", message_index);
                } else {
                    fputs("null", stdout);
                }
            }
            fputs("}\n", stdout);
        } else {
            printf("%s operation %u / W%x: %zu words, %zu expansions\n",
                   definition_name, operation_id, operation.wave, word_count,
                   lt48_compiled_wave_expansion_count(compiled));
        }
    }
    result = 0;
cleanup:
    free(variables);
    lt48_compiled_wave_close(compiled);
    lt48_compact_wave_close(wave);
    lt48_pin_map_close(pin_map);
    lt48_rez_close(rez);
    return result;
}

static int buffer_report(
    const char *operation, const char *path, const lt48_buffer *buffer,
    uint32_t start, uint32_t length, int json
) {
    lt48_buffer_checksum checksum;
    lt48_status status = lt48_buffer_checksum_range_cancellable(
        buffer, start, length, cli_cancel_check, NULL, &checksum
    );
    if (status != LT48_OK) {
        fprintf(stderr, "labtool48: buffer %s: %s\n", operation,
                lt48_status_string(status));
        return status == LT48_CANCELLED ? 9 : 1;
    }
    if (json) {
        printf("{\"schema\":1,\"operation\":");
        print_json_string(operation);
        printf(",\"path\":");
        print_json_string(path);
        printf(",\"buffer_size\":%zu,\"range_start\":%u,"
               "\"range_length\":%u,\"additive32\":%u,\"sha256\":\"",
               lt48_buffer_size(buffer), start, length, checksum.additive32);
        print_hex(checksum.sha256, LT48_SHA256_SIZE);
        puts("\"}");
    } else {
        printf("%s: %s\nsize: %zu\nrange: 0x%x + %u\nadditive32: %u\n"
               "sha256: ", operation, path, lt48_buffer_size(buffer), start,
               length, checksum.additive32);
        print_hex(checksum.sha256, LT48_SHA256_SIZE);
        putchar('\n');
    }
    return 0;
}

static int component_validate_command(const char *path, int json) {
    lt48_component_validation_report report;
    lt48_status status = lt48_component_bundle_validate(path, &report);
    const char *issue = lt48_component_validation_issue_name(report.issue);
    if (issue == NULL) issue = "unknown";
    if (json) {
        printf("{\"schema\":1,\"operation\":\"component-validate\","
               "\"path\":");
        print_json_string(path);
        printf(",\"valid\":%s,\"issue\":", status == LT48_OK ? "true" : "false");
        print_json_string(issue);
        fputs(",\"diagnostic\":", stdout);
        print_json_string(report.diagnostic);
        printf(",\"issue_object_index\":");
        if (report.issue_object_index == SIZE_MAX) fputs("null", stdout);
        else printf("%zu", report.issue_object_index);
        printf(",\"bundle_schema_version\":%u,\"document_count\":%zu,"
               "\"object_count\":%zu,\"combination_count\":%zu,"
               "\"inherited_object_count\":%zu,\"reference_count\":%zu,"
               "\"source_sha256\":\"",
               report.schema_version, report.document_count,
               report.object_count, report.combination_count,
               report.inherited_object_count, report.reference_count);
        print_hex(report.source_sha256, LT48_SHA256_SIZE);
        puts("\",\"usb_opened\":false}");
    } else {
        printf("component bundle: %s\nstatus: %s\ndiagnostic: %s\n"
               "schema: %u; documents: %zu; objects: %zu; combinations: %zu\n"
               "inheritance edges: %zu; references: %zu\nsource sha256: ",
               path, issue, report.diagnostic, report.schema_version,
               report.document_count, report.object_count,
               report.combination_count, report.inherited_object_count,
               report.reference_count);
        print_hex(report.source_sha256, LT48_SHA256_SIZE);
        putchar('\n');
    }
    return status == LT48_OK ? 0 : status == LT48_NO_MEMORY ? 6 : 2;
}

static int component_resolve_command(
    const char *path, const char *object_id, int json
) {
    lt48_component_resolution resolution;
    lt48_status status = lt48_component_bundle_resolve(
        path, object_id, &resolution
    );
    if (status != LT48_OK) {
        if (!json)
            fprintf(stderr, "labtool48: cannot resolve component object %s: %s\n",
                    object_id, lt48_status_string(status));
        else {
            printf("{\"schema\":1,\"operation\":\"component-resolve\","
                   "\"valid\":false,\"object_id\":");
            print_json_string(object_id);
            fputs(",\"error\":", stdout);
            print_json_string(lt48_status_string(status));
            puts(",\"usb_opened\":false}");
        }
        return status == LT48_NO_MEMORY ? 6 : 2;
    }
    if (json) {
        printf("{\"schema\":1,\"operation\":\"component-resolve\","
               "\"valid\":true,\"object_id\":");
        print_json_string(object_id);
        printf(",\"inheritance_depth\":%zu,"
               "\"effective_property_count\":%zu,"
               "\"changed_property_count\":%zu,\"effective\":",
               resolution.inheritance_depth,
               resolution.effective_property_count,
               resolution.changed_property_count);
        fwrite(resolution.effective_json, 1u,
               resolution.effective_json_size, stdout);
        fputs(",\"effective_sha256\":\"", stdout);
        print_hex(resolution.effective_sha256, LT48_SHA256_SIZE);
        fputs("\",\"delta\":", stdout);
        fwrite(resolution.delta_json, 1u, resolution.delta_json_size, stdout);
        fputs(",\"delta_sha256\":\"", stdout);
        print_hex(resolution.delta_sha256, LT48_SHA256_SIZE);
        puts("\",\"usb_opened\":false}");
    } else {
        printf("component object: %s\ninheritance depth: %zu\n"
               "effective properties: %zu; changed properties: %zu\n"
               "effective: %.*s\neffective sha256: ",
               object_id, resolution.inheritance_depth,
               resolution.effective_property_count,
               resolution.changed_property_count,
               (int)resolution.effective_json_size,
               (const char *)resolution.effective_json);
        print_hex(resolution.effective_sha256, LT48_SHA256_SIZE);
        printf("\ndelta: %.*s\ndelta sha256: ",
               (int)resolution.delta_json_size,
               (const char *)resolution.delta_json);
        print_hex(resolution.delta_sha256, LT48_SHA256_SIZE);
        putchar('\n');
    }
    lt48_component_resolution_clear(&resolution);
    return 0;
}

static int component_diff_command(
    const char *path, const char *base_id, const char *variant_id, int json
) {
    lt48_component_resolution resolution;
    lt48_status status = lt48_component_bundle_diff(
        path, base_id, variant_id, &resolution
    );
    if (status != LT48_OK) {
        if (json) {
            printf("{\"schema\":1,\"operation\":\"component-diff\"," 
                   "\"valid\":false,\"base_object_id\":");
            print_json_string(base_id);
            fputs(",\"object_id\":", stdout);
            print_json_string(variant_id);
            fputs(",\"error\":", stdout);
            print_json_string(lt48_status_string(status));
            puts(",\"usb_opened\":false}");
        } else {
            fprintf(stderr,
                    "labtool48: cannot diff variant %s against %s: %s\n",
                    variant_id, base_id, lt48_status_string(status));
        }
        return status == LT48_NO_MEMORY ? 6 : 2;
    }
    if (json) {
        printf("{\"schema\":1,\"operation\":\"component-diff\"," 
               "\"valid\":true,\"base_object_id\":");
        print_json_string(base_id);
        fputs(",\"object_id\":", stdout);
        print_json_string(variant_id);
        printf(",\"inheritance_depth\":%zu,\"effective_property_count\":%zu,"
               "\"changed_property_count\":%zu,\"delta\":",
               resolution.inheritance_depth,
               resolution.effective_property_count,
               resolution.changed_property_count);
        fwrite(resolution.delta_json, 1u, resolution.delta_json_size, stdout);
        fputs(",\"delta_sha256\":\"", stdout);
        print_hex(resolution.delta_sha256, LT48_SHA256_SIZE);
        puts("\",\"usb_opened\":false}");
    } else {
        printf("base variant: %s\nvariant: %s\nchanged properties: %zu\n"
               "delta: %.*s\ndelta sha256: ",
               base_id, variant_id, resolution.changed_property_count,
               (int)resolution.delta_json_size,
               (const char *)resolution.delta_json);
        print_hex(resolution.delta_sha256, LT48_SHA256_SIZE);
        putchar('\n');
    }
    lt48_component_resolution_clear(&resolution);
    return 0;
}

static int component_compile_plan_command(
    const char *path, size_t combination_index, const char *operation, int json
) {
    lt48_component_plan plan;
    lt48_status status = lt48_component_bundle_compile_plan(
        path, combination_index, operation, &plan
    );
    if (status != LT48_OK) {
        if (json) {
            printf("{\"schema\":1,\"operation\":\"component-compile-plan\","
                   "\"valid\":false,\"combination_index\":%zu,"
                   "\"requested_operation\":", combination_index);
            print_json_string(operation);
            fputs(",\"error\":", stdout);
            print_json_string(lt48_status_string(status));
            puts(",\"support_registry_changed\":false,\"usb_opened\":false}");
        } else {
            fprintf(stderr,
                    "labtool48: cannot compile component combination %zu "
                    "operation %s: %s\n", combination_index, operation,
                    lt48_status_string(status));
        }
        return status == LT48_NO_MEMORY ? 6 :
               status == LT48_UNSUPPORTED ? 3 : 2;
    }
    if (json) {
        printf("{\"schema\":1,\"operation\":\"component-compile-plan\","
               "\"valid\":true,\"combination_index\":%zu,\"operation_id\":",
               plan.combination_index);
        print_json_string(plan.operation_id);
        printf(",\"mutating\":%s,\"phase_count\":%zu,"
               "\"mapped_pin_state_count\":%zu,\"rail_state_count\":%zu,"
               "\"typed_phase_ir\":true,\"entry_phase_index\":%zu,"
               "\"cleanup_phase_index\":%zu,\"transport_ready\":false,"
               "\"plan\":", plan.mutating ? "true" : "false",
               plan.phase_count, plan.mapped_pin_state_count,
               plan.rail_state_count, plan.entry_phase_index,
               plan.cleanup_phase_index);
        fwrite(plan.json, 1u, plan.json_size, stdout);
        fputs(",\"plan_sha256\":\"", stdout);
        print_hex(plan.sha256, LT48_SHA256_SIZE);
        puts("\",\"support_registry_changed\":false,\"usb_opened\":false}");
    } else {
        printf("component authored plan: combination %zu; operation %s\n"
               "phases: %zu; mapped pin states: %zu; rail states: %zu\n"
               "typed phase IR: yes; entry: %zu; cleanup: %zu; "
               "transport ready: no\n"
               "plan: %.*s\nplan sha256: ",
               plan.combination_index, plan.operation_id, plan.phase_count,
               plan.mapped_pin_state_count, plan.rail_state_count,
               plan.entry_phase_index, plan.cleanup_phase_index,
               (int)plan.json_size, (const char *)plan.json);
        print_hex(plan.sha256, LT48_SHA256_SIZE);
        putchar('\n');
    }
    lt48_component_plan_clear(&plan);
    return 0;
}

static int component_plan_matrix_command(const char *path, int json) {
    lt48_component_plan_matrix matrix;
    cli_cancel_scope cancel_scope;
    lt48_status status;
    memset(&matrix, 0, sizeof(matrix));
    status = cli_cancel_scope_begin(&cancel_scope) ?
        lt48_component_bundle_compile_plan_matrix_cancellable(
            path, cli_cancel_check, NULL, &matrix) : LT48_FILE_ERROR;
    cli_cancel_scope_end(&cancel_scope);
    if (status != LT48_OK) {
        if (json) {
            fputs("{\"schema\":1,\"operation\":\"component-plan-matrix\","
                  "\"valid\":false,\"path\":", stdout);
            print_json_string(path);
            fputs(",\"error\":", stdout);
            print_json_string(lt48_status_string(status));
            puts(",\"support_registry_changed\":false,\"usb_opened\":false}");
        } else {
            fprintf(stderr, "labtool48: cannot compile component plan matrix: "
                    "%s\n", lt48_status_string(status));
        }
        return status == LT48_CANCELLED ? 9 :
               status == LT48_NO_MEMORY ? 6 :
               status == LT48_UNSUPPORTED ? 3 : 2;
    }
    if (json) {
        printf("{\"schema\":1,\"operation\":\"component-plan-matrix\","
               "\"valid\":true,\"combination_count\":%zu,"
               "\"plan_count\":%zu,\"mutating_plan_count\":%zu,"
               "\"transport_ready\":false,\"matrix\":",
               matrix.combination_count, matrix.plan_count,
               matrix.mutating_plan_count);
        fwrite(matrix.json, 1u, matrix.json_size, stdout);
        fputs(",\"matrix_sha256\":\"", stdout);
        print_hex(matrix.sha256, LT48_SHA256_SIZE);
        puts("\",\"support_registry_changed\":false,\"usb_opened\":false}");
    } else {
        printf("component plan matrix: %zu combinations; %zu plans; "
               "%zu mutating\ntransport ready: no\nmatrix: %.*s\n"
               "matrix sha256: ", matrix.combination_count,
               matrix.plan_count, matrix.mutating_plan_count,
               (int)matrix.json_size, (const char *)matrix.json);
        print_hex(matrix.sha256, LT48_SHA256_SIZE);
        putchar('\n');
    }
    lt48_component_plan_matrix_clear(&matrix);
    return 0;
}

static int component_validate_all_variants_command(
    const char *path, int json
) {
    lt48_component_all_variants_report report;
    cli_cancel_scope cancel_scope;
    lt48_status status;
    memset(&report, 0, sizeof(report));
    status = cli_cancel_scope_begin(&cancel_scope) ?
        lt48_component_bundle_validate_all_variants_cancellable(
            path, cli_cancel_check, NULL, &report) : LT48_FILE_ERROR;
    cli_cancel_scope_end(&cancel_scope);
    if (json) {
        fputs("{\"schema\":1,\"operation\":\"component-validate-all-variants\","
              "\"path\":", stdout);
        print_json_string(path);
        printf(",\"complete\":%s,\"structure_valid\":%s,"
               "\"semantics_complete\":%s,\"matrix_complete\":%s,"
               "\"object_count\":%zu,\"combination_count\":%zu,"
               "\"validated_combination_count\":%zu,\"plan_count\":%zu,"
               "\"mutating_plan_count\":%zu",
               report.complete ? "true" : "false",
               report.structure_valid ? "true" : "false",
               report.semantics_complete ? "true" : "false",
               report.matrix_complete ? "true" : "false",
               report.structure.object_count,
               report.semantics.combination_count,
               report.semantics.validated_combination_count,
               report.plan_count, report.mutating_plan_count);
        if (report.matrix_complete) {
            fputs(",\"matrix_sha256\":\"", stdout);
            print_hex(report.matrix_sha256, LT48_SHA256_SIZE);
            fputs("\"", stdout);
        } else {
            fputs(",\"matrix_sha256\":null", stdout);
        }
        if (status != LT48_OK) {
            fputs(",\"error\":", stdout);
            print_json_string(lt48_status_string(status));
        }
        puts(",\"support_registry_changed\":false,\"usb_opened\":false}");
    } else if (status == LT48_OK) {
        printf("all variants validated offline\nobjects: %zu; combinations: "
               "%zu/%zu; plans: %zu (%zu mutating)\nmatrix sha256: ",
               report.structure.object_count,
               report.semantics.validated_combination_count,
               report.semantics.combination_count,
               report.plan_count, report.mutating_plan_count);
        print_hex(report.matrix_sha256, LT48_SHA256_SIZE);
        putchar('\n');
    } else {
        fprintf(stderr, "labtool48: all-variant validation failed: %s\n",
                lt48_status_string(status));
    }
    return status == LT48_CANCELLED ? 9 :
           status == LT48_OK ? 0 : status == LT48_NO_MEMORY ? 6 :
           status == LT48_UNSUPPORTED ? 3 : 2;
}

static int adapter_test_plan_command(
    const char *path, const char *adapter_id, int json
) {
    lt48_component_adapter_test_plan plan;
    lt48_status status = lt48_component_bundle_adapter_test_plan(
        path, adapter_id, &plan);
    if (status != LT48_OK) {
        if (json) {
            fputs("{\"schema\":1,\"operation\":\"adapter-test-plan\","
                  "\"valid\":false,\"adapter_id\":", stdout);
            print_json_string(adapter_id);
            fputs(",\"error\":", stdout);
            print_json_string(lt48_status_string(status));
            puts(",\"usb_opened\":false}");
        } else {
            fprintf(stderr, "labtool48: adapter test-plan rejected: %s\n",
                    lt48_status_string(status));
        }
        return status == LT48_NO_MEMORY ? 6 : 2;
    }
    if (json) {
        printf("{\"schema\":1,\"operation\":\"adapter-test-plan\","
               "\"valid\":true,\"adapter_id\":");
        print_json_string(adapter_id);
        printf(",\"continuity_test_count\":%zu,"
               "\"isolation_test_count\":%zu,\"active_components\":%s,"
               "\"automatic_continuity_ready\":%s,\"plan\":",
               plan.continuity_test_count, plan.isolation_test_count,
               plan.active_components ? "true" : "false",
               plan.automatic_continuity_ready ? "true" : "false");
        fwrite(plan.json, 1u, plan.json_size, stdout);
        fputs(",\"plan_sha256\":\"", stdout);
        print_hex(plan.sha256, LT48_SHA256_SIZE);
        puts("\",\"support_registry_changed\":false,\"usb_opened\":false}");
    } else {
        printf("adapter test plan: %s\ncontinuity tests: %zu; isolation "
               "tests: %zu\nautomatic continuity ready: %s\nplan: %.*s\n"
               "plan sha256: ", adapter_id, plan.continuity_test_count,
               plan.isolation_test_count,
               plan.automatic_continuity_ready ? "yes" : "no",
               (int)plan.json_size, (const char *)plan.json);
        print_hex(plan.sha256, LT48_SHA256_SIZE);
        putchar('\n');
    }
    lt48_component_adapter_test_plan_clear(&plan);
    return 0;
}

static int component_build_payloads_command(const char *path, int json) {
    lt48_component_payload_build_plan plan;
    lt48_status status = lt48_component_bundle_plan_payload_builds(path, &plan);
    if (status != LT48_OK) {
        if (json) {
            fputs("{\"schema\":1,\"operation\":\"component-build-payloads\","
                  "\"valid\":false,\"path\":", stdout);
            print_json_string(path);
            fputs(",\"error\":", stdout);
            print_json_string(lt48_status_string(status));
            puts(",\"execution_performed\":false,\"usb_opened\":false}");
        } else {
            fprintf(stderr, "labtool48: payload build planning rejected: %s\n",
                    lt48_status_string(status));
        }
        return status == LT48_NO_MEMORY ? 6 : 2;
    }
    if (json) {
        printf("{\"schema\":1,\"operation\":\"component-build-payloads\","
               "\"valid\":true,\"payload_count\":%zu,"
               "\"authored_payload_count\":%zu,"
               "\"imported_payload_count\":%zu,\"plan\":",
               plan.payload_count, plan.authored_payload_count,
               plan.imported_payload_count);
        fwrite(plan.json, 1u, plan.json_size, stdout);
        fputs(",\"plan_sha256\":\"", stdout);
        print_hex(plan.sha256, LT48_SHA256_SIZE);
        puts("\",\"execution_performed\":false,\"usb_opened\":false}");
    } else {
        printf("payload build plan: %zu total; %zu authored; %zu imported\n"
               "local inputs verified: no; execution performed: no\n"
               "plan: %.*s\nplan sha256: ", plan.payload_count,
               plan.authored_payload_count, plan.imported_payload_count,
               (int)plan.json_size, (const char *)plan.json);
        print_hex(plan.sha256, LT48_SHA256_SIZE);
        putchar('\n');
    }
    lt48_component_payload_build_plan_clear(&plan);
    return 0;
}

static int component_verify_payload_inputs_command(
    const char *path, const char *root, int json
) {
    lt48_component_payload_input_verification verification;
    cli_cancel_scope cancel_scope;
    lt48_status status;
    memset(&verification, 0, sizeof(verification));
    status = cli_cancel_scope_begin(&cancel_scope) ?
        lt48_component_bundle_verify_payload_inputs_cancellable(
            path, root, cli_cancel_check, NULL, &verification) :
        LT48_FILE_ERROR;
    cli_cancel_scope_end(&cancel_scope);
    int has_report = verification.json != NULL;
    if (json) {
        fputs("{\"schema\":1,\"operation\":\"component-verify-payload-inputs\","
              "\"complete\":", stdout);
        fputs(has_report && verification.complete ? "true" : "false", stdout);
        if (has_report) {
            printf(",\"authored_payload_count\":%zu,"
                   "\"required_input_count\":%zu,"
                   "\"verified_input_count\":%zu,"
                   "\"rejected_input_count\":%zu,\"verification\":",
                   verification.authored_payload_count,
                   verification.required_input_count,
                   verification.verified_input_count,
                   verification.rejected_input_count);
            fwrite(verification.json, 1u, verification.json_size, stdout);
            fputs(",\"verification_sha256\":\"", stdout);
            print_hex(verification.sha256, LT48_SHA256_SIZE);
            fputs("\"", stdout);
        }
        if (status != LT48_OK) {
            fputs(",\"error\":", stdout);
            print_json_string(lt48_status_string(status));
        }
        puts(",\"execution_performed\":false,\"usb_opened\":false}");
    } else if (has_report) {
        printf("payload input verification: %s\ninputs: %zu/%zu verified; "
               "%zu rejected\nverification: %.*s\nverification sha256: ",
               verification.complete ? "complete" : "rejected",
               verification.verified_input_count,
               verification.required_input_count,
               verification.rejected_input_count,
               (int)verification.json_size,
               (const char *)verification.json);
        print_hex(verification.sha256, LT48_SHA256_SIZE);
        putchar('\n');
    } else {
        fprintf(stderr, "labtool48: payload input verification failed: %s\n",
                lt48_status_string(status));
    }
    lt48_component_payload_input_verification_clear(&verification);
    return status == LT48_CANCELLED ? 9 :
           status == LT48_OK ? 0 : status == LT48_NO_MEMORY ? 6 :
           status == LT48_SAFETY_REJECTED ? 3 : 2;
}

static int component_execute_payload_builds_command(
    const char *path, const char *root, const char *confirmation_text, int json
) {
    unsigned char confirmation[LT48_SHA256_SIZE];
    size_t confirmation_size = 0u;
    lt48_component_payload_build_execution execution;
    cli_cancel_scope cancel_scope;
    lt48_status status;
    if (!parse_hex_buffer(confirmation_text, confirmation,
                          sizeof(confirmation), &confirmation_size) ||
        confirmation_size != sizeof(confirmation)) {
        if (json)
            puts("{\"schema\":1,\"operation\":\"component-execute-payload-builds\","
                 "\"complete\":false,\"error\":\"invalid confirmation digest\","
                 "\"process_started\":false,\"usb_opened\":false}");
        else
            fprintf(stderr, "labtool48: confirmation must be 64 hex digits\n");
        return 2;
    }
    memset(&execution, 0, sizeof(execution));
    status = cli_cancel_scope_begin(&cancel_scope) ?
        lt48_component_bundle_execute_payload_builds_cancellable(
            path, root, confirmation, cli_cancel_check, NULL, &execution) :
        LT48_FILE_ERROR;
    cli_cancel_scope_end(&cancel_scope);
    if (json) {
        fputs("{\"schema\":1,\"operation\":\"component-execute-payload-builds\","
              "\"complete\":", stdout);
        fputs(status == LT48_OK && execution.complete ? "true" : "false",
              stdout);
        if (execution.json != NULL) {
            printf(",\"attempted_payload_count\":%zu,"
                   "\"started_payload_count\":%zu,"
                   "\"completed_payload_count\":%zu,"
                   "\"verified_output_count\":%zu,\"execution\":",
                   execution.attempted_payload_count,
                   execution.started_payload_count,
                   execution.completed_payload_count,
                   execution.verified_output_count);
            fwrite(execution.json, 1u, execution.json_size, stdout);
            fputs(",\"execution_sha256\":\"", stdout);
            print_hex(execution.sha256, LT48_SHA256_SIZE);
            fputs("\"", stdout);
        }
        if (status != LT48_OK) {
            fputs(",\"error\":", stdout);
            print_json_string(lt48_status_string(status));
        }
        printf(",\"cancelled\":%s,\"process_started\":%s,"
               "\"usb_opened\":false}\n",
               execution.cancelled ? "true" : "false",
               execution.started_payload_count != 0u ? "true" : "false");
    } else if (status == LT48_OK) {
        printf("payload builds complete: %zu/%zu; outputs verified: %zu\n"
               "execution: %.*s\nexecution sha256: ",
               execution.completed_payload_count,
               execution.attempted_payload_count,
               execution.verified_output_count,
               (int)execution.json_size, (const char *)execution.json);
        print_hex(execution.sha256, LT48_SHA256_SIZE);
        putchar('\n');
    } else {
        fprintf(stderr, "labtool48: payload build execution rejected: %s\n",
                lt48_status_string(status));
    }
    lt48_component_payload_build_execution_clear(&execution);
    return status == LT48_CANCELLED ? 9 :
           status == LT48_OK ? 0 : status == LT48_NO_MEMORY ? 6 :
           (status == LT48_PERMISSION_DENIED ||
            status == LT48_SAFETY_REJECTED) ? 3 : 2;
}

static int component_evidence_validate_command(const char *path, int json) {
    lt48_component_evidence_report report;
    lt48_status status = lt48_component_evidence_validate(path, &report);
    if (json) {
        fputs("{\"schema\":1,\"operation\":\"component-evidence-validate\","
              "\"path\":", stdout);
        print_json_string(path);
        printf(",\"valid\":%s,\"complete\":%s,\"evidence_id\":",
               status == LT48_OK ? "true" : "false",
               report.complete ? "true" : "false");
        if (report.evidence_id[0] != '\0')
            print_json_string(report.evidence_id);
        else
            fputs("null", stdout);
        fputs(",\"evidence_type\":", stdout);
        if (report.evidence_type[0] != '\0')
            print_json_string(report.evidence_type);
        else
            fputs("null", stdout);
        fputs(",\"method\":", stdout);
        if (report.method[0] != '\0') print_json_string(report.method);
        else fputs("null", stdout);
        printf(",\"passed\":%s,\"hardware_evidence\":%s,"
               "\"combination_index\":",
               report.passed ? "true" : "false",
               report.hardware_evidence ? "true" : "false");
        if (report.complete) printf("%zu", report.combination_index);
        else fputs("null", stdout);
        fputs(",\"operation_id\":", stdout);
        if (report.operation_id[0] != '\0') print_json_string(report.operation_id);
        else fputs("null", stdout);
        fputs(",\"diagnostic\":", stdout);
        print_json_string(report.diagnostic);
        fputs(",\"source_sha256\":\"", stdout);
        print_hex(report.source_sha256, LT48_SHA256_SIZE);
        puts("\",\"support_registry_changed\":false,\"usb_opened\":false}");
    } else if (status == LT48_OK) {
        printf("component evidence: %s\ntype/method: %s / %s\n"
               "result: %s; combination %zu; operation %s\n"
               "hardware evidence: %s\n%s\n",
               report.evidence_id, report.evidence_type, report.method,
               report.status, report.combination_index, report.operation_id,
               report.hardware_evidence ? "yes" : "no", report.diagnostic);
    } else {
        fprintf(stderr, "labtool48: evidence record rejected: %s (%s)\n",
                lt48_status_string(status), report.diagnostic);
    }
    return status == LT48_OK ? 0 : status == LT48_NO_MEMORY ? 6 : 2;
}

static int component_evidence_verify_command(
    const char *bundle, const char *record, const char *artifact, int json
) {
    lt48_component_evidence_verification verification;
    cli_cancel_scope cancel_scope;
    lt48_status status;
    memset(&verification, 0, sizeof(verification));
    status = cli_cancel_scope_begin(&cancel_scope) ?
        lt48_component_evidence_verify_cancellable(
            bundle, record, artifact, cli_cancel_check, NULL,
            &verification) : LT48_FILE_ERROR;
    cli_cancel_scope_end(&cancel_scope);
    if (json) {
        fputs("{\"schema\":1,\"operation\":\"component-evidence-verify\","
              "\"valid\":", stdout);
        fputs(status == LT48_OK ? "true" : "false", stdout);
        printf(",\"complete\":%s,\"artifact_size\":%zu,"
               "\"artifact_verified\":%s,\"bundle_verified\":%s,"
               "\"matrix_verified\":%s,\"plan_verified\":%s,"
               "\"evidence_passed\":%s,\"actual_artifact_sha256\":\"",
               verification.complete ? "true" : "false",
               verification.artifact_size,
               verification.artifact_verified ? "true" : "false",
               verification.bundle_verified ? "true" : "false",
               verification.matrix_verified ? "true" : "false",
               verification.plan_verified ? "true" : "false",
               verification.evidence.passed ? "true" : "false");
        print_hex(verification.artifact_sha256, LT48_SHA256_SIZE);
        fputs("\",\"diagnostic\":", stdout);
        print_json_string(verification.diagnostic);
        fputs(",\"support_registry_changed\":false,\"usb_opened\":false",
              stdout);
        if (status != LT48_OK) {
            fputs(",\"error\":", stdout);
            print_json_string(lt48_status_string(status));
        }
        puts("}");
    } else if (status == LT48_OK) {
        printf("component evidence verified: %s\nartifact bytes: %zu\n"
               "bundle/matrix/plan bindings: verified\nevidence result: %s\n"
               "%s\n", verification.evidence.evidence_id,
               verification.artifact_size,
               verification.evidence.passed ? "pass" : "fail",
               verification.diagnostic);
    } else {
        fprintf(stderr, "labtool48: evidence verification rejected: %s (%s)\n",
                lt48_status_string(status), verification.diagnostic);
    }
    return component_long_status_exit(status);
}

static int component_evidence_import_command(
    const char *bundle, const char *record, const char *artifact,
    const char *store, int json
) {
    lt48_component_evidence_import_report report;
    cli_cancel_scope cancel_scope;
    lt48_status status;
    memset(&report, 0, sizeof(report));
    status = cli_cancel_scope_begin(&cancel_scope) ?
        lt48_component_evidence_import_cancellable(
            bundle, record, artifact, store, cli_cancel_check, NULL,
            &report) : LT48_FILE_ERROR;
    cli_cancel_scope_end(&cancel_scope);
    if (json) {
        fputs("{\"schema\":1,\"operation\":\"component-evidence-import\","
              "\"imported\":", stdout);
        fputs(status == LT48_OK ? "true" : "false", stdout);
        fputs(",\"store\":", stdout);
        print_json_string(store);
        if (status == LT48_OK) {
            fputs(",\"evidence_id\":", stdout);
            print_json_string(report.evidence_id);
            printf(",\"record_size\":%zu,\"artifact_size\":%zu,"
                   "\"record_created\":%s,\"artifact_created\":%s,"
                   "\"evidence_passed\":%s,\"record_sha256\":\"",
                   report.record_size, report.artifact_size,
                   report.record_created ? "true" : "false",
                   report.artifact_created ? "true" : "false",
                   report.evidence_passed ? "true" : "false");
            print_hex(report.record_sha256, LT48_SHA256_SIZE);
            fputs("\",\"artifact_sha256\":\"", stdout);
            print_hex(report.artifact_sha256, LT48_SHA256_SIZE);
            fputs("\"", stdout);
        } else {
            fputs(",\"error\":", stdout);
            print_json_string(lt48_status_string(status));
        }
        puts(",\"support_registry_changed\":false,\"usb_opened\":false}");
    } else if (status == LT48_OK) {
        printf("component evidence imported: %s\nstore: %s\n"
               "record bytes: %zu (%s)\nartifact bytes: %zu (%s)\n"
               "recorded result: %s\n", report.evidence_id, store,
               report.record_size,
               report.record_created ? "created" : "already present",
               report.artifact_size,
               report.artifact_created ? "created" : "already present",
               report.evidence_passed ? "pass" : "fail");
    } else {
        fprintf(stderr, "labtool48: evidence import rejected: %s\n",
                lt48_status_string(status));
    }
    return component_long_status_exit(status);
}

static int component_evidence_audit_command(
    const char *bundle, const char *store, int json
) {
    lt48_component_evidence_audit audit;
    cli_cancel_scope cancel_scope;
    lt48_status status;
    memset(&audit, 0, sizeof(audit));
    status = cli_cancel_scope_begin(&cancel_scope) ?
        lt48_component_evidence_audit_store_cancellable(
            bundle, store, cli_cancel_check, NULL, &audit) :
        LT48_FILE_ERROR;
    cli_cancel_scope_end(&cancel_scope);
    int has_report = audit.json != NULL;
    if (json) {
        fputs("{\"schema\":1,\"operation\":\"component-evidence-audit\","
              "\"audited\":", stdout);
        fputs(has_report ? "true" : "false", stdout);
        if (has_report) {
            printf(",\"store_integrity_valid\":%s,\"release_ready\":false,"
                   "\"stored_record_count\":%zu,"
                   "\"verified_record_count\":%zu,"
                   "\"invalid_record_count\":%zu,\"plan_count\":%zu,"
                   "\"safety_validated_plan_count\":%zu,"
                   "\"behavior_validated_plan_count\":%zu,"
                   "\"engine_hardware_plan_count\":%zu,"
                   "\"device_hardware_plan_count\":%zu,"
                   "\"mutating_behavior_blocker_count\":%zu,\"audit\":",
                   audit.store_integrity_valid ? "true" : "false",
                   audit.stored_record_count, audit.verified_record_count,
                   audit.invalid_record_count, audit.plan_count,
                   audit.safety_validated_plan_count,
                   audit.behavior_validated_plan_count,
                   audit.engine_hardware_plan_count,
                   audit.device_hardware_plan_count,
                   audit.mutating_behavior_blocker_count);
            fwrite(audit.json, 1u, audit.json_size, stdout);
            fputs(",\"audit_sha256\":\"", stdout);
            print_hex(audit.sha256, LT48_SHA256_SIZE);
            fputs("\"", stdout);
        }
        if (status != LT48_OK) {
            fputs(",\"error\":", stdout);
            print_json_string(lt48_status_string(status));
        }
        puts(",\"support_registry_changed\":false,\"usb_opened\":false}");
    } else if (has_report) {
        printf("component evidence audit: %zu/%zu records verified; %zu "
               "invalid\nplans: %zu; safety %zu; behavior %zu; engine "
               "hardware %zu; device hardware %zu\nmutating behavior "
               "blockers: %zu; release ready: no\naudit: %.*s\n"
               "audit sha256: ", audit.verified_record_count,
               audit.stored_record_count, audit.invalid_record_count,
               audit.plan_count, audit.safety_validated_plan_count,
               audit.behavior_validated_plan_count,
               audit.engine_hardware_plan_count,
               audit.device_hardware_plan_count,
               audit.mutating_behavior_blocker_count, (int)audit.json_size,
               (const char *)audit.json);
        print_hex(audit.sha256, LT48_SHA256_SIZE);
        putchar('\n');
    } else {
        fprintf(stderr, "labtool48: evidence audit failed: %s\n",
                lt48_status_string(status));
    }
    lt48_component_evidence_audit_clear(&audit);
    return component_long_status_exit(status);
}

static int component_release_check_command(
    const char *bundle, const char *store, int json
) {
    lt48_component_release_check check;
    cli_cancel_scope cancel_scope;
    lt48_status status;
    memset(&check, 0, sizeof(check));
    status = cli_cancel_scope_begin(&cancel_scope) ?
        lt48_component_release_check_create_cancellable(
            bundle, store, cli_cancel_check, NULL, &check) :
        LT48_FILE_ERROR;
    cli_cancel_scope_end(&cancel_scope);
    int has_report = check.json != NULL;
    int release_ready = check.release_ready;
    if (json) {
        fputs("{\"schema\":1,\"operation\":\"component-release-check\","
              "\"checked\":", stdout);
        fputs(has_report ? "true" : "false", stdout);
        if (has_report) {
            printf(",\"release_ready\":%s,"
                   "\"evidence_store_integrity_valid\":%s,"
                   "\"plan_count\":%zu,\"evidence_candidate_count\":%zu,"
                   "\"transport_ready_plan_count\":%zu,"
                   "\"blocker_count\":%zu,\"check\":",
                   check.release_ready ? "true" : "false",
                   check.evidence_store_integrity_valid ? "true" : "false",
                   check.plan_count, check.evidence_candidate_count,
                   check.transport_ready_plan_count, check.blocker_count);
            fwrite(check.json, 1u, check.json_size, stdout);
            fputs(",\"check_sha256\":\"", stdout);
            print_hex(check.sha256, LT48_SHA256_SIZE);
            fputs("\"", stdout);
        }
        if (status != LT48_OK) {
            fputs(",\"error\":", stdout);
            print_json_string(lt48_status_string(status));
        }
        puts(",\"support_registry_changed\":false,\"usb_opened\":false}");
    } else if (has_report) {
        printf("component release check: not ready\nplans: %zu; evidence "
               "candidates: %zu; transport ready: %zu\nblockers: %zu; "
               "support registry changed: no\ncheck: %.*s\ncheck sha256: ",
               check.plan_count, check.evidence_candidate_count,
               check.transport_ready_plan_count, check.blocker_count,
               (int)check.json_size, (const char *)check.json);
        print_hex(check.sha256, LT48_SHA256_SIZE);
        putchar('\n');
    } else {
        fprintf(stderr, "labtool48: release check failed: %s\n",
                lt48_status_string(status));
    }
    lt48_component_release_check_clear(&check);
    if (status != LT48_OK) return component_long_status_exit(status);
    return has_report && !release_ready ? 3 : 0;
}

static int component_init_command(
    const char *path, const char *bundle_id, const char *version,
    const char *author, const char *license, const char *created_date,
    int json
) {
    unsigned char digest[LT48_SHA256_SIZE];
    lt48_status status = lt48_component_bundle_init(
        path, bundle_id, version, author, license, created_date, digest
    );
    if (status != LT48_OK) {
        if (json) {
            printf("{\"schema\":1,\"operation\":\"component-init\","
                   "\"created\":false,\"path\":");
            print_json_string(path);
            fputs(",\"error\":", stdout);
            print_json_string(lt48_status_string(status));
            puts(",\"usb_opened\":false}");
        } else {
            fprintf(stderr, "labtool48: cannot create component bundle %s: %s\n",
                    path, lt48_status_string(status));
        }
        return status == LT48_NO_MEMORY ? 6 : 2;
    }
    if (json) {
        printf("{\"schema\":1,\"operation\":\"component-init\","
               "\"created\":true,\"path\":");
        print_json_string(path);
        fputs(",\"bundle_id\":", stdout);
        print_json_string(bundle_id);
        fputs(",\"version\":", stdout);
        print_json_string(version);
        fputs(",\"source_sha256\":\"", stdout);
        print_hex(digest, LT48_SHA256_SIZE);
        puts("\",\"exclusive_creation\":true,\"usb_opened\":false}");
    } else {
        printf("created component bundle exclusively: %s\nid: %s\nversion: %s\n"
               "source sha256: ", path, bundle_id, version);
        print_hex(digest, LT48_SHA256_SIZE);
        putchar('\n');
    }
    return 0;
}

static int component_add_object_command(
    const char *input, const char *output, const char *version,
    const char *date, const char *object_record, const char *operation,
    const char *expected_kind, int json
) {
    lt48_component_revision_report report;
    lt48_status status = lt48_component_bundle_add_object_of_kind(
        input, output, version, date, object_record, expected_kind, &report
    );
    if (status != LT48_OK) {
        if (json) {
            printf("{\"schema\":1,\"operation\":");
            print_json_string(operation);
            fputs(",\"created\":false,\"output\":", stdout);
            print_json_string(output);
            fputs(",\"error\":", stdout);
            print_json_string(lt48_status_string(status));
            puts(",\"usb_opened\":false}");
        } else {
            fprintf(stderr, "labtool48: cannot create component revision %s: %s\n",
                    output, lt48_status_string(status));
        }
        return status == LT48_NO_MEMORY ? 6 : 2;
    }
    if (json) {
        printf("{\"schema\":1,\"operation\":");
        print_json_string(operation);
        fputs(",\"created\":true,\"input\":", stdout);
        print_json_string(input);
        fputs(",\"output\":", stdout);
        print_json_string(output);
        printf(",\"previous_object_count\":%zu,\"output_object_count\":%zu,"
               "\"previous_source_sha256\":\"",
               report.previous_object_count, report.output_object_count);
        print_hex(report.previous_source_sha256, LT48_SHA256_SIZE);
        fputs("\",\"object_record_sha256\":\"", stdout);
        print_hex(report.object_record_sha256, LT48_SHA256_SIZE);
        fputs("\",\"output_source_sha256\":\"", stdout);
        print_hex(report.output_source_sha256, LT48_SHA256_SIZE);
        puts("\",\"exclusive_creation\":true,\"usb_opened\":false}");
    } else {
        printf("created component revision exclusively: %s\n"
               "objects: %zu -> %zu\noutput source sha256: ",
               output, report.previous_object_count, report.output_object_count);
        print_hex(report.output_source_sha256, LT48_SHA256_SIZE);
        putchar('\n');
    }
    return 0;
}

static int component_semantics_command(const char *path, int json) {
    lt48_component_semantic_report report;
    lt48_status status = lt48_component_bundle_check_semantics(path, &report);
    if (json) {
        printf("{\"schema\":1,\"operation\":\"component-check-semantics\","
               "\"path\":");
        print_json_string(path);
        printf(",\"complete\":%s,\"object_count\":%zu,"
               "\"validated_object_count\":%zu,\"blocker_count\":%zu,"
               "\"invalid_object_count\":%zu,\"combination_count\":%zu,"
               "\"validated_combination_count\":%zu,"
               "\"invalid_combination_count\":%zu,"
               "\"first_problem_object_index\":",
               report.complete ? "true" : "false", report.object_count,
               report.validated_object_count, report.blocker_count,
               report.invalid_object_count, report.combination_count,
               report.validated_combination_count,
               report.invalid_combination_count);
        if (report.first_problem_object_index == SIZE_MAX) fputs("null", stdout);
        else printf("%zu", report.first_problem_object_index);
        fputs(",\"first_problem_object_id\":", stdout);
        if (report.first_problem_object_id[0] == '\0') fputs("null", stdout);
        else print_json_string(report.first_problem_object_id);
        fputs(",\"first_problem_combination_index\":", stdout);
        if (report.first_problem_combination_index == SIZE_MAX)
            fputs("null", stdout);
        else
            printf("%zu", report.first_problem_combination_index);
        fputs(",\"diagnostic\":", stdout);
        print_json_string(report.diagnostic);
        fputs(",\"source_sha256\":\"", stdout);
        print_hex(report.source_sha256, LT48_SHA256_SIZE);
        puts("\",\"support_registry_changed\":false,\"usb_opened\":false}");
    } else {
        printf("component semantic schemas: %s\n"
               "objects: %zu; validated: %zu; blockers: %zu; invalid: %zu\n"
               "combinations: %zu; validated: %zu; invalid: %zu\n"
               "first problem: %s; combination: %s\ndiagnostic: %s\n",
               report.complete ? "complete" : "incomplete",
               report.object_count, report.validated_object_count,
               report.blocker_count, report.invalid_object_count,
               report.combination_count, report.validated_combination_count,
               report.invalid_combination_count,
               report.first_problem_object_id[0] == '\0' ? "none" :
                   report.first_problem_object_id,
               report.first_problem_combination_index == SIZE_MAX ? "none" :
                   "see JSON index",
               report.diagnostic);
    }
    if (status != LT48_OK)
        return status == LT48_NO_MEMORY ? 6 : 2;
    return report.complete ? 0 : 3;
}

static lt48_status cli_buffer_load_exact(
    const char *path, size_t expected_size, lt48_buffer **out_buffer
) {
    lt48_buffer_import_options options = {
        expected_size,
        LT48_BUFFER_SHORT_REJECT,
        LT48_BUFFER_LONG_REJECT,
        0u,
        1u,
        0
    };
    return lt48_buffer_load_file_cancellable(
        path, &options, cli_cancel_check, NULL, NULL, out_buffer);
}

static lt48_status cli_buffer_save_exclusive(
    const lt48_buffer *buffer, const char *path,
    size_t offset, size_t length
) {
    return lt48_buffer_save_file_range_cancellable(
        buffer, path, offset, length, 0, cli_cancel_check, NULL);
}

static void print_image_import_result(
    const char *operation, const char *input, const char *output,
    const lt48_image_import_options *options,
    const lt48_image_import_result *result, int json
) {
    const char *format = lt48_image_format_name(result->format);
    if (json) {
        fputs("{\"schema\":1,\"operation\":", stdout);
        print_json_string(operation);
        fputs(",\"input\":", stdout);
        print_json_string(input);
        if (output != NULL) {
            fputs(",\"output\":", stdout);
            print_json_string(output);
        }
        printf(",\"format\":\"%s\",\"buffer_size\":%zu,"
               "\"address_base\":%llu,\"gap_policy\":\"%s\","
               "\"gap_fill_value\":%u,\"input_size\":%zu,"
               "\"record_count\":%zu,\"data_record_count\":%zu,"
               "\"encoded_data_bytes\":%zu,\"mapped_data_bytes\":%zu,"
               "\"out_of_range_bytes\":%zu,\"overlap_bytes\":%zu,"
               "\"gap_count\":%zu,\"gap_bytes\":%zu,"
               "\"minimum_source_address\":",
               format == NULL ? "unknown" : format, options->buffer_size,
               (unsigned long long)options->address_base,
               options->gap_policy == LT48_IMAGE_GAPS_FILL ? "fill" :
                                                              "preserve",
               options->gap_fill_value, result->input_size,
               result->record_count, result->data_record_count,
               result->encoded_data_byte_count,
               result->mapped_data_byte_count,
               result->out_of_range_byte_count, result->overlap_byte_count,
               result->gap_count, result->gap_byte_count);
        if (result->has_data)
            printf("%llu", (unsigned long long)result->minimum_source_address);
        else fputs("null", stdout);
        fputs(",\"maximum_source_address_exclusive\":", stdout);
        if (result->has_data)
            printf("%llu", (unsigned long long)
                   result->maximum_source_address_exclusive);
        else fputs("null", stdout);
        fputs(",\"execution_start_address\":", stdout);
        if (result->has_execution_start)
            printf("%llu", (unsigned long long)
                   result->execution_start_address);
        else fputs("null", stdout);
        puts(",\"usb_opened\":false}");
    } else {
        printf("%s: %s (%s)\ncapacity/base: %zu / 0x%llx\n"
               "records/data records: %zu/%zu; encoded/mapped bytes: "
               "%zu/%zu\nsource range: ", operation, input,
               format == NULL ? "unknown" : format, options->buffer_size,
               (unsigned long long)options->address_base,
               result->record_count, result->data_record_count,
               result->encoded_data_byte_count,
               result->mapped_data_byte_count);
        if (result->has_data)
            printf("0x%llx..0x%llx", (unsigned long long)
                   result->minimum_source_address, (unsigned long long)
                   result->maximum_source_address_exclusive);
        else fputs("none", stdout);
        printf("\ngaps: %zu regions / %zu bytes (%s", result->gap_count,
               result->gap_byte_count,
               options->gap_policy == LT48_IMAGE_GAPS_FILL ? "filled" :
                                                              "preserved");
        if (options->gap_policy == LT48_IMAGE_GAPS_FILL)
            printf(" with 0x%02x", options->gap_fill_value);
        puts(")");
        if (result->has_execution_start)
            printf("execution start metadata: 0x%llx (not programmed)\n",
                   (unsigned long long)result->execution_start_address);
        if (output != NULL) printf("output: %s\n", output);
    }
}

static int buffer_command_impl(int argc, char **argv) {
    const char *operation = argv[2];
    lt48_buffer *buffer = NULL;
    lt48_status status;
    uint32_t size;
    uint32_t start = 0u;
    uint32_t length;
    uint8_t value;
    int json = strcmp(argv[argc - 1], "--json") == 0;
    int effective_argc = argc - json;
    const char *input;
    const char *output;
    if (strcmp(operation, "image-inspect") == 0 ||
        strcmp(operation, "image-import") == 0) {
        int importing = strcmp(operation, "image-import") == 0;
        lt48_image_import_options options = {
            LT48_IMAGE_FORMAT_AUTO, 0u, 0u,
            LT48_IMAGE_GAPS_PRESERVE, 0xffu
        };
        lt48_image_import_result result;
        int option_index = importing ? 6 : 5;
        int format_set = 0;
        int base_set = 0;
        int fill_set = 0;
        json = 0;
        if (argc < option_index || !parse_u32(argv[4], &size) || size == 0u)
            return 2;
        options.buffer_size = size;
        input = argv[3];
        output = importing ? argv[5] : NULL;
        for (int index = option_index; index < argc; ++index) {
            if (strcmp(argv[index], "--json") == 0 && !json) {
                json = 1;
            } else if (strcmp(argv[index], "--format") == 0 &&
                       index + 1 < argc && !format_set &&
                       parse_image_format(argv[++index], &options.format)) {
                format_set = 1;
            } else if (strcmp(argv[index], "--base") == 0 &&
                       index + 1 < argc && !base_set &&
                       parse_u64(argv[++index], &options.address_base)) {
                base_set = 1;
            } else if (strcmp(argv[index], "--fill-gaps") == 0 &&
                       index + 1 < argc && !fill_set &&
                       parse_u8(argv[++index], &options.gap_fill_value)) {
                options.gap_policy = LT48_IMAGE_GAPS_FILL;
                fill_set = 1;
            } else {
                return 2;
            }
        }
        status = lt48_buffer_load_image_cancellable(
            input, &options, cli_cancel_check, NULL, &result, &buffer
        );
        if (status != LT48_OK) goto failed;
        if (importing)
            status = cli_buffer_save_exclusive(
                buffer, output, 0u, options.buffer_size
            );
        if (status != LT48_OK) goto failed;
        print_image_import_result(
            operation, input, output, &options, &result, json
        );
        lt48_buffer_close(buffer);
        return 0;
    }
    if (strcmp(operation, "word-read") == 0) {
        uint32_t width;
        uint64_t word;
        lt48_buffer_byte_order order;
        if (effective_argc != 8 || !parse_u32(argv[4], &size) || size == 0u ||
            !parse_u32(argv[5], &start) || !parse_u32(argv[6], &width) ||
            !parse_buffer_byte_order(argv[7], &order)) return 2;
        status = cli_buffer_load_exact(argv[3], size, &buffer);
        if (status == LT48_OK)
            status = lt48_buffer_get_word(buffer, start, width, order, &word);
        if (status != LT48_OK) goto failed;
        if (json) {
            printf("{\"schema\":1,\"operation\":\"word-read\",\"path\":");
            print_json_string(argv[3]);
            printf(",\"offset\":%u,\"width\":%u,\"byte_order\":\"%s\","
                   "\"value\":%llu,\"value_hex\":\"0x%0*llx\"}\n",
                   start, width,
                   order == LT48_BUFFER_LITTLE_ENDIAN ? "little" : "big",
                   (unsigned long long)word, (int)(width * 2u),
                   (unsigned long long)word);
        } else {
            printf("word at 0x%x: 0x%0*llx (%s-endian, %u bytes)\n",
                   start, (int)(width * 2u), (unsigned long long)word,
                   order == LT48_BUFFER_LITTLE_ENDIAN ? "little" : "big",
                   width);
        }
        lt48_buffer_close(buffer);
        return 0;
    }
    if (strcmp(operation, "word-edit") == 0) {
        lt48_buffer_checksum checksum;
        uint32_t width;
        uint64_t word;
        lt48_buffer_byte_order order;
        if (effective_argc != 10 || !parse_u32(argv[4], &size) || size == 0u ||
            !parse_u32(argv[5], &start) || !parse_u32(argv[6], &width) ||
            !parse_buffer_byte_order(argv[7], &order) ||
            !parse_u64(argv[8], &word)) return 2;
        input = argv[3];
        output = argv[9];
        status = cli_buffer_load_exact(input, size, &buffer);
        if (status == LT48_OK)
            status = lt48_buffer_set_word(buffer, start, width, order, word);
        if (status == LT48_OK)
            status = cli_buffer_save_exclusive(buffer, output, 0u, size);
        if (status == LT48_OK)
            status = lt48_buffer_checksum_range_cancellable(
                buffer, 0u, size, cli_cancel_check, NULL, &checksum);
        if (status != LT48_OK) goto failed;
        if (json) {
            printf("{\"schema\":1,\"operation\":\"word-edit\",\"input\":");
            print_json_string(input);
            fputs(",\"output\":", stdout);
            print_json_string(output);
            printf(",\"buffer_size\":%u,\"offset\":%u,\"width\":%u,"
                   "\"byte_order\":\"%s\",\"value\":%llu,"
                   "\"value_hex\":\"0x%0*llx\",\"sha256\":\"",
                   size, start, width,
                   order == LT48_BUFFER_LITTLE_ENDIAN ? "little" : "big",
                   (unsigned long long)word, (int)(width * 2u),
                   (unsigned long long)word);
            print_hex(checksum.sha256, LT48_SHA256_SIZE);
            puts("\"}");
        } else {
            printf("wrote 0x%0*llx at 0x%x (%s-endian, %u bytes): %s\n",
                   (int)(width * 2u), (unsigned long long)word, start,
                   order == LT48_BUFFER_LITTLE_ENDIAN ? "little" : "big",
                   width, output);
        }
        lt48_buffer_close(buffer);
        return 0;
    }
    if (strcmp(operation, "bind") == 0) {
        lt48_buffer_binding binding;
        const lt48_buffer_provenance *provenance;
        if (effective_argc != 7 || !parse_u32(argv[4], &size) || size == 0u ||
            !parse_u32(argv[5], &start) || !parse_u32(argv[6], &length) ||
            length == 0u) return 2;
        status = cli_buffer_load_exact(argv[3], size, &buffer);
        if (status == LT48_OK)
            status = lt48_buffer_bind_mutating_input(
                buffer, size, start, length, &binding
            );
        if (status != LT48_OK) goto failed;
        provenance = &binding.provenance;
        if (json) {
            printf("{\"schema\":1,\"operation\":\"bind\",\"path\":");
            print_json_string(argv[3]);
            printf(",\"definition_capacity\":%zu,\"range_start\":%zu,"
                   "\"range_length\":%zu,\"origin\":\"%s\","
                   "\"input_size\":%zu,\"imported_size\":%zu,"
                   "\"padded_size\":%zu,\"truncated_size\":%zu,"
                   "\"short_input\":\"%s\",\"long_input\":\"%s\","
                   "\"fill_value\":%u,\"word_size\":%zu,"
                   "\"reverse_word_bytes\":%s,\"mutation_count\":%zu,"
                   "\"image_sha256\":\"",
                   binding.definition_capacity, binding.range_offset,
                   binding.range_length,
                   provenance->origin == LT48_BUFFER_ORIGIN_FILE_IMPORT ?
                       "file-import" : "generated-fill",
                   provenance->input_size, provenance->imported_size,
                   provenance->padded_size, provenance->truncated_size,
                   provenance->short_input == LT48_BUFFER_SHORT_PAD ?
                       "pad" : "reject",
                   provenance->long_input == LT48_BUFFER_LONG_TRUNCATE ?
                       "truncate" : "reject",
                   provenance->fill_value, provenance->word_size_bytes,
                   provenance->reverse_word_bytes ? "true" : "false",
                   provenance->mutation_count);
            print_hex(binding.image_sha256, LT48_SHA256_SIZE);
            fputs("\",\"range_sha256\":\"", stdout);
            print_hex(binding.range_sha256, LT48_SHA256_SIZE);
            fputs("\",\"binding_sha256\":\"", stdout);
            print_hex(binding.binding_sha256, LT48_SHA256_SIZE);
            puts("\"}");
        } else {
            printf("mutating-input binding schema %u\n"
                   "geometry: %zu bytes; range: 0x%zx + %zu\n"
                   "origin: %s; input/imported/padded/truncated: "
                   "%zu/%zu/%zu/%zu; mutations: %zu\nimage sha256: ",
                   binding.schema_version, binding.definition_capacity,
                   binding.range_offset, binding.range_length,
                   provenance->origin == LT48_BUFFER_ORIGIN_FILE_IMPORT ?
                       "file import" : "generated fill",
                   provenance->input_size, provenance->imported_size,
                   provenance->padded_size, provenance->truncated_size,
                   provenance->mutation_count);
            print_hex(binding.image_sha256, LT48_SHA256_SIZE);
            fputs("\nrange sha256: ", stdout);
            print_hex(binding.range_sha256, LT48_SHA256_SIZE);
            fputs("\nbinding sha256: ", stdout);
            print_hex(binding.binding_sha256, LT48_SHA256_SIZE);
            putchar('\n');
        }
        lt48_buffer_close(buffer);
        return 0;
    }
    if (strcmp(operation, "import") == 0) {
        lt48_buffer_import_options options;
        lt48_buffer_import_result import_result;
        lt48_buffer_checksum checksum;
        uint32_t word_size = 1u;
        int reverse_words = 0;
        if (argc < 6 || !parse_u32(argv[4], &size) || size == 0u) return 2;
        input = argv[3];
        output = argv[5];
        options.buffer_size = size;
        options.short_input = LT48_BUFFER_SHORT_REJECT;
        options.long_input = LT48_BUFFER_LONG_REJECT;
        options.pad_value = 0u;
        options.word_size_bytes = 1u;
        options.reverse_word_bytes = 0;
        json = 0;
        for (int index = 6; index < argc; ++index) {
            if (strcmp(argv[index], "--json") == 0 && !json) {
                json = 1;
            } else if (strcmp(argv[index], "--pad") == 0 &&
                       index + 1 < argc &&
                       parse_u8(argv[++index], &options.pad_value) &&
                       options.short_input == LT48_BUFFER_SHORT_REJECT) {
                options.short_input = LT48_BUFFER_SHORT_PAD;
            } else if (strcmp(argv[index], "--truncate") == 0 &&
                       options.long_input == LT48_BUFFER_LONG_REJECT) {
                options.long_input = LT48_BUFFER_LONG_TRUNCATE;
            } else if (strcmp(argv[index], "--word-size") == 0 &&
                       index + 1 < argc &&
                       parse_u32(argv[++index], &word_size) &&
                       options.word_size_bytes == 1u) {
                options.word_size_bytes = word_size;
            } else if (strcmp(argv[index], "--reverse-word-bytes") == 0 &&
                       !reverse_words) {
                reverse_words = 1;
                options.reverse_word_bytes = 1;
            } else {
                return 2;
            }
        }
        status = lt48_buffer_load_file_cancellable(
            input, &options, cli_cancel_check, NULL, &import_result, &buffer
        );
        if (status == LT48_OK)
            status = cli_buffer_save_exclusive(buffer, output, 0u, size);
        if (status != LT48_OK) goto failed;
        status = lt48_buffer_checksum_range_cancellable(
            buffer, 0u, size, cli_cancel_check, NULL, &checksum
        );
        if (status != LT48_OK) goto failed;
        if (json) {
            printf("{\"schema\":1,\"operation\":\"import\","
                   "\"input\":");
            print_json_string(input);
            printf(",\"output\":");
            print_json_string(output);
            printf(",\"buffer_size\":%u,\"input_size\":%zu,"
                   "\"imported_size\":%zu,\"padded_size\":%zu,"
                   "\"truncated_size\":%zu,\"pad_value\":%u,"
                   "\"word_size\":%zu,\"reverse_word_bytes\":%s,"
                   "\"additive32\":%u,\"sha256\":\"",
                   size, import_result.input_size,
                   import_result.imported_size, import_result.padded_size,
                   import_result.truncated_size, options.pad_value,
                   options.word_size_bytes,
                   options.reverse_word_bytes ? "true" : "false",
                   checksum.additive32);
            print_hex(checksum.sha256, LT48_SHA256_SIZE);
            puts("\"}");
        } else {
            printf("import: %s -> %s\ninput: %zu, imported: %zu, padded: %zu, "
                   "truncated: %zu\nword bytes: %zu, reversed: %s\nsha256: ",
                   input, output, import_result.input_size,
                   import_result.imported_size, import_result.padded_size,
                   import_result.truncated_size, options.word_size_bytes,
                   options.reverse_word_bytes ? "yes" : "no");
            print_hex(checksum.sha256, LT48_SHA256_SIZE);
            putchar('\n');
        }
        lt48_buffer_close(buffer);
        return 0;
    }
    if (strcmp(operation, "compare") == 0) {
        lt48_buffer *expected_buffer = NULL;
        lt48_buffer_compare_result comparison;
        if ((effective_argc != 6 && effective_argc != 8) ||
            !parse_u32(argv[5], &size) || size == 0u) return 2;
        length = size;
        if (effective_argc == 8 &&
            (!parse_u32(argv[6], &start) || !parse_u32(argv[7], &length)))
            return 2;
        status = cli_buffer_load_exact(argv[3], size, &buffer);
        if (status == LT48_OK)
            status = cli_buffer_load_exact(
                argv[4], size, &expected_buffer
            );
        if (status == LT48_OK &&
            ((size_t)start > lt48_buffer_size(expected_buffer) ||
             (size_t)length >
                 lt48_buffer_size(expected_buffer) - (size_t)start))
            status = LT48_INVALID_ARGUMENT;
        if (status == LT48_OK)
            status = lt48_buffer_compare_range_cancellable(
                buffer, start, lt48_buffer_data(expected_buffer) + start,
                length, cli_cancel_check, NULL, &comparison
            );
        if (status != LT48_OK) {
            lt48_buffer_close(expected_buffer);
            goto failed;
        }
        if (json) {
            printf("{\"schema\":1,\"operation\":\"compare\","
                   "\"actual\":");
            print_json_string(argv[3]);
            printf(",\"expected\":");
            print_json_string(argv[4]);
            printf(",\"range_start\":%u,\"range_length\":%u,"
                   "\"equal\":%s", start, length,
                   comparison.equal ? "true" : "false");
            if (!comparison.equal) {
                printf(",\"first_mismatch_address\":%zu,"
                       "\"expected_value\":%u,\"actual_value\":%u",
                       comparison.first_mismatch_offset,
                       comparison.expected_value, comparison.actual_value);
            }
            puts("}");
        } else if (comparison.equal) {
            printf("compare: equal over 0x%x + %u bytes\n", start, length);
        } else {
            printf("compare: different at 0x%zx: expected 0x%02x, "
                   "actual 0x%02x\n", comparison.first_mismatch_offset,
                   comparison.expected_value, comparison.actual_value);
        }
        lt48_buffer_close(expected_buffer);
        lt48_buffer_close(buffer);
        return comparison.equal ? 0 : 1;
    }
    if (strcmp(operation, "checksum") == 0) {
        if ((effective_argc != 5 && effective_argc != 7) ||
            !parse_u32(argv[4], &size)) return 2;
        length = size;
        if (effective_argc == 7 &&
            (!parse_u32(argv[5], &start) || !parse_u32(argv[6], &length)))
            return 2;
        status = cli_buffer_load_exact(argv[3], size, &buffer);
        if (status != LT48_OK) goto failed;
        {
            int result = buffer_report(operation, argv[3], buffer,
                                       start, length, json);
            lt48_buffer_close(buffer);
            return result;
        }
    }
    if (strcmp(operation, "create") == 0) {
        if (effective_argc != 6 || !parse_u32(argv[3], &size) ||
            !parse_u8(argv[4], &value)) return 2;
        output = argv[5];
        status = lt48_buffer_create(size, value, &buffer);
        if (status == LT48_OK)
            status = cli_buffer_save_exclusive(buffer, output, 0u, size);
        start = 0u;
        length = size;
    } else if (strcmp(operation, "fill") == 0) {
        if (effective_argc != 9 || !parse_u32(argv[4], &size) ||
            !parse_u32(argv[5], &start) || !parse_u32(argv[6], &length) ||
            !parse_u8(argv[7], &value)) return 2;
        input = argv[3];
        output = argv[8];
        status = cli_buffer_load_exact(input, size, &buffer);
        if (status == LT48_OK)
            status = lt48_buffer_fill(buffer, start, length, value);
        if (status == LT48_OK)
            status = cli_buffer_save_exclusive(buffer, output, 0u, size);
    } else if (strcmp(operation, "edit") == 0) {
        if (effective_argc != 8 || !parse_u32(argv[4], &size) ||
            !parse_u32(argv[5], &start) || !parse_u8(argv[6], &value))
            return 2;
        input = argv[3];
        output = argv[7];
        length = 1u;
        status = cli_buffer_load_exact(input, size, &buffer);
        if (status == LT48_OK)
            status = lt48_buffer_set_byte(buffer, start, value);
        if (status == LT48_OK)
            status = cli_buffer_save_exclusive(buffer, output, 0u, size);
    } else if (strcmp(operation, "extract") == 0) {
        if (effective_argc != 8 || !parse_u32(argv[4], &size) ||
            !parse_u32(argv[5], &start) || !parse_u32(argv[6], &length))
            return 2;
        input = argv[3];
        output = argv[7];
        status = cli_buffer_load_exact(input, size, &buffer);
        if (status == LT48_OK)
            status = cli_buffer_save_exclusive(
                buffer, output, start, length
            );
    } else {
        return 2;
    }
    if (status != LT48_OK) goto failed;
    {
        uint32_t report_start = strcmp(operation, "extract") == 0 ?
                                start : 0u;
        uint32_t report_length = strcmp(operation, "extract") == 0 ?
                                 length : size;
        int result = buffer_report(operation, output, buffer,
                                   report_start, report_length, json);
        lt48_buffer_close(buffer);
        return result;
    }

failed:
    if (json) {
        printf("{\"schema\":1,\"operation\":");
        print_json_string(operation);
        printf(",\"status\":\"error\",\"error\":");
        print_json_string(lt48_status_string(status));
        puts("}");
    } else {
        fprintf(stderr, "labtool48: buffer %s: %s\n", operation,
                lt48_status_string(status));
    }
    lt48_buffer_close(buffer);
    return status == LT48_CANCELLED ? 9 : 1;
}

static int buffer_command(int argc, char **argv) {
    cli_cancel_scope scope;
    int result;
    if (!cli_cancel_scope_begin(&scope)) return 2;
    result = buffer_command_impl(argc, argv);
    cli_cancel_scope_end(&scope);
    return result;
}

static int jtag_plan_scan_command(int argc, char **argv) {
    lt48_jtag_scan_request request;
    lt48_jtag_plan *plan = NULL;
    lt48_jtag_plan_info info;
    const unsigned char *json_data;
    size_t json_size;
    unsigned char *tdi = NULL;
    size_t tdi_size;
    size_t parsed_size = 0u;
    uint32_t bits;
    int json = argc == 10;
    lt48_status status;
    if ((argc != 9 && argc != 10) ||
        (json && strcmp(argv[9], "--json") != 0) ||
        !parse_u32(argv[5], &bits) || bits == 0u ||
        !parse_u32(argv[6], &request.tck_hz) ||
        !parse_u32(argv[7], &request.profile_max_tck_hz)) return 2;
    if (strcmp(argv[3], "ir") == 0) request.kind = LT48_JTAG_SCAN_IR;
    else if (strcmp(argv[3], "dr") == 0) request.kind = LT48_JTAG_SCAN_DR;
    else return 2;
    if (strcmp(argv[8], "reset") == 0)
        request.safe_end_state = LT48_JTAG_TEST_LOGIC_RESET;
    else if (strcmp(argv[8], "idle") == 0)
        request.safe_end_state = LT48_JTAG_RUN_TEST_IDLE;
    else return 2;
    if (strlen(argv[4]) == 0u || strlen(argv[4]) % 2u != 0u) return 2;
    tdi_size = strlen(argv[4]) / 2u;
    tdi = malloc(tdi_size);
    if (tdi == NULL) return 6;
    if (!parse_hex_buffer(argv[4], tdi, tdi_size, &parsed_size) ||
        parsed_size != tdi_size) {
        free(tdi);
        return 2;
    }
    request.tdi = tdi;
    request.tdi_size = tdi_size;
    request.bit_count = bits;
    status = lt48_jtag_plan_scan(&request, &plan);
    free(tdi);
    if (status != LT48_OK) {
        if (json) {
            printf("{\"schema\":1,\"operation\":\"jtag-plan-scan\","
                   "\"valid\":false,\"error\":");
            print_json_string(lt48_status_string(status));
            puts(",\"usb_opened\":false}");
        } else {
            fprintf(stderr, "labtool48: cannot plan JTAG scan: %s\n",
                    lt48_status_string(status));
        }
        return status == LT48_NO_MEMORY ? 6 : 4;
    }
    (void)lt48_jtag_plan_get_info(plan, &info);
    (void)lt48_jtag_plan_json(plan, &json_data, &json_size);
    if (json) {
        fputs("{\"schema\":1,\"operation\":\"jtag-plan-scan\",\"plan\":",
              stdout);
        fwrite(json_data, 1u, json_size, stdout);
        fputs(",\"plan_sha256\":\"", stdout);
        print_hex(info.sha256, LT48_SHA256_SIZE);
        puts("\",\"usb_opened\":false}");
    } else {
        printf("offline JTAG %s scan: %zu bits, %zu TCK edges, %u Hz\n"
               "sampled edges: %zu; safe end: %s; transport ready: no\n"
               "plan SHA-256: ",
               info.kind == LT48_JTAG_SCAN_IR ? "IR" : "DR",
               info.bit_count, info.step_count, info.tck_hz,
               info.sampled_step_count,
               lt48_jtag_tap_state_name(info.final_state));
        print_hex(info.sha256, LT48_SHA256_SIZE);
        putchar('\n');
    }
    lt48_jtag_plan_close(plan);
    return 0;
}

static unsigned char *allocate_hex_bytes(
    const char *text, size_t *out_size
) {
    unsigned char *bytes;
    size_t parsed_size = 0u;
    size_t length = strlen(text);
    if (length == 0u || length % 2u != 0u) return NULL;
    *out_size = length / 2u;
    bytes = malloc(*out_size);
    if (bytes == NULL) return NULL;
    if (!parse_hex_buffer(text, bytes, *out_size, &parsed_size) ||
        parsed_size != *out_size) {
        free(bytes);
        return NULL;
    }
    return bytes;
}

static int jtag_evaluate_command(int argc, char **argv) {
    lt48_jtag_result_request request = {0};
    lt48_jtag_result result;
    unsigned char *tdo = NULL;
    unsigned char *expected = NULL;
    unsigned char *mask = NULL;
    uint32_t bits;
    int json = 0;
    lt48_status status;
    if (argc < 5 || !parse_u32(argv[4], &bits) || bits == 0u) return 2;
    tdo = allocate_hex_bytes(argv[3], &request.tdo_size);
    if (tdo == NULL) return 2;
    request.tdo = tdo;
    request.bit_count = bits;
    for (int index = 5; index < argc; ++index) {
        if (strcmp(argv[index], "--json") == 0 && !json) {
            json = 1;
        } else if (strcmp(argv[index], "--expected") == 0 &&
                   index + 1 < argc && expected == NULL) {
            expected = allocate_hex_bytes(argv[++index], &request.expected_size);
            if (expected == NULL) goto usage_failure;
            request.expected = expected;
        } else if (strcmp(argv[index], "--mask") == 0 &&
                   index + 1 < argc && mask == NULL) {
            mask = allocate_hex_bytes(argv[++index], &request.mask_size);
            if (mask == NULL) goto usage_failure;
            request.mask = mask;
        } else {
            goto usage_failure;
        }
    }
    status = lt48_jtag_evaluate_result(&request, &result);
    free(tdo); free(expected); free(mask);
    if (status != LT48_OK) return 2;
    if (json) {
        printf("{\"schema\":1,\"operation\":\"jtag-evaluate\","
               "\"bit_count\":%u,\"compared\":%s,\"matched\":%s,"
               "\"compared_bit_count\":%zu,\"stuck_low_observed\":%s,"
               "\"stuck_high_observed\":%s,\"first_mismatch\":",
               bits, result.compared ? "true" : "false",
               result.matched ? "true" : "false", result.compared_bit_count,
               result.stuck_low_observed ? "true" : "false",
               result.stuck_high_observed ? "true" : "false");
        if (result.has_mismatch)
            printf("{\"bit\":%zu,\"expected\":%u,\"actual\":%u}",
                   result.first_mismatch_bit, result.expected_bit,
                   result.actual_bit);
        else fputs("null", stdout);
        puts(",\"usb_opened\":false}");
    } else if (result.compared && !result.matched) {
        printf("TDO mismatch at bit %zu: expected %u, actual %u\n",
               result.first_mismatch_bit, result.expected_bit,
               result.actual_bit);
    } else {
        printf("TDO %s; stuck-low observed: %s; stuck-high observed: %s\n",
               result.compared ? "matched" : "observed without comparison",
               result.stuck_low_observed ? "yes" : "no",
               result.stuck_high_observed ? "yes" : "no");
    }
    return result.compared && !result.matched ? 1 : 0;

usage_failure:
    free(tdo); free(expected); free(mask);
    return 2;
}

static int jtag_decode_chain_command(int argc, char **argv) {
    unsigned char *tdo;
    size_t tdo_size;
    uint32_t bits;
    uint32_t element_count;
    lt48_jtag_chain_element *elements;
    lt48_jtag_chain_report report = {0};
    int json = argc == 7;
    lt48_status status;
    if ((argc != 6 && argc != 7) ||
        (json && strcmp(argv[6], "--json") != 0) ||
        !parse_u32(argv[4], &bits) || bits == 0u ||
        !parse_u32(argv[5], &element_count) || element_count == 0u)
        return 2;
    tdo = allocate_hex_bytes(argv[3], &tdo_size);
    if (tdo == NULL) {
        free(tdo);
        return 2;
    }
    elements = calloc(element_count, sizeof(*elements));
    if (elements == NULL) {
        free(tdo);
        return 6;
    }
    status = lt48_jtag_decode_chain(tdo, tdo_size, bits, element_count,
                                    elements, element_count, &report);
    free(tdo);
    if (json) {
        printf("{\"schema\":1,\"operation\":\"jtag-decode-chain\","
               "\"complete\":%s,\"requested_elements\":%zu,"
               "\"decoded_elements\":%zu,\"consumed_bits\":%zu,"
               "\"trailing_bits\":%zu,\"elements\":[",
               report.complete ? "true" : "false",
               report.requested_element_count, report.decoded_element_count,
               report.consumed_bit_count, report.trailing_bit_count);
        for (size_t index = 0u; index < report.decoded_element_count; ++index) {
            const lt48_jtag_chain_element *element = &elements[index];
            printf("%s{\"index\":%zu,\"kind\":\"%s\",\"first_bit\":%zu,"
                   "\"bit_count\":%zu",
                   index == 0u ? "" : ",", index,
                   element->kind == LT48_JTAG_CHAIN_IDCODE ? "idcode" : "bypass",
                   element->first_bit, element->bit_count);
            if (element->kind == LT48_JTAG_CHAIN_IDCODE)
                printf(",\"idcode\":%u,\"manufacturer_id\":%u,"
                       "\"part_number\":%u,\"version\":%u",
                       element->idcode, element->manufacturer_id,
                       element->part_number, element->version);
            putchar('}');
        }
        puts("],\"usb_opened\":false}");
    } else if (status == LT48_OK) {
        printf("decoded %zu JTAG chain elements from %zu bits\n",
               report.decoded_element_count, report.consumed_bit_count);
        for (size_t index = 0u; index < report.decoded_element_count; ++index) {
            const lt48_jtag_chain_element *element = &elements[index];
            if (element->kind == LT48_JTAG_CHAIN_IDCODE)
                printf("%zu: IDCODE 0x%08x manufacturer 0x%03x part 0x%04x version %u\n",
                       index, element->idcode, element->manufacturer_id,
                       element->part_number, element->version);
            else printf("%zu: BYPASS\n", index);
        }
    } else {
        fprintf(stderr, "labtool48: incomplete/ambiguous JTAG chain capture\n");
    }
    free(elements);
    return status == LT48_OK ? 0 : 2;
}

static int parse_contact_list(
    const char *text, uint8_t contacts[8], size_t *out_count
) {
    const char *cursor = text;
    size_t count = 0u;
    while (*cursor != '\0') {
        char *end = NULL;
        unsigned long value;
        if (count == 8u || *cursor == '-') return 0;
        value = strtoul(cursor, &end, 0);
        if (end == cursor || value == 0u || value > 48u) return 0;
        contacts[count++] = (uint8_t)value;
        if (*end == '\0') {
            cursor = end;
            break;
        }
        if (*end != ',') return 0;
        cursor = end + 1;
    }
    if (count < 6u) return 0;
    *out_count = count;
    return 1;
}

static int jtag_validate_profile_command(int argc, char **argv) {
    lt48_jtag_target_profile profile = {0};
    lt48_jtag_profile_report report;
    uint8_t contacts[8];
    size_t contact_count;
    uint32_t minimum;
    uint32_t maximum;
    uint32_t max_tck;
    char digest[LT48_SHA256_SIZE * 2u + 1u];
    int json = argc == 15;
    lt48_status status;
    if ((argc != 14 && argc != 15) ||
        (json && strcmp(argv[14], "--json") != 0) ||
        !parse_contact_list(argv[5], contacts, &contact_count) ||
        !parse_u32(argv[7], &minimum) || !parse_u32(argv[8], &maximum) ||
        !parse_u32(argv[9], &max_tck)) return 2;
    profile.adapter_id = argv[3];
    profile.adapter_revision = argv[4];
    profile.tck_contact = contacts[0];
    profile.tms_contact = contacts[1];
    profile.tdi_contact = contacts[2];
    profile.tdo_contact = contacts[3];
    profile.ground_contact = contacts[4];
    profile.vref_contact = contacts[5];
    profile.reset_contact = contact_count >= 7u ? contacts[6] : 0u;
    profile.target_power_contact = contact_count >= 8u ? contacts[7] : 0u;
    if (strcmp(argv[6], "external") == 0)
        profile.power_ownership = LT48_JTAG_POWER_EXTERNAL;
    else if (strcmp(argv[6], "adapter") == 0)
        profile.power_ownership = LT48_JTAG_POWER_ADAPTER;
    else if (strcmp(argv[6], "programmer") == 0)
        profile.power_ownership = LT48_JTAG_POWER_PROGRAMMER;
    else return 2;
    profile.vref_min_millivolts = minimum;
    profile.vref_max_millivolts = maximum;
    profile.maximum_tck_hz = max_tck;
    if (strcmp(argv[10], "present") == 0)
        profile.level_translation = LT48_JTAG_ASSURANCE_PRESENT;
    else if (strcmp(argv[10], "not-required") == 0)
        profile.level_translation = LT48_JTAG_ASSURANCE_NOT_REQUIRED;
    else return 2;
    if (strcmp(argv[11], "present") == 0)
        profile.contention_control = LT48_JTAG_ASSURANCE_PRESENT;
    else if (strcmp(argv[11], "not-required") == 0)
        profile.contention_control = LT48_JTAG_ASSURANCE_NOT_REQUIRED;
    else return 2;
    if (strcmp(argv[12], "none") == 0)
        profile.target_reset = LT48_JTAG_RESET_NOT_CONNECTED;
    else if (strcmp(argv[12], "hold") == 0)
        profile.target_reset = LT48_JTAG_RESET_HOLD_DURING_CONNECT;
    else if (strcmp(argv[12], "released") == 0)
        profile.target_reset = LT48_JTAG_RESET_RELEASED;
    else return 2;
    profile.disconnect_high_impedance = 1;
    if (strcmp(argv[13], "high-z") == 0)
        profile.disconnect_target_power_off = 0;
    else if (strcmp(argv[13], "high-z-power-off") == 0)
        profile.disconnect_target_power_off = 1;
    else return 2;
    status = lt48_jtag_validate_target_profile(&profile, &report);
    hex_string(report.sha256, LT48_SHA256_SIZE, digest);
    if (json) {
        printf("{\"schema\":1,\"operation\":\"jtag-validate-profile\","
               "\"valid\":%s,\"issue\":",
               status == LT48_OK ? "true" : "false");
        print_json_string(lt48_jtag_profile_issue_name(report.issue));
        fputs(",\"diagnostic\":", stdout);
        print_json_string(report.diagnostic);
        printf(",\"mapped_contact_count\":%zu,\"profile_sha256\":",
               report.mapped_contact_count);
        if (status == LT48_OK) print_json_string(digest);
        else fputs("null", stdout);
        puts(",\"transport_ready\":false,\"usb_opened\":false}");
    } else if (status == LT48_OK) {
        printf("complete external-target JTAG profile; %zu mapped contacts\n"
               "profile SHA-256: %s\ntransport ready: no\n",
               report.mapped_contact_count, digest);
    } else {
        fprintf(stderr, "labtool48: JTAG profile rejected (%s): %s\n",
                lt48_jtag_profile_issue_name(report.issue), report.diagnostic);
    }
    return status == LT48_OK ? 0 : 4;
}

static int diagnostics_exit_codes(int json) {
    if (json) {
        fputs("{\"schema\":1,\"exit_codes\":[", stdout);
        for (size_t index = 0u; index <= 9u; ++index) {
            if (index != 0u) putchar(',');
            printf("{\"code\":%zu,\"meaning\":", index);
            print_json_string(exit_status_meaning((int)index));
            putchar('}');
        }
        puts("]}");
    } else {
        for (size_t index = 0u; index <= 9u; ++index)
            printf("%zu\t%s\n", index, exit_status_meaning((int)index));
    }
    return 0;
}

static void print_trace_event_json(const lt48_trace_event *event) {
    printf("{\"sequence\":%zu,\"direction\":", event->sequence);
    print_json_string(event->direction == LT48_TRACE_OUT ? "out" : "in");
    printf(",\"endpoint\":%u,\"requested_length\":%zu,\"data_hex\":",
           event->endpoint, event->requested_length);
    print_json_string(event->data_hex);
    printf(",\"timeout_ms\":%u,\"status\":", event->timeout_ms);
    print_json_string(event->status);
    fputs(",\"error\":", stdout);
    if (event->error == NULL) fputs("null", stdout);
    else print_json_string(event->error);
    putchar('}');
}

static int trace_inspect_command(const char *path, int json) {
    lt48_transfer_trace *trace = NULL;
    lt48_trace_info info;
    char digest[LT48_SHA256_SIZE * 2u + 1u];
    lt48_status status = lt48_trace_open(path, &trace);
    if (status == LT48_OK) status = lt48_trace_get_info(trace, &info);
    if (status != LT48_OK) {
        fprintf(stderr, "labtool48: trace inspection failed: %s\n",
                lt48_status_string(status));
        lt48_trace_close(trace);
        return 2;
    }
    hex_string(info.trace_sha256, LT48_SHA256_SIZE, digest);
    if (json) {
        printf("{\"schema_version\":%u,\"event_count\":%zu,"
               "\"trace_sha256\":",
               info.schema_version, info.event_count);
        print_json_string(digest);
        fputs(",\"digest_claimed\":", stdout);
        fputs(info.had_claimed_sha256 ? "true" : "false", stdout);
        fputs(",\"metadata\":", stdout);
        if (info.metadata_json_size != 0u)
            fwrite(info.metadata_json, 1u, info.metadata_json_size, stdout);
        else
            fputs("{}", stdout);
        puts("}");
    } else {
        printf("valid schema-%u transport trace\n", info.schema_version);
        printf("events:       %zu\n", info.event_count);
        printf("trace SHA-256: %s%s\n", digest,
               info.had_claimed_sha256 ? " (verified)" : " (computed)");
        fputs("metadata:     ", stdout);
        fwrite(info.metadata_json, 1u, info.metadata_json_size, stdout);
        putchar('\n');
    }
    lt48_trace_close(trace);
    return 0;
}

static int trace_compare_command(
    const char *left_path, const char *right_path, int json
) {
    lt48_transfer_trace *left = NULL;
    lt48_transfer_trace *right = NULL;
    lt48_trace_comparison comparison;
    lt48_status status = lt48_trace_open(left_path, &left);
    if (status == LT48_OK) status = lt48_trace_open(right_path, &right);
    if (status == LT48_OK)
        status = lt48_trace_compare(left, right, &comparison);
    if (status != LT48_OK) {
        fprintf(stderr, "labtool48: trace comparison failed: %s\n",
                lt48_status_string(status));
        lt48_trace_close(right);
        lt48_trace_close(left);
        return 2;
    }
    if (json) {
        printf("{\"equal\":%s,\"first_difference\":",
               comparison.equal ? "true" : "false");
        if (comparison.equal) fputs("null", stdout);
        else printf("%zu", comparison.first_difference);
        fputs(",\"left\":", stdout);
        if (comparison.has_left) print_trace_event_json(&comparison.left);
        else fputs("null", stdout);
        fputs(",\"right\":", stdout);
        if (comparison.has_right) print_trace_event_json(&comparison.right);
        else fputs("null", stdout);
        puts("}");
    } else if (comparison.equal) {
        puts("traces are event-for-event equal");
    } else {
        printf("traces differ at event %zu\n", comparison.first_difference);
        printf("left:  %s\n", comparison.has_left ? "event present" : "end");
        printf("right: %s\n", comparison.has_right ? "event present" : "end");
    }
    lt48_trace_close(right);
    lt48_trace_close(left);
    return comparison.equal ? 0 : 1;
}

static int auto_id_eprom_plan_command(
    int has_pin_hint, uint16_t pin_hint, int json
) {
    lt48_auto_id_eprom_plan_info plan;
    lt48_status status = lt48_auto_id_eprom_plan(
        has_pin_hint, pin_hint, &plan
    );
    if (status != LT48_OK) {
        fprintf(stderr, "labtool48: cannot plan EPROM Auto-ID provider: %s\n",
                lt48_status_string(status));
        return status == LT48_NOT_FOUND ? 4 : 2;
    }
    if (json) {
        printf("{\"schema\":1,\"operation\":\"auto-id-eprom-plan\"," 
               "\"provider\":\"auto-select-eprom-six-recipe\"," 
               "\"probe_always_first\":true,"
               "\"selected_device_independent\":true,"
               "\"cleanup_finalizer_required\":true,"
               "\"executable\":false,\"offline\":true,"
               "\"usb_opened\":false,\"recipe_count\":%zu,"
               "\"eligible_recipe_count\":%zu,\"pin_hint\":",
               plan.recipe_count, plan.eligible_recipe_count);
        if (has_pin_hint) printf("%u", pin_hint); else fputs("null", stdout);
        printf(",\"maximum_vcc_centivolts\":%u,"
               "\"maximum_signature_vpp_centivolts\":%u,"
               "\"provider_sha256\":\"",
               plan.maximum_operating_vcc_centivolts,
               plan.maximum_signature_vpp_centivolts);
        print_hex(plan.provider_sha256, LT48_SHA256_SIZE);
        fputs("\",\"recipes\":[", stdout);
    } else {
        puts("Auto Select EPROM provider — offline plan; no USB opened");
        printf("probe first; selected device ignored; finalizer mandatory; "
               "%zu/%zu eligible recipes\n",
               plan.eligible_recipe_count, plan.recipe_count);
        printf("maximum rails: %.2f V operating, %.2f V signature\n",
               plan.maximum_operating_vcc_centivolts / 100.0,
               plan.maximum_signature_vpp_centivolts / 100.0);
    }
    for (size_t index = 0u; index < plan.recipe_count; ++index) {
        lt48_auto_id_eprom_recipe recipe;
        int eligible;
        (void)lt48_auto_id_eprom_recipe_get(index, &recipe);
        eligible = !has_pin_hint || recipe.package_pin_count == pin_hint;
        if (json) {
            if (index != 0u) putchar(',');
            printf("{\"pins\":%u,\"eligible\":%s,"
                   "\"vcc_centivolts\":%u,\"vpp_centivolts\":%u,"
                   "\"data_bits\":%u,\"address_patterns\":%u}",
                   recipe.package_pin_count, eligible ? "true" : "false",
                   recipe.operating_vcc_centivolts,
                   recipe.signature_vpp_centivolts,
                   recipe.data_pin_count,
                   1u << recipe.address_pattern_pin_count);
        } else {
            printf("  %2u pins%s: %u data bits, %u address pattern%s\n",
                   recipe.package_pin_count, eligible ? "" : " (filtered)",
                   recipe.data_pin_count,
                   1u << recipe.address_pattern_pin_count,
                   recipe.address_pattern_pin_count == 0u ? "" : "s");
        }
    }
    if (json) puts("]}");
    return 0;
}

static int auto_id_decode_socket_command(const char *text, int json) {
    unsigned char raw[12];
    size_t size = 0u;
    lt48_auto_id_socket_observation observation;
    lt48_auto_id_eprom_recipe recipe;
    lt48_status status;
    if (!parse_hex_buffer(text, raw, sizeof(raw), &size) || size != sizeof(raw))
        return 2;
    status = lt48_auto_id_eprom_decode_socket(raw, &observation);
    if (status != LT48_OK) {
        fprintf(stderr, "labtool48: cannot decode socket probe: %s\n",
                lt48_status_string(status));
        return status == LT48_NOT_FOUND ? 1 : 2;
    }
    status = lt48_auto_id_eprom_recipe_find(
        observation.package_pin_count, &recipe
    );
    if (json) {
        printf("{\"schema\":1,\"operation\":\"auto-id-decode-socket\","
               "\"raw\":");
        print_json_string(text);
        printf(",\"left_mask\":%u,\"right_mask\":%u,"
               "\"occupied_mask\":%u,\"first_half_position\":%u,"
               "\"last_half_position\":%u,\"package_pin_count\":%u,"
               "\"eprom_recipe_supported\":%s,\"offline\":true,"
               "\"usb_opened\":false}\n",
               observation.left_contact_mask, observation.right_contact_mask,
               observation.occupied_contact_mask,
               observation.first_half_position,
               observation.last_half_position,
               observation.package_pin_count,
               status == LT48_OK ? "true" : "false");
    } else {
        printf("socket probe: span %u..%u, %u pins; EPROM recipe %s\n",
               observation.first_half_position,
               observation.last_half_position,
               observation.package_pin_count,
               status == LT48_OK ? "available" : "not available");
    }
    return 0;
}

static int auto_id_eprom_prepare_command(
    uint8_t bus, uint8_t address, int acquire_signature,
    int has_pin_hint, uint16_t pin_hint, const char *report_path, int json
) {
    lt48_auto_id_eprom_preparation preparation;
    char provider_hex[LT48_SHA256_SIZE * 2u + 1u];
    char confirmation_hex[LT48_SHA256_SIZE * 2u + 1u];
    lt48_status status = lt48_auto_id_eprom_prepare(
        bus, address, acquire_signature, has_pin_hint, pin_hint,
        &preparation
    );
    if (status != LT48_OK) {
        fprintf(stderr, "labtool48: cannot prepare EPROM Auto-ID: %s\n",
                lt48_status_string(status));
        return status == LT48_NOT_FOUND ? 4 : 2;
    }
    if (report_path != NULL) {
        status = lt48_auto_id_eprom_operation_report_write_planned(
            report_path, &preparation);
        if (status != LT48_OK) {
            fprintf(stderr,
                    "labtool48: cannot create Auto-ID plan report: %s\n",
                    lt48_status_string(status));
            return 5;
        }
    }
    hex_string(preparation.plan.provider_sha256, LT48_SHA256_SIZE,
               provider_hex);
    hex_string(preparation.confirmation_sha256, LT48_SHA256_SIZE,
               confirmation_hex);
    if (json) {
        printf("{\"schema\":1,\"operation\":\"auto-id-eprom-prepare\"," 
               "\"mode\":\"%s\",\"bus\":%u,\"address\":%u,"
               "\"pin_hint\":",
               acquire_signature ? "signature" : "probe-only",
               bus, address);
        if (has_pin_hint) printf("%u", pin_hint); else fputs("null", stdout);
        printf(",\"recipe_count\":%zu,\"eligible_recipe_count\":%zu,"
               "\"maximum_vcc_centivolts\":%u,"
               "\"maximum_signature_vpp_centivolts\":%u,"
               "\"readiness_required\":true,"
               "\"finalizer_required\":true,"
               "\"executable\":false,\"offline\":true,"
               "\"usb_opened\":false,\"provider_sha256\":",
               preparation.plan.recipe_count,
               preparation.plan.eligible_recipe_count,
               preparation.plan.maximum_operating_vcc_centivolts,
               preparation.plan.maximum_signature_vpp_centivolts);
        print_json_string(provider_hex);
        fputs(",\"confirmation_sha256\":", stdout);
        print_json_string(confirmation_hex);
        fputs(",\"report\":", stdout);
        if (report_path != NULL) print_json_string(report_path);
        else fputs("null", stdout);
        puts("}");
    } else {
        puts("Auto Select EPROM — immutable offline preparation; no USB opened");
        printf("target: %03u:%03u\nmode: %s\n",
               bus, address,
               acquire_signature ? "signature (up to 12.00 V)" :
                                   "contact probe only");
        if (has_pin_hint) printf("explicit package hint: %u pins\n", pin_hint);
        else puts("explicit package hint: none");
        printf("provider SHA-256:     %s\nconfirmation SHA-256: %s\n",
               provider_hex, confirmation_hex);
        if (report_path != NULL) printf("plan report: %s\n", report_path);
        puts("same-handle transport readiness and a matching confirmation are "
             "required before future execution; execution is not exposed yet");
    }
    return 0;
}

static void diagnostics_json_check(
    const char *name, int passed, int *first
) {
    if (!*first) putchar(',');
    *first = 0;
    fputs("{\"name\":", stdout);
    print_json_string(name);
    printf(",\"passed\":%s}", passed ? "true" : "false");
}

static int diagnostics_self_test(const char *installation_path, int json) {
    lt48_diagnostics_report report;
    lt48_status status = lt48_diagnostics_self_test(
        installation_path, &report
    );
    char manifest[LT48_SHA256_SIZE * 2u + 1u];
    if (status != LT48_OK) {
        fprintf(stderr, "labtool48: diagnostics could not run: %s\n",
                lt48_status_string(status));
        return 5;
    }
    if (json) {
        int first = 1;
        printf("{\"schema\":%u,\"operation\":\"diagnostics-self-test\","
               "\"passed\":%s,\"checks\":[",
               report.schema_version, report.passed ? "true" : "false");
        diagnostics_json_check("sha256-known-answer",
                               report.sha256_check_passed, &first);
        diagnostics_json_check("operation-surface",
                               report.operation_surface_check_passed, &first);
        diagnostics_json_check("buffer-model",
                               report.buffer_model_check_passed, &first);
        diagnostics_json_check("jtag-planner",
                               report.jtag_planner_check_passed, &first);
        diagnostics_json_check("progress-model",
                               report.progress_model_check_passed, &first);
        if (report.installation_requested) {
            diagnostics_json_check(
                "installation-audit",
                report.installation_status == LT48_OK &&
                    report.installation_audit.catalogue_device_count != 0u &&
                    report.installation_audit.failed_device_count == 0u,
                &first
            );
        }
        fputs("],\"installation\":", stdout);
        if (!report.installation_requested) {
            fputs("null", stdout);
        } else {
            printf("{\"path\":");
            print_json_string(installation_path);
            fputs(",\"status\":", stdout);
            print_json_string(lt48_status_string(report.installation_status));
            printf(",\"catalogue_device_count\":%zu,"
                   "\"definition_file_count\":%zu,"
                   "\"resolved_device_count\":%zu,"
                   "\"failed_device_count\":%zu,\"manifest_sha256\":",
                   report.installation_audit.catalogue_device_count,
                   report.installation_audit.definition_file_count,
                   report.installation_audit.resolved_device_count,
                   report.installation_audit.failed_device_count);
            if (report.installation_status == LT48_OK) {
                hex_string(report.installation_audit.manifest_sha256,
                           LT48_SHA256_SIZE, manifest);
                print_json_string(manifest);
            } else {
                fputs("null", stdout);
            }
            putchar('}');
        }
        puts(",\"usb_opened\":false}");
    } else {
        printf("offline native self-test: %s\n",
               report.passed ? "PASS" : "FAIL");
        printf("  SHA-256 known answer: %s\n",
               report.sha256_check_passed ? "pass" : "FAIL");
        printf("  operation surface:   %s\n",
               report.operation_surface_check_passed ? "pass" : "FAIL");
        printf("  buffer model:        %s\n",
               report.buffer_model_check_passed ? "pass" : "FAIL");
        printf("  JTAG planner:        %s\n",
               report.jtag_planner_check_passed ? "pass" : "FAIL");
        printf("  progress model:      %s\n",
               report.progress_model_check_passed ? "pass" : "FAIL");
        if (report.installation_requested)
            printf("  installation audit:  %s (%zu catalogue, %zu resolved, "
                   "%zu failed)\n",
                   report.installation_status == LT48_OK &&
                       report.installation_audit.failed_device_count == 0u ?
                       "pass" : "FAIL",
                   report.installation_audit.catalogue_device_count,
                   report.installation_audit.resolved_device_count,
                   report.installation_audit.failed_device_count);
        puts("  USB opened:           no");
    }
    return report.passed ? 0 : 5;
}

static int diagnostics_progress_schema(int json) {
    const unsigned char *manifest;
    size_t manifest_size;
    lt48_status status = lt48_progress_manifest_json(
        &manifest, &manifest_size
    );
    if (status != LT48_OK) {
        fprintf(stderr, "labtool48: progress schema unavailable: %s\n",
                lt48_status_string(status));
        return 5;
    }
    if (json) {
        fwrite(manifest, 1u, manifest_size, stdout);
        putchar('\n');
    } else {
        puts("progress schema 1");
        puts("encoding: one JSON object per event");
        puts("scopes: operation, session, phase, progress");
        puts("sequence: zero-based and strictly increasing");
        puts("payload bytes allowed: no");
    }
    return 0;
}

static void print_json_nullable_string(const char *value) {
    if (value == NULL) fputs("null", stdout);
    else print_json_string(value);
}

static int report_inspect(const char *path, int json) {
    lt48_operation_report *report = NULL;
    lt48_operation_report_info info;
    lt48_status status = lt48_operation_report_open(path, &report);
    if (status != LT48_OK) {
        fprintf(stderr, "labtool48: cannot inspect operation report %s: %s\n",
                path, lt48_status_string(status));
        return 2;
    }
    status = lt48_operation_report_get_info(report, &info);
    if (status != LT48_OK) {
        lt48_operation_report_close(report);
        fprintf(stderr, "labtool48: cannot summarize operation report %s: %s\n",
                path, lt48_status_string(status));
        return 2;
    }
    if (json) {
        fputs("{\"schema\":1,\"valid\":true,\"integrity_verified\":true,"
              "\"path\":", stdout);
        print_json_string(path);
        fputs(",\"run_id\":", stdout);
        print_json_string(info.run_id);
        fputs(",\"device\":", stdout);
        print_json_string(info.device_key);
        fputs(",\"operation\":", stdout);
        print_json_string(info.operation);
        fputs(",\"status\":", stdout);
        print_json_string(info.status);
        fputs(",\"support_level\":", stdout);
        print_json_string(info.support_level);
        fputs(",\"address_range\":", stdout);
        if (info.has_address_range)
            printf("{\"start\":%u,\"length\":%u}",
                   info.address_start, info.address_length);
        else fputs("null", stdout);
        printf(",\"mismatch_override\":%s,\"cancelled\":%s,"
               "\"observed_result\":",
               info.mismatch_override ? "true" : "false",
               info.cancelled ? "true" : "false");
        print_json_nullable_string(info.observed_result);
        fputs(",\"plan_sha256\":", stdout);
        print_json_string(info.plan_sha256);
        fputs(",\"input_image_sha256\":", stdout);
        print_json_nullable_string(info.input_image_sha256);
        fputs(",\"output\":", stdout);
        if (info.has_output) {
            printf("{\"size\":%zu,\"sha256\":", info.output_size);
            print_json_nullable_string(info.output_sha256);
            putchar('}');
        } else {
            fputs("null", stdout);
        }
        printf(",\"resource_count\":%zu,\"phase_count\":%zu,"
               "\"session_event_count\":%zu,\"cleanup\":{"
               "\"phase_count\":%zu,\"failed_phase_count\":%zu,"
               "\"transport_close_status\":",
               info.resource_count, info.phase_count, info.session_event_count,
               info.cleanup_phase_count, info.cleanup_failed_phase_count);
        print_json_string(info.transport_close_status);
        fputs("},\"verification_result\":", stdout);
        print_json_string(info.verification_result);
        printf(",\"progress_observer_error_count\":%zu,\"has_failure\":%s,"
               "\"failure\":",
               info.progress_observer_error_count,
               info.has_failure ? "true" : "false");
        if (info.has_failure) {
            fputs("{\"type\":", stdout);
            print_json_string(info.failure_type);
            fputs(",\"message\":", stdout);
            print_json_string(info.failure_message);
            putchar('}');
        } else {
            fputs("null", stdout);
        }
        fputs(",\"report_sha256\":", stdout);
        print_json_string(info.report_sha256);
        puts("}");
    } else {
        printf("valid schema-%u operation report; integrity verified\n",
               info.schema_version);
        printf("run:       %s\n", info.run_id);
        printf("device:    %s\n", info.device_key);
        printf("operation: %s\n", info.operation);
        printf("status:    %s\n", info.status);
        printf("support:   %s\n", info.support_level);
        if (info.has_address_range)
            printf("range:     0x%x + %u bytes\n", info.address_start,
                   info.address_length);
        printf("override:  %s; cancelled: %s\n",
               info.mismatch_override ? "yes" : "no",
               info.cancelled ? "yes" : "no");
        printf("phases:    %zu (%zu cleanup, %zu failed)\n",
               info.phase_count, info.cleanup_phase_count,
               info.cleanup_failed_phase_count);
        printf("transport: %s\n", info.transport_close_status);
        if (info.has_failure)
            printf("failure:   %s — %s\n", info.failure_type,
                   info.failure_message);
        printf("report SHA-256: %s\n", info.report_sha256);
    }
    lt48_operation_report_close(report);
    return 0;
}

static int new_device_lib_command(const char *path,int json) {
    char root[4096];lt48_new_device_lib *lib=NULL;lt48_new_device_lib_report report={0};
    lt48_status st=path?LT48_OK:lt48_new_device_lib_default_path(root,sizeof root);
    if(st==LT48_OK)st=lt48_new_device_lib_open(path?path:root,&lib,&report);
    if(json) {
        printf("{\"status\":");print_json_string(lt48_status_string(st));
        printf(",\"root\":");print_json_string(report.root);
        printf(",\"sha256\":");print_json_string(report.sha256);
        printf(",\"diagnostic\":");print_json_string(report.diagnostic);
        printf(",\"resource_count\":%zu,\"bundle_count\":%zu,\"devices\":[",report.resources,report.bundles);
        for(size_t i=0;i<lt48_new_device_lib_count(lib);i++) {
            const lt48_new_device_record *r=lt48_new_device_lib_record(lib,i);
            printf("%s{\"id\":",i?",":"");print_json_string(r->id);
            printf(",\"manufacturer\":");print_json_string(r->manufacturer);
            printf(",\"part\":");print_json_string(r->part);
            printf(",\"kind\":");print_json_string(r->kind);
            printf(",\"qualification\":");print_json_string(r->status);
            printf(",\"base\":");print_json_string(r->base_key);
            printf(",\"source\":");print_json_string(r->source_path);printf("}");
        }
        printf("]}\n");
    } else {
        printf("NewDeviceLib: %s\n%s\n",report.root,report.diagnostic);
        for(size_t i=0;i<lt48_new_device_lib_count(lib);i++) {
            const lt48_new_device_record *r=lt48_new_device_lib_record(lib,i);
            printf("%s:%s — %s / %s [%s]\n",r->manufacturer,r->part,r->kind,r->status,r->id);
        }
        printf("%zu definitions, %zu resources, %zu Composer bundles; no USB access\n",report.records,report.resources,report.bundles);
    }
    lt48_new_device_lib_close(lib);return st==LT48_OK?0:2;
}

static int main_dispatch(int argc, char **argv) {
    if(argc>=3 && argc<=5 && !strcmp(argv[1],"new-device-lib") &&
       (!strcmp(argv[2],"list")||!strcmp(argv[2],"validate"))) {
        int json=!strcmp(argv[argc-1],"--json");
        if(argc==5&&!json)return 2;
        const char *path=argc>3 && strcmp(argv[3],"--json")?argv[3]:NULL;
        return new_device_lib_command(path,json);
    }

    if (argc >= 3 && argc <= 6 && strcmp(argv[1], "auto-id") == 0 &&
        strcmp(argv[2], "eprom-plan") == 0) {
        uint16_t pins = 0u;
        int has_pins = 0;
        int json = 0;
        for (int index = 3; index < argc; ++index) {
            if (strcmp(argv[index], "--json") == 0 && !json) {
                json = 1;
            } else if (strcmp(argv[index], "--pins") == 0 && !has_pins &&
                       index + 1 < argc && parse_u16(argv[++index], &pins)) {
                has_pins = 1;
            } else {
                usage(stderr, argv[0]);
                return 2;
            }
        }
        return auto_id_eprom_plan_command(has_pins, pins, json);
    }
    if (argc >= 8 && argc <= 13 && strcmp(argv[1], "auto-id") == 0 &&
        strcmp(argv[2], "prepare-eprom") == 0) {
        uint8_t bus = 0u;
        uint8_t address = 0u;
        uint16_t pins = 0u;
        int have_bus = 0;
        int have_address = 0;
        int has_pins = 0;
        int have_mode = 0;
        int acquire_signature = 0;
        int json = 0;
        const char *report_path = NULL;
        for (int index = 3; index < argc; ++index) {
            if (strcmp(argv[index], "--bus") == 0 && !have_bus &&
                index + 1 < argc) {
                have_bus = parse_u8(argv[++index], &bus);
            } else if (strcmp(argv[index], "--address") == 0 &&
                       !have_address && index + 1 < argc) {
                have_address = parse_u8(argv[++index], &address);
            } else if (strcmp(argv[index], "--pins") == 0 && !has_pins &&
                       index + 1 < argc) {
                has_pins = parse_u16(argv[++index], &pins);
            } else if (strcmp(argv[index], "--probe-only") == 0 &&
                       !have_mode) {
                have_mode = 1;
                acquire_signature = 0;
            } else if (strcmp(argv[index], "--signature") == 0 &&
                       !have_mode) {
                have_mode = 1;
                acquire_signature = 1;
            } else if (strcmp(argv[index], "--json") == 0 && !json) {
                json = 1;
            } else if (strcmp(argv[index], "--report") == 0 &&
                       report_path == NULL && index + 1 < argc) {
                report_path = argv[++index];
            } else {
                usage(stderr, argv[0]);
                return 2;
            }
        }
        if (!have_bus || !have_address || !have_mode) {
            usage(stderr, argv[0]);
            return 2;
        }
        return auto_id_eprom_prepare_command(
            bus, address, acquire_signature, has_pins, pins,
            report_path, json);
    }
    if ((argc == 4 || argc == 5) && strcmp(argv[1], "auto-id") == 0 &&
        strcmp(argv[2], "decode-socket") == 0) {
        if (argc == 5 && strcmp(argv[4], "--json") != 0) {
            usage(stderr, argv[0]);
            return 2;
        }
        return auto_id_decode_socket_command(argv[3], argc == 5);
    }
    if ((argc == 2 || argc == 3) && strcmp(argv[1], "version") == 0) {
        if (argc == 3 && strcmp(argv[2], "--json") != 0) {
            usage(stderr, argv[0]);
            return 2;
        }
        if (argc == 3) {
            fputs("{\"schema\":1,\"program\":\"labtool48\",\"version\":",
                  stdout);
            print_json_string(lt48_version_string());
            puts("}");
        } else {
            printf("labtool48 %s\n", lt48_version_string());
        }
        return 0;
    }
    if (argc >= 5 && argc <= 8 && strcmp(argv[1], "cache") == 0 &&
        strcmp(argv[2], "import") == 0) {
        const char *runtime_source = argv[3];
        int json = 0;
        int index = 5;
        while (index < argc) {
            if (strcmp(argv[index], "--runtime-source") == 0 &&
                index + 1 < argc && runtime_source == argv[3]) {
                runtime_source = argv[index + 1];
                index += 2;
            } else if (strcmp(argv[index], "--json") == 0 && !json) {
                json = 1;
                ++index;
            } else {
                usage(stderr, argv[0]);
                return 2;
            }
        }
        if (index != argc) {
            usage(stderr, argv[0]);
            return 2;
        }
        return cache_import_command(
            argv[3], runtime_source, argv[4], json
        );
    }
    if ((argc == 4 || argc == 5) && strcmp(argv[1], "cache") == 0 &&
        strcmp(argv[2], "verify") == 0) {
        if (argc == 5 && strcmp(argv[4], "--json") != 0) {
            usage(stderr, argv[0]);
            return 2;
        }
        return cache_verify_command(argv[3], argc == 5);
    }
    if ((argc == 5 || argc == 6) && strcmp(argv[1], "component") == 0 &&
        strcmp(argv[2], "validate") == 0 &&
        strcmp(argv[4], "--all-variants") == 0) {
        if (argc == 6 && strcmp(argv[5], "--json") != 0) {
            usage(stderr, argv[0]);
            return 2;
        }
        return component_validate_all_variants_command(argv[3], argc == 6);
    }
    if ((argc == 4 || argc == 5) && strcmp(argv[1], "component") == 0 &&
        strcmp(argv[2], "validate") == 0) {
        if (argc == 5 && strcmp(argv[4], "--json") != 0) {
            usage(stderr, argv[0]);
            return 2;
        }
        return component_validate_command(argv[3], argc == 5);
    }
    if ((argc == 5 || argc == 6) && strcmp(argv[1], "component") == 0 &&
        strcmp(argv[2], "resolve") == 0) {
        if (argc == 6 && strcmp(argv[5], "--json") != 0) {
            usage(stderr, argv[0]);
            return 2;
        }
        return component_resolve_command(argv[3], argv[4], argc == 6);
    }
    if ((argc == 9 || argc == 10) && strcmp(argv[1], "component") == 0 &&
        strcmp(argv[2], "init") == 0) {
        if (argc == 10 && strcmp(argv[9], "--json") != 0) {
            usage(stderr, argv[0]);
            return 2;
        }
        return component_init_command(
            argv[3], argv[4], argv[5], argv[6], argv[7], argv[8], argc == 10
        );
    }
    if ((argc == 8 || argc == 9) && strcmp(argv[1], "component") == 0 &&
        strcmp(argv[2], "add-object") == 0) {
        if (argc == 9 && strcmp(argv[8], "--json") != 0) {
            usage(stderr, argv[0]);
            return 2;
        }
        return component_add_object_command(
            argv[3], argv[4], argv[5], argv[6], argv[7],
            "component-add-object", NULL, argc == 9
        );
    }
    if ((argc == 8 || argc == 9) && strcmp(argv[1], "component") == 0 &&
        (strcmp(argv[2], "add-family") == 0 ||
         strcmp(argv[2], "add-variant") == 0 ||
         strcmp(argv[2], "add-package") == 0)) {
        const char *kind = strcmp(argv[2], "add-family") == 0 ? "family" :
            strcmp(argv[2], "add-variant") == 0 ? "variant" : "package";
        const char *operation = strcmp(argv[2], "add-family") == 0 ?
            "component-add-family" :
            strcmp(argv[2], "add-variant") == 0 ?
                "component-add-variant" : "component-add-package";
        if (argc == 9 && strcmp(argv[8], "--json") != 0) {
            usage(stderr, argv[0]);
            return 2;
        }
        return component_add_object_command(
            argv[3], argv[4], argv[5], argv[6], argv[7],
            operation, kind, argc == 9
        );
    }
    if ((argc == 8 || argc == 9) && strcmp(argv[1], "adapter") == 0 &&
        strcmp(argv[2], "init") == 0) {
        if (argc == 9 && strcmp(argv[8], "--json") != 0) {
            usage(stderr, argv[0]);
            return 2;
        }
        return component_add_object_command(
            argv[3], argv[4], argv[5], argv[6], argv[7],
            "adapter-init", "adapter", argc == 9
        );
    }
    if ((argc == 5 || argc == 6) && strcmp(argv[1], "adapter") == 0 &&
        strcmp(argv[2], "test-plan") == 0) {
        if (argc == 6 && strcmp(argv[5], "--json") != 0) {
            usage(stderr, argv[0]);
            return 2;
        }
        return adapter_test_plan_command(argv[3], argv[4], argc == 6);
    }
    if ((argc == 6 || argc == 7) && strcmp(argv[1], "component") == 0 &&
        strcmp(argv[2], "diff") == 0) {
        if (argc == 7 && strcmp(argv[6], "--json") != 0) {
            usage(stderr, argv[0]);
            return 2;
        }
        return component_diff_command(
            argv[3], argv[4], argv[5], argc == 7
        );
    }
    if ((argc == 4 || argc == 5) && strcmp(argv[1], "component") == 0 &&
        strcmp(argv[2], "build-payloads") == 0) {
        if (argc == 5 && strcmp(argv[4], "--json") != 0) {
            usage(stderr, argv[0]);
            return 2;
        }
        return component_build_payloads_command(argv[3], argc == 5);
    }
    if ((argc == 5 || argc == 6) && strcmp(argv[1], "component") == 0 &&
        strcmp(argv[2], "verify-payload-inputs") == 0) {
        if (argc == 6 && strcmp(argv[5], "--json") != 0) {
            usage(stderr, argv[0]);
            return 2;
        }
        return component_verify_payload_inputs_command(
            argv[3], argv[4], argc == 6);
    }
    if ((argc == 7 || argc == 8) && strcmp(argv[1], "component") == 0 &&
        strcmp(argv[2], "execute-payload-builds") == 0) {
        if (strcmp(argv[5], "--confirm") != 0 ||
            (argc == 8 && strcmp(argv[7], "--json") != 0)) {
            usage(stderr, argv[0]);
            return 2;
        }
        return component_execute_payload_builds_command(
            argv[3], argv[4], argv[6], argc == 8);
    }
    if ((argc == 4 || argc == 5) && strcmp(argv[1], "component") == 0 &&
        strcmp(argv[2], "check-semantics") == 0) {
        if (argc == 5 && strcmp(argv[4], "--json") != 0) {
            usage(stderr, argv[0]);
            return 2;
        }
        return component_semantics_command(argv[3], argc == 5);
    }
    if ((argc == 6 || argc == 7) && strcmp(argv[1], "component") == 0 &&
        strcmp(argv[2], "compile-plan") == 0) {
        uint32_t combination_index;
        if ((argc == 7 && strcmp(argv[6], "--json") != 0) ||
            !parse_u32(argv[4], &combination_index)) {
            usage(stderr, argv[0]);
            return 2;
        }
        return component_compile_plan_command(
            argv[3], combination_index, argv[5], argc == 7
        );
    }
    if ((argc == 4 || argc == 5) && strcmp(argv[1], "component") == 0 &&
        strcmp(argv[2], "plan-matrix") == 0) {
        if (argc == 5 && strcmp(argv[4], "--json") != 0) {
            usage(stderr, argv[0]);
            return 2;
        }
        return component_plan_matrix_command(argv[3], argc == 5);
    }
    if ((argc == 5 || argc == 6) && strcmp(argv[1], "component") == 0 &&
        strcmp(argv[2], "evidence") == 0 &&
        strcmp(argv[3], "validate") == 0) {
        if (argc == 6 && strcmp(argv[5], "--json") != 0) {
            usage(stderr, argv[0]);
            return 2;
        }
        return component_evidence_validate_command(argv[4], argc == 6);
    }
    if ((argc == 7 || argc == 8) && strcmp(argv[1], "component") == 0 &&
        strcmp(argv[2], "evidence") == 0 &&
        strcmp(argv[3], "verify") == 0) {
        if (argc == 8 && strcmp(argv[7], "--json") != 0) {
            usage(stderr, argv[0]);
            return 2;
        }
        return component_evidence_verify_command(
            argv[4], argv[5], argv[6], argc == 8);
    }
    if ((argc == 8 || argc == 9) && strcmp(argv[1], "component") == 0 &&
        strcmp(argv[2], "evidence") == 0 &&
        strcmp(argv[3], "import") == 0) {
        if (argc == 9 && strcmp(argv[8], "--json") != 0) {
            usage(stderr, argv[0]);
            return 2;
        }
        return component_evidence_import_command(
            argv[4], argv[5], argv[6], argv[7], argc == 9);
    }
    if ((argc == 6 || argc == 7) && strcmp(argv[1], "component") == 0 &&
        strcmp(argv[2], "evidence") == 0 &&
        strcmp(argv[3], "audit") == 0) {
        if (argc == 7 && strcmp(argv[6], "--json") != 0) {
            usage(stderr, argv[0]);
            return 2;
        }
        return component_evidence_audit_command(argv[4], argv[5], argc == 7);
    }
    if ((argc == 5 || argc == 6) && strcmp(argv[1], "component") == 0 &&
        strcmp(argv[2], "release-check") == 0) {
        if (argc == 6 && strcmp(argv[5], "--json") != 0) {
            usage(stderr, argv[0]);
            return 2;
        }
        return component_release_check_command(argv[3], argv[4], argc == 6);
    }
    if ((argc == 9 || argc == 10) && strcmp(argv[1], "jtag") == 0 &&
        strcmp(argv[2], "plan-scan") == 0) {
        int result = jtag_plan_scan_command(argc, argv);
        if (result == 2) usage(stderr, argv[0]);
        return result;
    }
    if (argc >= 5 && strcmp(argv[1], "jtag") == 0 &&
        strcmp(argv[2], "evaluate") == 0) {
        int result = jtag_evaluate_command(argc, argv);
        if (result == 2) usage(stderr, argv[0]);
        return result;
    }
    if ((argc == 6 || argc == 7) && strcmp(argv[1], "jtag") == 0 &&
        strcmp(argv[2], "decode-chain") == 0) {
        int result = jtag_decode_chain_command(argc, argv);
        if (result == 2 && argc != 7) usage(stderr, argv[0]);
        return result;
    }
    if ((argc == 14 || argc == 15) && strcmp(argv[1], "jtag") == 0 &&
        strcmp(argv[2], "validate-profile") == 0) {
        int result = jtag_validate_profile_command(argc, argv);
        if (result == 2) usage(stderr, argv[0]);
        return result;
    }
    if ((argc == 3 || argc == 4) &&
        strcmp(argv[1], "diagnostics") == 0 &&
        strcmp(argv[2], "exit-codes") == 0) {
        if (argc == 4 && strcmp(argv[3], "--json") != 0) {
            usage(stderr, argv[0]);
            return 2;
        }
        return diagnostics_exit_codes(argc == 4);
    }
    if ((argc == 3 || argc == 4) &&
        strcmp(argv[1], "diagnostics") == 0 &&
        strcmp(argv[2], "progress-schema") == 0) {
        if (argc == 4 && strcmp(argv[3], "--json") != 0) {
            usage(stderr, argv[0]);
            return 2;
        }
        return diagnostics_progress_schema(argc == 4);
    }
    if (argc >= 3 && argc <= 5 &&
        strcmp(argv[1], "diagnostics") == 0 &&
        strcmp(argv[2], "self-test") == 0) {
        const char *installation_path = NULL;
        int json = 0;
        for (int index = 3; index < argc; ++index) {
            if (strcmp(argv[index], "--json") == 0 && !json)
                json = 1;
            else if (installation_path == NULL)
                installation_path = argv[index];
            else {
                usage(stderr, argv[0]);
                return 2;
            }
        }
        return diagnostics_self_test(installation_path, json);
    }
    if ((argc == 4 || argc == 5) && strcmp(argv[1], "trace") == 0 &&
        strcmp(argv[2], "inspect") == 0) {
        if (argc == 5 && strcmp(argv[4], "--json") != 0) {
            usage(stderr, argv[0]);
            return 2;
        }
        return trace_inspect_command(argv[3], argc == 5);
    }
    if ((argc == 5 || argc == 6) && strcmp(argv[1], "trace") == 0 &&
        strcmp(argv[2], "compare") == 0) {
        if (argc == 6 && strcmp(argv[5], "--json") != 0) {
            usage(stderr, argv[0]);
            return 2;
        }
        return trace_compare_command(argv[3], argv[4], argc == 6);
    }
    if ((argc == 4 || argc == 5) && strcmp(argv[1], "report") == 0 &&
        strcmp(argv[2], "inspect") == 0) {
        if (argc == 5 && strcmp(argv[4], "--json") != 0) {
            usage(stderr, argv[0]);
            return 2;
        }
        return report_inspect(argv[3], argc == 5);
    }
    if (argc >= 6 && strcmp(argv[1], "operation") == 0 &&
        strcmp(argv[2], "plan") == 0) {
        int result = operation_plan_command(argc, argv, 3, 4, 5, 6);
        if (result == 2) usage(stderr, argv[0]);
        return result;
    }
    if (argc >= 5 && strcmp(argv[1], "plan") == 0) {
        int result = operation_plan_command(argc, argv, 2, 3, 4, 5);
        if (result == 2) usage(stderr, argv[0]);
        return result;
    }
    if (argc >= 2 && strcmp(argv[1], "read") == 0) {
        int result = top_level_read_command(argc, argv);
        if (result == 2) usage(stderr, argv[0]);
        return result;
    }
    if (argc >= 4 && strcmp(argv[1], "protect") == 0) {
        const char *operation = argc >= 5 ? argv[4] : NULL;
        int result;
        if (operation == NULL ||
            (strcmp(operation, "secure") != 0 &&
             strcmp(operation, "memory-protect") != 0)) {
            usage(stderr, argv[0]);
            return 2;
        }
        result = top_level_plan_command(argc, argv, operation, 5);
        if (result == 2) usage(stderr, argv[0]);
        return result;
    }
    if (argc >= 4) {
        static const struct {
            const char *command;
            const char *operation;
        } aliases[] = {
            {"blank-check", "blank-check"},
            {"verify", "verify"},
            {"program", "program"},
            {"over-program", "over-program"},
            {"compare", "compare"},
            {"erase", "erase"},
            {"secure", "secure"},
            {"memory-protect", "memory-protect"},
            {"configure", "configuration"},
            {"functional-test", "functional-test"},
            {"auto-id", "auto-id"},
            {"mass-production", "mass-production"},
            {"jtag-isp", "jtag-isp"}
        };
        for (size_t index = 0u;
             index < sizeof(aliases) / sizeof(aliases[0]); ++index) {
            if (strcmp(argv[1], aliases[index].command) == 0) {
                int result = top_level_plan_command(
                    argc, argv, aliases[index].operation, 4
                );
                if (result == 2) usage(stderr, argv[0]);
                return result;
            }
        }
    }
    if (argc >= 3 && strcmp(argv[1], "buffer") == 0) {
        int result = buffer_command(argc, argv);
        if (result == 2) usage(stderr, argv[0]);
        return result;
    }
    if ((argc == 3 || argc == 4) && strcmp(argv[1], "programmer") == 0 &&
        strcmp(argv[2], "list") == 0) {
        if (argc == 4 && strcmp(argv[3], "--json") != 0) {
            usage(stderr, argv[0]);
            return 2;
        }
        return list_programmers(argc == 4);
    }
    if ((argc == 4 || argc == 5) && strcmp(argv[1], "devices") == 0 &&
        strcmp(argv[2], "list") == 0) {
        if (argc == 5 && strcmp(argv[4], "--json") != 0) {
            usage(stderr, argv[0]);
            return 2;
        }
        return catalogue_search(argv[3], "", argc == 5);
    }
    if ((argc == 5 || argc == 6) && strcmp(argv[1], "devices") == 0 &&
        strcmp(argv[2], "search") == 0) {
        if (argc == 6 && strcmp(argv[5], "--json") != 0) {
            usage(stderr, argv[0]);
            return 2;
        }
        return catalogue_search(argv[3], argv[4], argc == 6);
    }
    if ((argc == 5 || argc == 6) && strcmp(argv[1], "devices") == 0 &&
        (strcmp(argv[2], "show") == 0 ||
         strcmp(argv[2], "guide") == 0)) {
        if (argc == 6 && strcmp(argv[5], "--json") != 0) {
            usage(stderr, argv[0]);
            return 2;
        }
        return catalogue_show(argv[3], argv[4], argc == 6);
    }
    if ((argc == 5 || argc == 6) && strcmp(argv[1], "support") == 0 &&
        strcmp(argv[2], "report") == 0) {
        if (argc == 6 && strcmp(argv[5], "--json") != 0) {
            usage(stderr, argv[0]);
            return 2;
        }
        return catalogue_support(argv[3], argv[4], argc == 6);
    }
    if (argc == 5 && strcmp(argv[1], "support") == 0 &&
        strcmp(argv[2], "export") == 0 && strcmp(argv[4], "--json") == 0)
        return catalogue_support_export(argv[3], 0);
    if (argc == 6 && strcmp(argv[1], "support") == 0 &&
        strcmp(argv[2], "export") == 0 && strcmp(argv[4], "--snapshot") == 0 &&
        strcmp(argv[5], "--json") == 0)
        return catalogue_support_export(argv[3], 1);
    if ((argc == 4 || argc == 5) && strcmp(argv[1], "support") == 0 &&
        strcmp(argv[2], "matrix") == 0) {
        if (argc == 5 && strcmp(argv[4], "--json") != 0) {
            usage(stderr, argv[0]);
            return 2;
        }
        return catalogue_support_matrix(argv[3], argc == 5);
    }
    if (argc >= 3 && strcmp(argv[1], "identify") == 0) {
        int result = identify_command(argc, argv, 2, 3);
        if (result == 2) usage(stderr, argv[0]);
        return result;
    }
    if (argc >= 3 && strcmp(argv[1], "programmer") == 0 &&
        strcmp(argv[2], "info") == 0) {
        int result = programmer_info(argc, argv);
        if (result == 2) usage(stderr, argv[0]);
        return result;
    }
    if (argc >= 8 && strcmp(argv[1], "programmer") == 0 &&
        strcmp(argv[2], "read-m37") == 0) {
        return programmer_read_m37(argc, argv);
    }
    if (argc >= 4 && strcmp(argv[1], "programmer") == 0 &&
        strcmp(argv[2], "load-fx2") == 0) {
        int result = programmer_load_fx2(argc, argv);
        if (result == 2 && argc < 8) usage(stderr, argv[0]);
        return result;
    }
    if (argc >= 3 && strcmp(argv[1], "programmer") == 0 &&
        strcmp(argv[2], "probe") == 0) {
        int result = open_programmer(argc, argv, 0);
        if (result == 2) {
            usage(stderr, argv[0]);
        }
        return result;
    }
    if ((argc == 4 || argc == 5) && strcmp(argv[1], "resources") == 0 &&
        strcmp(argv[2], "inspect") == 0) {
        if (argc == 5 && strcmp(argv[4], "--json") != 0) {
            usage(stderr, argv[0]);
            return 2;
        }
        return inspect_resources(argv[3], argc == 5);
    }
    if ((argc == 4 || argc == 5) && strcmp(argv[1], "resources") == 0 &&
        strcmp(argv[2], "devices") == 0) {
        if (argc == 5 && strcmp(argv[4], "--json") != 0) {
            usage(stderr, argv[0]);
            return 2;
        }
        return list_device_definitions(argv[3], argc == 5);
    }
    if ((argc == 5 || argc == 6) && strcmp(argv[1], "catalogue") == 0 &&
        strcmp(argv[2], "search") == 0) {
        if (argc == 6 && strcmp(argv[5], "--json") != 0) {
            usage(stderr, argv[0]);
            return 2;
        }
        return catalogue_search(argv[3], argv[4], argc == 6);
    }
    if ((argc == 4 || argc == 5) && strcmp(argv[1], "catalogue") == 0 &&
        strcmp(argv[2], "verify-installation") == 0) {
        if (argc == 5 && strcmp(argv[4], "--json") != 0) {
            usage(stderr, argv[0]);
            return 2;
        }
        return catalogue_verify_installation(argv[3], argc == 5);
    }
    if ((argc == 5 || argc == 6) && strcmp(argv[1], "resources") == 0 &&
        strcmp(argv[2], "resolve") == 0) {
        if (argc == 6 && strcmp(argv[5], "--json") != 0) {
            usage(stderr, argv[0]);
            return 2;
        }
        return resolve_definition(argv[3], argv[4], argc == 6);
    }
    if ((argc == 5 || argc == 6) && strcmp(argv[1], "catalogue") == 0 &&
        strcmp(argv[2], "show") == 0) {
        if (argc == 6 && strcmp(argv[5], "--json") != 0) {
            usage(stderr, argv[0]);
            return 2;
        }
        return catalogue_show(argv[3], argv[4], argc == 6);
    }
    if ((argc == 5 || argc == 6) && strcmp(argv[1], "catalogue") == 0 &&
        strcmp(argv[2], "support") == 0) {
        if (argc == 6 && strcmp(argv[5], "--json") != 0) {
            usage(stderr, argv[0]);
            return 2;
        }
        return catalogue_support(argv[3], argv[4], argc == 6);
    }
    if (argc >= 4 && strcmp(argv[1], "catalogue") == 0 &&
        strcmp(argv[2], "identify") == 0) {
        int result = identify_command(argc, argv, 3, 4);
        if (result == 2) usage(stderr, argv[0]);
        return result;
    }
    if ((argc == 5 || argc == 6) && strcmp(argv[1], "catalogue") == 0 &&
        strcmp(argv[2], "audit-m37") == 0) {
        if (argc == 6 && strcmp(argv[5], "--json") != 0) {
            usage(stderr, argv[0]);
            return 2;
        }
        return catalogue_audit_m37(argv[3], argv[4], argc == 6);
    }
    if (argc >= 7 && argc <= 10 && strcmp(argv[1], "catalogue") == 0 &&
        strcmp(argv[2], "plan-read") == 0) {
        uint32_t start;
        uint32_t length;
        const char *report_path = NULL;
        int json = 0;
        if (!parse_u32(argv[5], &start) || !parse_u32(argv[6], &length)) {
            usage(stderr, argv[0]);
            return 2;
        }
        for (int index = 7; index < argc; ++index) {
            if (strcmp(argv[index], "--json") == 0 && !json) json = 1;
            else if (strcmp(argv[index], "--report") == 0 &&
                     report_path == NULL && index + 1 < argc)
                report_path = argv[++index];
            else {
                usage(stderr, argv[0]);
                return 2;
            }
        }
        return catalogue_plan_read(
            argv[3], argv[4], start, length, report_path, json
        );
    }
    if (argc >= 7 && argc <= 10 && strcmp(argv[1], "catalogue") == 0 &&
        strcmp(argv[2], "plan-blank") == 0) {
        uint32_t start;
        uint32_t length;
        const char *report_path = NULL;
        int json = 0;
        if (!parse_u32(argv[5], &start) || !parse_u32(argv[6], &length)) {
            usage(stderr, argv[0]);
            return 2;
        }
        for (int index = 7; index < argc; ++index) {
            if (strcmp(argv[index], "--json") == 0 && !json) json = 1;
            else if (strcmp(argv[index], "--report") == 0 &&
                     report_path == NULL && index + 1 < argc)
                report_path = argv[++index];
            else {
                usage(stderr, argv[0]);
                return 2;
            }
        }
        return catalogue_plan_compare(
            argv[3], argv[4], start, length, NULL, report_path,
            LT48_OPERATION_BLANK_CHECK, json
        );
    }
    if (argc >= 8 && argc <= 11 && strcmp(argv[1], "catalogue") == 0 &&
        strcmp(argv[2], "plan-verify") == 0) {
        uint32_t start;
        uint32_t length;
        const char *report_path = NULL;
        int json = 0;
        if (!parse_u32(argv[5], &start) || !parse_u32(argv[6], &length)) {
            usage(stderr, argv[0]);
            return 2;
        }
        for (int index = 8; index < argc; ++index) {
            if (strcmp(argv[index], "--json") == 0 && !json) json = 1;
            else if (strcmp(argv[index], "--report") == 0 &&
                     report_path == NULL && index + 1 < argc)
                report_path = argv[++index];
            else {
                usage(stderr, argv[0]);
                return 2;
            }
        }
        return catalogue_plan_compare(
            argv[3], argv[4], start, length, argv[7], report_path,
            LT48_OPERATION_VERIFY, json
        );
    }
    if (argc >= 8 && argc <= 11 && strcmp(argv[1], "catalogue") == 0 &&
        strcmp(argv[2], "plan-compare") == 0) {
        uint32_t start;
        uint32_t length;
        const char *report_path = NULL;
        int json = 0;
        if (!parse_u32(argv[5], &start) || !parse_u32(argv[6], &length)) {
            usage(stderr, argv[0]);
            return 2;
        }
        for (int index = 8; index < argc; ++index) {
            if (strcmp(argv[index], "--json") == 0 && !json) json = 1;
            else if (strcmp(argv[index], "--report") == 0 &&
                     report_path == NULL && index + 1 < argc)
                report_path = argv[++index];
            else {
                usage(stderr, argv[0]);
                return 2;
            }
        }
        return catalogue_plan_compare(
            argv[3], argv[4], start, length, argv[7], report_path,
            LT48_OPERATION_COMPARE, json
        );
    }
    if ((argc == 4 || argc == 5) && strcmp(argv[1], "resources") == 0 &&
        strcmp(argv[2], "compact-audit") == 0) {
        if (argc == 5 && strcmp(argv[4], "--json") != 0) {
            usage(stderr, argv[0]);
            return 2;
        }
        return compact_audit(argv[3], argc == 5);
    }
    if ((argc == 4 || argc == 5) && strcmp(argv[1], "resources") == 0 &&
        strcmp(argv[2], "pinmaps") == 0) {
        if (argc == 5 && strcmp(argv[4], "--json") != 0) {
            usage(stderr, argv[0]);
            return 2;
        }
        return list_pin_maps(argv[3], argc == 5);
    }
    if (argc >= 6 && argc <= 8 && strcmp(argv[1], "resources") == 0 &&
        strcmp(argv[2], "compile") == 0) {
        uint16_t operation_id;
        int json = 0;
        lt48_pin_fallback_policy pin_policy = LT48_PIN_FALLBACK_STRICT;
        if (!parse_u16(argv[5], &operation_id)) {
            usage(stderr, argv[0]);
            return 2;
        }
        for (int index = 6; index < argc; ++index) {
            if (strcmp(argv[index], "--json") == 0 && !json) {
                json = 1;
            } else if (strcmp(argv[index], "--analysis-default-pins") == 0 &&
                       pin_policy == LT48_PIN_FALLBACK_STRICT) {
                pin_policy = LT48_PIN_FALLBACK_ANALYSIS_ALL;
            } else if (strcmp(argv[index],
                              "--resident-noop-default-pins") == 0 &&
                       pin_policy == LT48_PIN_FALLBACK_STRICT) {
                pin_policy = LT48_PIN_FALLBACK_RESIDENT_NOOP;
            } else {
                usage(stderr, argv[0]);
                return 2;
            }
        }
        return compile_definition_operation(
            argv[3], argv[4], operation_id, pin_policy, json,
            NULL, 0, 0
        );
    }
    if (argc >= 7 && strcmp(argv[1], "resources") == 0 &&
        strcmp(argv[2], "interpret") == 0) {
        uint16_t operation_id;
        lt48_runtime_options options = {0};
        unsigned char parameter[32];
        unsigned char capture_responses[32];
        int json = 0;
        int protocol_json = 0;
        int resumable = 0;
        if (!parse_u16(argv[5], &operation_id) ||
            !parse_u32(argv[6], &options.mode)) {
            usage(stderr, argv[0]);
            return 2;
        }
        for (int index = 7; index < argc; ++index) {
            if (strcmp(argv[index], "--json") == 0 && !json) {
                json = 1;
            } else if (strcmp(argv[index], "--protocol-json") == 0 &&
                       !protocol_json) {
                protocol_json = 1;
                json = 1;
            } else if (strcmp(argv[index], "--resumable") == 0 && !resumable) {
                resumable = 1;
            } else if (strcmp(argv[index], "--address") == 0 &&
                       index + 1 < argc &&
                       parse_u32(argv[++index], &options.address)) {
                /* parsed */
            } else if (strcmp(argv[index], "--parameter-hex") == 0 &&
                       index + 1 < argc &&
                       parse_hex_buffer(argv[++index], parameter,
                                        sizeof(parameter),
                                        &options.parameter_size)) {
                options.parameter_buffer = parameter;
            } else if (strcmp(argv[index], "--capture-hex") == 0 &&
                       index + 1 < argc &&
                       parse_hex_buffer(argv[++index], capture_responses,
                                        sizeof(capture_responses),
                                        &options.capture_response_size)) {
                options.capture_response_bytes = capture_responses;
                options.require_capture_responses = 1;
            } else if (strcmp(argv[index], "--initial-voltage") == 0 &&
                       index + 1 < argc &&
                       parse_initial_voltage(
                           argv[++index], options.initial_voltage_centivolts)) {
                /* parsed */
            } else {
                usage(stderr, argv[0]);
                return 2;
            }
        }
        return compile_definition_operation(
            argv[3], argv[4], operation_id, LT48_PIN_FALLBACK_STRICT,
            json, &options, protocol_json,
            resumable
        );
    }
    if (argc >= 3 && strcmp(argv[1], "programmer") == 0 && strcmp(argv[2], "leds") == 0)
        return open_programmer(argc, argv, 2);
    if (argc >= 3 && strcmp(argv[1], "programmer") == 0 &&
        strcmp(argv[2], "initialize") == 0) {
        int result = open_programmer(argc, argv, 1);
        if (result == 2) {
            usage(stderr, argv[0]);
        }
        return result;
    }
    usage(stderr, argv[0]);
    return 2;
}

int main(int argc, char **argv) {
    FILE *captured_stdout = NULL;
    FILE *captured_stderr = NULL;
    int saved_stdout = -1;
    int saved_stderr = -1;
    int result;
    int capture_ready = 0;
    char *diagnostic = NULL;

    if (!argument_requests_json(argc, argv)) return main_dispatch(argc, argv);

    fflush(stdout);
    fflush(stderr);
    captured_stdout = tmpfile();
    captured_stderr = tmpfile();
    if (captured_stdout != NULL && captured_stderr != NULL) {
        saved_stdout = dup(STDOUT_FILENO);
        saved_stderr = dup(STDERR_FILENO);
    }
    if (saved_stdout >= 0 && saved_stderr >= 0 &&
        dup2(fileno(captured_stdout), STDOUT_FILENO) >= 0 &&
        dup2(fileno(captured_stderr), STDERR_FILENO) >= 0) {
        clearerr(stdout);
        clearerr(stderr);
        capture_ready = 1;
    }
    if (!capture_ready) {
        if (saved_stdout >= 0) {
            (void)dup2(saved_stdout, STDOUT_FILENO);
            (void)close(saved_stdout);
        }
        if (saved_stderr >= 0) {
            (void)dup2(saved_stderr, STDERR_FILENO);
            (void)close(saved_stderr);
        }
        if (captured_stdout != NULL) fclose(captured_stdout);
        if (captured_stderr != NULL) fclose(captured_stderr);
        fputs("{\"schema\":1,\"success\":false,\"exit_status\":5,"
              "\"category\":\"diagnostic-failure\","
              "\"message\":\"cannot isolate machine-readable output\"}\n",
              stdout);
        return 5;
    }

    result = main_dispatch(argc, argv);
    fflush(stdout);
    fflush(stderr);
    (void)dup2(saved_stdout, STDOUT_FILENO);
    (void)dup2(saved_stderr, STDERR_FILENO);
    (void)close(saved_stdout);
    (void)close(saved_stderr);
    clearerr(stdout);
    clearerr(stderr);

    if (result == 0) {
        int stdout_ok = copy_captured(captured_stdout, stdout);
        int stderr_ok = copy_captured(captured_stderr, stderr);
        fclose(captured_stdout);
        fclose(captured_stderr);
        if (!stdout_ok || !stderr_ok) return 5;
        return 0;
    }

    diagnostic = captured_diagnostic(captured_stderr);
    if (diagnostic == NULL &&
        captured_looks_like_complete_json(captured_stdout)) {
        int stdout_ok = copy_captured(captured_stdout, stdout);
        fclose(captured_stdout);
        fclose(captured_stderr);
        if (!stdout_ok) return 5;
        return result;
    }
    if (diagnostic == NULL)
        diagnostic = captured_diagnostic(captured_stdout);
    fclose(captured_stdout);
    fclose(captured_stderr);
    printf("{\"schema\":1,\"success\":false,\"exit_status\":%d,"
           "\"category\":", result);
    print_json_string(exit_status_meaning(result));
    fputs(",\"message\":", stdout);
    print_json_string(diagnostic == NULL ? exit_status_meaning(result) :
                                             diagnostic);
    puts("}");
    free(diagnostic);
    return result;
}
