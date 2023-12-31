/*
Author: Yang Gu
Date last modified: 05/12/2023
Organization: ECE6122 Class

Description:
The 'Model' header file defines the 'Model' and 'SubModel' classes for managing complex 3D models in OpenGL. 
It handles the loading and processing of models using the ASSIMP library, and manages textures, meshes, and sub-models. 
The 'Model' class is responsible for drawing the model, updating model states, handling collisions, and managing lighting. 
The 'SubModel' class represents a part of the model, containing its own meshes, transformation data, and lighting information. 
This file is integral to the project in the ECE6122 class.
*/
#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <src/stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <src/mesh.h>
#include <src/shader.h>
#include <src/lights.h>
#include <src/collision.h>
#include <src/room.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);

class SubModel {
public:
    std::vector<Mesh> meshes;
    std::string meshName;
    
    glm::vec3 velocity;
    glm::vec3 angularVelocity;
    glm::mat4 modelMatrix;
    
    PointLight pointLight;
    AABB boundingBox;

    // Default constructor
    SubModel() 
        : modelMatrix(glm::mat4(1.0f)),
          meshName(""), 
          velocity(glm::vec3(0.0f)),
          angularVelocity(glm::vec3(0.0f)),
          pointLight(defaultPointLight(glm::vec3(0.0f))) {}

    // Constructor with parameters
    SubModel(std::vector<Mesh> meshes, std::string meshName)
    :   meshes(meshes), meshName(meshName),           
        velocity(glm::vec3(0.0f)),
        angularVelocity(glm::vec3(0.0f)),
        modelMatrix(glm::mat4(1.0f))
    {
        pointLight = defaultPointLight(computeCentroid());
        computeBoundingBox();
    }

   void update(float deltaTime) {
        // Calculate translation to origin and back
        glm::vec3 centroid = pointLight.globalPos; // Assuming pointLight.position is at the centroid
        glm::mat4 toOrigin = glm::translate(glm::mat4(1.0f), -centroid);
        glm::mat4 backFromOrigin = glm::translate(glm::mat4(1.0f), centroid);

        // Apply translation
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), velocity * deltaTime);
        modelMatrix = translation * modelMatrix;

        // Apply rotation around the submodel's own Y-axis
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angularVelocity.y * deltaTime, glm::vec3(0.0f, 1.0f, 0.0f));
        modelMatrix = backFromOrigin * rotation * toOrigin * modelMatrix;

        // Update bounding box and point light position
        pointLight.globalPos = glm::vec3(modelMatrix * glm::vec4(pointLight.localPos, 1.0f));
        pointLight.light_cube.position = pointLight.globalPos;
        boundingBox.globalMin = glm::vec3(modelMatrix * glm::vec4(boundingBox.localMin, 1.0f));
        boundingBox.globalMax = glm::vec3(modelMatrix * glm::vec4(boundingBox.localMax, 1.0f));
    }

    // Function to draw the submodel
    void Draw(Shader& shader) {
        shader.use();
        shader.setMat4("model", modelMatrix);
        set_Pointlight_in_shader(pointLight, shader);
        for (unsigned int j = 0; j < meshes.size(); j++) {
            meshes[j].Draw(shader);
        }
    }

    void DrawLightCube(Shader& shader) {
        pointLight.light_cube.Draw(shader);
    }

private:
    glm::vec3 computeCentroid() {
        if (!meshes.empty()){
            glm::vec3 centroid(0.0f);
            for (const Mesh& mesh : meshes) {
                centroid += mesh.calculateCentroid();
            }
            centroid /= static_cast<float>(meshes.size());
            return centroid;
        }
        else{
            return glm::vec3(0.0f);
        }
    }

    void computeBoundingBox() {
        if (!meshes.empty()) {
            boundingBox = meshes[0].getBoundingBox();
            for (const Mesh& mesh : meshes) {
                AABB meshBox = mesh.getBoundingBox();
                boundingBox.localMin = glm::min(boundingBox.localMin, meshBox.localMin);
                boundingBox.localMax = glm::max(boundingBox.localMax, meshBox.localMax);
            }
            boundingBox.globalMin = boundingBox.localMax;
            boundingBox.globalMax = boundingBox.localMax;
        }
    }
};

class Model 
{
public:
    // model data 
    vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh> m;
    string directory;
    bool gammaCorrection;
    
    vector<SubModel> subModels;

    // constructor, expects a filepath to a 3D model.
    Model(string const &path, bool gamma = false) : gammaCorrection(gamma)
    {
        loadModel(path);
    }

    // draws the model, and thus all its meshes
    void Draw(Shader &shader)
    {
        for (SubModel& subModel: subModels)
            subModel.Draw(shader);
        
    }

    void DrawLightCubes(Shader &shader){
        for (SubModel& subModel : subModels) 
            subModel.DrawLightCube(shader);
        
    }

    void updateModels(float deltaTime) {
        for (SubModel& subModel: subModels) 
            subModel.update(deltaTime);
    }

    void randomLightDensity() {
        for (SubModel& subModel: subModels)
            update_indensity_random(subModel.pointLight);
        
    }

    void updateRandomMovement(){
        for (SubModel& subModel: subModels) {
            subModel.velocity = glm::vec3((rand() % 100 - 50) / 50.0f, (rand() % 100 - 50) / 50.0f, (rand() % 100 - 50) / 50.0f);
            subModel.angularVelocity = glm::vec3(0.0f, (rand() % 100 - 50) / 50.0f, 0.0f);
        }
    }

    void checkModelCollisions() {
        for (int i = 0; i < subModels.size(); ++i){
            AABB aabb1 = subModels[i].boundingBox;
            for (int j = i + 1; j < subModels.size(); ++j) {
                AABB aabb2 = subModels[j].boundingBox;
                if (intersect(aabb1, aabb2)) {
                    // std::cout<< "collide! \n";
                    subModels[i].velocity *= -1.0f;
                    subModels[j].velocity *= -1.0f;
                }
            }
        }
    }

    void checkRoomCollisions(const Room& room){
        for (int i = 0; i < subModels.size(); ++i){
            glm::vec3 a = checkBoxRoomCollisions(subModels[i].boundingBox, room);
            subModels[i].velocity *= a;
        }
    }
    
private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string const &path)
    {
        // read file via ASSIMP
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        // check for errors
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }
        // retrieve the directory path of the filepath
        directory = path.substr(0, path.find_last_of('/'));

        // process ASSIMP's root node recursively
        processNode(scene->mRootNode, scene);
    }

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode *node, const aiScene *scene)
    {
        // process each mesh located at the current node
        for(unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // the node object only contains indices to index the actual objects in the scene. 
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            m.push_back(processMesh(mesh, scene));
        }

        // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for(unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
            if (node->mNumChildren== 4){
                SubModel subModel(m, node->mName.C_Str());
                if (subModel.meshName=="Gost"){
                    subModel.pointLight.ambient = glm::vec3(1.0f);
                    subModel.pointLight.init_ambient = glm::vec3(1.0f);
                }
                subModels.push_back(subModel);
            }
            m.clear();
        }
    }

    Mesh processMesh(aiMesh *mesh, const aiScene *scene)
    {
        // data to fill
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        // walk through each of the mesh's vertices
        for(unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            // normals
            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }
            // texture coordinates
            if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x; 
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
                // tangent
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;
                // bitangent
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.Bitangent = vector;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }
        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for(unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for(unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);        
        }
        // process materials
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];    
        // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
        // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
        // Same applies to other texture as the following list summarizes:
        // diffuse: texture_diffuseN
        // specular: texture_specularN
        // normal: texture_normalN

        // 1. diffuse maps
        vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. specular maps
        vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // 3. normal maps
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 4. height maps
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        
        // return a mesh object created from the extracted mesh data
        return Mesh(vertices, indices, textures);
    }

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
    {
        vector<Texture> textures;
        for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            bool skip = false;
            for(unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                    break;
                }
            }
            if(!skip)
            {   // if texture hasn't been loaded already, load it
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
            }
        }
        return textures;
    }
};


unsigned int TextureFromFile(const char *path, const string &directory, bool gamma)
{
    string filename = string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

#endif
