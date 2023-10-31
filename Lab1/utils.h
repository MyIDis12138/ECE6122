/*
Author:  Yang Gu
Date last modified: 25/9/2023
Organization: ECE6122 Class

Description:

Header file of the utilities functions.
    
*/


#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <string>

/**
 * Computes the sum of all elements in a vector.
 *
 * @param vec       The vector containing double values.
 * @return          The sum of all elements in the vector.
 */
double sum(const std::vector<double>& vec);

/**
 * Formats a given number into scientific notation.
 *
 * @param value     The number to be formatted.
 * @return          A string representation of the number in scientific notation.
 */
std::string formatScientific(double value);

/**
 * Prompts the user to input basic information including the number of rows and columns,
 * x and y separation distances, and the common charge on the points.
 *
 * @param q             Reference to store the common charge on the points.
 * @param N             Reference to store the number of rows.
 * @param M             Reference to store the number of columns.
 * @param xSeparation   Reference to store the x separation distance.
 * @param ySeparation   Reference to store the y separation distance.
 */
void getBasicInfo(double &q, int &N, int &M, double &xSeparation, double &ySeparation);

/**
 * Prompts the user to input a location in space represented by x, y, and z coordinates.
 *
 * @param x             Reference to store the x-coordinate.
 * @param y             Reference to store the y-coordinate.
 * @param z             Reference to store the z-coordinate.
 */
void getXYZ(double &x, double &y, double &z);

/**
 * Prompts the user to ask if they want to continue with a new location.
 *
 * @return              True if the user want to  continue with a new location, false otherwise.
 */
bool getChoice();

#endif //UTILS_H
