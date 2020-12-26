#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define boolean int
#define true 1
#define false 0

#define gen_matrix true
#define out_matrix false

typedef struct {
    int dimension;
    int* elements;
} matrix;



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

matrix* generate_matrix (char name) {
    char* line, * end;
    size_t len = 0;

    printf("Введите размерность матрицы %c: ", name);
    getline(&line, &len, stdin);
    int dimension = (int) strtol(line, &end, 10);
    matrix* m = create_matrix(dimension);

    for (int i = 0; i < m->dimension; ++i)
        for (int j = 0; j < m->dimension; ++j)
            m->elements[i * m->dimension + j] = ((int) rand()) % 10;

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



int proc_coords [2];
int proc_num, proc_rank;
int block_size, grid_size, matrix_dim;

matrix* A = NULL, * B = NULL, * C = NULL, * A_part = NULL, * B_part = NULL, * C_part = NULL, * temp_part = NULL;

MPI_Comm grid_comm, col_comm, row_comm;
MPI_Datatype MPI_BLOCK;

MPI_Status status;
MPI_Request request;


boolean is_root () {
    return proc_rank == 0;
}

void solve_parallel() {
    if (is_root()) printf("Исходя из количества процессов выбран параллельный способ решения\n");

    grid_size = (int) sqrt(proc_num);
    if (proc_num != grid_size * grid_size) {
        if (is_root()) printf("Невозможно построить сеть для такого количества процессов!\n");
        return;
    }

    double start_time = MPI_Wtime();

    MPI_Cart_create(MPI_COMM_WORLD, 2, (int[]) {grid_size, grid_size},
                    (boolean[]) {true, false}, false, &grid_comm);
    MPI_Cart_coords(grid_comm, proc_rank, 2, proc_coords);
    MPI_Cart_sub(grid_comm, (int[]) {true, false}, &col_comm);
    MPI_Cart_sub(grid_comm, (int[]) {false, true}, &row_comm);

    MPI_Bcast(&matrix_dim, 1, MPI_INT, 0, MPI_COMM_WORLD);
    block_size = matrix_dim / grid_size;
    A_part = create_matrix(block_size);
    B_part = create_matrix(block_size);
    C_part = create_matrix(block_size);
    temp_part= create_matrix(block_size);

    MPI_Type_vector(block_size, block_size, matrix_dim, MPI_INT, &MPI_BLOCK);
    MPI_Type_commit(&MPI_BLOCK);

    if (is_root()) for (int i = 0; i < proc_num; ++i) {
            int coords [2];
            MPI_Cart_coords(grid_comm, i, 2, coords);
            MPI_Isend(A->elements + coords[0] * matrix_dim * block_size + coords[1] * block_size,
                      1, MPI_BLOCK, i, 0, grid_comm, &request);
            MPI_Isend(B->elements + coords[0] * matrix_dim * block_size + coords[1] * block_size,
                      1, MPI_BLOCK, i, 0, grid_comm, &request);
        }

    MPI_Recv(A_part->elements, block_size * block_size, MPI_INT, 0, 0, grid_comm, &status);
    MPI_Recv(B_part->elements, block_size * block_size, MPI_INT, 0, 0, grid_comm, &status);

    for (int i = 0; i < grid_size; ++i) {
        int p = (proc_coords[0] + i) % grid_size;
        if (proc_coords[1] == p) for (int j = 0; j < block_size * block_size; j++)
                temp_part->elements[j] = A_part->elements[j];
        MPI_Bcast(temp_part->elements, block_size * block_size, MPI_INT, p, row_comm);

        for (int j = 0; j < block_size; j++) for (int k = 0; k<block_size; k++) {
                int t = 0;
                for (int l = 0; l < block_size; l++)
                    t += temp_part->elements[j * block_size + l] * B_part->elements[l * block_size + k];
                C_part->elements[j * block_size + k] += t;
            }

        int giver, getter;
        MPI_Cart_shift(col_comm, 0, 1, &giver, &getter);
        MPI_Sendrecv_replace(B_part->elements, block_size * block_size, MPI_INT, getter, proc_rank,
                             giver, MPI_ANY_TAG, col_comm, &status);
    }

    if (!is_root()) MPI_Send(C_part->elements, block_size * block_size, MPI_INT, 0, 0, grid_comm);
    else {
        MPI_Isend(C_part->elements, block_size * block_size, MPI_INT, 0, 0, grid_comm, &request);
        for (int i = 0; i < proc_num; ++i) {
            int coords[2];
            MPI_Cart_coords(grid_comm, i, 2, coords);
            MPI_Recv(C->elements + coords[0] * matrix_dim * block_size + coords[1] * block_size,
                     1, MPI_BLOCK, i, 0, grid_comm, &status);
        }
        double end_time = MPI_Wtime();
        printf("Времени прошло: %lf\n", end_time - start_time);
        if (out_matrix) {
            printf("Матрица C:\n");
            print_matrix(C);
        }
    }

    remove_matrix(A_part);
    remove_matrix(B_part);
    remove_matrix(C_part);
    remove_matrix(temp_part);
}

void solve_stepped() {
    printf("Исходя из количества процессов выбран последовательный способ решения\n");
    double start_time = MPI_Wtime();
    for (int i = 0; i < C->dimension; i++) for (int j = 0; j < C->dimension; j++) for (int k = 0; k < C->dimension; k++)
        C->elements[i * C->dimension + j] += A->elements[i * A->dimension + k] * B->elements[k * B->dimension + j];
    double end_time = MPI_Wtime();
    printf("Времени прошло: %lf\n", end_time - start_time);
    if (out_matrix) {
        printf("Матрица C:\n");
        print_matrix(C);
    }
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &proc_num);
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);

    if (is_root()) {
        if (gen_matrix) {
            srand(time(NULL));
            A = generate_matrix('A');
            B = generate_matrix('B');
        } else {
            printf("Введите информацию о матрице А:\n");
            A = input_matrix();
            printf("Введите информацию о матрице B:\n");
            B = input_matrix();
        }

        if (A->dimension != B->dimension) {
            printf("Размерности матриц не совпадают!\n");
            return 0;
        } else matrix_dim = A->dimension;

        if (out_matrix) {
            printf("Матрица А:\n");
            print_matrix(A);
            printf("Матрица B:\n");
            print_matrix(B);
        }

        C = create_matrix(matrix_dim);
    }

    if (proc_num > 1) solve_parallel();
    else solve_stepped();

    remove_matrix(A);
    remove_matrix(B);
    remove_matrix(C);

    MPI_Finalize();
    return 0;
}

// 1500
// 1: 46.355093
// 4: 12.930102
// 9: 11.316381
// 16: 10.086656
// 25: 10.680353
// 36: 11.173264
// 49: 13.648021
// 64: 14.931917
// 81: 19.988985
// 100: 26.735850
