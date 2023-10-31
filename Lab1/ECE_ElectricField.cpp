/*
Author:  Yang Gu
Date last modified: 25/9/2023
Organization: ECE6122 Class

Description:

Implementation file for the ECE_ElectricField class, which inherits from ECE_PointCharge.
This class calculates and stores the electric field components.
    
*/

#include "ECE_ElectricField.h"
#include <cmath>

ECE_ElectricField::ECE_ElectricField(double x, double y, double z, double q)
    : ECE_PointCharge(x, y, z, q){
        Ex = 0.0;
        Ey = 0.0;
        Ez = 0.0;
        this->fieldComputed = false;
    }

void ECE_ElectricField::computeFieldAt(double x, double y, double z) {
    double dx = x - this->x;
    double dy = y - this->y;
    double dz = z - this->z;
    
    double r_squared = dx * dx + dy * dy + dz * dz;
    double base_r = sqrt(r_squared)*r_squared;

    this->Ex = this->k * this->q * dx / base_r;
    this->Ey = this->k * this->q * dy / base_r;
    this->Ez = this->k * this->q * dz / base_r;

    fieldComputed = true;
}

void ECE_ElectricField::getElectricField(double &Ex, double &Ey, double &Ez) {
    if (!fieldComputed) {
        throw std::runtime_error("Electric field has not been computed.");
    } else {
        Ex = this->Ex;
        Ey = this->Ey;
        Ez = this->Ez;
    }
}
