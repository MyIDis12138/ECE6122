/*
Author: Yang Gu
Date last modified: 05/12/2023
Organization: ECE6122 Class

Description:
The lightCube class is used to store the information of a light cube for debugging. 
It provides functions to setup and draw the light cube.
This file is part of the project in the ECE6122 class.
*/
#ifndef LIGHT_CUBE_H
#define LIGHT_CUBE_H

#include <src/shader.h>
#include <glm/glm.hpp>

class LightCube {
public:
    glm::vec3 position;
    glm::vec3 color;
    float size;

    unsigned int VAO, VBO, EBO;
    LightCube() {};

    LightCube(glm::vec3 pos, glm::vec3 col, float sz)
        : position(pos), color(col), size(sz) {
        // Initialize VAO, VBO, EBO and set up vertex data and attributes
        setupCube();
    };

    void Draw(Shader& shader) {
        // Set the model matrix
        glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(size)); // Scale the cube to the desired size

        // Set shader uniforms
        shader.setMat4("model", modelMatrix);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    };

private:
    void setupCube() {
        float vertices[] = {
            // positions          
            -0.5f * size, -0.5f * size, -0.5f * size, // 0
            0.5f * size, -0.5f * size, -0.5f * size, // 1
            0.5f * size,  0.5f * size, -0.5f * size, // 2
            -0.5f * size,  0.5f * size, -0.5f * size, // 3
            -0.5f * size, -0.5f * size,  0.5f * size, // 4
            0.5f * size, -0.5f * size,  0.5f * size, // 5
            0.5f * size,  0.5f * size,  0.5f * size, // 6
            -0.5f * size,  0.5f * size,  0.5f * size  // 7 
        };

        unsigned int indices[] = {
            0, 2, 1, 0, 3, 2, // Back face
            4, 6, 5, 4, 7, 6, // Front face
            4, 1, 5, 4, 0, 1, // Bottom face
            3, 6, 2, 3, 7, 6, // Top face
            1, 6, 5, 1, 2, 6, // Right face
            4, 3, 0, 4, 7, 3  // Left face
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

};

#endif