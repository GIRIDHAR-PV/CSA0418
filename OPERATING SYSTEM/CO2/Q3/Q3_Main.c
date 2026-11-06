#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>
 
#define CAMERAS 64
#define WORK 3000000
 
typedef struct {
    int first;
    int last;
    unsigned long long result;
} Job;
 
void *process_frames(void *arg)
{
    Job *job = (Job *)arg;
    unsigned long long total = 0;
 
    for (int camera = job->first; camera < job->last; camera++) {
        unsigned long long value = (unsigned long long)camera + 1;
        for (int i = 0; i < WORK; i++)
            total += (value * (unsigned long long)(i + 1)) % 97;
    }
 
    job->result = total;
    return NULL;
}
 
int main(int argc, char *argv[])
{
    int workers = argc > 1 ? atoi(argv[1]) : 4;
    if (workers < 1 || workers > CAMERAS) {
        printf("Use a worker count from 1 to 64\n");
        return 1;
    }
 
    pthread_t threads[CAMERAS];
    Job jobs[CAMERAS];
    int base = CAMERAS / workers;
    int extra = CAMERAS % workers;
    int start = 0;
    unsigned long long total = 0;
 
    for (int i = 0; i < workers; i++) {
        int amount = base + (i < extra);
        jobs[i].first = start;
        jobs[i].last = start + amount;
        jobs[i].result = 0;
        start += amount;
        pthread_create(&threads[i], NULL, process_frames, &jobs[i]);
    }
 
    for (int i = 0; i < workers; i++) {
        pthread_join(threads[i], NULL);
        total += jobs[i].result;
    }
 
    printf("Cameras processed: %d\n", CAMERAS);
    printf("Worker threads: %d\n", workers);
    printf("Combined result: %llu\n", total);
    return 0;
}

