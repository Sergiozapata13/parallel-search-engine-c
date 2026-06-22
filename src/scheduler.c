#include "scheduler.h"
#include "search.h"

#include <inttypes.h>
#include <limits.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
    const char *target;
    const char *charset;
    size_t charset_size;
    size_t length;
    uint64_t total_space;
    uint64_t chunk_size;
    int verbose;
    int full_scan;

    uint64_t next_start;
    uint64_t attempts;
    uint64_t found_index;
    int winning_thread;

    atomic_int stop;
    atomic_int found;
    atomic_int error;

    pthread_mutex_t work_mutex;
    pthread_mutex_t print_mutex;
} SharedSearchState;

typedef struct {
    int thread_id;
    SharedSearchState *shared;
} WorkerArgs;

long scheduler_detect_logical_processors(void)
{
    long count;

    count = sysconf(_SC_NPROCESSORS_ONLN);

    if (count < 1L) {
        return 1L;
    }

    return count;
}

static void report_range(SharedSearchState *shared,
                         int thread_id,
                         uint64_t start,
                         uint64_t end)
{
    pthread_mutex_lock(&shared->print_mutex);
    printf("[thread %d] range=[%" PRIu64 ", %" PRIu64 ")\n",
           thread_id,
           start,
           end);
    pthread_mutex_unlock(&shared->print_mutex);
}

static void report_attempt(SharedSearchState *shared,
                           int thread_id,
                           uint64_t range_attempt,
                           uint64_t index,
                           const char *candidate)
{
    pthread_mutex_lock(&shared->print_mutex);
    printf("[thread %d] range_attempt=%" PRIu64 " index=%" PRIu64 " candidate=%s\n",
           thread_id,
           range_attempt,
           index,
           candidate);
    pthread_mutex_unlock(&shared->print_mutex);
}

static int take_work_range(SharedSearchState *shared,
                           uint64_t *start,
                           uint64_t *end)
{
    pthread_mutex_lock(&shared->work_mutex);

    if (atomic_load_explicit(&shared->stop, memory_order_acquire) ||
        shared->next_start >= shared->total_space) {
        pthread_mutex_unlock(&shared->work_mutex);
        return 0;
    }

    *start = shared->next_start;

    if (shared->chunk_size > shared->total_space - *start) {
        *end = shared->total_space;
    } else {
        *end = *start + shared->chunk_size;
    }

    shared->next_start = *end;

    pthread_mutex_unlock(&shared->work_mutex);
    return 1;
}

static void add_attempts(SharedSearchState *shared, uint64_t local_attempts)
{
    pthread_mutex_lock(&shared->work_mutex);
    shared->attempts += local_attempts;
    pthread_mutex_unlock(&shared->work_mutex);
}

static void mark_found(SharedSearchState *shared, int thread_id, uint64_t found_index)
{
    pthread_mutex_lock(&shared->work_mutex);

    if (!atomic_load_explicit(&shared->found, memory_order_acquire)) {
        shared->found_index = found_index;
        shared->winning_thread = thread_id;
        atomic_store_explicit(&shared->found, 1, memory_order_release);

        if (!shared->full_scan) {
            atomic_store_explicit(&shared->stop, 1, memory_order_release);
        }
    }

    pthread_mutex_unlock(&shared->work_mutex);
}

static void mark_error(SharedSearchState *shared)
{
    atomic_store_explicit(&shared->error, 1, memory_order_release);
    atomic_store_explicit(&shared->stop, 1, memory_order_release);
}

static void *worker_run(void *arg)
{
    WorkerArgs *worker;
    SharedSearchState *shared;
    char *candidate;
    uint64_t local_attempts;

    worker = (WorkerArgs *)arg;
    shared = worker->shared;
    local_attempts = 0U;

    candidate = malloc(shared->length + 1U);
    if (candidate == NULL) {
        mark_error(shared);
        return NULL;
    }

    while (!atomic_load_explicit(&shared->stop, memory_order_acquire)) {
        uint64_t start;
        uint64_t end;
        uint64_t i;
        uint64_t range_attempt;

        if (!take_work_range(shared, &start, &end)) {
            break;
        }

        report_range(shared, worker->thread_id, start, end);

        range_attempt = 0U;

        for (i = start; i < end; i++) {
            if (atomic_load_explicit(&shared->stop, memory_order_acquire)) {
                break;
            }

            if (index_to_candidate(i,
                                   shared->charset,
                                   shared->charset_size,
                                   shared->length,
                                   candidate,
                                   shared->length + 1U) != 0) {
                mark_error(shared);
                free(candidate);
                add_attempts(shared, local_attempts);
                return NULL;
            }

            range_attempt++;
            local_attempts++;

            if (shared->verbose) {
                report_attempt(shared,
                               worker->thread_id,
                               range_attempt,
                               i,
                               candidate);
            }

            if (strcmp(candidate, shared->target) == 0) {
                mark_found(shared, worker->thread_id, i);

                if (!shared->full_scan) {
                    break;
                }
            }
        }
    }

    free(candidate);
    add_attempts(shared, local_attempts);
    return NULL;
}

static void init_result(ParallelSearchResult *result)
{
    result->found = 0;
    result->winning_thread = -1;
    result->found_index = 0U;
    result->attempts = 0U;
}

int scheduler_parallel_search(const char *target,
                              const char *charset,
                              size_t charset_size,
                              size_t length,
                              uint64_t total_space,
                              uint64_t chunk_size,
                              int verbose,
                              int full_scan,
                              long thread_count,
                              ParallelSearchResult *result)
{
    SharedSearchState shared;
    pthread_t *threads;
    WorkerArgs *args;
    size_t i;
    size_t created_threads;

    if (target == NULL || charset == NULL || result == NULL) {
        return -1;
    }

    if (charset_size == 0U || length == 0U || total_space == 0U || chunk_size == 0U) {
        return -1;
    }

    if (thread_count < 1L || thread_count > INT_MAX) {
        return -1;
    }

    init_result(result);

    memset(&shared, 0, sizeof(shared));
    shared.target = target;
    shared.charset = charset;
    shared.charset_size = charset_size;
    shared.length = length;
    shared.total_space = total_space;
    shared.chunk_size = chunk_size;
    shared.verbose = verbose;
    shared.full_scan = full_scan;
    shared.next_start = 0U;
    shared.attempts = 0U;
    shared.found_index = 0U;
    shared.winning_thread = -1;

    atomic_init(&shared.stop, 0);
    atomic_init(&shared.found, 0);
    atomic_init(&shared.error, 0);

    if (pthread_mutex_init(&shared.work_mutex, NULL) != 0) {
        return -1;
    }

    if (pthread_mutex_init(&shared.print_mutex, NULL) != 0) {
        pthread_mutex_destroy(&shared.work_mutex);
        return -1;
    }

    threads = calloc((size_t)thread_count, sizeof(*threads));
    args = calloc((size_t)thread_count, sizeof(*args));

    if (threads == NULL || args == NULL) {
        free(threads);
        free(args);
        pthread_mutex_destroy(&shared.print_mutex);
        pthread_mutex_destroy(&shared.work_mutex);
        return -1;
    }

    created_threads = 0U;

    for (i = 0U; i < (size_t)thread_count; i++) {
        args[i].thread_id = (int)i;
        args[i].shared = &shared;

        if (pthread_create(&threads[i], NULL, worker_run, &args[i]) != 0) {
            mark_error(&shared);
            break;
        }

        created_threads++;
    }

    for (i = 0U; i < created_threads; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            mark_error(&shared);
        }
    }

    if (atomic_load_explicit(&shared.error, memory_order_acquire)) {
        free(threads);
        free(args);
        pthread_mutex_destroy(&shared.print_mutex);
        pthread_mutex_destroy(&shared.work_mutex);
        return -1;
    }

    result->found = atomic_load_explicit(&shared.found, memory_order_acquire);
    result->winning_thread = shared.winning_thread;
    result->found_index = shared.found_index;
    result->attempts = shared.attempts;

    free(threads);
    free(args);
    pthread_mutex_destroy(&shared.print_mutex);
    pthread_mutex_destroy(&shared.work_mutex);

    return result->found ? 1 : 0;
}
