/*
Author:  Yang Gu
Date last modified: 20/9/2023
Organization: ECE6122 Class

Description:

Source code of the utilities functions.
    
*/

#include "utils.h"
#include <iostream>
#include <cmath>
#include <iomanip>
#include <sstream>

double sum(const std::vector<double>& vec) {
    double res = 0.0;
    for (double i: vec) {
        res += i;
    }
    return res;
}

std::string formatScientific(double value) {
    if (value == 0.0) {
        return "0.0";
    }

    int exponent = static_cast<int>(std::log10(std::abs(value)));
    double mantissa = value / std::pow(10.0, exponent);

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(4) << mantissa << " * 10^" << exponent;
    return oss.str();
}

void getBasicInfo(int& numOfThreads, int& rows, int& cols, double& xSeparation, double& ySeparation, double& q){
    // Get the number of rows and columns
    while (true) {
        std::cout << "Please enter the number of threads: ";
        std::string input;
        std::getline(std::cin, input);
        std::istringstream stream(input);
        if (stream >>numOfThreads && numOfThreads>0) {
            break;
        } else {
            std::cout << "Invalid input. Please enter a positive integers." << std::endl;
        }
    }

    // Get the number of rows and columns
    while (true) {
        std::cout << "Please enter the number of rows and columns in the N x M array: ";
        std::string input;
        std::getline(std::cin, input);
        std::istringstream stream(input);
        if (stream >> rows >> cols && rows>0 && cols>0) {
            break;
        } else {
            std::cout << "Invalid input. Please enter two positive integers separated by space." << std::endl;
        }
    }

    // Get the x and y separation distances
    while (true) {
        std::cout << "Please enter the x and y separation distances in meters: ";
        std::string input;
        std::getline(std::cin, input);
        std::istringstream stream(input);
        if (stream >> xSeparation >> ySeparation && xSeparation>0 && ySeparation>0) {
            break;
        } else {
            std::cout << "Invalid input. Please enter two positive floating-point numbers separated by space." << std::endl;
        }
    }

    // Get the common charge on the points
    while (true) {
        std::cout << "Please enter the common charge on the points in micro C: ";
        std::string input;
        std::getline(std::cin, input);
        std::istringstream stream(input);
        if (stream >> q) {
            break;
        } else {
            std::cout << "Invalid input. Please enter a floating-point number." << std::endl;
        }
    }
}

void getXYZ(double& x, double& y, double& z){
    while (true) {
        std::cout << "\nPlease enter the location in space to determine the electric field (x y z) in meters: ";
        std::string input;
        std::getline(std::cin, input);
        std::istringstream stream(input);
        if (stream >> x >> y >> z) {
            break;
        } else {
            std::cout << "Invalid input. Please enter three floating-point numbers." << std::endl;
        }
    }
}

bool getChoice(){
    char choice;
    while (true) {
        std::cout << "Do you want to enter a new location (Y/N)? ";
        std::string input;
        std::getline(std::cin, input);
        std::istringstream stream(input);
        if (stream >> choice && (choice=='Y' || choice=='N')) {
            break;
        } else {
            std::cout << "Invalid input. Please enter Y or N." << std::endl;
        }
    }
    return choice=='Y';
}
