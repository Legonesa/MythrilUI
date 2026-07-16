#ifndef MYTHRILUITYPE_HPP
#define MYTHRILUITYPE_HPP

#include <iostream>
#include <texture.hpp>
#include <text.hpp>
#include <variant>

// =====================================================================
// FONTS & TEXTURES RESOURCE STRUCTURES
// =====================================================================

// Structure to store loaded FreeType font properties and its rendering class
struct Font
{
    unsigned int index;     // ID index of this font inside the global Fonts pool
    TextRendeer* Renderer;  // Pointer to the FreeType rendering layout class
    std::string FontPath;   // Target file path of the loaded .ttf font
    unsigned int fontSize;  // Loaded pixel size of the glyph characters
};

// Represents a loaded 2D texture asset with metadata
struct Mythril_Texture
{
    Texture* Texture;       // Pointer to low-level OpenGL Texture memory
    const char* ImagePath;  // Resource file path of the texture
    int index;              // Texture ID index inside the global Textures pool
};

// =====================================================================
// UI OBJECT DATA DEFINITIONS
// =====================================================================

// Data parameters of a static 2D image component
struct Mythril_IMAGE
{
    unsigned int index;             // Render tree hierarchy order index
    unsigned int VAO, VBO;          // Vertex Array Object and Vertex Buffer Object handles
    bool isEnable = true;           // Rendering state toggle (drawn on screen only when true)
    float Position[2];              // Screen coordinate position offset (X, Y)
    float Size[2];                  // Geometry dimensions (width, height)
    Mythril_Texture* texture = NULL;   // Texture asset used for this image component
};

// Data parameters of a static or dynamic text label component
struct Mythril_TEXT
{
    Font* font = NULL;      // Assigned font face asset from the global pool
    std::string text;       // The character string array to be rendered on screen
    bool isEnable = true;   // Rendering state visibility flag
    unsigned int index;     // Render hierarchy order index
    float Position[2];      // Text baseline starting coordinates
    float scale;            // Scale multiplier of the letters
    float color[3];         // Text color values normalized in RGB [0.0 - 1.0]
};

// Data parameters of a static button component
struct Mythril_BUTTON
{
    Mythril_TEXT* BUTTON_text = new Mythril_TEXT;     // Internal text widget representing the label
    bool isEnable = true;                       // Interaction and visibility state toggle
    unsigned int VAO, VBO, index;               // Graphic buffer objects and hierarchy indices
    float Position[2];                          // Layout placement coordinates
    float Size[2];                              // Button boundary dimensions
    std::variant<float* , Mythril_Texture*> skin;  // Dynamic background skin: either solid color or texture asset
};

// =====================================================================
// GLOBAL RESOURCE POOLS
// =====================================================================
static std::vector<Font*> Fonts;            // Cached list of unique active fonts
static std::vector<Mythril_Texture*> Textures; // Cached list of unique loaded textures
static Shader* UIShader = NULL;             // Unified GLSL shader program for 2D UI render


#endif