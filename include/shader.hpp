#ifndef SHADER_HPP
#define SHADER_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

// Abstraction layer managing compiling and uniform loading for OpenGL GLSL programs
class Shader
{
public:
    unsigned int progID; // GPU compiler compiled shader program identifier
    
    // Opens files, builds pipeline compilation trees and reports link failures
    Shader(const char* vertexPath, const char* fragPath);

    // Binds the shading logic into the active GL context
    void use();

    // Uniform value setters to communicate parameters between CPU and GPU uniforms
    void setBool(const std::string &name, bool value);
    void setInt(const std::string &name, int value);
    void setFloat(const std::string &name, float value);
    void setVec2(const std::string &name, glm::vec2 &value);
    void setVec2(const std::string &name, float x, float y);
    void setVec3(const std::string &name, glm::vec3 &value);
    void setVec3(const std::string &name, float x, float y, float z);
    void setVec4(const std::string &name, glm::vec4 &value);
    void setVec4(const std::string &name, float x, float y, float z, float w);
    void setMat2(const std::string &name, glm::mat2 &value);
    void setMat3(const std::string &name, glm::mat3 &value);
    void setMat4(const std::string &name, glm::mat4 &value);
};

#endif