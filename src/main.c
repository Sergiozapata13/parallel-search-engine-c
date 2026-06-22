#include "config.h"
#include "charset.h"
#include "search.h"
#include "scheduler.h"
#include "timer.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    Config cfg;
    const char *config_path;
    const char *charset;
    size_t alphabet_size;
    int overflow;
    uint64_t total_space;
    long logical_processors;
    double start_time;
    double end_time;

    if (argc != 2) {
        fprintf(stderr, "Uso: %s <archivo_config>\n", argv[0]);
        return EXIT_FAILURE;
    }

    config_path = argv[1];

    if (config_load(config_path, &cfg) != 0) {
        fprintf(stderr, "Error: no se pudo cargar la configuración.\n");
        return EXIT_FAILURE;
    }

    if (config_validate(&cfg) != 0) {
        fprintf(stderr, "Error: configuración inválida.\n");
        return EXIT_FAILURE;
    }

    charset = charset_for_mode(cfg.mode);
    if (charset == NULL) {
        fprintf(stderr, "Error: modo de charset inválido.\n");
        return EXIT_FAILURE;
    }

    if (!charset_validate_string(charset, cfg.target)) {
        fprintf(stderr, "Error: target contiene caracteres no permitidos para el modo seleccionado.\n");
        return EXIT_FAILURE;
    }

    alphabet_size = charset_len(charset);
    overflow = 0;
    total_space = search_space_size(alphabet_size, cfg.length, &overflow);

    if (overflow) {
        fprintf(stderr, "Error: el espacio de búsqueda excede uint64_t.\n");
        return EXIT_FAILURE;
    }

    logical_processors = scheduler_detect_logical_processors();

    start_time = timer_now_seconds();

    printf("=== parallel-search-engine-c ===\n");
    config_print(&cfg);
    printf("Charset size       : %zu\n", alphabet_size);
    printf("Search space       : %" PRIu64 "\n", total_space);
    printf("Logical processors : %ld\n", logical_processors);
    printf("Status             : base inicializada correctamente\n");

    end_time = timer_now_seconds();

    printf("Init time          : %.6f s\n", end_time - start_time);

    return EXIT_SUCCESS;
}
