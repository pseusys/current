# OS and concurrent programming

## Lab 1

### Question one

1. Function arguments in general are allocated in stack, but since there are only three of them they might be passed in processor registers.
2. After the end of function -> end of scope of their visibility.
3. It is allocated on stack as well.

### Question two

1. A pointer to some allocated heap memory.
2. In heap.
3. We take values from stack at positions `v1 + i` and `v2 + 1`, calculate their sum and write it to heap at position `r + i`.
4. In heap.
5. The code will be:

```c

void vect_sum(int* v1, int* v2, int* result, int size) {
    for (int i = 0; i < size; i++) {
        result[i] = v1[i] + v2[i];
    }
}

int main() {
    int v1[] = {1, 2, 4, 7};
    int v2[] = {3, 4, 9, 2};
    int result[4];

    vect_sum(v1, v2, result, 4);
    print_vect(result, size);
    exit(0); 
}

```

6. Variable allocated in stack is only present in its' scope (function, cycle, etc.). Heap variable has no limit in lifetime and is present untill `free` is called or program finishes.

### Question three

1. Line #1: we assign pointer variable to integer value. Line #6: we output pointer as integer. Line #12: we assign a negative value to pointer.

### Question four

1. Because of the pointer type, `long int` takes 64 bits in memory.
2. The code will be `((void*) (p1 + 1)) - ((void*) p1)`.
3. Yes, it is. Result is 80 bytes.
4. We conclude that `char*`, `*uint8_t` and `void*` are 8 bits long (as expected).
5. Apparently compiler doesn't want us to use pointer arithmetics with `void*`.
6. Arrays are positioned directly one after another in memory, so the result is number of elements in the second array.

### Question five

1. Compiler (`gcc`) will be run -> linter (also `gcc`) will be run.
2. Same, but `$@` is reference to current target name and `$<` is reference to first dependency target.
3. Same, but `%` is a wildcard for filename.
4. Same, but `ALL` is a special target that depends on array of all executables.
5. `.PHONY` pseudotarget makes execution of targets independent from existance of corresponding file.

### MISC

> Russian bad word: **сука**
