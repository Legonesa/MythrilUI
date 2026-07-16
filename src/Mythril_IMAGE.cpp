#include <iostream>
#include <glm/glm.hpp>
#include <MythrilUI.hpp>
#include <GLFW/glfw3.h>
#include <Mythril_IMAGE.hpp>

// =====================================================================
// IMAGE WIDGET CONSTRUCTORS
// Builds a standard 2D Quad (2 Triangles, 6 Indices) mapped to UI space
// =====================================================================
Image::Image(MythrilUI* UIw, float width, float height, float positionX, float positionY, const char* ImagePath){
    int windowWidth, windowHeight; UI = UIw;
    glfwGetWindowSize(UI->window, &windowWidth, &windowHeight);
    
    // Store original pixel dimensions and coordinates
    IMG_OBJ->Position[0] = positionX; IMG_OBJ->Position[1] = positionY;
    IMG_OBJ->Size[0] = width; IMG_OBJ->Size[1] = height;
    
    // Normalize coordinates based on screen aspect ratio to prevent stretching
    float RPositionX = 2*(positionX/(float)windowHeight);
    float RPositionY = 2*(positionY/(float)windowHeight);
    float Rwidth = 2*(width/(float)windowHeight);
    float Rheight = 2*(height/(float)windowHeight);

    // Define 4 corners of the quad (X, Y, Z, U, V)
    float vertices[] = {
        RPositionX + (Rwidth/2), RPositionY + (Rheight/2), 0.0f, 1.0f, 1.0f, // Top Right
        RPositionX + (Rwidth/2), RPositionY - (Rheight/2), 0.0f, 1.0f, 0.0f, // Bottom Right
        RPositionX - (Rwidth/2), RPositionY - (Rheight/2), 0.0f, 0.0f, 0.0f, // Bottom Left
        RPositionX - (Rwidth/2), RPositionY + (Rheight/2), 0.0f, 0.0f, 1.0f  // Top Left
    };
    unsigned int EBO0;
    static unsigned int indices0[] = {  
        0, 1, 3, // First Triangle
        1, 2, 3  // Second Triangle
    };
    IMG_OBJ->index = UI->Objects.size();

    // Generate and bind OpenGL buffers
    glGenVertexArrays(1, &IMG_OBJ->VAO);
    glGenBuffers(1, &EBO0);
    glGenBuffers(1, &IMG_OBJ->VBO);
    glBindVertexArray(IMG_OBJ->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, IMG_OBJ->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO0);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices0), indices0, GL_STATIC_DRAW);
    
    // Position attribute (Location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Texture Coordinate attribute (Location 1)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // Fetch texture from global pool or load it if it doesn't exist
    for(int i = 0; i < Textures.size(); i++){
        if(ImagePath == Textures.at(i)->ImagePath){
            IMG_OBJ->texture = Textures.at(i);
            std::cout << "Texture found!" << std::endl;
            break;
        }
    }
    if(IMG_OBJ->texture == NULL){
        IMG_OBJ->texture = Textures.at(CreateTexture(ImagePath));
    }
    UI->Objects.push_back(IMG_OBJ);
}
Image::Image(MythrilUI* UIw, float width, float height, glm::vec2 position, const char* ImagePath){
    int windowWidth, windowHeight; UI = UIw;
    glfwGetWindowSize(UI->window, &windowWidth, &windowHeight);
    IMG_OBJ->Position[0] = position.x; IMG_OBJ->Position[1] = position.y;
    IMG_OBJ->Size[0] = width; IMG_OBJ->Size[1] = height;
    float RPositionX = 2*(position.x/(float)windowHeight);
    float RPositionY = 2*(position.y/(float)windowHeight);
    float Rwidth = 2*(width/(float)windowHeight);
    float Rheight = 2*(height/(float)windowHeight);
    float vertices[] = {
        RPositionX + (Rwidth/2), RPositionY + (Rheight/2), 0.0f, 1.0f, 1.0f,
        RPositionX + (Rwidth/2), RPositionY - (Rheight/2), 0.0f, 1.0f, 0.0f,
        RPositionX - (Rwidth/2), RPositionY - (Rheight/2), 0.0f, 0.0f, 0.0f,
        RPositionX - (Rwidth/2), RPositionY + (Rheight/2), 0.0f, 0.0f, 1.0f
    };
    unsigned int EBO0;
    static unsigned int indices0[] = {  
        0, 1, 3,  
        1, 2, 3
    };
    IMG_OBJ->index = UI->Objects.size();
    glGenVertexArrays(1, &IMG_OBJ->VAO);
    glGenBuffers(1, &EBO0);
    glGenBuffers(1, &IMG_OBJ->VBO);
    glBindVertexArray(IMG_OBJ->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, IMG_OBJ->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO0);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices0), indices0, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    for(int i = 0; i < Textures.size(); i++){
        if(ImagePath == Textures.at(i)->ImagePath){
            IMG_OBJ->texture = Textures.at(i);
            std::cout << "Texture found!" << std::endl;
            break;
        }
    }
    if(IMG_OBJ->texture == NULL){
        IMG_OBJ->texture = Textures.at(CreateTexture(ImagePath));
    }
    UI->Objects.push_back(IMG_OBJ);
}
Image::Image(MythrilUI* UIw, glm::vec2 size, glm::vec2 position, const char* ImagePath){
    int windowWidth, windowHeight; UI = UIw;
    glfwGetWindowSize(UI->window, &windowWidth, &windowHeight);
    IMG_OBJ->Position[0] = position.x; IMG_OBJ->Position[1] = position.y;
    IMG_OBJ->Size[0] = size.x; IMG_OBJ->Size[1] = size.y;
    float RPositionX = 2*(position.x/(float)windowHeight);
    float RPositionY = 2*(position.y/(float)windowHeight);
    float Rwidth = 2*(size.x/(float)windowHeight);
    float Rheight = 2*(size.y/(float)windowHeight);
    float vertices[] = {
        RPositionX + (Rwidth/2), RPositionY + (Rheight/2), 0.0f, 1.0f, 1.0f,
        RPositionX + (Rwidth/2), RPositionY - (Rheight/2), 0.0f, 1.0f, 0.0f,
        RPositionX - (Rwidth/2), RPositionY - (Rheight/2), 0.0f, 0.0f, 0.0f,
        RPositionX - (Rwidth/2), RPositionY + (Rheight/2), 0.0f, 0.0f, 1.0f
    };
    unsigned int EBO0;
    static unsigned int indices0[] = {  
        0, 1, 3,  
        1, 2, 3
    };
    IMG_OBJ->index = UI->Objects.size();
    glGenVertexArrays(1, &IMG_OBJ->VAO);
    glGenBuffers(1, &EBO0);
    glGenBuffers(1, &IMG_OBJ->VBO);
    glBindVertexArray(IMG_OBJ->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, IMG_OBJ->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO0);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices0), indices0, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    for(int i = 0; i < Textures.size(); i++){
        if(ImagePath == Textures.at(i)->ImagePath){
            IMG_OBJ->texture = Textures.at(i);
            std::cout << "Texture found!" << std::endl;
            break;
        }
    }
    if(IMG_OBJ->texture == NULL){
        IMG_OBJ->texture = Textures.at(CreateTexture(ImagePath));
    }
    UI->Objects.push_back(IMG_OBJ);
}
Image::Image(MythrilUI* UIw, glm::vec2 position, const char* ImagePath){
    int windowWidth, windowHeight;
    IMG_OBJ->index = UI->Objects.size();
    for(int i = 0; i < Textures.size(); i++){
        if(ImagePath == Textures.at(i)->ImagePath){
            IMG_OBJ->texture = Textures.at(i);
            std::cout << "Texture found!" << std::endl;
            break;
        }
    }
    if(IMG_OBJ->texture == NULL){
        IMG_OBJ->texture = Textures.at(CreateTexture(ImagePath));
    }
    UI = UIw;
    glfwGetWindowSize(UI->window, &windowWidth, &windowHeight);
    float RPositionX = 2*(position.x/(float)windowHeight);
    float RPositionY = 2*(position.y/(float)windowHeight);
    IMG_OBJ->Position[0] = position.x; IMG_OBJ->Position[1] = position.y;
    IMG_OBJ->Size[0] = IMG_OBJ->texture->Texture->width; IMG_OBJ->Size[1] = IMG_OBJ->texture->Texture->height;
    float Rwidth = 2*(IMG_OBJ->texture->Texture->width/(float)windowHeight);
    float Rheight = 2*(IMG_OBJ->texture->Texture->height/(float)windowHeight);
    float vertices[] = {
        RPositionX + (Rwidth/2), RPositionY + (Rheight/2), 0.0f, 1.0f, 1.0f,
        RPositionX + (Rwidth/2), RPositionY - (Rheight/2), 0.0f, 1.0f, 0.0f,
        RPositionX - (Rwidth/2), RPositionY - (Rheight/2), 0.0f, 0.0f, 0.0f,
        RPositionX - (Rwidth/2), RPositionY + (Rheight/2), 0.0f, 0.0f, 1.0f
    };
    unsigned int EBO0;
    static unsigned int indices0[] = {  
        0, 1, 3,  
        1, 2, 3
    };
    glGenVertexArrays(1, &IMG_OBJ->VAO);
    glGenBuffers(1, &EBO0);
    glGenBuffers(1, &IMG_OBJ->VBO);
    glBindVertexArray(IMG_OBJ->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, IMG_OBJ->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO0);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices0), indices0, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    UI->Objects.push_back(IMG_OBJ);
}
Image::Image(MythrilUI* UIw, float positionX, float positionY, const char* ImagePath){
    int windowWidth, windowHeight;
    IMG_OBJ->index = UI->Objects.size();
    for(int i = 0; i < Textures.size(); i++){
        if(ImagePath == Textures.at(i)->ImagePath){
            IMG_OBJ->texture = Textures.at(i);
            std::cout << "Texture found!" << std::endl;
            break;
        }
    }
    if(IMG_OBJ->texture == NULL){
        IMG_OBJ->texture = Textures.at(CreateTexture(ImagePath));
    }
    UI = UIw;
    glfwGetWindowSize(UI->window, &windowWidth, &windowHeight);
    float RPositionX = 2*(positionX/(float)windowHeight);
    float RPositionY = 2*(positionY/(float)windowHeight);
    IMG_OBJ->Position[0] = positionX; IMG_OBJ->Position[1] = positionY;
    IMG_OBJ->Size[0] = IMG_OBJ->texture->Texture->width; IMG_OBJ->Size[1] = IMG_OBJ->texture->Texture->height;
    float Rwidth = 2*(IMG_OBJ->texture->Texture->width/(float)windowHeight);
    float Rheight = 2*(IMG_OBJ->texture->Texture->height/(float)windowHeight);
    float vertices[] = {
        RPositionX + (Rwidth/2), RPositionY + (Rheight/2), 0.0f, 1.0f, 1.0f,
        RPositionX + (Rwidth/2), RPositionY - (Rheight/2), 0.0f, 1.0f, 0.0f,
        RPositionX - (Rwidth/2), RPositionY - (Rheight/2), 0.0f, 0.0f, 0.0f,
        RPositionX - (Rwidth/2), RPositionY + (Rheight/2), 0.0f, 0.0f, 1.0f
    };
    unsigned int EBO0;
    static unsigned int indices0[] = {  
        0, 1, 3,  
        1, 2, 3
    };
    glGenVertexArrays(1, &IMG_OBJ->VAO);
    glGenBuffers(1, &EBO0);
    glGenBuffers(1, &IMG_OBJ->VBO);
    glBindVertexArray(IMG_OBJ->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, IMG_OBJ->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO0);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices0), indices0, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    UI->Objects.push_back(IMG_OBJ);
}

// =====================================================================
// DYNAMIC MODIFIERS
// Updates VBO data dynamically when widget is resized or moved
// =====================================================================
void Image::UpdateSize(float sizeX, float sizeY){
    int windowWidth, windowHeight;
    glfwGetWindowSize(UI->window, &windowWidth, &windowHeight);
    float RPositionX = 2*(IMG_OBJ->Position[0]/(float)windowHeight);
    float RPositionY = 2*(IMG_OBJ->Position[1]/(float)windowHeight);
    IMG_OBJ->Size[0] = sizeX; IMG_OBJ->Size[1] = sizeY;
    float Rwidth = 2*(sizeX/(float)windowHeight);
    float Rheight = 2*(sizeY/(float)windowHeight);

    // Recalculate geometry
    float vertices[] = {
        RPositionX + (Rwidth/2), RPositionY + (Rheight/2), 0.0f, 1.0f, 1.0f,
        RPositionX + (Rwidth/2), RPositionY - (Rheight/2), 0.0f, 1.0f, 0.0f,
        RPositionX - (Rwidth/2), RPositionY - (Rheight/2), 0.0f, 0.0f, 0.0f,
        RPositionX - (Rwidth/2), RPositionY + (Rheight/2), 0.0f, 0.0f, 1.0f
    };

    // Update GPU Buffer with new vertices
    glBindVertexArray(IMG_OBJ->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, IMG_OBJ->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindVertexArray(0);
}
void Image::UpdateSize(glm::vec2 size){
    int windowWidth, windowHeight;
    glfwGetWindowSize(UI->window, &windowWidth, &windowHeight);
    float RPositionX = 2*(IMG_OBJ->Position[0]/(float)windowHeight);
    float RPositionY = 2*(IMG_OBJ->Position[1]/(float)windowHeight);
    IMG_OBJ->Size[0] = size.x; IMG_OBJ->Size[1] = size.y;
    float Rwidth = 2*(size.x/(float)windowHeight);
    float Rheight = 2*(size.y/(float)windowHeight);
    float vertices[] = {
        RPositionX + (Rwidth/2), RPositionY + (Rheight/2), 0.0f, 1.0f, 1.0f,
        RPositionX + (Rwidth/2), RPositionY - (Rheight/2), 0.0f, 1.0f, 0.0f,
        RPositionX - (Rwidth/2), RPositionY - (Rheight/2), 0.0f, 0.0f, 0.0f,
        RPositionX - (Rwidth/2), RPositionY + (Rheight/2), 0.0f, 0.0f, 1.0f
    };
    glBindVertexArray(IMG_OBJ->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, IMG_OBJ->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindVertexArray(0);
}
void Image::UpdatePosition(glm::vec2 position){
    int windowWidth, windowHeight;
    glfwGetWindowSize(UI->window, &windowWidth, &windowHeight);
    float RPositionX = 2*(position.x/(float)windowHeight);
    float RPositionY = 2*(position.y/(float)windowHeight);
    IMG_OBJ->Position[0] = position.x; IMG_OBJ->Position[1] = position.y;
    float Rwidth = 2*(IMG_OBJ->Size[0]/(float)windowHeight);
    float Rheight = 2*(IMG_OBJ->Size[1]/(float)windowHeight);
    float vertices[] = {
        RPositionX + (Rwidth/2), RPositionY + (Rheight/2), 0.0f, 1.0f, 1.0f,
        RPositionX + (Rwidth/2), RPositionY - (Rheight/2), 0.0f, 1.0f, 0.0f,
        RPositionX - (Rwidth/2), RPositionY - (Rheight/2), 0.0f, 0.0f, 0.0f,
        RPositionX - (Rwidth/2), RPositionY + (Rheight/2), 0.0f, 0.0f, 1.0f
    };
    glBindVertexArray(IMG_OBJ->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, IMG_OBJ->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindVertexArray(0);
}
void Image::UpdatePosition(float positionX, float positionY){
    int windowWidth, windowHeight;
    glfwGetWindowSize(UI->window, &windowWidth, &windowHeight);
    float RPositionX = 2*(positionX/(float)windowHeight);
    float RPositionY = 2*(positionY/(float)windowHeight);
    IMG_OBJ->Position[0] = positionX; IMG_OBJ->Position[1] = positionY;
    float Rwidth = 2*(IMG_OBJ->Size[0]/(float)windowHeight);
    float Rheight = 2*(IMG_OBJ->Size[1]/(float)windowHeight);
    float vertices[] = {
        RPositionX + (Rwidth/2), RPositionY + (Rheight/2), 0.0f, 1.0f, 1.0f,
        RPositionX + (Rwidth/2), RPositionY - (Rheight/2), 0.0f, 1.0f, 0.0f,
        RPositionX - (Rwidth/2), RPositionY - (Rheight/2), 0.0f, 0.0f, 0.0f,
        RPositionX - (Rwidth/2), RPositionY + (Rheight/2), 0.0f, 0.0f, 1.0f
    };
    glBindVertexArray(IMG_OBJ->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, IMG_OBJ->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindVertexArray(0);
}
void Image::UpdateTexture(const char* ImagePath){
    // Smart Garbage Collection: Only delete old texture from VRAM if no other widget is using it
    if(ImagePath != IMG_OBJ->texture->ImagePath){
        bool isChanged = false, TextureUsing = false;

        // Scan active pool to see if old texture is shared
        for(int i = 0; i < UI->Objects.size(); i++){
            if(std::holds_alternative<Mythril_IMAGE*>(UI->Objects.at(i)))
                if(std::get<Mythril_IMAGE*>(UI->Objects.at(i))->texture == IMG_OBJ->texture && IMG_OBJ->index != i){
                    TextureUsing = true;
                }
        }

        // Free memory if texture is isolated
        if(TextureUsing == false){
            if(Textures.size() == 1){
                Textures.pop_back();
                std::cout << "New Size of Textures:" << Textures.size() << std::endl;
            }
            else{
                // Shift array to close the gap
                for(int i = IMG_OBJ->texture->index; i < Textures.size()-1; i++){
                    Textures.at(i) = Textures.at(i+1);
                    Textures.at(i)->index--;
                }
                Textures.pop_back();
                std::cout << "New Size of Textures:" << Textures.size() << std::endl;
            }
            delete IMG_OBJ->texture->Texture; // Release GPU VRAM
            delete IMG_OBJ->texture;
        }

        // Assign new texture
        for(int i = 0; i < Textures.size(); i++){
            if(ImagePath == Textures.at(i)->ImagePath){
                IMG_OBJ->texture = Textures.at(i);
                isChanged = true;
                std::cout << "Texture found!" << std::endl;
                break;
            }
        }
        if(isChanged == false){
            IMG_OBJ->texture = Textures.at(CreateTexture(ImagePath));
        }
    }
    else std::cout << "Texture already using" << std::endl;
}
void Image::enable(bool value){
    IMG_OBJ->isEnable = value;
}
void Image::Delete(){
    this->~Image();
}
Image::~Image(){
    std::cout << "Deleted: " << IMG_OBJ->index << std::endl;
    if(UI->Objects.size() == 1){
        UI->Objects.pop_back();
        std::cout << "New Size of Objects:" << UI->Objects.size() << std::endl;}
    else{
        for(int i = IMG_OBJ->index; i<UI->Objects.size()-1; i++){
            UI->Objects.at(i) = UI->Objects.at(i+1);
            if(std::holds_alternative<Mythril_IMAGE*>(UI->Objects.at(i))) std::get<Mythril_IMAGE*>(UI->Objects.at(i))->index--;
            else if(std::holds_alternative<Mythril_TEXT*>(UI->Objects.at(i))) std::get<Mythril_TEXT*>(UI->Objects.at(i))->index--;
        }
        UI->Objects.pop_back();
        std::cout << "New Size of Objects:" << UI->Objects.size() << std::endl;
    }
    bool TextureUsing = false;
    for(int i = 0; i < UI->Objects.size(); i++){
        if(std::holds_alternative<Mythril_IMAGE*>(UI->Objects.at(i)))
            if(std::get<Mythril_IMAGE*>(UI->Objects.at(i))->texture == IMG_OBJ->texture){
                TextureUsing = true;
            }
    }
    if(TextureUsing == false){
        if(Textures.size() == 1){
            Textures.pop_back();
            std::cout << "New Size of Textures:" << Textures.size() << std::endl;
        }
        else{
            for(int i = IMG_OBJ->texture->index; i < Textures.size()-1; i++){
                Textures.at(i) = Textures.at(i+1);
                Textures.at(i)->index--;
            }
            Textures.pop_back();
            std::cout << "New Size of Textures:" << Textures.size() << std::endl;
        }
        delete IMG_OBJ->texture->Texture;
        delete IMG_OBJ->texture;
    }
    delete IMG_OBJ;
}