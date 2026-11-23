#include <labtool48/labtool48.h>

#include "json_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct lt48_property_document {
    lt48_json_document *json;
    lt48_property_model model;
    int owns_strings;
};

static char *property_string_duplicate(const char *text) {
    size_t size;
    char *copy;
    if (text == NULL) return NULL;
    size = strlen(text) + 1u;
    copy = malloc(size);
    if (copy != NULL) memcpy(copy, text, size);
    return copy;
}

static lt48_status property_invalid(
    lt48_property_validation_issue *issue,
    size_t page_index,
    size_t control_index,
    const char *reason
) {
    if (issue != NULL) {
        issue->page_index = page_index;
        issue->control_index = control_index;
        issue->reason = reason;
    }
    return LT48_FORMAT_ERROR;
}

static int nonempty(const char *text) {
    return text != NULL && text[0] != '\0';
}

lt48_status lt48_property_model_validate(
    const lt48_property_model *model,
    lt48_property_validation_issue *out_issue
) {
    size_t page_index;
    if (out_issue != NULL) {
        out_issue->page_index = SIZE_MAX;
        out_issue->control_index = SIZE_MAX;
        out_issue->reason = NULL;
    }
    if (model == NULL) return LT48_INVALID_ARGUMENT;
    if (!nonempty(model->title))
        return property_invalid(
            out_issue, SIZE_MAX, SIZE_MAX, "property-sheet title is empty");
    if (model->page_count == 0u ||
        model->page_count > LT48_PROPERTY_MAX_PAGES ||
        model->pages == NULL)
        return property_invalid(
            out_issue, SIZE_MAX, SIZE_MAX,
            "property-sheet page count is outside 1..20");
    for (page_index = 0u; page_index < model->page_count; ++page_index) {
        const lt48_property_page *page = &model->pages[page_index];
        size_t kind_counts[4] = {0u, 0u, 0u, 0u};
        size_t control_index;
        if (!nonempty(page->title))
            return property_invalid(
                out_issue, page_index, SIZE_MAX, "page title is empty");
        if ((page->control_count != 0u && page->controls == NULL) ||
            page->control_count > LT48_PROPERTY_MAX_CONTROLS_PER_PAGE)
            return property_invalid(
                out_issue, page_index, SIZE_MAX,
                "page control count exceeds the recovered template");
        for (control_index = 0u; control_index < page->control_count;
             ++control_index) {
            const lt48_property_control *control =
                &page->controls[control_index];
            size_t earlier_page;
            if (!nonempty(control->key) || !nonempty(control->label))
                return property_invalid(
                    out_issue, page_index, control_index,
                    "control key or label is empty");
            if (control->kind < LT48_PROPERTY_NUMBER ||
                control->kind > LT48_PROPERTY_CHOICE)
                return property_invalid(
                    out_issue, page_index, control_index,
                    "control kind is unknown");
            ++kind_counts[control->kind];
            if ((control->kind == LT48_PROPERTY_NUMBER &&
                 kind_counts[control->kind] >
                     LT48_PROPERTY_MAX_NUMBER_CONTROLS) ||
                (control->kind == LT48_PROPERTY_CHECKBOX &&
                 kind_counts[control->kind] >
                     LT48_PROPERTY_MAX_CHECKBOX_CONTROLS) ||
                (control->kind == LT48_PROPERTY_CHOICE &&
                 kind_counts[control->kind] >
                     LT48_PROPERTY_MAX_CHOICE_CONTROLS))
                return property_invalid(
                    out_issue, page_index, control_index,
                    "control count for its kind exceeds the recovered template");
            if (control->kind == LT48_PROPERTY_NUMBER) {
                const lt48_property_number *number =
                    &control->metadata.number;
                if (number->radix < 2u || number->radix > 36u ||
                    number->minimum > number->maximum ||
                    control->value < number->minimum ||
                    control->value > number->maximum)
                    return property_invalid(
                        out_issue, page_index, control_index,
                        "numeric radix, bounds, or value is invalid");
            } else if (control->kind == LT48_PROPERTY_CHECKBOX) {
                if (control->value > 1u)
                    return property_invalid(
                        out_issue, page_index, control_index,
                        "checkbox value is not zero or one");
            } else {
                const lt48_property_choice *choice =
                    &control->metadata.choice;
                size_t choice_index;
                if (choice->count == 0u ||
                    choice->count > LT48_PROPERTY_MAX_CHOICES ||
                    choice->labels == NULL || control->value > choice->count)
                    return property_invalid(
                        out_issue, page_index, control_index,
                        "choice count, labels, or value is invalid");
                for (choice_index = 0u; choice_index < choice->count;
                     ++choice_index)
                    if (!nonempty(choice->labels[choice_index]))
                        return property_invalid(
                            out_issue, page_index, control_index,
                            "choice label is empty");
            }
            for (earlier_page = 0u; earlier_page <= page_index;
                 ++earlier_page) {
                const lt48_property_page *candidate_page =
                    &model->pages[earlier_page];
                size_t limit = earlier_page == page_index ? control_index :
                    candidate_page->control_count;
                size_t earlier_control;
                for (earlier_control = 0u; earlier_control < limit;
                     ++earlier_control)
                    if (strcmp(
                            control->key,
                            candidate_page->controls[earlier_control].key
                        ) == 0)
                        return property_invalid(
                            out_issue, page_index, control_index,
                            "control key is duplicated");
            }
        }
    }
    return LT48_OK;
}

lt48_status lt48_property_model_apply_values(
    lt48_property_model *model,
    const lt48_property_value *values,
    size_t value_count
) {
    lt48_property_control **controls = NULL;
    uint32_t *original_values = NULL;
    unsigned char *matched = NULL;
    size_t control_count = 0u;
    size_t page_index;
    size_t value_index;
    size_t flat_index = 0u;
    lt48_status status;
    if (model == NULL || (value_count != 0u && values == NULL))
        return LT48_INVALID_ARGUMENT;
    status = lt48_property_model_validate(model, NULL);
    if (status != LT48_OK) return status;
    for (page_index = 0u; page_index < model->page_count; ++page_index)
        control_count += model->pages[page_index].control_count;
    if (value_count != control_count) return LT48_FORMAT_ERROR;
    if (control_count == 0u) return LT48_OK;
    controls = calloc(control_count, sizeof(*controls));
    original_values = calloc(control_count, sizeof(*original_values));
    matched = calloc(control_count, sizeof(*matched));
    if (controls == NULL || original_values == NULL || matched == NULL) {
        free(matched);
        free(original_values);
        free(controls);
        return LT48_NO_MEMORY;
    }
    for (page_index = 0u; page_index < model->page_count; ++page_index) {
        lt48_property_page *page = &model->pages[page_index];
        size_t control_index;
        for (control_index = 0u; control_index < page->control_count;
             ++control_index) {
            controls[flat_index] = &page->controls[control_index];
            original_values[flat_index] = page->controls[control_index].value;
            ++flat_index;
        }
    }
    for (value_index = 0u; value_index < value_count; ++value_index) {
        size_t candidate;
        if (!nonempty(values[value_index].key)) {
            status = LT48_FORMAT_ERROR;
            goto rollback;
        }
        for (candidate = 0u; candidate < control_count; ++candidate)
            if (strcmp(values[value_index].key, controls[candidate]->key) == 0)
                break;
        if (candidate == control_count || matched[candidate]) {
            status = LT48_FORMAT_ERROR;
            goto rollback;
        }
        matched[candidate] = 1u;
        controls[candidate]->value = values[value_index].value;
    }
    status = lt48_property_model_validate(model, NULL);
    if (status == LT48_OK) goto done;
rollback:
    for (flat_index = 0u; flat_index < control_count; ++flat_index)
        controls[flat_index]->value = original_values[flat_index];
done:
    free(matched);
    free(original_values);
    free(controls);
    return status;
}

static int property_u32(
    const lt48_json_value *value,
    uint32_t *out_value
) {
    uint64_t parsed;
    if (!lt48_json_u64(value, &parsed) || parsed > UINT32_MAX)
        return 0;
    *out_value = (uint32_t)parsed;
    return 1;
}

static lt48_status property_control_parse(
    const lt48_json_value *value,
    lt48_property_control *out_control
) {
    static const char *const common_fields[] = {
        "key", "label", "kind", "value"
    };
    static const char *const number_fields[] = {
        "key", "label", "kind", "value",
        "minimum", "maximum", "radix"
    };
    static const char *const choice_fields[] = {
        "key", "label", "kind", "value", "choices"
    };
    const char *kind;
    uint64_t parsed;
    memset(out_control, 0, sizeof(*out_control));
    if (lt48_json_value_type(value) != LT48_JSON_OBJECT)
        return LT48_FORMAT_ERROR;
    out_control->key = lt48_json_string(
        lt48_json_object_member(value, "key"));
    out_control->label = lt48_json_string(
        lt48_json_object_member(value, "label"));
    kind = lt48_json_string(lt48_json_object_member(value, "kind"));
    if (kind == NULL) return LT48_FORMAT_ERROR;
    if (strcmp(kind, "number") == 0) {
        if (!lt48_json_object_exact(
                value, number_fields,
                sizeof(number_fields) / sizeof(number_fields[0])) ||
            !property_u32(
                lt48_json_object_member(value, "value"),
                &out_control->value) ||
            !property_u32(
                lt48_json_object_member(value, "minimum"),
                &out_control->metadata.number.minimum) ||
            !property_u32(
                lt48_json_object_member(value, "maximum"),
                &out_control->metadata.number.maximum) ||
            !lt48_json_u64(
                lt48_json_object_member(value, "radix"), &parsed) ||
            parsed > UINT8_MAX)
            return LT48_FORMAT_ERROR;
        out_control->kind = LT48_PROPERTY_NUMBER;
        out_control->metadata.number.radix = (uint8_t)parsed;
    } else if (strcmp(kind, "checkbox") == 0) {
        int checked;
        if (!lt48_json_object_exact(
                value, common_fields,
                sizeof(common_fields) / sizeof(common_fields[0])) ||
            !lt48_json_bool(
                lt48_json_object_member(value, "value"), &checked))
            return LT48_FORMAT_ERROR;
        out_control->kind = LT48_PROPERTY_CHECKBOX;
        out_control->value = checked ? 1u : 0u;
    } else if (strcmp(kind, "choice") == 0) {
        const lt48_json_value *choices =
            lt48_json_object_member(value, "choices");
        size_t count;
        const char **labels;
        size_t index;
        if (!lt48_json_object_exact(
                value, choice_fields,
                sizeof(choice_fields) / sizeof(choice_fields[0])) ||
            !property_u32(
                lt48_json_object_member(value, "value"),
                &out_control->value) ||
            lt48_json_value_type(choices) != LT48_JSON_ARRAY)
            return LT48_FORMAT_ERROR;
        count = lt48_json_array_count(choices);
        if (count == 0u || count > LT48_PROPERTY_MAX_CHOICES)
            return LT48_FORMAT_ERROR;
        labels = calloc(count, sizeof(*labels));
        if (labels == NULL) return LT48_NO_MEMORY;
        for (index = 0u; index < count; ++index) {
            labels[index] = lt48_json_string(
                lt48_json_array_item(choices, index));
            if (labels[index] == NULL) {
                free(labels);
                return LT48_FORMAT_ERROR;
            }
        }
        out_control->kind = LT48_PROPERTY_CHOICE;
        out_control->metadata.choice.labels = labels;
        out_control->metadata.choice.count = count;
    } else {
        return LT48_FORMAT_ERROR;
    }
    return LT48_OK;
}

void lt48_property_document_close(lt48_property_document *document) {
    size_t page_index;
    if (document == NULL) return;
    for (page_index = 0u; page_index < document->model.page_count;
         ++page_index) {
        lt48_property_page *page = &document->model.pages[page_index];
        size_t control_index;
        for (control_index = 0u; control_index < page->control_count;
             ++control_index) {
            lt48_property_control *control = &page->controls[control_index];
            if (control->kind == LT48_PROPERTY_CHOICE) {
                if (document->owns_strings) {
                    size_t choice_index;
                    for (choice_index = 0u;
                         choice_index < control->metadata.choice.count;
                         ++choice_index)
                        free((void *)control->metadata.choice.labels[choice_index]);
                }
                free((void *)control->metadata.choice.labels);
            }
            if (document->owns_strings) {
                free((void *)control->key);
                free((void *)control->label);
            }
        }
        if (document->owns_strings) free((void *)page->title);
        free(page->controls);
    }
    if (document->owns_strings) free((void *)document->model.title);
    free(document->model.pages);
    lt48_json_document_close(document->json);
    free(document);
}

typedef struct option_choice_schema {
    const char *label;
    const char *const *choices;
    size_t count;
} option_choice_schema;

static const char *const memory_checkbox_labels[] = {
    "Insertion Test", "Device ID Check", "Blank Check", "Program",
    "Verify Passes", "MemProt/ProgCfig", "Auto-Increment",
    "Dummy OmitErrData", "Dummy OmitErrAddr", "Dummy DecErrAddr",
    "Dummy OmitProgErrData", "Dummy OmitProgVfy",
    "Auto Erase/Over Write", "Prog.Encrypt.Tbl.", "Decrypt While Read",
    "Dummy OmitMemProtMsg", "Edit Chip Size Only", "Skip Cali. Data",
    "1.Twice VCC +/- 5%", "2.Twice VCC +/- 10%", "3.Once",
    "1.Binary", "2.ASCII Hex.", "3.ASCII Decimal", "4.Modulo-26",
    "1.Mult. VCC min/max", "2.Once VCC", "Disable", "Enable",
    "Programmed"
};

static const char *const logic_checkbox_labels[] = {
    "Insertion Test", "Blank Check", "Program", "Verify", "Function Test",
    "Secure aft.Prog.", "FunTst aft.Secure", "Auto Erase/Over Write",
    "Device ID Check", "1.Twice VCC +/- 5%", "2.Twice VCC +/- 10%",
    "3.Once", "Error only", "All", "Step by step", "Low", "High", "HighZ"
};

static const char *const verify_choices[] = {
    "1.Twice VCC +/- 5%", "2.Twice VCC +/- 10%", "3.Once"
};
static const char *const increment_choices[] = {
    "1.Binary", "2.ASCII Hex.", "3.ASCII Decimal", "4.Modulo-26"
};
static const char *const verify_voltage_choices[] = {
    "1.Mult. VCC min/max", "2.Once VCC"
};
static const char *const encrypt_choices[] = {
    "Disable", "Enable", "Programmed"
};
static const char *const display_choices[] = {
    "Error only", "All", "Step by step"
};
static const char *const x_state_choices[] = {"Low", "High", "HighZ"};

static const option_choice_schema memory_choice_schema[] = {
    {"Verify option", verify_choices, 3u},
    {"AutoInc.data format", increment_choices, 4u},
    {"Verify option", verify_voltage_choices, 2u},
    {"Encrypt Option", encrypt_choices, 3u}
};
static const option_choice_schema logic_choice_schema[] = {
    {"FunTest Voltage", verify_choices, 3u},
    {"FunTest Display", display_choices, 3u},
    {"Vector 'X's State", x_state_choices, 3u}
};
static const char *const memory_number_labels[] = {
    "Start Address", "End Address", "Buffer Size", "AutoInc.Start",
    "AutoInc.End", "AutoInc.Value"
};
static const char *const download_number_labels[] = {
    "Start Address", "End Address", "Start Erase", "End Erase",
    "Buffer Size", "AutoInc.Start", "AutoInc.End", "AutoInc.Value"
};

static int operation_option_class_kind(uint16_t device_class) {
    if ((device_class >= 2000u && device_class <= 2018u) ||
        device_class == 2029u) return 1;
    if (device_class >= 2019u && device_class <= 2028u) return 2;
    if (device_class >= 2030u && device_class <= 2047u) return 3;
    return 0;
}

static char *operation_option_key(const char *kind, size_t index) {
    char buffer[48];
    int length = snprintf(buffer, sizeof(buffer), "option.%s.%zu", kind, index);
    if (length < 0 || (size_t)length >= sizeof(buffer)) return NULL;
    return property_string_duplicate(buffer);
}

lt48_status lt48_property_document_from_operation_options(
    uint16_t device_class,
    uint32_t capacity_bytes,
    uint32_t element_count,
    const lt48_family_operation_options *options,
    lt48_property_document **out_document
) {
    lt48_property_document *document;
    lt48_property_page *page;
    const char *const *checkbox_labels;
    const char *const *number_labels = NULL;
    const option_choice_schema *choice_schema;
    size_t checkbox_count;
    size_t number_count = 0u;
    size_t choice_count;
    size_t control_count = 0u;
    size_t control_index = 0u;
    uint32_t number_values[32];
    uint32_t number_minimum[32] = {0u};
    uint32_t number_maximum[32] = {0u};
    uint32_t address_max;
    int class_kind;
    size_t index;
    lt48_status status;
    if (options == NULL || out_document == NULL) return LT48_INVALID_ARGUMENT;
    *out_document = NULL;
    class_kind = operation_option_class_kind(device_class);
    if (class_kind == 0) return LT48_NOT_FOUND;
    memcpy(number_values, options->number_values, sizeof(number_values));
    address_max = element_count != 0u ? element_count - 1u :
                  capacity_bytes != 0u ? capacity_bytes - 1u : 0u;
    if (class_kind == 2) {
        checkbox_labels = logic_checkbox_labels;
        checkbox_count = sizeof(logic_checkbox_labels) /
                         sizeof(logic_checkbox_labels[0]);
        choice_schema = logic_choice_schema;
        choice_count = sizeof(logic_choice_schema) /
                       sizeof(logic_choice_schema[0]);
    } else {
        checkbox_labels = memory_checkbox_labels;
        checkbox_count = sizeof(memory_checkbox_labels) /
                         sizeof(memory_checkbox_labels[0]);
        choice_schema = memory_choice_schema;
        choice_count = class_kind == 3 ? 4u : 3u;
        if (class_kind == 1) {
            number_labels = memory_number_labels;
            number_count = sizeof(memory_number_labels) /
                           sizeof(memory_number_labels[0]);
            number_values[0] = 0u;
            number_values[1] = address_max;
            number_values[2] = capacity_bytes;
            if (number_values[3] == 0u && capacity_bytes != 0u)
                number_values[3] = capacity_bytes - 1u;
            if (number_values[4] == 0u && capacity_bytes >= 8u)
                number_values[4] = capacity_bytes - 8u;
            number_values[5] = 1u;
            number_maximum[0] = number_maximum[1] = address_max;
            number_minimum[2] = capacity_bytes;
            number_maximum[2] = 0x04000000u;
            number_maximum[3] = number_maximum[4] = capacity_bytes;
            number_maximum[5] = 0x10000000u;
        } else {
            number_labels = download_number_labels;
            number_count = sizeof(download_number_labels) /
                           sizeof(download_number_labels[0]);
            number_values[0] = 0u;
            number_values[1] = address_max;
            number_values[2] = 0u;
            number_values[3] = address_max;
            number_values[4] = capacity_bytes;
            if (number_values[5] == 0u && capacity_bytes != 0u)
                number_values[5] = capacity_bytes - 1u;
            if (number_values[6] == 0u && capacity_bytes >= 8u)
                number_values[6] = capacity_bytes - 8u;
            number_values[7] = 1u;
            for (index = 0u; index < 4u; ++index)
                number_maximum[index] = address_max;
            number_minimum[4] = capacity_bytes;
            number_maximum[4] = 0x40000000u;
            number_maximum[5] = number_maximum[6] = capacity_bytes;
            number_maximum[7] = 0x10000000u;
        }
    }
    for (index = 0u; index < number_count; ++index)
        if ((options->number_presence & (UINT32_C(1) << index)) != 0u)
            ++control_count;
    for (index = 0u; index < checkbox_count; ++index)
        if ((options->checkbox_presence & (UINT32_C(1) << index)) != 0u)
            ++control_count;
    for (index = 0u; index < choice_count; ++index)
        if ((options->choice_presence & (UINT32_C(1) << index)) != 0u)
            ++control_count;
    if (control_count == 0u) return LT48_NOT_FOUND;
    document = calloc(1u, sizeof(*document));
    if (document == NULL) return LT48_NO_MEMORY;
    document->owns_strings = 1;
    document->model.title = property_string_duplicate("Device Operation Option");
    document->model.pages = calloc(1u, sizeof(*document->model.pages));
    document->model.page_count = 1u;
    if (document->model.title == NULL || document->model.pages == NULL) {
        lt48_property_document_close(document);
        return LT48_NO_MEMORY;
    }
    page = document->model.pages;
    page->title = property_string_duplicate("Options");
    page->controls = calloc(control_count, sizeof(*page->controls));
    page->control_count = control_count;
    if (page->title == NULL || page->controls == NULL) {
        lt48_property_document_close(document);
        return LT48_NO_MEMORY;
    }
    for (index = 0u; index < number_count; ++index) {
        lt48_property_control *control;
        if ((options->number_presence & (UINT32_C(1) << index)) == 0u) continue;
        control = &page->controls[control_index++];
        control->key = operation_option_key("number", index);
        control->label = property_string_duplicate(number_labels[index]);
        control->kind = LT48_PROPERTY_NUMBER;
        control->value = number_values[index];
        control->metadata.number.minimum = number_minimum[index];
        control->metadata.number.maximum = number_maximum[index];
        control->metadata.number.radix = 16u;
    }
    for (index = 0u; index < checkbox_count; ++index) {
        lt48_property_control *control;
        if ((options->checkbox_presence & (UINT32_C(1) << index)) == 0u) continue;
        control = &page->controls[control_index++];
        control->key = operation_option_key("checkbox", index);
        control->label = property_string_duplicate(checkbox_labels[index]);
        control->kind = LT48_PROPERTY_CHECKBOX;
        control->value =
            (options->checkbox_values & (UINT32_C(1) << index)) != 0u;
    }
    for (index = 0u; index < choice_count; ++index) {
        lt48_property_control *control;
        const char **labels;
        size_t choice_index;
        if ((options->choice_presence & (UINT32_C(1) << index)) == 0u) continue;
        control = &page->controls[control_index++];
        control->key = operation_option_key("choice", index);
        control->label = property_string_duplicate(choice_schema[index].label);
        control->kind = LT48_PROPERTY_CHOICE;
        control->value = options->choice_values[index];
        labels = calloc(choice_schema[index].count, sizeof(*labels));
        control->metadata.choice.labels = labels;
        control->metadata.choice.count = choice_schema[index].count;
        if (labels != NULL)
            for (choice_index = 0u;
                 choice_index < choice_schema[index].count; ++choice_index)
                labels[choice_index] = property_string_duplicate(
                    choice_schema[index].choices[choice_index]);
    }
    for (index = 0u; index < page->control_count; ++index) {
        lt48_property_control *control = &page->controls[index];
        if (control->key == NULL || control->label == NULL ||
            (control->kind == LT48_PROPERTY_CHOICE &&
             control->metadata.choice.labels == NULL)) {
            lt48_property_document_close(document);
            return LT48_NO_MEMORY;
        }
        if (control->kind == LT48_PROPERTY_CHOICE) {
            size_t choice_index;
            for (choice_index = 0u;
                 choice_index < control->metadata.choice.count; ++choice_index)
                if (control->metadata.choice.labels[choice_index] == NULL) {
                    lt48_property_document_close(document);
                    return LT48_NO_MEMORY;
                }
        }
    }
    status = lt48_property_model_validate(&document->model, NULL);
    if (status != LT48_OK) {
        lt48_property_document_close(document);
        return status;
    }
    *out_document = document;
    return LT48_OK;
}

lt48_status lt48_property_document_open(
    const char *path,
    lt48_property_document **out_document
) {
    static const char *const root_fields[] = {
        "schema_version", "title", "pages"
    };
    static const char *const page_fields[] = {"title", "controls"};
    lt48_property_document *document;
    const lt48_json_value *root;
    const lt48_json_value *pages;
    uint64_t schema_version;
    size_t page_count;
    size_t page_index;
    lt48_status status;
    if (path == NULL || out_document == NULL)
        return LT48_INVALID_ARGUMENT;
    *out_document = NULL;
    document = calloc(1u, sizeof(*document));
    if (document == NULL) return LT48_NO_MEMORY;
    status = lt48_json_document_open(path, &document->json);
    if (status != LT48_OK) {
        lt48_property_document_close(document);
        return status;
    }
    root = lt48_json_document_root(document->json);
    pages = lt48_json_object_member(root, "pages");
    if (!lt48_json_object_exact(
            root, root_fields,
            sizeof(root_fields) / sizeof(root_fields[0])) ||
        !lt48_json_u64(
            lt48_json_object_member(root, "schema_version"),
            &schema_version) ||
        schema_version != 1u ||
        lt48_json_value_type(pages) != LT48_JSON_ARRAY) {
        lt48_property_document_close(document);
        return LT48_FORMAT_ERROR;
    }
    page_count = lt48_json_array_count(pages);
    if (page_count == 0u || page_count > LT48_PROPERTY_MAX_PAGES) {
        lt48_property_document_close(document);
        return LT48_FORMAT_ERROR;
    }
    document->model.title = lt48_json_string(
        lt48_json_object_member(root, "title"));
    document->model.pages = calloc(
        page_count, sizeof(*document->model.pages));
    if (document->model.pages == NULL) {
        lt48_property_document_close(document);
        return LT48_NO_MEMORY;
    }
    document->model.page_count = page_count;
    for (page_index = 0u; page_index < page_count; ++page_index) {
        const lt48_json_value *page_value =
            lt48_json_array_item(pages, page_index);
        const lt48_json_value *controls;
        size_t control_count;
        size_t control_index;
        lt48_property_page *page = &document->model.pages[page_index];
        if (!lt48_json_object_exact(
                page_value, page_fields,
                sizeof(page_fields) / sizeof(page_fields[0]))) {
            lt48_property_document_close(document);
            return LT48_FORMAT_ERROR;
        }
        page->title = lt48_json_string(
            lt48_json_object_member(page_value, "title"));
        controls = lt48_json_object_member(page_value, "controls");
        if (lt48_json_value_type(controls) != LT48_JSON_ARRAY) {
            lt48_property_document_close(document);
            return LT48_FORMAT_ERROR;
        }
        control_count = lt48_json_array_count(controls);
        if (control_count > LT48_PROPERTY_MAX_CONTROLS_PER_PAGE) {
            lt48_property_document_close(document);
            return LT48_FORMAT_ERROR;
        }
        if (control_count != 0u) {
            page->controls = calloc(
                control_count, sizeof(*page->controls));
            if (page->controls == NULL) {
                lt48_property_document_close(document);
                return LT48_NO_MEMORY;
            }
        }
        page->control_count = control_count;
        for (control_index = 0u; control_index < control_count;
             ++control_index) {
            status = property_control_parse(
                lt48_json_array_item(controls, control_index),
                &page->controls[control_index]);
            if (status != LT48_OK) {
                lt48_property_document_close(document);
                return status;
            }
        }
    }
    status = lt48_property_model_validate(&document->model, NULL);
    if (status != LT48_OK) {
        lt48_property_document_close(document);
        return status;
    }
    *out_document = document;
    return LT48_OK;
}

lt48_property_model *lt48_property_document_model(
    lt48_property_document *document
) {
    return document == NULL ? NULL : &document->model;
}
