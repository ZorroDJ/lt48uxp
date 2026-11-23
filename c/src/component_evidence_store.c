#define _POSIX_C_SOURCE 200809L

#include <labtool48/labtool48.h>

#include "internal.h"

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct evidence_fact {
    size_t combination_index;
    char operation_id[96];
    char type[48];
    int passed;
} evidence_fact;

typedef struct evidence_output {
    unsigned char *data;
    size_t size;
    size_t capacity;
} evidence_output;

static char *evidence_join_path(const char *left, const char *right) {
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

static void evidence_digest_hex(
    const unsigned char digest[LT48_SHA256_SIZE],
    char output[LT48_SHA256_SIZE * 2u + 1u]
) {
    static const char digits[] = "0123456789abcdef";
    for (size_t index = 0u; index < LT48_SHA256_SIZE; ++index) {
        output[index * 2u] = digits[digest[index] >> 4u];
        output[index * 2u + 1u] = digits[digest[index] & 0x0fu];
    }
    output[LT48_SHA256_SIZE * 2u] = '\0';
}

static lt48_status ensure_evidence_directory(const char *path) {
    struct stat info;
    if (mkdir(path, 0700) == 0) return LT48_OK;
    if (errno != EEXIST || lstat(path, &info) != 0 || !S_ISDIR(info.st_mode))
        return LT48_FILE_ERROR;
    return LT48_OK;
}

static lt48_status evidence_hash_file_cancellable(
    const char *path, unsigned char digest[LT48_SHA256_SIZE],
    size_t *out_size, lt48_cancel_check_fn cancelled, void *cancel_context
) {
    unsigned char buffer[65536];
    lt48_sha256_context context;
    FILE *file;
    size_t total = 0u;
    if (cancelled != NULL && cancelled(cancel_context)) return LT48_CANCELLED;
    file = fopen(path, "rb");
    if (file == NULL) return LT48_FILE_ERROR;
    lt48_sha256_init(&context);
    for (;;) {
        size_t count;
        if (cancelled != NULL && cancelled(cancel_context)) {
            fclose(file);
            return LT48_CANCELLED;
        }
        count = fread(buffer, 1u, sizeof(buffer), file);
        if (count > SIZE_MAX - total) {
            fclose(file);
            return LT48_FILE_ERROR;
        }
        if (count != 0u) lt48_sha256_update(&context, buffer, count);
        total += count;
        if (count != sizeof(buffer)) {
            if (ferror(file) || fclose(file) != 0) return LT48_FILE_ERROR;
            break;
        }
    }
    lt48_sha256_final(&context, digest);
    *out_size = total;
    return LT48_OK;
}

static lt48_status existing_object_matches(
    const char *path, const unsigned char expected[LT48_SHA256_SIZE],
    size_t expected_size, lt48_cancel_check_fn cancelled,
    void *cancel_context
) {
    unsigned char actual[LT48_SHA256_SIZE];
    size_t actual_size;
    lt48_status status = evidence_hash_file_cancellable(
        path, actual, &actual_size, cancelled, cancel_context);
    if (status != LT48_OK) return status;
    return actual_size == expected_size &&
           memcmp(actual, expected, LT48_SHA256_SIZE) == 0 ?
           LT48_OK : LT48_SAFETY_REJECTED;
}

static lt48_status copy_content_addressed(
    const char *source_path, const char *target_path,
    const unsigned char expected[LT48_SHA256_SIZE], size_t expected_size,
    lt48_cancel_check_fn cancelled, void *cancel_context, int *out_created
) {
    unsigned char buffer[65536];
    FILE *source = NULL;
    int target = -1;
    size_t total = 0u;
    lt48_status status = LT48_OK;
    *out_created = 0;
    if (cancelled != NULL && cancelled(cancel_context)) return LT48_CANCELLED;
    target = open(target_path,
                  O_WRONLY | O_CREAT | O_EXCL | O_CLOEXEC | O_NOFOLLOW,
                  0600);
    if (target < 0) {
        if (errno == EEXIST)
            return existing_object_matches(
                target_path, expected, expected_size, cancelled,
                cancel_context);
        return LT48_FILE_ERROR;
    }
    source = fopen(source_path, "rb");
    if (source == NULL) {
        status = LT48_FILE_ERROR;
        goto done;
    }
    for (;;) {
        if (cancelled != NULL && cancelled(cancel_context)) {
            status = LT48_CANCELLED;
            goto done;
        }
        size_t count = fread(buffer, 1u, sizeof(buffer), source);
        size_t offset = 0u;
        if (count > SIZE_MAX - total) {
            status = LT48_FILE_ERROR;
            goto done;
        }
        while (offset < count) {
            if (cancelled != NULL && cancelled(cancel_context)) {
                status = LT48_CANCELLED;
                goto done;
            }
            ssize_t written = write(target, buffer + offset, count - offset);
            if (written < 0) {
                if (errno == EINTR) continue;
                status = LT48_FILE_ERROR;
                goto done;
            }
            offset += (size_t)written;
        }
        total += count;
        if (count != sizeof(buffer)) {
            if (ferror(source)) status = LT48_FILE_ERROR;
            break;
        }
    }
done:
    if (source != NULL && fclose(source) != 0 && status == LT48_OK)
        status = LT48_FILE_ERROR;
    if (target >= 0 && fsync(target) != 0 && status == LT48_OK)
        status = LT48_FILE_ERROR;
    if (target >= 0 && close(target) != 0 && status == LT48_OK)
        status = LT48_FILE_ERROR;
    if (status == LT48_OK && total != expected_size)
        status = LT48_SAFETY_REJECTED;
    if (status == LT48_OK)
        status = existing_object_matches(
            target_path, expected, expected_size, cancelled,
            cancel_context);
    if (status != LT48_OK) unlink(target_path);
    else *out_created = 1;
    return status;
}

lt48_status lt48_component_evidence_import_cancellable(
    const char *bundle_path, const char *evidence_path,
    const char *artifact_path, const char *store_path,
    lt48_cancel_check_fn cancelled, void *cancel_context,
    lt48_component_evidence_import_report *out_report
) {
    lt48_component_evidence_verification verification;
    unsigned char record_digest[LT48_SHA256_SIZE];
    size_t record_size;
    char record_hex[LT48_SHA256_SIZE * 2u + 1u];
    char artifact_hex[LT48_SHA256_SIZE * 2u + 1u];
    char record_name[LT48_SHA256_SIZE * 2u + 6u];
    char *records = NULL;
    char *objects = NULL;
    char *record_target = NULL;
    char *artifact_target = NULL;
    lt48_status status;
    if (bundle_path == NULL || evidence_path == NULL || artifact_path == NULL ||
        store_path == NULL || store_path[0] == '\0' || out_report == NULL)
        return LT48_INVALID_ARGUMENT;
    memset(out_report, 0, sizeof(*out_report));
    if (cancelled != NULL && cancelled(cancel_context)) return LT48_CANCELLED;
    status = lt48_component_evidence_verify_cancellable(
        bundle_path, evidence_path, artifact_path, cancelled, cancel_context,
        &verification);
    if (status != LT48_OK) return status;
    status = evidence_hash_file_cancellable(
        evidence_path, record_digest, &record_size, cancelled, cancel_context);
    if (status != LT48_OK ||
        memcmp(record_digest, verification.evidence.source_sha256,
               LT48_SHA256_SIZE) != 0)
        return status == LT48_OK ? LT48_SAFETY_REJECTED : status;
    evidence_digest_hex(record_digest, record_hex);
    evidence_digest_hex(verification.artifact_sha256, artifact_hex);
    snprintf(record_name, sizeof(record_name), "%s.json", record_hex);
    status = ensure_evidence_directory(store_path);
    if (status != LT48_OK) goto done;
    records = evidence_join_path(store_path, "records");
    objects = evidence_join_path(store_path, "objects");
    if (records == NULL || objects == NULL) {
        status = LT48_NO_MEMORY;
        goto done;
    }
    if ((status = ensure_evidence_directory(records)) != LT48_OK ||
        (status = ensure_evidence_directory(objects)) != LT48_OK)
        goto done;
    record_target = evidence_join_path(records, record_name);
    artifact_target = evidence_join_path(objects, artifact_hex);
    if (record_target == NULL || artifact_target == NULL) {
        status = LT48_NO_MEMORY;
        goto done;
    }
    status = copy_content_addressed(
        artifact_path, artifact_target, verification.artifact_sha256,
        verification.artifact_size, cancelled, cancel_context,
        &out_report->artifact_created);
    if (status != LT48_OK) goto done;
    status = copy_content_addressed(
        evidence_path, record_target, record_digest, record_size, cancelled,
        cancel_context,
        &out_report->record_created);
    if (status != LT48_OK) goto done;
    memcpy(out_report->record_sha256, record_digest, LT48_SHA256_SIZE);
    memcpy(out_report->artifact_sha256, verification.artifact_sha256,
           LT48_SHA256_SIZE);
    out_report->record_size = record_size;
    out_report->artifact_size = verification.artifact_size;
    out_report->evidence_passed = verification.evidence.passed;
    snprintf(out_report->evidence_id, sizeof(out_report->evidence_id), "%s",
             verification.evidence.evidence_id);
done:
    if (status != LT48_OK && out_report->artifact_created &&
        artifact_target != NULL)
        unlink(artifact_target);
    free(artifact_target);
    free(record_target);
    free(objects);
    free(records);
    if (status != LT48_OK) memset(out_report, 0, sizeof(*out_report));
    return status;
}

lt48_status lt48_component_evidence_import(
    const char *bundle_path, const char *evidence_path,
    const char *artifact_path, const char *store_path,
    lt48_component_evidence_import_report *out_report
) {
    return lt48_component_evidence_import_cancellable(
        bundle_path, evidence_path, artifact_path, store_path, NULL, NULL,
        out_report);
}

const char *lt48_component_evidence_level_name(
    lt48_component_evidence_level level
) {
    switch (level) {
    case LT48_COMPONENT_EVIDENCE_PLANNED:
        return "planned";
    case LT48_COMPONENT_EVIDENCE_SAFETY_VALIDATED:
        return "safety-validated";
    case LT48_COMPONENT_EVIDENCE_BEHAVIOR_VALIDATED:
        return "behavior-validated";
    case LT48_COMPONENT_EVIDENCE_ENGINE_HARDWARE_VERIFIED:
        return "engine-hardware-verified";
    case LT48_COMPONENT_EVIDENCE_DEVICE_HARDWARE_VERIFIED:
        return "device-hardware-verified";
    default:
        return NULL;
    }
}

void lt48_component_evidence_audit_clear(
    lt48_component_evidence_audit *audit
) {
    if (audit == NULL) return;
    free(audit->json);
    free(audit->plans);
    memset(audit, 0, sizeof(*audit));
}

static int evidence_output_append(
    evidence_output *output, const void *data, size_t size
) {
    unsigned char *grown;
    size_t required;
    size_t capacity;
    if (size > SIZE_MAX - output->size) return 0;
    required = output->size + size;
    if (required > output->capacity) {
        capacity = output->capacity == 0u ? 512u : output->capacity;
        while (capacity < required) {
            if (capacity > SIZE_MAX / 2u) return 0;
            capacity *= 2u;
        }
        grown = realloc(output->data, capacity);
        if (grown == NULL) return 0;
        output->data = grown;
        output->capacity = capacity;
    }
    if (size != 0u) memcpy(output->data + output->size, data, size);
    output->size = required;
    return 1;
}

static int evidence_output_text(evidence_output *output, const char *text) {
    return evidence_output_append(output, text, strlen(text));
}

static int evidence_output_size(evidence_output *output, size_t value) {
    char text[32];
    int length = snprintf(text, sizeof(text), "%zu", value);
    return length > 0 && (size_t)length < sizeof(text) &&
           evidence_output_append(output, text, (size_t)length);
}

static int evidence_output_json_string(
    evidence_output *output, const char *text
) {
    if (!evidence_output_text(output, "\"")) return 0;
    for (const unsigned char *cursor = (const unsigned char *)text;
         *cursor != 0u; ++cursor) {
        if (*cursor == '\"' || *cursor == '\\') {
            unsigned char pair[2] = {'\\', *cursor};
            if (!evidence_output_append(output, pair, 2u)) return 0;
        } else if (*cursor < 0x20u) {
            return 0;
        } else if (!evidence_output_append(output, cursor, 1u)) {
            return 0;
        }
    }
    return evidence_output_text(output, "\"");
}

static int evidence_record_filename(const char *name) {
    if (name == NULL || strlen(name) != LT48_SHA256_SIZE * 2u + 5u ||
        strcmp(name + LT48_SHA256_SIZE * 2u, ".json") != 0)
        return 0;
    for (size_t index = 0u; index < LT48_SHA256_SIZE * 2u; ++index)
        if (!((name[index] >= '0' && name[index] <= '9') ||
              (name[index] >= 'a' && name[index] <= 'f')))
            return 0;
    return 1;
}

static int append_fact(
    evidence_fact **facts, size_t *count, size_t *capacity,
    const lt48_component_evidence_report *report
) {
    evidence_fact *grown;
    if (*count == *capacity) {
        size_t next = *capacity == 0u ? 16u : *capacity * 2u;
        if (next < *capacity || next > SIZE_MAX / sizeof(**facts)) return 0;
        grown = realloc(*facts, next * sizeof(**facts));
        if (grown == NULL) return 0;
        *facts = grown;
        *capacity = next;
    }
    (*facts)[*count].combination_index = report->combination_index;
    snprintf((*facts)[*count].operation_id,
             sizeof((*facts)[*count].operation_id), "%s", report->operation_id);
    snprintf((*facts)[*count].type, sizeof((*facts)[*count].type), "%s",
             report->evidence_type);
    (*facts)[*count].passed = report->passed;
    ++*count;
    return 1;
}

static int append_missing_evidence(
    evidence_output *output, uint32_t missing
) {
    static const struct { uint32_t bit; const char *name; } names[] = {
        {LT48_COMPONENT_EVIDENCE_MISSING_SIMULATION, "simulation"},
        {LT48_COMPONENT_EVIDENCE_MISSING_FAULT_INJECTION, "fault-injection"},
        {LT48_COMPONENT_EVIDENCE_MISSING_BEHAVIOR, "behavior-conformance"},
        {LT48_COMPONENT_EVIDENCE_MISSING_ADAPTER_BENCH, "adapter-bench"},
        {LT48_COMPONENT_EVIDENCE_MISSING_ENGINE_HARDWARE, "engine-hardware"},
        {LT48_COMPONENT_EVIDENCE_MISSING_DEVICE_HARDWARE, "device-hardware"}
    };
    int first = 1;
    if (!evidence_output_text(output, "[")) return 0;
    for (size_t index = 0u; index < sizeof(names) / sizeof(names[0]); ++index) {
        if ((missing & names[index].bit) == 0u) continue;
        if ((!first && !evidence_output_text(output, ",")) ||
            !evidence_output_json_string(output, names[index].name))
            return 0;
        first = 0;
    }
    return evidence_output_text(output, "]");
}

static lt48_status evidence_audit_store_with_matrix(
    const char *bundle_path, const char *store_path,
    const lt48_component_plan_matrix *matrix,
    lt48_cancel_check_fn cancelled, void *cancel_context,
    lt48_component_evidence_audit *out_audit
) {
    evidence_fact *facts = NULL;
    size_t fact_count = 0u;
    size_t fact_capacity = 0u;
    char *records_path = NULL;
    char *objects_path = NULL;
    DIR *directory = NULL;
    struct dirent *entry;
    evidence_output output = {0};
    lt48_status status;
    if (bundle_path == NULL || store_path == NULL || matrix == NULL ||
        out_audit == NULL)
        return LT48_INVALID_ARGUMENT;
    memset(out_audit, 0, sizeof(*out_audit));
    if (cancelled != NULL && cancelled(cancel_context)) return LT48_CANCELLED;
    status = LT48_OK;
    records_path = evidence_join_path(store_path, "records");
    objects_path = evidence_join_path(store_path, "objects");
    if (records_path == NULL || objects_path == NULL) {
        status = LT48_NO_MEMORY;
        goto done;
    }
    directory = opendir(records_path);
    if (directory == NULL) {
        status = LT48_FILE_ERROR;
        goto done;
    }
    errno = 0;
    while ((entry = readdir(directory)) != NULL) {
        char *record_path = NULL;
        char *artifact_path = NULL;
        char artifact_name[LT48_SHA256_SIZE * 2u + 1u];
        char source_name[LT48_SHA256_SIZE * 2u + 1u];
        struct stat info;
        lt48_component_evidence_report record;
        lt48_component_evidence_verification verification;
        if (cancelled != NULL && cancelled(cancel_context)) {
            status = LT48_CANCELLED;
            goto scan_done;
        }
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0)
            continue;
        ++out_audit->stored_record_count;
        if (!evidence_record_filename(entry->d_name)) {
            ++out_audit->invalid_record_count;
            continue;
        }
        record_path = evidence_join_path(records_path, entry->d_name);
        if (record_path == NULL) {
            status = LT48_NO_MEMORY;
            goto scan_done;
        }
        if (lstat(record_path, &info) != 0 || !S_ISREG(info.st_mode) ||
            lt48_component_evidence_validate(record_path, &record) != LT48_OK) {
            ++out_audit->invalid_record_count;
            free(record_path);
            errno = 0;
            continue;
        }
        evidence_digest_hex(record.source_sha256, source_name);
        if (strncmp(entry->d_name, source_name,
                    LT48_SHA256_SIZE * 2u) != 0) {
            ++out_audit->invalid_record_count;
            free(record_path);
            errno = 0;
            continue;
        }
        for (size_t index = 0u; index < LT48_SHA256_SIZE * 2u; ++index)
            artifact_name[index] = (char)tolower(
                (unsigned char)record.artifact_sha256[index]);
        artifact_name[LT48_SHA256_SIZE * 2u] = '\0';
        artifact_path = evidence_join_path(objects_path, artifact_name);
        if (artifact_path == NULL) {
            free(record_path);
            status = LT48_NO_MEMORY;
            goto scan_done;
        }
        status = lt48_component_evidence_verify_with_matrix_internal(
            bundle_path, record_path, artifact_path, matrix, cancelled,
            cancel_context, &verification);
        if (status != LT48_OK ||
            !append_fact(&facts, &fact_count, &fact_capacity,
                         &verification.evidence)) {
            if (status == LT48_CANCELLED) {
                free(artifact_path);
                free(record_path);
                goto scan_done;
            }
            if (status == LT48_OK && verification.complete)
                status = LT48_NO_MEMORY;
            else {
                ++out_audit->invalid_record_count;
                status = LT48_OK;
            }
            free(artifact_path);
            free(record_path);
            if (status == LT48_NO_MEMORY) goto scan_done;
            errno = 0;
            continue;
        }
        ++out_audit->verified_record_count;
        if (verification.evidence.passed)
            ++out_audit->passing_record_count;
        else
            ++out_audit->failing_record_count;
        free(artifact_path);
        free(record_path);
        errno = 0;
    }
    if (errno != 0) status = LT48_FILE_ERROR;
scan_done:
    if (closedir(directory) != 0 && status == LT48_OK)
        status = LT48_FILE_ERROR;
    directory = NULL;
    if (status != LT48_OK) goto done;
    out_audit->plans = calloc(matrix->plan_count ? matrix->plan_count : 1u,
                              sizeof(*out_audit->plans));
    if (out_audit->plans == NULL) {
        status = LT48_NO_MEMORY;
        goto done;
    }
    for (size_t combination = 0u; combination < matrix->combination_count;
         ++combination) {
        for (unsigned value = 1u; value <= LT48_OPERATION_KIND_COUNT; ++value) {
            lt48_component_plan plan;
            lt48_component_evidence_plan_status *item;
            int simulation = 0;
            int fault = 0;
            int behavior = 0;
            int adapter = 0;
            int engine = 0;
            int device = 0;
            if (cancelled != NULL && cancelled(cancel_context)) {
                status = LT48_CANCELLED;
                goto done;
            }
            status = lt48_component_bundle_compile_plan(
                bundle_path, combination,
                lt48_operation_kind_name((lt48_operation_kind)value), &plan);
            if (status == LT48_NOT_FOUND) continue;
            if (status != LT48_OK) goto done;
            item = &out_audit->plans[out_audit->plan_count++];
            item->combination_index = combination;
            item->mutating = plan.mutating;
            snprintf(item->operation_id, sizeof(item->operation_id), "%s",
                     plan.operation_id);
            for (size_t index = 0u; index < fact_count; ++index) {
                if (!facts[index].passed ||
                    facts[index].combination_index != combination ||
                    strcmp(facts[index].operation_id, plan.operation_id) != 0)
                    continue;
                if (strcmp(facts[index].type, "simulation") == 0) simulation = 1;
                else if (strcmp(facts[index].type, "fault-injection") == 0) fault = 1;
                else if (strcmp(facts[index].type, "windows-reference") == 0 ||
                         strcmp(facts[index].type,
                                "manufacturer-conformance") == 0) behavior = 1;
                else if (strcmp(facts[index].type, "adapter-bench") == 0) adapter = 1;
                else if (strcmp(facts[index].type, "engine-hardware") == 0) engine = 1;
                else if (strcmp(facts[index].type, "device-hardware") == 0) device = 1;
            }
            item->missing_evidence =
                (simulation ? 0u : LT48_COMPONENT_EVIDENCE_MISSING_SIMULATION) |
                (fault ? 0u : LT48_COMPONENT_EVIDENCE_MISSING_FAULT_INJECTION) |
                (behavior ? 0u : LT48_COMPONENT_EVIDENCE_MISSING_BEHAVIOR) |
                (adapter ? 0u : LT48_COMPONENT_EVIDENCE_MISSING_ADAPTER_BENCH) |
                (engine ? 0u : LT48_COMPONENT_EVIDENCE_MISSING_ENGINE_HARDWARE) |
                (device ? 0u : LT48_COMPONENT_EVIDENCE_MISSING_DEVICE_HARDWARE);
            item->level = LT48_COMPONENT_EVIDENCE_PLANNED;
            if (simulation && fault)
                item->level = LT48_COMPONENT_EVIDENCE_SAFETY_VALIDATED;
            if (item->level >= LT48_COMPONENT_EVIDENCE_SAFETY_VALIDATED &&
                behavior)
                item->level = LT48_COMPONENT_EVIDENCE_BEHAVIOR_VALIDATED;
            if (item->level >= LT48_COMPONENT_EVIDENCE_BEHAVIOR_VALIDATED &&
                adapter && engine)
                item->level = LT48_COMPONENT_EVIDENCE_ENGINE_HARDWARE_VERIFIED;
            if (item->level >=
                    LT48_COMPONENT_EVIDENCE_ENGINE_HARDWARE_VERIFIED && device)
                item->level = LT48_COMPONENT_EVIDENCE_DEVICE_HARDWARE_VERIFIED;
            if (item->level >= LT48_COMPONENT_EVIDENCE_SAFETY_VALIDATED)
                ++out_audit->safety_validated_plan_count;
            if (item->level >= LT48_COMPONENT_EVIDENCE_BEHAVIOR_VALIDATED)
                ++out_audit->behavior_validated_plan_count;
            if (item->level >=
                LT48_COMPONENT_EVIDENCE_ENGINE_HARDWARE_VERIFIED)
                ++out_audit->engine_hardware_plan_count;
            if (item->level >=
                LT48_COMPONENT_EVIDENCE_DEVICE_HARDWARE_VERIFIED)
                ++out_audit->device_hardware_plan_count;
            if (plan.mutating &&
                item->level < LT48_COMPONENT_EVIDENCE_BEHAVIOR_VALIDATED)
                ++out_audit->mutating_behavior_blocker_count;
            lt48_component_plan_clear(&plan);
        }
    }
    out_audit->store_integrity_valid = out_audit->invalid_record_count == 0u;
    out_audit->release_ready = 0;
    {
        char bundle_hex[LT48_SHA256_SIZE * 2u + 1u];
        char matrix_hex[LT48_SHA256_SIZE * 2u + 1u];
        evidence_digest_hex(matrix->bundle_sha256, bundle_hex);
        evidence_digest_hex(matrix->sha256, matrix_hex);
        if (!evidence_output_text(&output,
                "{\"schema_version\":1,\"bundle_sha256\":") ||
            !evidence_output_json_string(&output, bundle_hex) ||
            !evidence_output_text(&output, ",\"matrix_sha256\":") ||
            !evidence_output_json_string(&output, matrix_hex) ||
            !evidence_output_text(&output, ",\"store_integrity_valid\":") ||
            !evidence_output_text(&output,
                out_audit->store_integrity_valid ? "true" : "false") ||
            !evidence_output_text(&output, ",\"release_ready\":false,"
                "\"stored_record_count\":") ||
            !evidence_output_size(&output, out_audit->stored_record_count) ||
            !evidence_output_text(&output, ",\"verified_record_count\":") ||
            !evidence_output_size(&output, out_audit->verified_record_count) ||
            !evidence_output_text(&output, ",\"invalid_record_count\":") ||
            !evidence_output_size(&output, out_audit->invalid_record_count) ||
            !evidence_output_text(&output, ",\"passing_record_count\":") ||
            !evidence_output_size(&output, out_audit->passing_record_count) ||
            !evidence_output_text(&output, ",\"failing_record_count\":") ||
            !evidence_output_size(&output, out_audit->failing_record_count) ||
            !evidence_output_text(&output, ",\"plans\":[")) {
            status = LT48_NO_MEMORY;
            goto done;
        }
        for (size_t index = 0u; index < out_audit->plan_count; ++index) {
            const lt48_component_evidence_plan_status *item =
                &out_audit->plans[index];
            if (cancelled != NULL && cancelled(cancel_context)) {
                status = LT48_CANCELLED;
                goto done;
            }
            if ((index != 0u && !evidence_output_text(&output, ",")) ||
                !evidence_output_text(&output, "{\"combination_index\":") ||
                !evidence_output_size(&output, item->combination_index) ||
                !evidence_output_text(&output, ",\"operation_id\":") ||
                !evidence_output_json_string(&output, item->operation_id) ||
                !evidence_output_text(&output, ",\"mutating\":") ||
                !evidence_output_text(&output,
                    item->mutating ? "true" : "false") ||
                !evidence_output_text(&output, ",\"level\":") ||
                !evidence_output_json_string(
                    &output, lt48_component_evidence_level_name(item->level)) ||
                !evidence_output_text(&output, ",\"missing_evidence\":") ||
                !append_missing_evidence(&output, item->missing_evidence) ||
                !evidence_output_text(&output, "}")) {
                status = LT48_NO_MEMORY;
                goto done;
            }
        }
        if (!evidence_output_text(&output, "]}") ||
            !evidence_output_append(&output, "\0", 1u)) {
            status = LT48_NO_MEMORY;
            goto done;
        }
    }
    out_audit->json = output.data;
    out_audit->json_size = output.size - 1u;
    lt48_sha256(output.data, output.size - 1u, out_audit->sha256);
    output.data = NULL;
    status = out_audit->store_integrity_valid ? LT48_OK : LT48_SAFETY_REJECTED;
done:
    if (directory != NULL) closedir(directory);
    free(output.data);
    free(facts);
    free(objects_path);
    free(records_path);
    if (status != LT48_OK && status != LT48_SAFETY_REJECTED)
        lt48_component_evidence_audit_clear(out_audit);
    return status;
}

lt48_status lt48_component_evidence_audit_store_cancellable(
    const char *bundle_path, const char *store_path,
    lt48_cancel_check_fn cancelled, void *cancel_context,
    lt48_component_evidence_audit *out_audit
) {
    lt48_component_plan_matrix matrix;
    lt48_status status;
    if (bundle_path == NULL || store_path == NULL || out_audit == NULL)
        return LT48_INVALID_ARGUMENT;
    memset(out_audit, 0, sizeof(*out_audit));
    status = lt48_component_bundle_compile_plan_matrix_cancellable(
        bundle_path, cancelled, cancel_context, &matrix);
    if (status != LT48_OK) return status;
    status = evidence_audit_store_with_matrix(
        bundle_path, store_path, &matrix, cancelled, cancel_context,
        out_audit);
    lt48_component_plan_matrix_clear(&matrix);
    return status;
}

lt48_status lt48_component_evidence_audit_store(
    const char *bundle_path, const char *store_path,
    lt48_component_evidence_audit *out_audit
) {
    return lt48_component_evidence_audit_store_cancellable(
        bundle_path, store_path, NULL, NULL, out_audit);
}

void lt48_component_release_check_clear(
    lt48_component_release_check *check
) {
    if (check == NULL) return;
    free(check->json);
    memset(check, 0, sizeof(*check));
}

lt48_status lt48_component_release_check_create_cancellable(
    const char *bundle_path, const char *store_path,
    lt48_cancel_check_fn cancelled, void *cancel_context,
    lt48_component_release_check *out_check
) {
    lt48_component_plan_matrix matrix;
    lt48_component_evidence_audit audit;
    evidence_output output = {0};
    char bundle_hex[LT48_SHA256_SIZE * 2u + 1u];
    char matrix_hex[LT48_SHA256_SIZE * 2u + 1u];
    char audit_hex[LT48_SHA256_SIZE * 2u + 1u];
    lt48_status audit_status;
    lt48_status status;
    int evidence_blocker;
    if (bundle_path == NULL || store_path == NULL || out_check == NULL)
        return LT48_INVALID_ARGUMENT;
    memset(out_check, 0, sizeof(*out_check));
    if (cancelled != NULL && cancelled(cancel_context)) return LT48_CANCELLED;
    status = lt48_component_bundle_compile_plan_matrix_cancellable(
        bundle_path, cancelled, cancel_context, &matrix);
    if (status != LT48_OK) return status;
    audit_status = evidence_audit_store_with_matrix(
        bundle_path, store_path, &matrix, cancelled, cancel_context, &audit);
    if (audit_status != LT48_OK && audit_status != LT48_SAFETY_REJECTED) {
        lt48_component_plan_matrix_clear(&matrix);
        return audit_status;
    }
    out_check->plan_count = audit.plan_count;
    out_check->evidence_store_integrity_valid = audit.store_integrity_valid;
    for (size_t index = 0u; index < audit.plan_count; ++index) {
        const lt48_component_evidence_plan_status *plan = &audit.plans[index];
        lt48_component_evidence_level required = plan->mutating ?
            LT48_COMPONENT_EVIDENCE_ENGINE_HARDWARE_VERIFIED :
            LT48_COMPONENT_EVIDENCE_BEHAVIOR_VALIDATED;
        if (cancelled != NULL && cancelled(cancel_context)) {
            status = LT48_CANCELLED;
            goto done;
        }
        if (plan->level >= required) ++out_check->evidence_candidate_count;
    }
    evidence_blocker = out_check->evidence_candidate_count !=
                       out_check->plan_count;
    out_check->blocker_count =
        (audit.store_integrity_valid ? 0u : 1u) +
        (evidence_blocker ? 1u : 0u) +
        (out_check->plan_count != 0u ? 1u : 0u) +
        2u; /* independent review and explicit registry approval */
    out_check->transport_ready_plan_count = 0u;
    out_check->release_ready = 0;
    evidence_digest_hex(matrix.bundle_sha256, bundle_hex);
    evidence_digest_hex(matrix.sha256, matrix_hex);
    evidence_digest_hex(audit.sha256, audit_hex);
    if (!evidence_output_text(&output,
            "{\"schema_version\":1,\"bundle_sha256\":") ||
        !evidence_output_json_string(&output, bundle_hex) ||
        !evidence_output_text(&output, ",\"matrix_sha256\":") ||
        !evidence_output_json_string(&output, matrix_hex) ||
        !evidence_output_text(&output, ",\"evidence_audit_sha256\":") ||
        !evidence_output_json_string(&output, audit_hex) ||
        !evidence_output_text(&output, ",\"evidence_store_integrity_valid\":") ||
        !evidence_output_text(&output,
            audit.store_integrity_valid ? "true" : "false") ||
        !evidence_output_text(&output, ",\"release_ready\":false,"
            "\"support_registry_changed\":false,\"plan_count\":") ||
        !evidence_output_size(&output, out_check->plan_count) ||
        !evidence_output_text(&output, ",\"evidence_candidate_count\":") ||
        !evidence_output_size(&output,
                              out_check->evidence_candidate_count) ||
        !evidence_output_text(&output, ",\"transport_ready_plan_count\":0,"
            "\"plans\":[")) {
        status = LT48_NO_MEMORY;
        goto done;
    }
    for (size_t index = 0u; index < audit.plan_count; ++index) {
        const lt48_component_evidence_plan_status *plan = &audit.plans[index];
        lt48_component_evidence_level required = plan->mutating ?
            LT48_COMPONENT_EVIDENCE_ENGINE_HARDWARE_VERIFIED :
            LT48_COMPONENT_EVIDENCE_BEHAVIOR_VALIDATED;
        int candidate = plan->level >= required;
        if ((index != 0u && !evidence_output_text(&output, ",")) ||
            !evidence_output_text(&output, "{\"combination_index\":") ||
            !evidence_output_size(&output, plan->combination_index) ||
            !evidence_output_text(&output, ",\"operation_id\":") ||
            !evidence_output_json_string(&output, plan->operation_id) ||
            !evidence_output_text(&output, ",\"mutating\":") ||
            !evidence_output_text(&output,
                                  plan->mutating ? "true" : "false") ||
            !evidence_output_text(&output, ",\"evidence_level\":") ||
            !evidence_output_json_string(
                &output, lt48_component_evidence_level_name(plan->level)) ||
            !evidence_output_text(&output, ",\"required_evidence_level\":") ||
            !evidence_output_json_string(
                &output, lt48_component_evidence_level_name(required)) ||
            !evidence_output_text(&output, ",\"evidence_candidate\":") ||
            !evidence_output_text(&output, candidate ? "true" : "false") ||
            !evidence_output_text(&output,
                ",\"transport_ready\":false,"
                "\"proposed_availability\":\"unavailable\"}")) {
            status = LT48_NO_MEMORY;
            goto done;
        }
    }
    if (!evidence_output_text(&output, "],\"blockers\":[")) {
        status = LT48_NO_MEMORY;
        goto done;
    }
    {
        int first = 1;
#define APPEND_RELEASE_BLOCKER(condition, name) do { \
        if ((condition)) { \
            if ((!first && !evidence_output_text(&output, ",")) || \
                !evidence_output_json_string(&output, (name))) { \
                status = LT48_NO_MEMORY; \
                goto done; \
            } \
            first = 0; \
        } \
    } while (0)
        APPEND_RELEASE_BLOCKER(!audit.store_integrity_valid,
                               "evidence-store-integrity");
        APPEND_RELEASE_BLOCKER(evidence_blocker,
                               "per-operation-evidence-insufficient");
        APPEND_RELEASE_BLOCKER(out_check->plan_count != 0u,
                               "engine-transport-lowering-missing");
        APPEND_RELEASE_BLOCKER(1, "independent-review-missing");
        APPEND_RELEASE_BLOCKER(1, "explicit-registry-approval-missing");
#undef APPEND_RELEASE_BLOCKER
    }
    if (!evidence_output_text(&output, "]}") ||
        !evidence_output_append(&output, "\0", 1u)) {
        status = LT48_NO_MEMORY;
        goto done;
    }
    out_check->json = output.data;
    out_check->json_size = output.size - 1u;
    lt48_sha256(output.data, output.size - 1u, out_check->sha256);
    output.data = NULL;
    status = audit_status;
done:
    free(output.data);
    lt48_component_evidence_audit_clear(&audit);
    lt48_component_plan_matrix_clear(&matrix);
    if (status != LT48_OK && status != LT48_SAFETY_REJECTED)
        lt48_component_release_check_clear(out_check);
    return status;
}

lt48_status lt48_component_release_check_create(
    const char *bundle_path, const char *store_path,
    lt48_component_release_check *out_check
) {
    return lt48_component_release_check_create_cancellable(
        bundle_path, store_path, NULL, NULL, out_check);
}
