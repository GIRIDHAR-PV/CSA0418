#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define N 8
#define MAXCYL 199
int req[N] = {98, 183, 37, 122, 14, 124, 65, 67};

int cmp(const void *a, const void *b) { return *(const int *)a - *(const int *)b; }

void show(const char *name, const int order[], int n, int head, int extra) {
    int total = 0, pos = head;
    printf("%-6s %d", name, head);
    for (int i = 0; i < n; i++) {
        printf(" %d", order[i]);
        total += abs(order[i] - pos);
        pos = order[i];
    }
    printf(" -> %d cylinders\n", total + extra);
}

void sstf_order(const int requests[], int n, int head, int out[]) {
    int done[1000] = {0}, pos = head;
    for (int k = 0; k < n; k++) {
        int best = -1, bestdist = 1000000;
        for (int i = 0; i < n; i++) {
            if (!done[i]) {
                int d = abs(requests[i] - pos);
                if (d < bestdist) { bestdist = d; best = i; }
            }
        }
        done[best] = 1;
        out[k] = requests[best];
        pos = requests[best];
    }
}

int main(void) {
    int head = 53, order[N + 2];
    show("FCFS", req, N, head, 0);

    sstf_order(req, N, head, order);
    show("SSTF", order, N, head, 0);

    int s[N], n = 0, i;
    memcpy(s, req, sizeof(req));
    qsort(s, N, sizeof s[0], cmp);

    /* SCAN: first move toward cylinder 0 */
    for (i = N - 1; i >= 0; i--) if (s[i] < head) order[n++] = s[i];
    order[n++] = 0;
    for (i = 0; i < N; i++) if (s[i] > head) order[n++] = s[i];
    show("SCAN", order, n, head, 0);

    /* LOOK: turn at the last request in each direction */
    n = 0;
    for (i = N - 1; i >= 0; i--) if (s[i] < head) order[n++] = s[i];
    for (i = 0; i < N; i++) if (s[i] > head) order[n++] = s[i];
    show("LOOK", order, n, head, 0);

    /* C-SCAN, upward direction; jump from MAXCYL to 0 not counted in printed travel */
    n = 0;
    for (i = 0; i < N; i++) if (s[i] > head) order[n++] = s[i];
    order[n++] = MAXCYL;
    int up = 0, p = head;
    for (int k = 0; k < n; k++) { up += abs(order[k] - p); p = order[k]; }
    int rest[N], m = 0;
    for (i = 0; i < N; i++) if (s[i] < head) rest[m++] = s[i];
    int down = 0; p = 0;
    for (int k = 0; k < m; k++) { down += abs(rest[k] - p); p = rest[k]; }
    printf("C-SCAN %d ... %d, jump to 0 ... %d -> %d cylinders (+%d for the jump)\n",
           head, MAXCYL, rest[m-1], up + down, MAXCYL);

    /* C-LOOK upward, wrapping to lowest request */
    n = 0;
    for (i = 0; i < N; i++) if (s[i] > head) order[n++] = s[i];
    for (i = 0; i < N; i++) if (s[i] < head) order[n++] = s[i];
    show("C-LOOK", order, n, head, 0);
    return 0;
}
