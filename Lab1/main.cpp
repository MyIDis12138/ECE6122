// /*
// Author:  Yang Gu
// Date last modified: 25/9/2023
// Organization: ECE6122 Class

// Description:

// Main function of Lab2
    
// */
// #define TESTING_MODE // for testing
#include <fstream> // for testing
#include <iostream>
#include <cmath>
#include <vector>
#include <thread>
#include <chrono>
#include <iomanip>
#include <condition_variable>
#include <mutex>
#include <atomic>

#include "utils.h"
#include "ECE_ElectricField.h"

using namespace std;

const int MAX_THREADS = thread::hardware_concurrency();
int activeThreads = 0;

mutex mtx;
condition_variable cv;

vector<ECE_ElectricField> vecElectric;
vector<double> vecEx, vecEy, vecEz; 

double x, y, z;

bool breakloop = false;

atomic<int> completedThreads(0);
bool startFlag = false;
bool terminateFlag = false;

/**
 * Calculates the aggregated electric field at a specific point (x, y, z) for a segment of ECE_ElectricField objects.
 * The function is presumably designed to be run in main thread, which is responsible for a segment of the vector.
 *
 * @param id              The ID representing the main thread. This determines the segment of the vector the function will operate on.
 * @param num_to_calculate The number of ECE_ElectricField objects each thread or task should calculate the electric field for.
 * @param max_num         The maximum index up to which the calculation should be performed in the vector. This prevents overrunning the end of the vector.
 */
void mainCalculateElectricField(const int id, const int num_to_calculate, const int max_num){
    double tempEx(0.0), tempEy(0.0), tempEz(0.0);
    double sumEx(0.0), sumEy(0.0), sumEz(0.0);
    unsigned long start_index = id * num_to_calculate;
    unsigned long stop_index = max_num;

    if (stop_index > start_index){
        for (int i = start_index; i < stop_index; ++i){
            vecElectric[i].computeFieldAt(x, y, z);
            vecElectric[i].getElectricField(tempEx, tempEy, tempEz);
            sumEx += tempEx;
            sumEy += tempEy;
            sumEz += tempEz;
        }
    }
    vecEx[id] = sumEx;
    vecEy[id] = sumEy;
    vecEz[id] = sumEz;
};

/**
 * Calculates the aggregated electric field at a specific point (x, y, z) for a segment of ECE_ElectricField objects.
 * This function is designed to be run in parallel across multiple threads, where each thread is responsible for a segment of the vector.
 * Threads will wait until signalled by the main thread to start calculations and will notify the main thread upon completion.
 *
 * @param id              The ID representing the current thread. This determines the segment of the vector the function will operate on.
 * @param num_to_calculate The number of ECE_ElectricField objects each thread should calculate the electric field for.
 * @param max_num         The maximum index up to which the calculation should be performed in the vector. This prevents overrunning the end of the vector.
 */
void CalculateElectricField(const int id, const int num_to_calculate, const int max_num)
{
    double tempEx(0.0), tempEy(0.0), tempEz(0.0);
    double sumEx(0.0), sumEy(0.0), sumEz(0.0);
    unsigned long start_index = id * num_to_calculate;
    unsigned long stop_index = start_index + num_to_calculate;

    if (stop_index > max_num)
    {
        stop_index = max_num;
    }

    while (true)    
    {
        // waiting to be signalled 
        // the main thread should not wait
        // main thread should break out
        sumEx = 0.0;
        sumEy = 0.0;
        sumEz = 0.0;
        {
            unique_lock<mutex> lock(mtx);
            cv.wait(lock, [] { return startFlag || terminateFlag; });
        }
        if (terminateFlag) {
            break;
        }
        // Check if calculations should be started or if the thread should exit
        // Check is signalled to exit
        // 
        // Do its part of the calculation
        for (int i = start_index; i < stop_index; ++i){
            vecElectric[i].computeFieldAt(x, y, z);
            vecElectric[i].getElectricField(tempEx, tempEy, tempEz);
            sumEx += tempEx;
            sumEy += tempEy;
            sumEz += tempEz;
        }

        vecEx[id] = sumEx;
        vecEy[id] = sumEy;
        vecEz[id] = sumEz;

        // Signal that it is done with its part
        completedThreads++;
        if (completedThreads.load() == activeThreads ) {
            cv.notify_all();  // Notify the main thread
        }

        // Wait for the main thread to finish its work before the next cycle
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [] { return !startFlag || terminateFlag; });
        if (terminateFlag) {
            break;
        }
    }
}

/**
 * Function for testing
 * Saves the electric field coordinates from a vector of ECE_ElectricField objects to a file.
 *
 * @param filename    The name of the file to which the coordinates will be saved.
 * @param vecElectricf The vector of ECE_ElectricField objects containing the coordinates.
 */
void saveCoordinatesToFile(const std::string& filename, vector<ECE_ElectricField>& vecElectricf) {
    std::ofstream outFile(filename);

    if (!outFile.is_open()) {
        std::cerr << "Error: Unable to open file for writing: " << filename << std::endl;
        return;
    }

    outFile << "ID\tX\tY\tZ\n"; // Header for the file

    for (size_t i = 0; i < vecElectricf.size(); ++i) {
        outFile << i << "\t" 
                << vecElectricf[i].getX() << "\t" 
                << vecElectricf[i].getY() << "\t" 
                << vecElectricf[i].getZ() << "\n";
    }

    outFile.close();
    std::cout << "Coordinates saved to " << filename << std::endl;
}


int main() {

    cout << "Your computer supports " << MAX_THREADS << " concurrent threads." << endl;
    int rows, cols;
    double q, xSeparation, ySeparation;

    #ifdef TESTING_MODE
        string input = "2 2 \n 0.01 0.03 \n 0.02 \n";
        //string input = "100 100 \n 0.01 0.03 \n 0.02 \n";
        istringstream iss(input);

        iss >> rows >>cols;
        iss >> xSeparation >> ySeparation;
        iss >> q;
        saveCoordinatesToFile("coordinates.txt", vecElectric);
    #else
        // User input for array size, separation distances, charge, and point location
        getBasicInfo(q, rows, cols, xSeparation, ySeparation);
    #endif

    int max_num = cols*rows;

    activeThreads = min(max_num, MAX_THREADS-1);
    int num_per_thread = max_num/activeThreads;
    vector<thread> vecThreads(activeThreads);

    vecEx.resize(activeThreads+1);
    vecEy.resize(activeThreads+1);
    vecEz.resize(activeThreads+1);

    // create 2D charges array
    double halfRow = double(rows-1.0)/2.0;
    double halfCol = double(cols-1.0)/2.0;
    for (double i = 0; i < rows; ++i) {
        for (double j = 0; j < cols; ++j) {
            double chargeX = (i-halfRow) * ySeparation;
            double chargeY = (j-halfCol) * xSeparation;
            vecElectric.push_back(ECE_ElectricField(chargeY, chargeX, 0, q));  // just a sample value
        }
    }

    for (int i = 0; i < activeThreads; ++i)
    {
        vecThreads[i] = thread(CalculateElectricField, i, num_per_thread, max_num);
    }

    while (true)
    {
        double Ex = 0.0;
        double Ey = 0.0;
        double Ez = 0.0;
        double Enorm = 0.0;

        #ifdef TESTING_MODE
            string input = "1 2 3\n";
            //string input = "0 0 0\n";
            istringstream iss(input);
            iss >> x >> y >> z;
        #else
            getXYZ(x, y, z);
        #endif

        //start
        {
            unique_lock<mutex> lock(mtx);
            startFlag = true;
            completedThreads.store(0);
        }
        auto start_time = chrono::high_resolution_clock::now();
        cv.notify_all();

        int main_thread_num = activeThreads;
        mainCalculateElectricField(main_thread_num, num_per_thread, max_num);

        //receive 
        {
            unique_lock<mutex> lock(mtx);
            cv.wait(lock, [] { return completedThreads.load() == activeThreads; });
        }

        #ifdef TESTING_MODE
            bool checkall =true;
            for(auto point: vecElectric){
                checkall = point.fieldComputed && checkall;
            }
            if (checkall){
                cout<<"All points have been computed\n";
            }
            else
            {
                cerr<<"Error!\n";
            }
            
        #endif

        Ex = sum(vecEx);
        Ey = sum(vecEy);
        Ez = sum(vecEz);
        Enorm = sqrt(Ex * Ex + Ey * Ey + Ez * Ez);
        
        auto end_time = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);

        {
            cout << "The electric field at ("<< x << ", "<< y << " ," << z << ") in V/m is\n";
            cout << "Ex = " << formatScientific(Ex) << "\n";
            cout << "Ey = " << formatScientific(Ey) << "\n";
            cout << "Ez = " << formatScientific(Ez) << "\n";
            cout << "|E| = " << formatScientific(Enorm) << "\n";
            cout << "The calculation took " << duration.count() << " microseconds!\n";
        }

        {
            unique_lock<mutex> lock(mtx);
            startFlag = false;
        }
        cv.notify_all();

        #ifdef TESTING_MODE
            break;
        #else
            if (!getChoice()) {
                cout << "Bye!" << endl;
                break;
            }
        #endif

    }

    {
        unique_lock<mutex> lock(mtx);
        terminateFlag = true;
    }
    cv.notify_all();

    for (auto& t : vecThreads) {
        t.join();
    }

    return 0;
}
