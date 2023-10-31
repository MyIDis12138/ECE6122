/*
Author:  Yang Gu
Date last modified: 31/10/2023
Organization: ECE6122 Class

Description:

This program perfroms CUDA-based 2D Random Walk Simulation to compare 
the performance of three different types of CUDA memory allocation:
1. Standard CUDA memory allocation
2. Pinned (page-locked) memory
3. Managed (unified) memory
    
*/

#include <iostream>
#include <curand_kernel.h>
#include <cmath>
#include <chrono>
#include <cassert>
#include <cstring>
#include <cstdlib>

/**
 * CUDA kernel to simulate random walks.
 * @param x - Pointer to the array storing x-coordinates of walkers.
 * @param y - Pointer to the array storing y-coordinates of walkers.
 * @param num_steps - Number of steps for each walker.
 * @param num_walkers - Total number of walkers.
 */
__global__ void randomWalkKernel(int *x, int *y, int num_steps, int num_walkers) {
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    curandState state;
    curand_init(tid, tid, 0, &state);

    int local_x = 0;
    int local_y = 0;

    for (int i = 0; i < num_steps; ++i) {
        float rnd = curand_uniform(&state);
        if (rnd < 0.25f)
            local_x++;
        else if (rnd < 0.5f)
            local_x--;
        else if (rnd < 0.75f)
            local_y++;
        else
            local_y--;

    }

    x[tid] = local_x;
    y[tid] = local_y;
}

/**
 * Calculate the average distance of walkers from the origin.
 * @param x - Array of x-coordinates.
 * @param y - Array of y-coordinates.
 * @param num_walkers - Number of walkers.
 * @return The average distance.
 */
float calculateAverageDistance(int *x, int *y, const int num_walkers) {
    float total_distance = 0.0;
    for (int i = 0; i < num_walkers; ++i) {
        total_distance += sqrtf(x[i] * x[i] + y[i] * y[i]);
    }
    return total_distance / num_walkers;
}

/**
 * Conducts the random walk and prints out results.
 * @param x - Array of x-coordinates.
 * @param y - Array of y-coordinates.
 * @param num_walkers - Number of walkers.
 * @param num_steps - Number of steps for each walker.
 * @param blocksPerGrid - Number of blocks in the CUDA grid.
 * @param blockSize - Size of each block (number of threads).
 * @param memoryType - Type of CUDA memory used (e.g., "Normal", "Pinned", "Managed").
 */
void performRandomWalkAndReport(int *x, int *y, const int num_walkers, const int num_steps, const int blocksPerGrid, const int blockSize, const char* memoryType) {
    auto start = std::chrono::high_resolution_clock::now();
    randomWalkKernel<<<blocksPerGrid, blockSize>>>(x, y, num_steps, num_walkers);
    cudaDeviceSynchronize();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    float avg_distance = calculateAverageDistance(x, y, num_walkers);
    std::cout << memoryType << " CUDA memory Allocation:" << std::endl;
    std::cout << "    Time to calculate(microsec): " << duration.count() << std::endl;
    std::cout << "    Average distance from origin: " << avg_distance << std::endl;
}


/**
 * Random walk simulation using standard CUDA memory allocation.
 * @param num_walkers - Number of walkers.
 * @param num_steps - Number of steps for each walker.
 * @param blocksPerGrid - Number of blocks in the CUDA grid.
 * @param blockSize - Size of each block (number of threads).
 */
void normalMemoryAllocation(const int &num_walkers, const int &num_steps, const int &blocksPerGrid, const int &blockSize) {
    int *x, *y;
    cudaMalloc(&x, num_walkers * sizeof(int));
    cudaMalloc(&y, num_walkers * sizeof(int));

    auto start = std::chrono::high_resolution_clock::now();
    randomWalkKernel<<<blocksPerGrid, blockSize>>>(x, y, num_steps, num_walkers);
    cudaDeviceSynchronize();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    int *host_x = new int[num_walkers];
    int *host_y = new int[num_walkers];
    cudaMemcpy(host_x, x, num_walkers * sizeof(int), cudaMemcpyDeviceToHost);
    cudaMemcpy(host_y, y, num_walkers * sizeof(int), cudaMemcpyDeviceToHost);

    float avg_distance = calculateAverageDistance(host_x, host_y, num_walkers);
    std::cout << "Normal CUDA memory Allocation:" << std::endl;
    std::cout << "    Time to calculate(microsec): " << duration.count() << std::endl;
    std::cout << "    Average distance from origin: " << avg_distance << std::endl;

    cudaFree(x);
    cudaFree(y);
    delete[] host_x;
    delete[] host_y;
}
/**
 * Random walk simulation using pinned (page-locked) memory.
 * @param num_walkers - Number of walkers.
 * @param num_steps - Number of steps for each walker.
 * @param blocksPerGrid - Number of blocks in the CUDA grid.
 * @param blockSize - Size of each block (number of threads).
 */
void pinnedMemoryAllocation(const int &num_walkers, const int &num_steps, const int &blocksPerGrid, const int &blockSize) {
    int *pinned_x, *pinned_y;
    cudaMallocHost(&pinned_x, num_walkers * sizeof(int));
    cudaMallocHost(&pinned_y, num_walkers * sizeof(int));

    performRandomWalkAndReport(pinned_x, pinned_y, num_walkers, num_steps, blocksPerGrid, blockSize, "Pinned");

    cudaFreeHost(pinned_x);
    cudaFreeHost(pinned_y);
}

/**
 * Random walk simulation using managed (unified) memory.
 * @param num_walkers - Number of walkers.
 * @param num_steps - Number of steps for each walker.
 * @param blocksPerGrid - Number of blocks in the CUDA grid.
 * @param blockSize - Size of each block (number of threads).
 */
void managedMemoryAllocation(const int &num_walkers, const int &num_steps, const int &blocksPerGrid, const int &blockSize) {
    int *managed_x, *managed_y;
    cudaMallocManaged(&managed_x, num_walkers * sizeof(int));
    cudaMallocManaged(&managed_y, num_walkers * sizeof(int));

    performRandomWalkAndReport(managed_x, managed_y, num_walkers, num_steps, blocksPerGrid, blockSize, "Managed");

    cudaFree(managed_x);
    cudaFree(managed_y);
}

int main(int argc, char **argv) {
    int num_walkers = 0;
    int num_steps = 0;

    assert(argc == 5 && "Invalid number of arguments. Usage: Lab4 -W <number of walkers> -I <number of steps>");


    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-W") == 0) {
            num_walkers = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-I") == 0) {
            num_steps = atoi(argv[++i]);
        }
    }

    assert(num_walkers > 0 && "Number of walkers must be greater than 0");
    assert(num_steps > 0 && "Number of steps must be greater than 0");

    std::cout << "Lab4 -W " << num_walkers << " -I " << num_steps << std::endl;

    cudaDeviceProp deviceProp;
    cudaGetDeviceProperties(&deviceProp, 0);
    int blockSize = deviceProp.maxThreadsPerBlock / 4;
    int blocksPerGrid = (num_walkers + blockSize - 1) / blockSize;

    normalMemoryAllocation(num_walkers, num_steps, blocksPerGrid, blockSize);

    pinnedMemoryAllocation(num_walkers, num_steps, blocksPerGrid, blockSize);

    managedMemoryAllocation(num_walkers, num_steps, blocksPerGrid, blockSize);

    std::cout << "Bye" << std::endl;
    return 0;
}
