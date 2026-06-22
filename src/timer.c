#include "timer.h"

#include <time.h>

double timer_now_seconds(void)
{
    struct timespec ts;

    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        return 0.0;
    }

    return (double)ts.tv_sec + ((double)ts.tv_nsec / 1000000000.0);
}
