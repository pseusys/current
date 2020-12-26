#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "matrix.h"

matrix* create_matrix (int d) {
    matrix* m = malloc(sizeof(matrix));
    m->dimension = d;
    m->elements = calloc(d * d, sizeof(int));
    return m;
}

matrix* input_matrix () {
    char* line, * end;
    size_t len = 0;

    printf("Введите размерность матрицы: ");
    getline(&line, &len, stdin);
    int dimension = (int) strtol(line, &end, 10);
    matrix* m = create_matrix(dimension);

    printf("Введите строки матрицы, разделяя элементы пробелами!\n");
    for (int i = 0; i < m->dimension; ++i) {
        printf("Введите строку №%d: ", i + 1);
        getline(&line, &len, stdin);

        char* str = strtok(line, " ");
        int j = 0;
        while (str != NULL) {
            m->elements[i * m->dimension + j] = (int) strtol(str, &end, 10);
            str = strtok(NULL, " ");
            j++;
        }

        if (j != m->dimension) {
            printf("Повторите ввод! ");
            i--;
        }
    }

    return m;
}



void print_matrix(matrix* m) {
    printf("Матрица %dx%d элементов:\n", m->dimension, m->dimension);
    for (int i = 0; i < m->dimension; ++i) {
        printf("[");
        for (int j = 0; j < m->dimension; ++j) {
            printf(" %d", m->elements[i * m->dimension + j]);
            if (j < m->dimension - 1) printf(",");
        }
        printf(" ]\n");
    }
}



void remove_matrix (matrix* m) {
    if (m == NULL) return;
    free(m->elements);
    free(m);
}
