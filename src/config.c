#include "config.h"

#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *trim(char *s)
{
    char *end;

    while (isspace((unsigned char)*s)) {
        s++;
    }

    if (*s == '\0') {
        return s;
    }

    end = s + strlen(s) - 1;

    while (end > s && isspace((unsigned char)*end)) {
        *end = '\0';
        end--;
    }

    return s;
}

static int parse_u64(const char *text, uint64_t *out)
{
    char *end;
    unsigned long long value;

    errno = 0;
    value = strtoull(text, &end, 10);

    if (errno != 0 || end == text || *end != '\0') {
        return -1;
    }

    *out = (uint64_t)value;
    return 0;
}

static void config_set_defaults(Config *cfg)
{
    cfg->mode = 0;
    cfg->length = 0;
    cfg->target[0] = '\0';
    cfg->verbose = 0;
    cfg->print_ranges = 1;
    cfg->chunk_size = DEFAULT_CHUNK_SIZE;
}

int config_load(const char *path, Config *cfg)
{
    FILE *file;
    char line[256];

    if (path == NULL || cfg == NULL) {
        return -1;
    }

    config_set_defaults(cfg);

    file = fopen(path, "r");
    if (file == NULL) {
        perror("fopen");
        return -1;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        char *key;
        char *value;
        char *equals;
        size_t value_len;
        uint64_t parsed;

        line[strcspn(line, "\n")] = '\0';

        key = trim(line);

        if (*key == '\0' || *key == '#') {
            continue;
        }

        equals = strchr(key, '=');
        if (equals == NULL) {
            fprintf(stderr, "Línea inválida en config: %s\n", key);
            fclose(file);
            return -1;
        }

        *equals = '\0';
        value = trim(equals + 1);
        key = trim(key);

        if (strcmp(key, "mode") == 0) {
            if (parse_u64(value, &parsed) != 0 || parsed > 4U) {
                fclose(file);
                return -1;
            }
            cfg->mode = (int)parsed;
        } else if (strcmp(key, "length") == 0) {
            if (parse_u64(value, &parsed) != 0 || parsed > MAX_PASSWORD_LEN) {
                fclose(file);
                return -1;
            }
            cfg->length = (size_t)parsed;
        } else if (strcmp(key, "target") == 0) {
            value_len = strlen(value);
            if (value_len > MAX_PASSWORD_LEN) {
                fclose(file);
                return -1;
            }
            memcpy(cfg->target, value, value_len + 1U);
        } else if (strcmp(key, "verbose") == 0) {
            if (parse_u64(value, &parsed) != 0 || parsed > 1U) {
                fclose(file);
                return -1;
            }
            cfg->verbose = (int)parsed;
        } else if (strcmp(key, "print_ranges") == 0) {
            if (parse_u64(value, &parsed) != 0 || parsed > 1U) {
                fclose(file);
                return -1;
            }
            cfg->print_ranges = (int)parsed;
        } else if (strcmp(key, "chunk_size") == 0) {
            if (parse_u64(value, &parsed) != 0 || parsed == 0U) {
                fclose(file);
                return -1;
            }
            cfg->chunk_size = parsed;
        } else {
            fprintf(stderr, "Clave desconocida en config: %s\n", key);
            fclose(file);
            return -1;
        }
    }

    fclose(file);
    return 0;
}

int config_validate(const Config *cfg)
{
    if (cfg == NULL) {
        return -1;
    }

    if (cfg->mode < 1 || cfg->mode > 4) {
        fprintf(stderr, "mode debe estar entre 1 y 4.\n");
        return -1;
    }

    if (cfg->length == 0U || cfg->length > MAX_PASSWORD_LEN) {
        fprintf(stderr, "length debe estar entre 1 y %d.\n", MAX_PASSWORD_LEN);
        return -1;
    }

    if (strlen(cfg->target) != cfg->length) {
        fprintf(stderr, "target debe tener exactamente %zu caracteres.\n", cfg->length);
        return -1;
    }

    if (cfg->verbose != 0 && cfg->verbose != 1) {
        fprintf(stderr, "verbose debe ser 0 o 1.\n");
        return -1;
    }

    if (cfg->print_ranges != 0 && cfg->print_ranges != 1) {
        fprintf(stderr, "print_ranges debe ser 0 o 1.\n");
        return -1;
    }

    if (cfg->chunk_size == 0U) {
        fprintf(stderr, "chunk_size debe ser mayor que 0.\n");
        return -1;
    }

    return 0;
}

void config_print(const Config *cfg)
{
    if (cfg == NULL) {
        return;
    }

    printf("Mode               : %d\n", cfg->mode);
    printf("Length             : %zu\n", cfg->length);
    printf("Target             : %s\n", cfg->target);
    printf("Verbose            : %d\n", cfg->verbose);
    printf("Print ranges       : %d\n", cfg->print_ranges);
    printf("Chunk size         : %" PRIu64 "\n", cfg->chunk_size);
}
