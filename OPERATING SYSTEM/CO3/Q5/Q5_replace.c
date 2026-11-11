#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAXF 1024

int find(const int f[], int n, int page) {
    for (int i = 0; i < n; i++)
        if (f[i] == page) return i;
    return -1;
}

int fifo(const int ref[], int len, int frames) {
    int f[MAXF], used = 0, next = 0, faults = 0;

    for (int i = 0; i < len; i++) {
        if (find(f, used, ref[i]) >= 0) continue;

        faults++;

        if (used < frames)
            f[used++] = ref[i];
        else {
            f[next] = ref[i];
            next = (next + 1) % frames;
        }
    }
    return faults;
}

int lru(const int ref[], int len, int frames) {
    int f[MAXF], last[MAXF], used = 0, faults = 0;

    for (int i = 0; i < len; i++) {
        int k = find(f, used, ref[i]);

        if (k >= 0) {
            last[k] = i;
            continue;
        }

        faults++;

        if (used < frames) {
            f[used] = ref[i];
            last[used] = i;
            used++;
        } else {
            int victim = 0;

            for (int j = 1; j < frames; j++)
                if (last[j] < last[victim])
                    victim = j;

            f[victim] = ref[i];
            last[victim] = i;
        }
    }
    return faults;
}

int optimal(const int ref[], int len, int frames) {
    int f[MAXF], used = 0, faults = 0;

    for (int i = 0; i < len; i++) {
        if (find(f, used, ref[i]) >= 0) continue;

        faults++;

        if (used < frames) {
            f[used++] = ref[i];
            continue;
        }

        int victim = 0, farthest = -1;

        for (int j = 0; j < frames; j++) {
            int next = len;

            for (int t = i + 1; t < len; t++)
                if (ref[t] == f[j]) {
                    next = t;
                    break;
                }

            if (next > farthest) {
                farthest = next;
                victim = j;
            }
        }

        f[victim] = ref[i];
    }
    return faults;
}

int clock_algo(const int ref[], int len, int frames) {
    int f[MAXF], refbit[MAXF];
    int used = 0, hand = 0, faults = 0;

    for (int i = 0; i < len; i++) {
        int hit = find(f, used, ref[i]);

        if (hit >= 0) {
            refbit[hit] = 1;
            continue;
        }

        faults++;

        if (used < frames) {
            f[used] = ref[i];
            refbit[used++] = 1;
            continue;
        }

        while (1) {
            if (refbit[hand] == 0) {
                f[hand] = ref[i];
                refbit[hand] = 1;
                hand = (hand + 1) % frames;
                break;
            }

            refbit[hand] = 0;
            hand = (hand + 1) % frames;
        }
    }
    return faults;
}

int main(void) {
    int ref[] = {
        7,0,1,2,0,3,0,4,2,3,
        0,3,2,1,2,0,1,7,0,1
    };

    int len = 20;

    printf("===== FIFO, LRU AND OPTIMAL =====\n");

    for (int fr = 3; fr <= 4; fr++) {
        printf("%d frames: FIFO %d, LRU %d, Optimal %d\n",
               fr,
               fifo(ref, len, fr),
               lru(ref, len, fr),
               optimal(ref, len, fr));
    }

    printf("\n===== BELADY'S ANOMALY TEST =====\n");

    int b[] = {1,2,3,4,1,2,5,1,2,3,4,5};

    printf("FIFO: 3 frames %d faults, 4 frames %d faults\n",
           fifo(b, 12, 3), fifo(b, 12, 4));

    printf("\n===== CLOCK ALGORITHM =====\n");

    printf("Clock check: 3 frames %d, 4 frames %d\n",
           clock_algo(ref, len, 3),
           clock_algo(ref, len, 4));

    return 0;
}
