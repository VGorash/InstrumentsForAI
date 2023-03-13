#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <malloc.h>

#include "matrix.h"

// элементарная операция по умножению одной строки и одного столбца. Возвращает число
float calculate_element(const matrix_t* a, const matrix_t* b, int i, int j){
    float result = 0;
    for (int k=0; k<a->m; k++){
        result += a->data[i][k] * b->data[k][j];
    }
   return result;
}

// умножает две матрицы, возвращает новую
// указатель константный, так как исходные матрицы мы не изменяем
matrix_t multiply_matrices_single_thread(const matrix_t* a, const matrix_t* b){
    if(a->m != b->n){
        printf("Matrix shapes are inconsistent!");
        exit(-1);
    }
    matrix_t c = init_matrix(a->n, b->m);
    fill_matrix_zeros(&c);
    for (int i=0; i<c.n; i++){
        for (int j=0; j<c.m; j++){
            c.data[i][j] = calculate_element(a, b, i, j);
        }
    }
    return c;
}

// начинаем работать с несколькими потоками
// инкапсулируем одну элементарную операцию
typedef struct task_t{
    const matrix_t* a; // откуда брать строку
    const matrix_t* b; // откуда брать столбец
    matrix_t* c; // куда записывать результат
    int i; // номер строки
} task_t;

// не нашел на C нормальной встроенной реализации очереди. Поэтому делаем свою
typedef struct queue_t{
    task_t* data; // массив всех элементов
    int elements_count; // количество элементов;
    int current_element; // номер элемента, который сейчас первый в очереди
    pthread_mutex_t* mutex; // чтобы потоки не модифицировали очередь одновременно
} queue_t;

// функция, которую исполняют потоки
void* worker(void* params){
    queue_t* queue = (queue_t*) params; // получаем указатель на очередь
    while(1){ // берем элементы из очереди, пока они там есть
        pthread_mutex_lock(queue->mutex); // ждем доступа к очереди
        if(queue->current_element == queue->elements_count){ // если в очереди все забрали, завершаемся
            // не забываем при выходе освободить мьютекс
            pthread_mutex_unlock(queue->mutex);
            break;
        }
        task_t task = queue->data[queue->current_element];
        queue->current_element++; // показали, что этот элемент мы забрали
        pthread_mutex_unlock(queue->mutex); // очередь нам больше не нужна, освобождаем к ней доступ

        // выполняем перемножение
        // мьютекс здесь не нужен, так как мы записываем разные элементы матрицы, а исходные матрицы не меняются
        for(int j=0; j<task.c->m; j++){
            task.c->data[task.i][j] = calculate_element(task.a, task.b, task.i, j);
        }
    }
    return NULL;
}

// итоговая функция многопоточного перемножения
matrix_t multiply_matrices_multiple_threads(const matrix_t* a, const matrix_t* b, int threads_count){
    if(a->m != b->n){
        printf("Matrix shapes are inconsistent!");
        exit(-1);
    }

    queue_t queue;
    // количество элементарных операций
    queue.elements_count = a->n;
    // выделяем память под очередь
    queue.data = (task_t*) malloc(sizeof(task_t) * queue.elements_count);
    queue.current_element = 0;

    // инициализируем мьютекс
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    queue.mutex = &mutex;

    // инициализируем матрицу результата
    matrix_t c = init_matrix(a->n, b->m);
    fill_matrix_zeros(&c);

    for (int i=0; i<c.n; i++){
        // кладем в очередь новую задачу
        queue.data[i].a = a;
        queue.data[i].b = b;
        queue.data[i].c = &c;
        queue.data[i].i = i;
    }

    // создаем пул потоков
    pthread_t threads[threads_count];
    for(int i=0; i<threads_count; i++){
        pthread_create(&threads[i], NULL, worker, (void*) &queue);
    }
    for (int i=0; i<threads_count; i++){
        pthread_join(threads[i], NULL);
    }

    // освобождаем память
    free(queue.data);

    return c;
}

int main(){
    // init matrices
    matrix_t a = init_matrix(5, 8);
    matrix_t b = init_matrix(8, 5);
    fill_matrix(&a);
    fill_matrix(&b);

    clock_t begin;
    clock_t end;

    // умножаем одним потоком
    printf("Single thread\n");
    begin = clock();
    matrix_t c = multiply_matrices_single_thread(&a, &b);
    end = clock();
    print_matrix(&c);
    printf("Execution time: %f\n", (double) (end - begin));

    // умножаем несколькими потоками (поставил 20 по количеству своих ядер)
    int num_threads = 20;
    printf("%i threads\n", num_threads);
    begin = clock();
    matrix_t d = multiply_matrices_multiple_threads(&a, &b, num_threads);
    end = clock();
    print_matrix(&d);
    printf("Execution time: %f\n", (double) (end - begin));

    // освобождаем память
    delete_matrix(&a);
    delete_matrix(&b);
    delete_matrix(&c);
    delete_matrix(&d);
}