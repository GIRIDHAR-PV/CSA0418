#include <stdio.h>
#include <string.h>

#define BLOCKS 32
char disk[BLOCKS]; /* '.' = free, otherwise file letter */

int alloc_contiguous(char file, int size) {
    int run = 0, start = -1;
    for (int b = 0; b < BLOCKS; b++) {
        if (disk[b] == '.') {
            if (run == 0) start = b;
            run++;
            if (run == size) {
                for (int k = start; k < start + size; k++) disk[k] = file;
                return start;
            }
        } else {
            run = 0;
            start = -1;
        }
    }
    return -1;
}

int alloc_indexed(char file, int size, int index[]) {
    int got = 0, idx = -1;
    for (int b = 0; b < BLOCKS && got <= size; b++) {
        if (disk[b] != '.') continue;
        if (idx < 0) idx = b; /* index block */
        else index[got++] = b;
        if (got == size) break;
    }
    if (idx < 0 || got < size) return -1;
    disk[idx] = (char)(file + 32);
    for (int i = 0; i < size; i++) disk[index[i]] = file;
    return idx;
}

void release(char file) {
    for (int b = 0; b < BLOCKS; b++)
        if (disk[b] == file) disk[b] = '.';
}

int main(void) {
    memset(disk, '.', BLOCKS);
    alloc_contiguous('A', 6);
    alloc_contiguous('B', 8);
    alloc_contiguous('C', 5);
    alloc_contiguous('D', 7);
    release('B');
    release('D');
    printf("%.32s\n", disk);
    printf("E (10 blocks) contiguous: start %d\n", alloc_contiguous('E', 10));
    int index[16];
    int ib = alloc_indexed('E', 10, index);
    printf("E (10 blocks) indexed: index block %d, data blocks", ib);
    for (int i = 0; i < 10; i++) printf(" %d", index[i]);
    printf("\n%.32s\n", disk);
    return 0;
}
