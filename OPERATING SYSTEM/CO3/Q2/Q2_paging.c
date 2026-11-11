#include <stdio.h>

#define PAGE_SIZE 2048
#define PAGES 8

int page_table[PAGES] = {5, 2, -1, 7, 0, -1, 3, -1};

int main(void) {
    unsigned logical[] = {0, 1500, 3100, 6200, 9000, 2049};

    for (int i = 0; i < 6; i++) {
        unsigned a = logical[i];
        unsigned p = a / PAGE_SIZE;
        unsigned d = a % PAGE_SIZE;

        if (p >= PAGES) {
            printf("%u -> page %u offset %u : invalid address\n",
                   a, p, d);
        } else if (page_table[p] == -1) {
            printf("%u -> page %u offset %u : page fault\n",
                   a, p, d);
        } else {
            unsigned frame = page_table[p];
            printf("%u -> page %u offset %u : physical %u\n",
                   a, p, d, frame * PAGE_SIZE + d);
        }
    }
    return 0;
}
