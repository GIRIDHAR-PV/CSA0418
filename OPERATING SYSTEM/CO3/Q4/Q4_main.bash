
# ==========================================
# Q4 - VIRTUAL MEMORY AND DEMAND PAGING
# ==========================================

# Create faults.c
cat > faults.c <<'EOF'
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <time.h>

static long page_faults(void) {
    struct rusage u;
    getrusage(RUSAGE_SELF, &u);
    return u.ru_minflt + u.ru_majflt;
}

int main(int argc, char **argv) {
    size_t mb = (argc > 1) ? (size_t)atol(argv[1]) : 64;
    int random_mode = (argc > 2) ? atoi(argv[2]) : 0;

    size_t n = mb * 1024 * 1024;
    char *p = malloc(n);

    if (!p) {
        perror("malloc");
        return 1;
    }

    long f0 = page_faults();
    clock_t t0 = clock();

    if (random_mode) {
        srand(1);
        for (size_t i = 0; i < n; i += 4096) {
            size_t off = (size_t)(rand() % (n / 4096)) * 4096;
            p[off] = (char)i;
        }
    } else {
        for (size_t i = 0; i < n; i += 4096)
            p[i] = (char)i;
    }

    clock_t t1 = clock();
    long f1 = page_faults();

    printf("%s %zu MB: page faults = %ld, time = %.3f s\n",
           random_mode ? "RANDOM" : "SEQ",
           mb, f1 - f0,
           (double)(t1 - t0) / CLOCKS_PER_SEC);

    free(p);
    return 0;
}
EOF

echo "===== SEQUENTIAL VS RANDOM ACCESS ====="
gcc -O2 -std=c11 -Wall faults.c -o faults

for m in 16 64 128
do
    ./faults $m 0
    ./faults $m 1
done

# ==========================================
# COPY-ON-WRITE PROGRAM
# ==========================================

cat > cow.c <<'EOF'
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>

static long faults(void) {
    struct rusage u;
    getrusage(RUSAGE_SELF, &u);
    return u.ru_minflt + u.ru_majflt;
}

int main(void) {
    size_t n = 64u * 1024 * 1024;
    char *p = malloc(n);

    if (!p) {
        perror("malloc");
        return 1;
    }

    for (size_t i = 0; i < n; i += 4096)
        p[i] = 1;

    pid_t c = fork();

    if (c < 0) {
        perror("fork");
        free(p);
        return 1;
    }

    if (c == 0) {
        long f0 = faults();

        for (size_t i = 0; i < n; i += 4096)
            p[i] = 2;

        printf("CHILD faults after writing = %ld\n",
               faults() - f0);
        fflush(stdout);
        free(p);
        _exit(0);
    }

    wait(NULL);
    printf("PARENT faults after child exited = %ld\n", faults());

    free(p);
    return 0;
}
EOF

echo "===== COPY-ON-WRITE TEST ====="
gcc -O2 -std=c11 -Wall cow.c -o cow
./cow

# ==========================================
# PAGE FAULTS VS NUMBER OF FRAMES
# ==========================================

cat > frames.c <<'EOF'
#include <stdio.h>

int frames_of(int ref[], int len, int nf) {
    int f[32], used = 0, faults = 0;

    for (int i = 0; i < len; i++) {
        int hit = 0;

        for (int j = 0; j < used; j++) {
            if (f[j] == ref[i]) {
                hit = 1;
                break;
            }
        }

        if (hit)
            continue;

        faults++;

        if (used < nf) {
            f[used++] = ref[i];
        } else {
            int next = 0, far = -1;

            for (int j = 0; j < nf; j++) {
                int t;

                for (t = i + 1; t < len; t++) {
                    if (ref[t] == f[j])
                        break;
                }

                if (t > far) {
                    far = t;
                    next = j;
                }
            }

            f[next] = ref[i];
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

    printf("===== PAGE FAULTS VS FRAMES =====\n");

    for (int nf = 1; nf <= 10; nf++) {
        printf("frames=%2d faults=%d\n",
               nf, frames_of(ref, len, nf));
    }

    return 0;
}
EOF

echo "===== PAGE FAULTS VS FRAMES ====="
gcc -O2 -std=c11 -Wall frames.c -o frames
./frames

echo "===== Q4 TESTS COMPLETED ====="
