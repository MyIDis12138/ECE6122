#ifndef ROOM_H
#define ROOM_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <src/shader.h>

class Room {
public:
    // Constructor
    Room(const std::string& floortexturePath, const std::string& ceilingtexturePath, const std::string& walltexturePath) {
        setupFloor();
        //setupCeiling();
        setupWall();

        loadTexture(floortexturePath, floorTexture);
        //loadTexture(ceilingtexturePath);
        loadTexture(walltexturePath, wallTexture);
    }

    // Render the floor
    void Draw(Shader& shader) {
        drawFloor(shader);
        // drawCeiling(shader);
        drawWalls(shader);
    }

private:
    unsigned int floorVBO, floorVAO, floorTexture;
    unsigned int ceilingVBO, ceilingVAO, ceilingTexture;
    unsigned int wallVBO, wallVAO, wallTexture;

    void setupFloor() {
        float floorSize     = 50.0f; 
        float tilingFactor  = 10.0f; 

        float vertices[] = {
            // Positions         // Texture Coords (tiled)
            floorSize, 0.0f, -floorSize,  tilingFactor, 0.0f,
           -floorSize, 0.0f, -floorSize,  0.0f,         0.0f,
           -floorSize, 0.0f,  floorSize,  0.0f,         tilingFactor,

            floorSize, 0.0f, -floorSize,  tilingFactor, 0.0f,
           -floorSize, 0.0f,  floorSize,  0.0f,         tilingFactor,
            floorSize, 0.0f,  floorSize,  tilingFactor, tilingFactor
        };

        glGenVertexArrays(1, &floorVAO);
        glGenBuffers(1, &floorVBO);

        glBindVertexArray(floorVAO);
        glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // Texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0); 
        glBindVertexArray(0); 
    }

    void setupCeiling(){
        float ceilingSize = 50.0f;
        float tilingFactor = 10.0f;
        float ceilingHeight = 20.0f; // Height of the ceiling from the floor

        float vertices[] = {
            // Positions                // Texture Coords
            ceilingSize, ceilingHeight, ceilingSize,  tilingFactor, 0.0f,
           -ceilingSize, ceilingHeight, ceilingSize,  0.0f,         0.0f,
           -ceilingSize, ceilingHeight, -ceilingSize,  0.0f,        tilingFactor,

            ceilingSize, ceilingHeight, ceilingSize,  tilingFactor,  0.0f,
           -ceilingSize, ceilingHeight, -ceilingSize,  0.0f,         tilingFactor,
            ceilingSize, ceilingHeight, -ceilingSize,  tilingFactor, tilingFactor
        };

        glGenVertexArrays(1, &ceilingVAO);
        glGenBuffers(1, &ceilingVBO);

        glBindVertexArray(ceilingVAO);
        glBindBuffer(GL_ARRAY_BUFFER, ceilingVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0); 
        glBindVertexArray(0); 
    }

    void setupWall(){
        float wallWidth = 50.0f;
        float wallHeight = 50.0f; // Same as ceiling height
        float tilingFactor = 1.0f; // Adjust as needed

        float vertices[] = {
            // One wall positioned at one end of the floor
            // Positions              // Texture Coords
           -wallWidth, 0.0f,        wallWidth,  0.0f,           0.0f,
           -wallWidth, 0.0f,       -wallWidth,  tilingFactor,   0.0f,
           -wallWidth, wallHeight, -wallWidth,  tilingFactor,   tilingFactor,

           -wallWidth, 0.0f,        wallWidth,  0.0f,           0.0f,
           -wallWidth, wallHeight, -wallWidth,  tilingFactor,   tilingFactor,
           -wallWidth, wallHeight,  wallWidth,  0.0f,           tilingFactor
        };

        glGenVertexArrays(1, &wallVAO);
        glGenBuffers(1, &wallVBO);

        glBindVertexArray(wallVAO);
        glBindBuffer(GL_ARRAY_BUFFER, wallVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0); 
        glBindVertexArray(0);
    }

    void drawFloor(Shader& shader) {
        shader.use();
        glm::mat4 floorModel = glm::mat4(1.0f); // Modify as needed for your scene
        shader.setMat4("model", floorModel);
        
        shader.setInt("texture_diffuse1", 0); // Assuming the texture unit 0 for the floor
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);

        glBindVertexArray(floorVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    void drawCeiling(Shader& shader) {
        shader.use();
        glm::mat4 ceilingModel = glm::mat4(1.0f); // Modify as needed for your scene
        shader.setMat4("model", ceilingModel);

        shader.setInt("texture_diffuse1", 0); // Assuming the texture unit 0 for the ceiling
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ceilingTexture);

        glBindVertexArray(ceilingVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    void drawWalls(Shader& shader) {
        shader.use();
        glm::mat4 wallModel = glm::mat4(1.0f); // Modify as needed for your scene
        shader.setMat4("model", wallModel);

        shader.setInt("texture_diffuse1", 0); // Assuming the texture unit 0 for the walls
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, wallTexture);

        glBindVertexArray(wallVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }


    void loadTexture(const std::string& texturePath, unsigned int &texture) {
        glGenTextures(1, &texture);

        int width, height, nrChannels;
        unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        } else {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);
    }
};

#endif // ROOM_H
