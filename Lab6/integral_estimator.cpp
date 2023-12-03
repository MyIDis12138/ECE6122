#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <random>

// Function to estimate the integral of x^2
double monte_carlo_integral_x_squared(int local_samples) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1);
    double local_sum = 0.0;

    for (int i = 0; i < local_samples; ++i) {
        double x = dis(gen);
        local_sum += x * x;
    }

    return local_sum;
}

// Function to estimate the integral of e^(-x^2)
double monte_carlo_integral_exp(int local_samples) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1);
    double local_sum = 0.0;

    for (int i = 0; i < local_samples; ++i) {
        double x = dis(gen);
        local_sum += exp(-x * x);
    }

    return local_sum;
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size, P, N;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Parse command line arguments
    if (argc != 5) {
        if (rank == 0) {
            std::cerr << "Usage: " << argv[0] << " -P <1 or 2> -N <number of samples>" << std::endl;
        }
        MPI_Abort(MPI_COMM_WORLD, 1);
        return 1;
    }

    P = std::atoi(argv[2]);
    N = std::atoi(argv[4]);
    int local_samples = N / size;
    double local_estimate, global_estimate;

    // Calculate local estimate based on which integral to compute
    if (P == 1) {
        local_estimate = monte_carlo_integral_x_squared(local_samples);
    } else {
        local_estimate = monte_carlo_integral_exp(local_samples);
    }

    // Combine the results from all processes
    MPI_Reduce(&local_estimate, &global_estimate, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    // Root process computes the average of results and prints the final estimate
    if (rank == 0) {
        global_estimate /= (N * 1.0); // Divide by total number of samples for average
        std::cout << "The estimate for integral " << P << " is " << global_estimate << std::endl;
    }

    MPI_Finalize();
    return 0;
}
