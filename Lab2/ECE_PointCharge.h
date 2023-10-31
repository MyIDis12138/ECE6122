/*
Author:  Yang Gu
Date last modified: 03/10/2023
Organization: ECE6122 Class

Description:

Header file for the ECE_PointCharge class.
This class represents a point charge in 3D space.
    
*/

#ifndef ECE_POINTCHARGE_H
#define ECE_POINTCHARGE_H

class ECE_PointCharge {
protected:
    double x; // x-coordinate.
    double y; // y-coordinate.
    double z; // z-coordinate.
    double q; // charge of the point.

public:
    /**
     * @brief Constructor for ECE_PointCharge class.
     * 
     * @param x The x-coordinate of the point charge.
     * @param y The y-coordinate of the point charge.
     * @param z The z-coordinate of the point charge.
     * @param q The charge of the point in micro Coulomb.
     */
    ECE_PointCharge(double x, double y, double z, double q);
    /**
     * @brief Set the location (coordinates) of the point charge.
     * 
     * @param x The new x-coordinate of the point charge.
     * @param y The new y-coordinate of the point charge.
     * @param z The new z-coordinate of the point charge.
     */
    void setLocation(double x, double y, double z);
    /**
     * @brief Set the charge of the point.
     * 
     * @param q The new charge value.
     */
    void setCharge(double q);
};

#endif // ECE_POINTCHARGE_H
