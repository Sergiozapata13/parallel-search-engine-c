#ifndef CONFIG_H
#define CONFIG_H

#include <stddef.h>
#include <stdint.h>

#define MAX_PASSWORD_LEN 10
#define DEFAULT_CHUNK_SIZE 10000ULL

typedef struct {
    int mode;
    size_t length;
    char target[MAX_PASSWORD_LEN + 1];
    int verbose;
    int print_ranges;
    uint64_t chunk_size;
} Config;

int config_load(const char *path, Config *cfg);
int config_validate(const Config *cfg);
void config_print(const Config *cfg);

#endif
