#include "config.h"
#include "charset.h"
#include "search.h"
#include "scheduler.h"
#include "timer.h"

#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_usage(const char *program_name)
{
    fprintf(stderr,
            "Uso:\n"
            "  %s <archivo_config>\n"
            "  %s <archivo_config> --threads <N>\n"
            "  %s <archivo_config> --full-scan\n"
            "  %s <archivo_config> --threads <N> --full-scan\n\n"
            "Ejemplos:\n"
            "  %s config/example.conf\n"
            "  %s config/example.conf --threads 4\n"
            "  %s config/benchmark.conf --threads 4 --full-scan\n",
            program_name,
            program_name,
            program_name,
            program_name,
            program_name,
            program_name,
            program_name);
}

static int parse_thread_count(const char *text, long *out)
{
    char *end;
    long value;

    if (text == NULL || out == NULL) {
        return -1;
    }

    errno = 0;
    value = strtol(text, &end, 10);

    if (errno != 0 || end == text || *end != '\0') {
        return -1;
    }

    if (value < 1L || value > INT_MAX) {
        return -1;
    }

    *out = value;
    return 0;
}

static int parse_arguments(int argc,
                           char **argv,
                           const char **config_path,
                           long *requested_threads,
                           int *threads_was_set,
                           int *full_scan)
{
    int i;

    if (argc < 2) {
        return -1;
    }

    if (config_path == NULL ||
        requested_threads == NULL ||
        threads_was_set == NULL ||
        full_scan == NULL) {
        return -1;
    }

    *config_path = argv[1];
    *requested_threads = 0L;
    *threads_was_set = 0;
    *full_scan = 0;

    i = 2;
    while (i < argc) {
        if (strcmp(argv[i], "--threads") == 0) {
            if (*threads_was_set || i + 1 >= argc) {
                return -1;
            }

            if (parse_thread_count(argv[i + 1], requested_threads) != 0) {
                return -1;
            }

            *threads_was_set = 1;
            i += 2;
        } else if (strcmp(argv[i], "--full-scan") == 0) {
            if (*full_scan) {
                return -1;
            }

            *full_scan = 1;
            i++;
        } else {
            return -1;
        }
    }

    return 0;
}

int main(int argc, char **argv)
{
    Config cfg;
    const char *config_path;
    const char *charset;
    size_t alphabet_size;
    int overflow;
    uint64_t total_space;
    long logical_processors;
    long worker_threads;
    long requested_threads;
    int threads_was_set;
    int full_scan;
    double start_time;
    double end_time;
    ParallelSearchResult result;
    int search_status;
    char found_candidate[MAX_PASSWORD_LEN + 1U];

    if (parse_arguments(argc, argv, &config_path, &requested_threads, &threads_was_set, &full_scan) != 0) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

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

    if (threads_was_set) {
        if (requested_threads > logical_processors) {
            fprintf(stderr,
                    "Error: --threads no debe exceder los procesadores lógicos detectados (%ld).\n",
                    logical_processors);
            return EXIT_FAILURE;
        }

        worker_threads = requested_threads;
    } else {
        worker_threads = logical_processors;
    }

    printf("=== parallel-search-engine-c ===\n");
    config_print(&cfg);
    printf("Charset size       : %zu\n", alphabet_size);
    printf("Search space       : %" PRIu64 "\n", total_space);
    printf("Logical processors : %ld\n", logical_processors);
    printf("Execution mode     : pthread dynamic scheduler\n");
    printf("Worker threads     : %ld\n", worker_threads);
    printf("Full scan          : %s\n", full_scan ? "yes" : "no");

    start_time = timer_now_seconds();

    search_status = scheduler_parallel_search(cfg.target,
                                              charset,
                                              alphabet_size,
                                              cfg.length,
                                              total_space,
                                              cfg.chunk_size,
                                              cfg.verbose,
                                              full_scan,
                                              worker_threads,
                                              &result);

    end_time = timer_now_seconds();

    if (search_status < 0) {
        fprintf(stderr, "Error: falló la búsqueda paralela.\n");
        return EXIT_FAILURE;
    }

    if (search_status == 0) {
        printf("Found              : no\n");
        printf("Attempts           : %" PRIu64 "\n", result.attempts);
        printf("Elapsed time       : %.6f s\n", end_time - start_time);
        return EXIT_FAILURE;
    }

    if (index_to_candidate(result.found_index,
                           charset,
                           alphabet_size,
                           cfg.length,
                           found_candidate,
                           sizeof(found_candidate)) != 0) {
        fprintf(stderr, "Error: no se pudo reconstruir el candidato encontrado.\n");
        return EXIT_FAILURE;
    }

    printf("Found              : yes\n");
    printf("Found candidate    : %s\n", found_candidate);
    printf("Found index        : %" PRIu64 "\n", result.found_index);
    printf("Winning thread     : %d\n", result.winning_thread);
    printf("Attempts           : %" PRIu64 "\n", result.attempts);
    printf("Elapsed time       : %.6f s\n", end_time - start_time);

    if ((end_time - start_time) > 0.0) {
        printf("Throughput         : %.2f attempts/s\n",
               (double)result.attempts / (end_time - start_time));
    }

    return EXIT_SUCCESS;
}
