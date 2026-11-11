
# Create seg.c
cat > seg.c <<'EOF'
#include <stdio.h>

typedef struct {
    unsigned base, limit;
} Seg;

int main(void) {
    Seg st[4] = {
        {1000, 400},
        {3000, 200},
        {5000, 600},
        {8000, 100}
    };

    struct {
        int seg;
        unsigned off;
    } req[] = {
        {0, 0}, {0, 399}, {0, 400},
        {1, 100}, {2, 599}, {2, 600},
        {3, 50}, {4, 10}
    };

    for (int i = 0; i < 8; i++) {
        int s = req[i].seg;
        unsigned off = req[i].off;

        if (s < 0 || s >= 4) {
            printf("seg %d off %u : INVALID SEGMENT\n", s, off);
            continue;
        }

        if (off >= st[s].limit) {
            printf("seg %d off %u : SEGMENTATION FAULT (limit %u)\n",
                   s, off, st[s].limit);
        } else {
            printf("seg %d off %u : physical %u\n",
                   s, off, st[s].base + off);
        }
    }

    return 0;
}
EOF

echo "===== Q3: SEGMENTATION ====="
gcc -Wall seg.c -o seg
./seg

echo "===== Q3: MEMORY MAPS ====="
cat /proc/self/maps

# Create segfault.c
cat > segfault.c <<'EOF'
#include <stdio.h>

int main(void) {
    int *p = NULL;
    *p = 42;
    return 0;
}
EOF

echo "===== Q3: SEGMENTATION FAULT TEST ====="
gcc -std=c11 -Wall segfault.c -o segfault
./segfault
echo "exit status = $?"

echo "===== Q3: SYSTEM ERROR LOG ====="
dmesg | tail -n 3

# Create fragmentation.c
cat > fragmentation.c <<'EOF'
#include <stdio.h>
#include <stdlib.h>

#define MEM 100000
#define N 20

int main(void) {
    static int mem[MEM];
    int sizes[N], start[N], used = 0;

    srand(7);

    for (int i = 0; i < N; i++) {
        sizes[i] = 100 + rand() % 5000;
        int s = -1;

        for (int j = 0; j + sizes[i] <= MEM; j++) {
            int ok = 1;

            for (int k = j; k < j + sizes[i]; k++) {
                if (mem[k]) {
                    ok = 0;
                    j = k;
                    break;
                }
            }

            if (ok) {
                s = j;
                break;
            }
        }

        start[i] = s;

        if (s < 0) {
            printf("seg %d size %d : NO FIT\n", i, sizes[i]);
            continue;
        }

        for (int k = s; k < s + sizes[i]; k++)
            mem[k] = 1;

        used += sizes[i];
    }

    int holes = 0, holed = 0;

    for (int i = 0; i < MEM;) {
        if (!mem[i]) {
            int j = i;
            while (j < MEM && !mem[j])
                j++;

            if (j - i >= 100) {
                holes++;
                holed += j - i;
            }

            i = j;
        } else {
            i++;
        }
    }

    printf("used = %d, holes>=100 = %d, free in holes = %d\n",
           used, holes, holed);

    return 0;
}
EOF

echo "===== Q3: FRAGMENTATION EXPERIMENT ====="
gcc -Wall fragmentation.c -o fragmentation
./fragmentation

echo "===== Q3 TESTS COMPLETED ====="
