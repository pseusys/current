#include <stdlib.h>

int main(void) {
    printf("%e\n%e\n%Le\n", 0.1f + 0.2f - 0.3f, 0.1 + 0.2 - 0.3, 0.1L + 0.2L - 0.3L);
    return EXIT_SUCCESS;
}
