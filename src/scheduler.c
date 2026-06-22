#include "scheduler.h"

#include <unistd.h>

long scheduler_detect_logical_processors(void)
{
    long count;

    count = sysconf(_SC_NPROCESSORS_ONLN);

    if (count < 1L) {
        return 1L;
    }

    return count;
}
