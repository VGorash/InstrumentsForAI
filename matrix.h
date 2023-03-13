#ifndef ISSII_MATRIX_H
#define ISSII_MATRIX_H

typedef struct matrix_t {
    float** data;
    int n;
    int m;
} matrix_t;

// создает в пямяти новый объект матрицы
matrix_t init_matrix(int n, int m);

// очищает память
// мы везде передаем указатели, чтобы не копировать объект
void delete_matrix(matrix_t* matrix);

// заполняет матрицу числами
void fill_matrix(matrix_t* matrix);

// заполняет матрицу нулями
void fill_matrix_zeros(matrix_t* matrix);

// выводит матрицу в консоль
// указатель константный, так как матрицу мы не изменяем
void print_matrix(const matrix_t* matrix);


#endif //ISSII_MATRIX_H
