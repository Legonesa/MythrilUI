#include<glad/glad.h>
#include<iostream>
#include<string>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<transform.hpp>

// =====================================================================
// MATRIX TRANSFORMATION UTILITIES
// Abstracts away complex GLM matrix math for easier UI layout manipulation
// =====================================================================
void Transform::reset(){
    trans = glm::mat4(1.0f); // Reset to Identity matrix
}

// Rotates around a specific world axis
void Transform::rotate(float rotation, Lines line){
    if(line == X) trans = glm::rotate(trans, rotation, glm::vec3(1.0f, 0.0f, 0.0f));
    if(line == Y) trans = glm::rotate(trans, rotation, glm::vec3(0.0f, 1.0f, 0.0f));
    if(line == Z) trans = glm::rotate(trans, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
}
void Transform::rotate(float rotation, Lines line1, Lines line2){
    glm::vec3 rotationVector = glm::vec3(0.0f, 0.0f, 0.0f);
    if(line1 == X || line2 == X) rotationVector.x = 1.0f;
    if(line1 == Y || line2 == Y) rotationVector.y = 1.0f;
    if(line1 == Z || line2 == Z) rotationVector.z = 1.0f;
    trans = glm::rotate(trans, rotation, rotationVector);
}
void Transform::rotate(float rotation){
    trans = glm::rotate(trans, rotation, glm::vec3(1.0f, 1.0f, 1.0f));
}
void Transform::translate(glm::vec3 transform){
    trans = glm::translate(trans, transform);
}

// Modifies a specific raw float value inside the 4x4 Matrix directly.
// This is used extensively in MythrilUI to correct widescreen Aspect Ratios!
void Transform::setValue(int line, int row, float value){
    trans[line][row] = value;
}

// Builds a 3D Perspective Projection matrix
void Transform::perspective(float fov, float widht, float height, float near, float far){
    trans = glm::perspective(fov, widht/height, near, far);
}

glm::mat4 Transform::getTransform(){
    return trans;
}