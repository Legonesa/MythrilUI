#define STB_IMAGE_IMPLEMENTATION

#include<texture.hpp>
#include<glad/glad.h>
#include<iostream>
#include<string>
#include "stb_image.h"

// =====================================================================
// STB_IMAGE LOADER & GPU VRAM ALLOCATOR
// =====================================================================
Texture::Texture(const char* image_path){
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    // Standard Wrap parameters (Repeat texture if coordinates exceed 1.0)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Linear filtering to keep textures relatively smooth when scaled
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int nrChannels, rwidth, rheight;

    // OpenGL expects the 0.0 coordinate on the Y-axis to be on the bottom, 
    // but images usually have 0.0 at the top. We must flip it.
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(image_path, &rwidth, &rheight, &nrChannels, 0);
    this->width = rwidth; this->height = rheight;

    // Push image data from CPU RAM to GPU VRAM
    if(data){
        // Automatically determine format (with or without Alpha channel/transparency)
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D); // Generate lower-res versions for distant rendering
    }
    else{
        std::cout << "Failed to read image!" << std::endl;
    }

    // Free CPU memory since the GPU now has the data
    stbi_image_free(data);
}

void Texture::use(){
    glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture::setTextureParametri(GLenum parametriName, GLint paramValue){
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, parametriName, paramValue);
}

void Texture::useTextureAs(GLenum textureNumber){
    glActiveTexture(textureNumber); // Bind to specific slot (e.g. GL_TEXTURE0)
    glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture::Delete(){
    this->~Texture();
}

Texture::~Texture(){
    glDeleteTextures(1, &textureID);
}