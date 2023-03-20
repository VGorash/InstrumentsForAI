#include "CL/cl.h"
#include "matrix.h"
#include <stdio.h>
#include <time.h>

#define MAX_SOURCE_SIZE 4096

void flatten_matrix(matrix_t* matrix, cl_float* buffer){
    for(int i=0; i<matrix->n; i++){
        for(int j=0; j<matrix->m; j++){
            float value = matrix->data[i][j];
            buffer[i * matrix->m + j] = value;
        }
    }
}

int main(){
    matrix_t a = init_matrix(5, 8);
    matrix_t b = init_matrix(8, 5);
    fill_matrix(&a);
    fill_matrix(&b);

    clock_t begin;
    clock_t end;

    cl_platform_id platform_id;
    cl_uint ret_num_platforms;
    cl_int ret;
    ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);

    cl_device_id device_id;
    cl_uint ret_num_devices;
    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);

    cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);


    cl_program program = NULL;
    cl_kernel kernel = NULL;

    FILE *fp;
    const char fileName[] = "kernel.cl";
    size_t source_size;
    char *source_str;
    int i;

    fp = fopen(fileName, "r");
    if (!fp) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }
    source_str = (char *)malloc(MAX_SOURCE_SIZE);
    source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose(fp);

/* создать бинарник из кода программы */
    program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);

/* скомпилировать программу */
    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

    begin = clock();

/* создать кернел */
    kernel = clCreateKernel(program, "test", &ret);

    cl_mem memory_buffer_a = NULL;
    cl_mem memory_buffer_b = NULL;
    cl_mem memory_buffer_c = NULL;
    cl_mem memory_sizes = NULL;

    cl_float* linear_matrix_a = (cl_float *) malloc(sizeof(cl_float) * a.n * a.m);
    cl_float* linear_matrix_b = (cl_float *) malloc(sizeof(cl_float) * b.n * b.m);
    cl_float* linear_matrix_c = (cl_float *) malloc(sizeof(cl_float) * a.n * b.m);

    flatten_matrix(&a, linear_matrix_a);
    flatten_matrix(&b, linear_matrix_b);

    cl_int sizes[3] =  {a.n, a.m, b.m};

/* создать буфер */
    memory_buffer_a = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(cl_float) * a.n * a.m, NULL, &ret);
    memory_buffer_b = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(cl_float) * b.n * b.m, NULL, &ret);
    memory_sizes = clCreateBuffer(context, CL_MEM_READ_ONLY, 3 * sizeof(cl_int), NULL, &ret);
    memory_buffer_c = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(cl_float) * a.n * b.m, NULL, &ret);

/* записать данные в буфер */
    ret = clEnqueueWriteBuffer(command_queue, memory_buffer_a, CL_TRUE, 0, sizeof(cl_float) * a.n * a.m, linear_matrix_a, 0, NULL, NULL);
    ret = clEnqueueWriteBuffer(command_queue, memory_buffer_b, CL_TRUE, 0, sizeof(cl_float) * b.n * b.m, linear_matrix_b, 0, NULL, NULL);
    ret = clEnqueueWriteBuffer(command_queue, memory_sizes, CL_TRUE, 0, 3 * sizeof(cl_int), sizes, 0, NULL, NULL);

    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&memory_buffer_a);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&memory_buffer_b);
    ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&memory_buffer_c);
    ret = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&memory_sizes);

    size_t global_work_size[1] = { a.n };
/* выполнить кернел */
    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, global_work_size, NULL, 0, NULL, NULL);

    ret = clEnqueueReadBuffer(command_queue, memory_buffer_c, CL_TRUE, 0, sizeof(cl_float) * a.n * b.m, linear_matrix_c, 0, NULL, NULL);
    matrix_t c = init_matrix(a.n, b.m);

    for(int i=0; i<c.n; i++){
        for(int j=0; j<c.m; j++){
            c.data[i][j] = linear_matrix_c[i * c.m + j];
        }
    }

    end = clock();

    print_matrix(&c);
    printf("Execution time: %f\n", (double) (end - begin));

    delete_matrix(&a);
    delete_matrix(&b);
    delete_matrix(&c);

    free(linear_matrix_a);
    free(linear_matrix_b);
    free(linear_matrix_c);

    return 0;
}