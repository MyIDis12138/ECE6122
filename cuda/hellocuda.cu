#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cuda.h>
#include <cuda_runtime.h>
#include <assert.h>

#define N 10000000

#define MAX_ERR 1e-6

void vector_add(float *out, float *a, float *b, int n) {
    for(int i = 0; i < n; i++){
        out[i] = a[i] + b[i];
    }
}

__global__ void vector_CUDAadd(float *out, float *a, float *b, int n)
{ 
//    int index = threadIdx.x;
//    int stride = blockDim.x;
    int tid = threadIdx.x * blockDim.x + blockDim.x;

    if (tid < N){
        out[tid] = a[tid] + b[tid];
    }
//    for (int i = 0; i < n; i += stride){
//        out[i] = a[i] + b[i];
//    }
}

int main(){
    float *a, *b, *out; 
    float *d_a, *d_b, *d_out;
    float size = N * sizeof(float);

    // Allocate memory
    a   = (float*)malloc(size);
    b   = (float*)malloc(size);
    out = (float*)malloc(size);

    // Initialize array
    for(int i = 0; i < N; i++){
        a[i] = 1.0f; b[i] = 2.0f;
    }

    // Allocate device memory
    cudaMalloc((void **)&d_a, size);
    cudaMalloc((void **)&d_b, size);
    cudaMalloc((void **)&d_out, size);

    cudaMemcpy(d_a, a, size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_b, b, size, cudaMemcpyHostToDevice);

    int block_size = 256;
    int grid_size = ((N + block_size) / block_size);

    vector_CUDAadd<<<grid_size, block_size>>>(d_out, d_a, d_b, N);

//    cudaMemcpy(out, d_out, size, cudaMemcpyDeviceToHost);

//    for(int i = 0; i < N; i++){
//        assert(fabs(out[i] - a[i] - b[i]) < MAX_ERR);
//    }

    free(a); free(b); free(out);
    cudaFree(d_a); cudaFree(d_b); cudaFree(d_out);

}

