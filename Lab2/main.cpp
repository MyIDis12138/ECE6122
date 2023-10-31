// /*
// Author:  Yang Gu
// Date last modified: 03/10/2023
// Organization: ECE6122 Class

// Description:

// Main function of Lab2
    
// */
//#define DEBUG // for debugging
#include <iostream>
#include <cmath>
#include <vector>
#include <iomanip>
#include <omp.h>

#include "utils.h"
#include "ECE_ElectricField.h"

using namespace std;

int main() {
    // Declaration of a vector to store electric field values
    vector<ECE_ElectricField> vecElectric;

    // Variable declarations for grid specifications and other parameters
    int rows, cols, numOfThreads;
    double q, xSeparation, ySeparation;

    // Input section; uses a preset string for debugging or prompts user for real-time input
#ifdef DEBUG
    string input = "16 \n 1000 1000 \n 0.01 0.03 \n 0.02 \n";
    istringstream iss(input);

    iss >> numOfThreads;
    iss >> rows >>cols;
    iss >> xSeparation >> ySeparation;
    iss >> q;
#else
    getBasicInfo(numOfThreads, rows, cols, xSeparation, ySeparation, q);
#endif

    int max_num = cols * rows;
    vecElectric.reserve(max_num);   // Reserve memory for electric field values

    // Ensuring the number of threads is not greater than the number of grid points
    numOfThreads = max_num > numOfThreads ? numOfThreads : max_num;
    omp_set_num_threads(numOfThreads);
    int num_per_thread = max_num/numOfThreads;

    const double halfRow = double(rows - 1.0) / 2.0 * ySeparation;
    const double halfCol = double(cols - 1.0) / 2.0 * xSeparation;

    // Parallel computation to populate the vecElectric with ECE_ElectricField objects
    #pragma omp parallel for
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                double chargeX = j*xSeparation - halfCol;
                double chargeY = i*ySeparation - halfRow;
                vecElectric[i * cols + j] = ECE_ElectricField(chargeX, chargeY, 0, q);
            }
        }

    double start_time, end_time;
    double Ex = 0.0, Ey = 0.0, Ez = 0.0;
    double x, y, z;
    bool continueLoop = true;  

    // Parallel block for computing electric field at specific points and printing results
    #pragma omp parallel shared(x, y, z, Ex, Ey, Ez, continueLoop)
    {
        int id = omp_get_thread_num();
        unsigned long start_index = id * num_per_thread;
        unsigned long stop_index = start_index + num_per_thread;
        stop_index = stop_index > max_num ? max_num : stop_index;
        double tempEx = 0.0, tempEy = 0.0, tempEz = 0.0;

        while (continueLoop)
        {   
            Ex = 0.0;
            Ey = 0.0;
            Ez = 0.0;
            double sumEx = 0.0, sumEy = 0.0, sumEz = 0.0, Enorm = 0.0;

            // Master thread obtaining the coordinates and starting the timer
            #pragma omp master
            {
#ifdef DEBUG
                string input = "1 2 3\n";
                istringstream iss(input);
                iss >> x >> y >> z;
#else
                getXYZ(x, y, z);
#endif
                start_time = omp_get_wtime();
            }
            
            #pragma omp barrier
            sumEx = 0.0;
            sumEy = 0.0;
            sumEz = 0.0;

            for (int i = start_index; i < stop_index; ++i){
                vecElectric[i].computeFieldAt(x, y, z);
                vecElectric[i].getElectricField(tempEx, tempEy, tempEz);
                sumEx += tempEx;
                sumEy += tempEy;
                sumEz += tempEz;
            }

            // Aggregate results from all threads
            #pragma omp critical
            {
                    Ex += sumEx;
                    Ey += sumEy;
                    Ez += sumEz;
            }
            #pragma omp barrier

            // Master thread printing the results and asking the user for continuation
            #pragma omp master
            {

                Enorm = sqrt(Ex * Ex + Ey * Ey + Ez * Ez);
                end_time = omp_get_wtime();
                
                cout << "The electric field at ("<< x << ", "<< y << " ," << z << ") in V/m is\n";
                cout << "Ex = " << formatScientific(Ex) << "\n";
                cout << "Ey = " << formatScientific(Ey) << "\n";
                cout << "Ez = " << formatScientific(Ez) << "\n";
                cout << "|E| = " << formatScientific(Enorm) << "\n";
                cout << "The calculation took " << (end_time - start_time)*1e6 << " microseconds!\n";

                // Checking if the user wants to continue
#ifdef DEBUG
                continueLoop = false;
#else
                if (!getChoice()) {
                    cout << "Bye!" << endl;
                    continueLoop = false;
                }
#endif
                #pragma omp flush(continueLoop)
            }
        #pragma omp barrier
        }
    }
    
    return 0;
}