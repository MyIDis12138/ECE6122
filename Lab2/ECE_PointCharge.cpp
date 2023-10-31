/*
Author:  Yang Gu
Date last modified: 03/10/2023
Organization: ECE6122 Class

Description:

Implementation file for the ECE_PointCharge class.
This class represents a point charge in 3D space.
    
*/

#include "ECE_PointCharge.h"

ECE_PointCharge::ECE_PointCharge(double x, double y, double z, double q){
    this->x = x;
    this->y = y;
    this->z = z;
    this->q = q*1.0e-6;
}

void ECE_PointCharge::setLocation(double x, double y, double z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

void ECE_PointCharge::setCharge(double q) {
    this->q = q;
}
