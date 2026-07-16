#ifndef TEXT_HPP
#define TEXT_HPP

#include <map>
#include <texture.hpp>
#include <shader.hpp>
#include <iostream>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <vector>
#include <glm/gtc/matrix_transform.hpp>


// Dedicated text rendering shader program pointer
static Shader* TextShader = NULL;

// Metric layout attributes for a loaded single FreeType glyph
struct Character
{
    unsigned int TextureID; // Grayscale texture identifier generated on character load
    glm::ivec2 Size;        // Pixel dimensions of glyph bitmap (width, height)
    glm::ivec2 Bearing;     // Horizontal/Vertical alignments relative to baseline
    unsigned int Advance;   // Space to move to print the next letter (in 1/64th pixels)
};

// Font layout calculator and GPU renderer using FreeType buffers
class TextRendeer
{
public:
    std::map<char, Character> Characters;   // Cache storing character mapping details
    unsigned int fontSIZE;                  // Configured fallback character height

    // Loads face definitions and compiles ASCII glifs into textures
    TextRendeer(std::string font, unsigned int fontSize);

    // Base Draw: Simple single-line text printing layout
    void RenderText(unsigned int width, unsigned int height, std::string text, float x, float y, float scale, glm::vec3 color = glm::vec3(1.0f));
    
    // Multiline Wrap: Fits sentences inside box margins, injecting "..." dynamically if text overflows
    void RenderText(unsigned int width, unsigned int height, std::string text, float x, float y, float scale, float maxX, float maxY, glm::vec3 color = glm::vec3(1.0f));
    
    // Input Alignment Draw: Constrains dynamic texts from right to left inside input fields
    void RenderTextBOX(unsigned int width, unsigned int height, std::string text, float x, float y, float scale, float maxX, float maxY, float _TEXTBOXverticalMARGIN_, glm::vec3 color = glm::vec3(1.0f));
    
    // Helper: Returns pixel width of a specific string
    float getTextWidth(std::string text, float scale);
    
    // Helper: Calculates lines needed to wrap words inside constraints
    int GetLineCount(std::string text, float scale, float maxX, float maxY, std::vector<std::string>* words);
private:
    unsigned int VAO, VBO; // Dynamic layout buffers updated per glyph during rendering
};


#endif