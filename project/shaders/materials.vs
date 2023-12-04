#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal; // Normal vector
layout (location = 2) in vec2 aTexCoords;

out VS_OUT {
    vec2 texCoords;
    vec3 fragPos;   // Fragment position
    vec3 normal;    // Normal vector
} vs_out;

uniform mat4 transform;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    vs_out.texCoords = aTexCoords;
    vec4 worldPos = model * vec4(aPos, 1.0); // Transform vertex position to world space
    vs_out.fragPos = worldPos.xyz;
    vs_out.normal = mat3(transpose(inverse(model))) * aNormal; // Transform normals to world space
    gl_Position = projection * view * worldPos; 
}
