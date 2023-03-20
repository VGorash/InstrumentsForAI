__kernel void test(__global float *a, __global float *b, __global float *c, __global int *sizes)
{
	// получаем текущий id.
	int i = get_global_id(0);
	for(int j=0; j<sizes[2]; j++){
	    float result = 0;
	    for(int k=0; k<sizes[1]; k++){
            result += a[i * sizes[1] + k] * b[k * sizes[2] + j];
        }
        c[i * sizes[2] + j] = result;
	}
}