#include "matrix.h"

#include <malloc.h>
#include <stdio.h>
#include <math.h>

// создает в пямяти новый объект матрицы
matrix_t init_matrix(int n, int m){
    float** data = (float**) malloc(sizeof(float*) * n);
    for(int i=0; i<n; i++){
        float* row = (float*) malloc(sizeof(float) * m);
        data[i] = row;
    }
    matrix_t result;
    result.data = data;
    result.n = n;
    result.m = m;
    return result;
}

// очищает память
// мы везде передаем указатели, чтобы не копировать объект
void delete_matrix(matrix_t* matrix){
    for(int i=0; i<matrix->n; i++){
        free(matrix->data[i]);
    }
    free(matrix->data);
}

// заполняет матрицу числами
void fill_matrix(matrix_t* matrix){
    for (int i=0; i<matrix->n; i++){
        for (int j=0; j<matrix->m; j++){
            matrix->data[i][j] = (float) sin((i * matrix->n + j + 1));
        }
    }
}

// заполняет матрицу нулями
void fill_matrix_zeros(matrix_t* matrix){
    for (int i=0; i<matrix->n; i++){
        for (int j=0; j<matrix->m; j++){
            matrix->data[i][j] = 0.f;
        }
    }
}

// выводит матрицу в консоль
// указатель константный, так как матрицу мы не изменяем
void print_matrix(const matrix_t* matrix){
    for(int i=0; i<matrix->n; i++){
        for(int j=0; j<matrix->m; j++){
            printf("%.2f ", matrix->data[i][j]);
        }
        printf("\n");
    }
}

