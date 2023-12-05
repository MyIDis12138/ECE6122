/*
Author: Yang Gu
Date last modified: 05/12/2023
Organization: ECE6122 Class

Description:
The lights header contains three lights: a directional light, a point light and a spot light.
It provides functions to set up the lights and to update the lights' intensity.
This file is part of the project in the ECE6122 class.
*/
#ifndef LIGHTS_H
#define LIGHTS_H

#include <glm/glm.hpp>

#include <src/shader.h>
#include <src/light_cube.h>

#include <random>

struct DirLight {
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct PointLight {
    glm::vec3 localPos;
    glm::vec3 globalPos;

    float constant;
    float linear;
    float quadratic;
    
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    glm::vec3 init_ambient;
    LightCube light_cube;     
};

struct SpotLight {
    glm::vec3 position;
    glm::vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular; 
};

void set_Dirlight_in_shader(const DirLight& dir_light, const Shader& shader) {
    shader.setVec3("dirLight.direction", dir_light.direction);
    shader.setVec3("dirLight.ambient", dir_light.ambient);
    shader.setVec3("dirLight.diffuse", dir_light.diffuse);
    shader.setVec3("dirLight.specular", dir_light.specular);
}

void set_Pointlight_in_shader(const PointLight& point_light, const Shader& shader) {
    shader.setVec3("pointLight.position", point_light.globalPos);
    shader.setFloat("pointLight.constant", point_light.constant);
    shader.setFloat("pointLight.linear", point_light.linear);
    shader.setFloat("pointLight.quadratic", point_light.quadratic);
    shader.setVec3("pointLight.ambient", point_light.ambient);
    shader.setVec3("pointLight.diffuse", point_light.diffuse);
    shader.setVec3("pointLight.specular", point_light.specular);
}

void set_Spotlight_in_shader(const SpotLight& spot_light, const Shader& shader) {
    shader.setVec3("spotLight.position", spot_light.position);
    shader.setVec3("spotLight.direction", spot_light.direction);
    shader.setFloat("spotLight.cutOff", spot_light.cutOff);
    shader.setFloat("spotLight.outerCutOff", spot_light.outerCutOff);
    shader.setFloat("spotLight.constant", spot_light.constant);
    shader.setFloat("spotLight.linear", spot_light.linear);
    shader.setFloat("spotLight.quadratic", spot_light.quadratic);
    shader.setVec3("spotLight.ambient", spot_light.ambient);
    shader.setVec3("spotLight.diffuse", spot_light.diffuse);
    shader.setVec3("spotLight.specular", spot_light.specular);
}


DirLight defaultDirLight(glm::vec3 direction) {
    DirLight dir_light;
    dir_light.direction = direction;
    dir_light.ambient = glm::vec3(0.3f, 0.3f, 0.3f);
    dir_light.diffuse = glm::vec3(0.7f, 0.7f, 0.7f);
    dir_light.specular = glm::vec3(0.5f, 0.5f, 0.5f);
    return dir_light;
}

PointLight defaultPointLight(glm::vec3 position) {
    PointLight point_light;
    point_light.localPos = position;
    point_light.globalPos = position;
    point_light.constant = 1.0f;
    point_light.linear = 0.022f;
    point_light.quadratic = 0.0019f;
    point_light.ambient = glm::vec3(0.5f, 0.25f, 0.05f); //default to orange
    point_light.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    point_light.specular = glm::vec3(0.5f, 0.5f, 0.5f);

    point_light.init_ambient = point_light.ambient;
    point_light.light_cube = LightCube(position, glm::vec3(1.0f), 1.0f);
    return point_light;
}

SpotLight defaultSpotLight(glm::vec3 position) {
    SpotLight spot_light;
    spot_light.position = position;
    spot_light.direction = glm::vec3(0.0f, -1.0f, 0.0f);
    spot_light.cutOff = glm::cos(glm::radians(12.5f));
    spot_light.outerCutOff = glm::cos(glm::radians(15.0f));
    spot_light.constant = 1.0f;
    spot_light.linear = 0.09f;
    spot_light.quadratic = 0.032f;
    spot_light.ambient = glm::vec3(1.0f, 1.0f, 1.0f);
    spot_light.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    spot_light.specular = glm::vec3(0.5f, 0.5f, 0.5f);
    return spot_light;
}

void update_indensity_random(PointLight& point_light) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0f, 0.2f);
    float amp = dis(gen) - 0.1f;
    point_light.ambient += amp*point_light.init_ambient;
}

#endif // LIGHTS_H