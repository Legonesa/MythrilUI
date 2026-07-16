#ifndef MYTHRIL_TEXT_HPP
#define MYTHRIL_TEXT_HPP

#include <iostream>
#include <MythrilUI.hpp>
#include <MythrilUITYPE.hpp>

// Open-source Roboto default font settings to prevent potential copyright issues
static std::string defaultFontPath = "fonts/Roboto-Regular.ttf";
static unsigned int defaultFontSize = 48;

// Set fallback default font face configurations
void defaultFont(std::string fontPath);
void ChangeDefFontSize(unsigned int fontSize);

// Standalone widget representing text blocks
class Text
{
private:
    Mythril_TEXT* TX_OBJ = new Mythril_TEXT;  // Internal parameter container (size, color, letters)
    MythrilUI* UI;                         // Pointer to active viewport frame manager
public:
    // Overloaded constructors to allow layout customization options
    Text(MythrilUI* UIw, std::string text, float positionX, float positionY, float scale, glm::vec3 color);
    Text(MythrilUI* UIw, std::string fontPath, std::string text, float positionX, float positionY, float scale, glm::vec3 color);
    Text(MythrilUI* UIw, std::string fontPath, std::string text, glm::vec2 position, float scale, glm::vec3 color);
    Text(MythrilUI* UIw, std::string text, glm::vec2 position, float scale, glm::vec3 color);
    
    // Changes the mapped font style dynamically
    void ChangeFont(std::string fontPath);

    // Dynamic state modifiers
    void UpdateText(std::string text);
    void UpdateScale(float scale);
    void UpdatePosition(glm::vec2 position);
    void UpdatePosition(float x, float y);

    // Toggles text rendering on and off
    void enable(bool value);

    // Secure dynamic allocations release wrappers
    void Delete();
    ~Text();
};

#endif