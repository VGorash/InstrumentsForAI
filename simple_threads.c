#include <stdio.h>
#include <time.h>
#include <pthread.h>

#define size_n 5
#define size_m 5

int matrix_a[size_n][size_m];
int matrix_b[size_n][size_m];
int matrix_c[size_n][size_m] = {0};
int matrix_d[size_n][size_m] = {0};

int i_global = 0;

void fill_matrix(int matrix[size_n][size_m]){
    for (int i=0; i<size_n; i++){
        for (int j=0; j<size_m; j++){
            matrix[i][j] = i * size_n + j + 1;
        }
    }
}

void print_matrix(int matrix[size_n][size_m]){
    for(int i=0; i<size_n; i++){
        for(int j=0; j<size_n; j++){
            printf("%i ", matrix[i][j]);
        }
        printf("\n");
    }
}

void* multiply_row(void* params){
    int i = i_global++;
    for (int j=0; j<size_m; j++){
        for (int k=0; k<size_m; k++){
            matrix_c[i][j] += matrix_a[i][k] * matrix_b[k][j];
        }
    }
    return NULL;
}

int main(){
    pthread_t threads[size_n];
    fill_matrix(matrix_a);
    fill_matrix(matrix_b);
    clock_t begin = clock();
    for (int i=0; i<size_n; i++){
        pthread_create(&threads[i], NULL, multiply_row, NULL);
    }
    for (int i=0; i<size_n; i++){
        pthread_join(threads[i], NULL);
    }
    clock_t end = clock();
    double exec_time = (double) (end - begin);
    // print_matrix(matrix_c);
    printf("Execution time: %f\n", exec_time);

    begin = clock();
    for (int i=0; i<size_n; i++){
        for (int j=0; j<size_m; j++){
            for (int k=0; k<size_m; k++){
                matrix_d[i][j] += matrix_a[i][k] * matrix_b[k][j];
            }
        }
    }
    end = clock();

    // print_matrix(matrix_d);
    exec_time = (double) (end - begin);
    printf("Execution time: %f\n", exec_time);
}



