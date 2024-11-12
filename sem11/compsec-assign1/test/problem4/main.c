#include <stdio.h>
#include <dlfcn.h>

typedef void (*operation)(int, int);


int main() {
    void *handle;
    operation add, subtract;
    char *error;

    handle = dlopen("./libmathlib.so", RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        return 1;
    }

    dlerror();

    add = (operation) dlsym(handle, "add");
    if ((error = dlerror()) != NULL) {
        fprintf(stderr, "%s\n", error);
        return 1;
    }

    subtract = (operation) dlsym(handle, "subtract");
    if ((error = dlerror()) != NULL) {
        fprintf(stderr, "%s\n", error);
        return 1;
    }

    add(5, 3);
    subtract(5, 3);

    dlclose(handle);
    return 0;
}
