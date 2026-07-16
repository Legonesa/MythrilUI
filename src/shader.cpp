#include<shader.hpp>

// =====================================================================
// SHADER COMPILER PIPELINE
// Reads raw GLSL code from disk, compiles it into GPU binaries, 
// and links them into an executable shader program.
// =====================================================================
Shader::Shader(const char* vertexPath, const char* fragPath){
    std::string vertexCode;
    std::string fragCode;
    std::ifstream vertexShaderFile;
    std::ifstream fragShaderFile;

    // Enable exceptions to catch file IO errors
    vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fragShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        vertexShaderFile.open(vertexPath);
        fragShaderFile.open(fragPath);
        std::stringstream vertexStream, fragStream;

        vertexStream << vertexShaderFile.rdbuf();
        fragStream << fragShaderFile.rdbuf();

        vertexShaderFile.close();
        fragShaderFile.close();

        vertexCode = vertexStream.str();
        fragCode = fragStream.str();
    }
    catch(const std::exception& e)
    {
        std::cout << "SHADER FILE READ ERROR!" << std::endl;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragCode.c_str();

    // 1. Compile Vertex Shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glCompileShader(vertexShader);
    int status;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
    if(!status){
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << infoLog << std::endl;
    }

    // 2. Compile Fragment Shader
    unsigned int fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fShaderCode, NULL);
    glCompileShader(fragShader);
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &status);
    if(!status){
        char infoLog[512];
        glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
        std::cout << infoLog << std::endl;
    }

    // 3. Link Shaders into Program
    progID = glCreateProgram();
    glAttachShader(progID, vertexShader);
    glAttachShader(progID, fragShader);
    glLinkProgram(progID);
    glGetProgramiv(progID, GL_LINK_STATUS, &status);
    if(!status){
        char infoLog[512];
        glGetProgramInfoLog(progID, 512, NULL, infoLog);
        std::cout << infoLog << std::endl;
    }

    // Clean up individual shaders as they are now linked into the program
    glDeleteShader(fragShader);
    glDeleteShader(vertexShader);
}

// =====================================================================
// UNIFORM SETTERS (Passes data from CPU to GPU registers)
// =====================================================================
void Shader::use(){
    glUseProgram(progID);
}
void Shader::setBool(const std::string &name, bool value){
    glUniform1i(glGetUniformLocation(progID, name.c_str()), int(value));
}
void Shader::setInt(const std::string &name, int value){
    glUniform1i(glGetUniformLocation(progID, name.c_str()), value);
}
void Shader::setFloat(const std::string &name, float value){
    glUniform1f(glGetUniformLocation(progID, name.c_str()), value);
}
void Shader::setVec2(const std::string &name, glm::vec2 &value){
    glUniform2fv(glGetUniformLocation(progID, name.c_str()), 1, &value[0]);
}
void Shader::setVec2(const std::string &name, float x, float y){
    glUniform2f(glGetUniformLocation(progID, name.c_str()), x, y);
}
void Shader::setVec3(const std::string &name, glm::vec3 &value){
    glUniform3fv(glGetUniformLocation(progID, name.c_str()), 1, &value[0]);
}
void Shader::setVec3(const std::string &name, float x, float y, float z){
    glUniform3f(glGetUniformLocation(progID, name.c_str()), x, y, z);
}
void Shader::setVec4(const std::string &name, glm::vec4 &value){
    glUniform4fv(glGetUniformLocation(progID, name.c_str()), 1, &value[0]);
}
void Shader::setVec4(const std::string &name, float x, float y, float z, float w){
    glUniform4f(glGetUniformLocation(progID, name.c_str()), x, y, z, w);
}
void Shader::setMat2(const std::string &name, glm::mat2 &value){
    glUniformMatrix2fv(glGetUniformLocation(progID, name.c_str()), 1, GL_FALSE, &value[0][0]);
}
void Shader::setMat3(const std::string &name, glm::mat3 &value){
    glUniformMatrix3fv(glGetUniformLocation(progID, name.c_str()), 1, GL_FALSE, &value[0][0]);
}
void Shader::setMat4(const std::string &name, glm::mat4 &value){
    glUniformMatrix4fv(glGetUniformLocation(progID, name.c_str()), 1, GL_FALSE, &value[0][0]);
}