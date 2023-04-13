#ifndef GENERAL_H
#define GENERAL_H

typedef void (*evaluator) (int, int, double*, double*, double*, int, int);

typedef void (*on_start) (int, int);
typedef void (*on_end) (int, int);
typedef int (*read_and_assert_matrix_size) (int, char*[], int);
typedef void (*allocate_matrixes) (int, int, int, double**, double**, double**);
typedef void (*initialize_matrixes) (int, int, int, double*, double*, double*);
typedef void (*print_matrixes_error) (int, double*, double*, double*, double*);

typedef struct {
    evaluator eval;
    read_and_assert_matrix_size rams;
    allocate_matrixes am;
    initialize_matrixes im;
    print_matrixes_error pme;
    on_start start;
    on_end end;
} eval_config;

void matrix_multiplication_default(int width, int height, double *A, double *B, double *C);
int read_and_assert_matrix_size_default(int argc, char* argv[], int processes);
void allocate_matrixes_default(int my_rank, int mat_size, int share, double **A, double **B, double **C);
void initialize_matrixes_default(int my_rank, int mat_size, int share, double *A, double *B, double *C);
void print_matrixes_error_default(int mat_size, double *A, double *B, double *C, double *C_check);

int eval(int argc, char* argv[], char* eval_name, eval_config* config);

#endif // GENERAL_H
