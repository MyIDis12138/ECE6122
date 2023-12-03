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

    // Root process checks command line arguments and broadcasts to all processes
    if (rank == 0) {
        if (argc != 5) {
            std::cerr << "Usage: " << argv[0] << " -P <1 or 2> -N <number of samples>" << std::endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
            return 1;
        }
        P = std::atoi(argv[2]);
        N = std::atoi(argv[4]);
    }
    // Broadcast P and N to all processes
    MPI_Bcast(&P, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int local_samples = N / size;
    double local_estimate;
    std::vector<double> global_estimates(size);

    // Calculate local estimate based on which integral to compute
    if (P == 1) {
        local_estimate = monte_carlo_integral_x_squared(local_samples);
    } else {
        local_estimate = monte_carlo_integral_exp(local_samples);
    }

    // Gather the results from all processes to the root process
    MPI_Gather(&local_estimate, 1, MPI_DOUBLE, global_estimates.data(), 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Root process computes the average of results and prints the final estimate
    if (rank == 0) {
        double final_estimate = 0.0;
        for (double estimate : global_estimates) {
            final_estimate += estimate;
        }
        final_estimate /= (N * 1.0); // Divide by total number of samples for average
        std::cout << "The estimate for integral " << P << " is " << final_estimate << std::endl;
    }

    MPI_Finalize();
    return 0;
}