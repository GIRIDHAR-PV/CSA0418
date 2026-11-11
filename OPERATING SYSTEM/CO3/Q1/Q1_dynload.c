#include <stdio.h>
#include <dlfcn.h>

int main(void) {
    void *h = dlopen("./libmylib.so", RTLD_LAZY);

    if (!h) {
        printf("Library missing: %s\n", dlerror());
        return 1;
    }

    int (*fn)(int, int) = (int (*)(int, int))dlsym(h, "add");

    if (!fn) {
        printf("Symbol error: %s\n", dlerror());
        dlclose(h);
        return 1;
    }

    printf("Runtime result: %d\n", fn(10, 20));
    dlclose(h);
    return 0;
}
