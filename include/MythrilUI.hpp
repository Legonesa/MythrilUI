#ifndef MYTHRILUI_HPP
#define MYTHRILUI_HPP

#include <iostream>
#include <variant>
#include <vector>
#include <MythrilUITYPE.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <transform.hpp>

// Core UI Engine class that manages rendering pipelines of stable modular widgets
class MythrilUI
{
private:
    
public:
    GLFWwindow* window; // Target GLFW window handle context

    // Type-safe vector pool storing pointers to active widgets in our tree
    std::vector<std::variant<Mythril_IMAGE*, Mythril_TEXT*, Mythril_BUTTON*>> Objects;

    MythrilUI(GLFWwindow* Window);

    // Loops through the Objects pool and issues respective OpenGL draw calls
    void RenderUI();
};

// =====================================================================
// RESOURCE CREATION INLINE HELPERS
// =====================================================================

// Instantiates a new Font face and adds its reference directly into the global pool
int CreateFont(std::string fontPath, unsigned int fontSize){
    int index = Fonts.size();
    Fonts.push_back(new Font);
    Fonts.at(index)->index = index;
    Fonts.at(index)->FontPath = fontPath;
    Fonts.at(index)->fontSize = fontSize;
    Fonts.at(index)->Renderer = new TextRendeer(Fonts.at(index)->FontPath, Fonts.at(index)->fontSize);
    std::cout << "Created font! " << Fonts.size() << std::endl;
    return index;
}

// Loads a brand-new texture from local storage using stb_image into the GPU
int CreateTexture(const char* texturePath){
    int index = Textures.size();
    Textures.push_back(new Mythril_Texture);
    Textures.at(index)->index = index;
    Textures.at(index)->ImagePath = texturePath;
    Textures.at(index)->Texture = new Texture(texturePath);
    std::cout << "Created Texture! " << std::endl;
    return index;
}

#endif