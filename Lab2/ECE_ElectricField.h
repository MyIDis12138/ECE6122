/*
Author:  Yang Gu
Date last modified: 03/10/2023
Organization: ECE6122 Class

Description:

Header file for the ECE_ElectricField class, which inherits from ECE_PointCharge.
This class calculates and stores the electric field components.
    
*/

#ifndef ECE_ELECTRICFIELD_H
#define ECE_ELECTRICFIELD_H

#include "ECE_PointCharge.h"
#include <stdexcept>

const double k = 9.0e9; // Coulomb's constant N*m^2 /mC^2 

class ECE_ElectricField : public ECE_PointCharge {
protected:
    double Ex; // Electric field in the x-direction.
    double Ey; // Electric field in the y-direction.
    double Ez; // Electric field in the z-direction.

public:
    bool fieldComputed; // Flag to track whether the electric field has been computed.
    /**
     * @brief Constructor for ECE_ElectricField class.
     * 
     * @param x The x-coordinate of the point charge.
     * @param y The y-coordinate of the point charge.
     * @param z The z-coordinate of the point charge.
     * @param q The charge of the point.
     */
    ECE_ElectricField(double x, double y, double z, double q);
    /**
     * @brief Calculate the electric field at a specified point.
     * 
     * @param x The x-coordinate where the electric field is calculated.
     * @param y The y-coordinate where the electric field is calculated.
     * @param z The z-coordinate where the electric field is calculated.
     */
    void computeFieldAt(double x, double y, double z);
    /**
     * @brief Get the calculated electric field components.
     * 
     * @param Ex Reference to store the electric field in the x-direction.
     * @param Ey Reference to store the electric field in the y-direction.
     * @param Ez Reference to store the electric field in the z-direction.
     */
    void getElectricField(double &Ex, double &Ey, double &Ez);

    // for testing
    double getX() {return this->x;}
    double getY() {return this->y;}
    double getZ() {return this->z;}   
};

#endif // ECE_ELECTRICFIELD_H
