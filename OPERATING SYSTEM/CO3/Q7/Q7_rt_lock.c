#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/mman.h>

#define BUF (1 * 1024 * 1024)
#define ITERS 100000

long long ns()
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (long long)t.tv_sec * 1000000000LL + t.tv_nsec;
}

void work(char *p)
{
    for (int i = 0; i < BUF; i += 64)
        p[i]++;
}

int main(int argc, char *argv[])
{
    int lock = (argc > 1) ? atoi(argv[1]) : 0;

    char *p = aligned_alloc(4096, BUF);
    if (!p) {
        perror("aligned_alloc");
        return 1;
    }

    memset(p, 0, BUF);

    if (lock) {
        if (mlock(p, BUF) != 0) {
            perror("mlock");
            free(p);
            return 1;
        }
    }

    long long worst = 0;

    for (int i = 0; i < ITERS; i++) {
        long long start = ns();
        work(p);
        long long elapsed = ns() - start;

        if (elapsed > worst)
            worst = elapsed;

        if (i % 1000 == 0)
            usleep(1);
    }

    printf("%s: worst-case delay = %lld ns\n",
           lock ? "LOCKED" : "UNLOCKED", worst);

    if (lock)
        munlock(p, BUF);

    free(p);
    return 0;
}
