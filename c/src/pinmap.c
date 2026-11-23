#include <labtool48/labtool48.h>

#include <stdlib.h>
#include <string.h>

typedef struct pin_group_owned {
    uint16_t kind;
    unsigned char *pins;
    size_t pin_count;
} pin_group_owned;

struct lt48_pin_map {
    uint8_t package_pin_count;
    pin_group_owned *groups;
    size_t group_count;
    lt48_logical_pin *logical;
    size_t logical_count;
};

static uint16_t read_u16(const unsigned char *data) {
    return (uint16_t)((uint16_t)data[0] | (uint16_t)data[1] << 8);
}

void lt48_pin_map_close(lt48_pin_map *pin_map) {
    if (pin_map == NULL) {
        return;
    }
    for (size_t index = 0; index < pin_map->group_count; ++index) {
        free(pin_map->groups[index].pins);
    }
    free(pin_map->groups);
    free(pin_map->logical);
    free(pin_map);
}

lt48_status lt48_pin_map_parse(
    const lt48_pin_code *pin_code, lt48_pin_map **out_pin_map
) {
    lt48_pin_map *pin_map;
    size_t raw_group_count;
    size_t logical_count;
    size_t position = 3;
    int center;
    unsigned char selected[48] = {0};
    if (pin_code == NULL || out_pin_map == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    *out_pin_map = NULL;
    if (pin_code->data_size < 3 || pin_code->pin_count == 0 ||
        pin_code->pin_count > 48) {
        return LT48_FORMAT_ERROR;
    }
    raw_group_count = pin_code->data[0];
    logical_count = pin_code->data[1];
    pin_map = calloc(1, sizeof(*pin_map));
    if (pin_map == NULL) {
        return LT48_NO_MEMORY;
    }
    pin_map->groups = calloc(raw_group_count + 1, sizeof(*pin_map->groups));
    pin_map->logical = calloc(logical_count == 0 ? 1 : logical_count,
                              sizeof(*pin_map->logical));
    if (pin_map->groups == NULL || pin_map->logical == NULL) {
        lt48_pin_map_close(pin_map);
        return LT48_NO_MEMORY;
    }
    pin_map->package_pin_count = pin_code->pin_count;
    pin_map->logical_count = logical_count;
    center = (48 - pin_code->pin_count) / 2;
    for (size_t index = 0; index < raw_group_count; ++index) {
        pin_group_owned *group = &pin_map->groups[index];
        size_t count;
        if (pin_code->data_size - position < 4) {
            lt48_pin_map_close(pin_map);
            return LT48_FORMAT_ERROR;
        }
        group->kind = read_u16(pin_code->data + position);
        count = read_u16(pin_code->data + position + 2);
        position += 4;
        if (count > pin_code->data_size - position) {
            lt48_pin_map_close(pin_map);
            return LT48_FORMAT_ERROR;
        }
        for (size_t previous = 0; previous < index; ++previous) {
            if (pin_map->groups[previous].kind == group->kind) {
                lt48_pin_map_close(pin_map);
                return LT48_FORMAT_ERROR;
            }
        }
        group->pins = malloc(count == 0 ? 1 : count);
        if (group->pins == NULL) {
            lt48_pin_map_close(pin_map);
            return LT48_NO_MEMORY;
        }
        group->pin_count = count;
        for (size_t pin_index = 0; pin_index < count; ++pin_index) {
            unsigned char raw_pin = pin_code->data[position + pin_index];
            int signed_pin = raw_pin < 0x80 ? raw_pin : (int)raw_pin - 0x100;
            group->pins[pin_index] = (unsigned char)((signed_pin + center) * 4);
            if (group->kind == 1 && raw_pin < pin_code->pin_count) {
                selected[raw_pin] = 1;
            }
        }
        position += count;
    }
    pin_map->group_count = raw_group_count;
    for (size_t index = 0; index < logical_count; ++index) {
        uint16_t number;
        uint16_t raw_code;
        int signed_code;
        int socket_position;
        if (pin_code->data_size - position < 4) {
            lt48_pin_map_close(pin_map);
            return LT48_FORMAT_ERROR;
        }
        number = read_u16(pin_code->data + position);
        raw_code = read_u16(pin_code->data + position + 2);
        for (size_t previous = 0; previous < index; ++previous) {
            if (pin_map->logical[previous].number == number) {
                lt48_pin_map_close(pin_map);
                return LT48_FORMAT_ERROR;
            }
        }
        signed_code = raw_code < 0x8000 ? raw_code : (int)raw_code - 0x10000;
        socket_position = signed_code + center;
        if (socket_position < 0 || socket_position >= 48) {
            lt48_pin_map_close(pin_map);
            return LT48_FORMAT_ERROR;
        }
        pin_map->logical[index].number = number;
        pin_map->logical[index].engine_pin = (uint8_t)(socket_position * 4);
        position += 4;
    }
    if (position != pin_code->data_size) {
        lt48_pin_map_close(pin_map);
        return LT48_FORMAT_ERROR;
    }
    {
        pin_group_owned *group_zero = NULL;
        size_t count = 0;
        for (size_t index = 0; index < raw_group_count; ++index) {
            if (pin_map->groups[index].kind == 0) {
                group_zero = &pin_map->groups[index];
                free(group_zero->pins);
                group_zero->pins = NULL;
                break;
            }
        }
        if (group_zero == NULL) {
            group_zero = &pin_map->groups[pin_map->group_count++];
            group_zero->kind = 0;
        }
        for (size_t pin = 0; pin < pin_code->pin_count; ++pin) {
            if (!selected[pin]) {
                ++count;
            }
        }
        group_zero->pins = malloc(count == 0 ? 1 : count);
        if (group_zero->pins == NULL) {
            lt48_pin_map_close(pin_map);
            return LT48_NO_MEMORY;
        }
        group_zero->pin_count = count;
        count = 0;
        for (size_t pin = 0; pin < pin_code->pin_count; ++pin) {
            if (!selected[pin]) {
                group_zero->pins[count++] = (unsigned char)((pin + center) * 4);
            }
        }
    }
    *out_pin_map = pin_map;
    return LT48_OK;
}

uint8_t lt48_pin_map_package_pin_count(const lt48_pin_map *pin_map) {
    return pin_map == NULL ? 0 : pin_map->package_pin_count;
}

size_t lt48_pin_map_group_count(const lt48_pin_map *pin_map) {
    return pin_map == NULL ? 0 : pin_map->group_count;
}

size_t lt48_pin_map_logical_count(const lt48_pin_map *pin_map) {
    return pin_map == NULL ? 0 : pin_map->logical_count;
}

lt48_status lt48_pin_map_get_group(
    const lt48_pin_map *pin_map, size_t index, lt48_pin_group *out_group
) {
    if (pin_map == NULL || out_group == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    if (index >= pin_map->group_count) {
        return LT48_NOT_FOUND;
    }
    out_group->kind = pin_map->groups[index].kind;
    out_group->pins = pin_map->groups[index].pins;
    out_group->pin_count = pin_map->groups[index].pin_count;
    return LT48_OK;
}

lt48_status lt48_pin_map_find_group(
    const lt48_pin_map *pin_map, uint16_t kind, lt48_pin_group *out_group
) {
    if (pin_map == NULL || out_group == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    for (size_t index = 0; index < pin_map->group_count; ++index) {
        if (pin_map->groups[index].kind == kind) {
            return lt48_pin_map_get_group(pin_map, index, out_group);
        }
    }
    return LT48_NOT_FOUND;
}

lt48_status lt48_pin_map_get_logical(
    const lt48_pin_map *pin_map, size_t index, lt48_logical_pin *out_pin
) {
    if (pin_map == NULL || out_pin == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    if (index >= pin_map->logical_count) {
        return LT48_NOT_FOUND;
    }
    *out_pin = pin_map->logical[index];
    return LT48_OK;
}

lt48_status lt48_pin_map_find_logical(
    const lt48_pin_map *pin_map, uint16_t number, lt48_logical_pin *out_pin
) {
    if (pin_map == NULL || out_pin == NULL) {
        return LT48_INVALID_ARGUMENT;
    }
    for (size_t index = 0; index < pin_map->logical_count; ++index) {
        if (pin_map->logical[index].number == number) {
            *out_pin = pin_map->logical[index];
            return LT48_OK;
        }
    }
    return LT48_NOT_FOUND;
}

/* Original kernel table initialization zeroes every declared control record
 * before loading the groups present in TPinCode (0043d040 / 00488ae0).
 * A declared but absent group therefore has zero members. */
lt48_status lt48_pin_map_complete_groups_internal(lt48_pin_map *map, size_t count) {
    if (!map || count > 256) return LT48_INVALID_ARGUMENT;
    for (size_t i=0;i<map->group_count;i++)
        if (map->groups[i].kind >= count) return LT48_FORMAT_ERROR;
    for (size_t kind=0;kind<count;kind++) {
        size_t i=0;for (;i<map->group_count;i++)if(map->groups[i].kind==kind)break;
        if(i<map->group_count)continue;
        pin_group_owned *groups=realloc(map->groups,(map->group_count+1)*sizeof(*groups));
        if(!groups)return LT48_NO_MEMORY;
        map->groups=groups;
        map->groups[map->group_count++]=(pin_group_owned){(uint16_t)kind,NULL,0};
    }
    return LT48_OK;
}

/* Restrict a digest-verified peripheral group without changing socket mapping. */
lt48_status lt48_pin_map_limit_group_internal(lt48_pin_map *map, unsigned kind,
    size_t expected, size_t count) {
    if(!map || count>expected)return LT48_INVALID_ARGUMENT;
    for(size_t i=0;i<map->group_count;i++)if(map->groups[i].kind==kind){
        if(map->groups[i].pin_count!=expected)return LT48_SAFETY_REJECTED;
        map->groups[i].pin_count=count;return LT48_OK;
    }
    return LT48_NOT_FOUND;
}
