#ifndef MYTHRIL_IMAGE_HPP
#define MYTHRIL_IMAGE_HPP

#include <iostream>
#include <GLFW/glfw3.h>
#include <MythrilUI.hpp>
#include <MythrilUITYPE.hpp>

// Standalone widget wrapping 2D static images
class Image
{
private:
    MythrilUI* UI;                             // Associated parent UI viewport frame
    Mythril_IMAGE* IMG_OBJ = new Mythril_IMAGE;   // Internal parameter block (buffers, dimensions)
public:
    // Overloaded constructors supporting different positioning styles and coordinates
    Image(MythrilUI* UIw, float width, float height, float positionX, float positionY, const char* ImagePath);
    Image(MythrilUI* UIw, float width, float height, glm::vec2 position, const char* ImagePath);
    Image(MythrilUI* UIw, glm::vec2 size, glm::vec2 position, const char* ImagePath);
    Image(MythrilUI* UIw, glm::vec2 position, const char* ImagePath);
    Image(MythrilUI* UIw, float positionX, float positionY, const char* ImagePath);
    
    // Dynamic geometry recalculations (re-calculates coordinates when resized)
    void UpdateSize(glm::vec2 size);
    void UpdateSize(float x, float y);
    void UpdatePosition(glm::vec2 position);
    void UpdatePosition(float x, float y);

    // Switches the rendered image texture dynamically
    void UpdateTexture(const char* ImagePath);

    // Visibility management
    void enable(bool value);

    // Dynamic memory cleanup wrappers
    void Delete();
    ~Image();
};

#endif