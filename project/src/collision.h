/*
Author: Yang Gu
Date last modified: 05/12/2023
Organization: ECE6122 Class

Description:
The collision class is used to store the collision information of a model. It also provides
functions to check for collisions with the room and to update the model's velocity.
*/
#ifndef COLLISION_H
#define COLLISION_H

#include <glm/glm.hpp>

#include <src/room.h>

struct AABB {
    glm::vec3 localMin;
    glm::vec3 localMax;
    glm::vec3 globalMin;
    glm::vec3 globalMax;
};

bool intersect(const AABB& a, const AABB& b) {
    // Check for no overlap on each axis
    if (a.globalMax.x < b.globalMin.x || a.globalMin.x > b.globalMax.x) return false;
    if (a.globalMax.y < b.globalMin.y || a.globalMin.y > b.globalMax.y) return false;
    if (a.globalMax.z < b.globalMin.z || a.globalMin.z > b.globalMax.z) return false;

    // Overlapping on all axes means AABBs intersect
    return true;
}

glm::vec3 checkBoxRoomCollisions(const AABB& box, const Room& room) {
    glm::vec3 velocity = glm::vec3(1.0f);
    // Check collision with each face of the room
    if (box.globalMin.x < room.minCorner.x) {
        velocity.x = -1.0f; // Invert X velocity
    }
    if (box.globalMax.x > room.maxCorner.x) {
        velocity.x = -1.0f;
    }
    if (box.globalMin.y < room.minCorner.y) {
        velocity.y = -1.0f; // Invert Y velocity
    }
    if (box.globalMax.y > room.maxCorner.y) {
        velocity.y = -1.0f;
    }
    if (box.globalMin.z < room.minCorner.z) {
        velocity.z = -1.0f; // Invert Z velocity        
    }
    if (box.globalMax.z > room.maxCorner.z) {
        velocity.z = -1.0f;
    }

    return velocity;
}


#endif // COLLISION_H