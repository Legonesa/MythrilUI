#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include<glad/glad.h>
#include<iostream>
#include<string>

// Wrapper using stb_image to load image files directly into GPU VRAM
class Texture
{
private:
    unsigned int textureID; // GPU-assigned texture memory address
public:
    int width, height;      // Stored image resolution metrics

    // Initializes stb_image, flips Y-coordinates on load, and builds mipmaps
    Texture(const char* image_path);

    // Configures wrapping parameters or texture scaling filtering modes
    void setTextureParametri(GLenum parametriName, GLint paramValue);

    // Binds the texture directly to a target active slot unit (e.g. GL_TEXTURE1)
    void useTextureAs(GLenum textureNumber);

    // Standard binding
    void use();

    // Secure GPU buffer release wrapper
    void Delete();
    ~Texture();
};
#endif