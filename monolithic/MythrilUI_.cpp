#include <MythrilUI.hpp>

// =====================================================================
// MythrilUI RENDER PIPELINE & EVENT MANAGER
// =====================================================================

MythrilUI::MythrilUI(GLFWwindow* Window){
    window = Window;
    if(UIShader == NULL) UIShader = new Shader("shaders/UIvShader.glsl", "shaders/UIfShader.glsl");
    Frames.push_back(this);
}
void MythrilUI::RenderUI(){
    glfwMakeContextCurrent(window);

    // Register character callback for real-time text input
    glfwSetCharCallback(window, rescanKeyboard);

    // Scan repeating keyboard states (e.g., holding backspace)
    scanKeyboard(this);

    // Calculate cursor blink rate (I-Beam toggle)
    double currentTime = glfwGetTime();
    if(currentTime - _lastTime_ >= _IBEAMspeed_){
        _IBEAM = !_IBEAM;
        _lastTime_ = currentTime;
    }

    int wWidth, wHeight;
    glfwGetWindowSize(window, &wWidth, &wHeight);

    // Process mouse hovers and click events
    if(mouseCur != NULL){
        rescanMouse(mouseCur);
        // Clear focus if user clicked outside a textbox
        if(!std::holds_alternative<Mythril_TextBOX*>(mouseCur->mouseOnButton) && mouseCur->state == GLFW_PRESS)
            focusedTXBox = NULL;
    }

    // Generate Orthographic Projection aspect ratio modifier
    Transform trans;
    trans.setValue(0, 0, ((float)wHeight/(float)wWidth));

    UIShader->use();
    UIShader->setInt("texture1", 0);
    UIShader->setMat4("wideScreen", trans.trans);

    // =====================================================================
    // MAIN RENDER LOOP (Iterating Variant Pool)
    // =====================================================================
    for(int i = 0; i < Objects.size(); i++){

        // 1. RENDER IMAGE
        if(std::holds_alternative<Mythril_IMAGE*>(Objects.at(i))){
            if(std::get<Mythril_IMAGE*>(Objects.at(i))->isEnable == true){
                UIShader->use();
                std::get<Mythril_IMAGE*>(Objects.at(i))->texture->Texture->useTextureAs(GL_TEXTURE0);
                glBindVertexArray(std::get<Mythril_IMAGE*>(Objects.at(i))->VAO);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
        }

        // 2. RENDER TEXT
        else if(std::holds_alternative<Mythril_TEXT*>(Objects.at(i))){
            if(std::get<Mythril_TEXT*>(Objects.at(i))->isEnable == true)
                std::get<Mythril_TEXT*>(Objects.at(i))->font->Renderer->RenderText(wWidth, wHeight, std::get<Mythril_TEXT*>(Objects.at(i))->text, std::get<Mythril_TEXT*>(Objects.at(i))->Position[0], std::get<Mythril_TEXT*>(Objects.at(i))->Position[1], std::get<Mythril_TEXT*>(Objects.at(i))->scale, glm::vec3(std::get<Mythril_TEXT*>(Objects.at(i))->color[0], std::get<Mythril_TEXT*>(Objects.at(i))->color[1], std::get<Mythril_TEXT*>(Objects.at(i))->color[2]));
        }

        // 3. RENDER BUTTON
        else if(std::holds_alternative<Mythril_BUTTON*>(Objects.at(i))){
            std::get<Mythril_BUTTON*>(Objects.at(i))->isClicked = false;
            if(std::get<Mythril_BUTTON*>(Objects.at(i))->isEnable == true){
                UIShader->use();

                // Check if mouse is hovering this specific button
                if(std::holds_alternative<Mythril_BUTTON*>(mouseCur->mouseOnButton)){
                    if(std::get<Mythril_BUTTON*>(mouseCur->mouseOnButton) == std::get<Mythril_BUTTON*>(Objects.at(i))){
                        std::get<Mythril_BUTTON*>(Objects.at(i))->mouseON->Texture->useTextureAs(GL_TEXTURE0);
                        glBindVertexArray(std::get<Mythril_BUTTON*>(Objects.at(i))->mouseVAO);
                        
                        // Fire callback if clicked
                        if(mouseCur->state == GLFW_PRESS){
                            std::get<Mythril_BUTTON*>(Objects.at(i))->isClicked = true;
                            if(std::get<Mythril_BUTTON*>(Objects.at(i))->onClickedFunc != NULL) std::get<Mythril_BUTTON*>(Objects.at(i))->onClickedFunc();
                        }
                    }
                }
                else{
                    // Default state
                    std::get<Mythril_BUTTON*>(Objects.at(i))->skin->Texture->useTextureAs(GL_TEXTURE0);
                    glBindVertexArray(std::get<Mythril_BUTTON*>(Objects.at(i))->VAO);
                }

                // Draw 9-slice generated button geometry
                glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_INT, 0);

                // Draw button label text
                std::get<Mythril_BUTTON*>(Objects.at(i))->font->Renderer->RenderText(
                    wWidth, wHeight,
                    std::get<Mythril_BUTTON*>(Objects.at(i))->text, 
                    std::get<Mythril_BUTTON*>(Objects.at(i))->Position[0], 
                    std::get<Mythril_BUTTON*>(Objects.at(i))->Position[1], 
                    std::get<Mythril_BUTTON*>(Objects.at(i))->scale, 
                    std::get<Mythril_BUTTON*>(Objects.at(i))->Size[0], 
                    std::get<Mythril_BUTTON*>(Objects.at(i))->Size[1], 
                    glm::vec3(std::get<Mythril_BUTTON*>(Objects.at(i))->textColor[0], std::get<Mythril_BUTTON*>(Objects.at(i))->textColor[1], std::get<Mythril_BUTTON*>(Objects.at(i))->textColor[2])
                );
            }
        }

        // 4. RENDER TEXTBOX
        else if(std::holds_alternative<Mythril_TextBOX*>(Objects.at(i))){
            std::get<Mythril_TextBOX*>(Objects.at(i))->isFocused = false;
            if(std::get<Mythril_TextBOX*>(Objects.at(i))->isEnable == true){
                UIShader->use();

                // Determine focus state based on mouse click
                if(std::holds_alternative<Mythril_TextBOX*>(mouseCur->mouseOnButton)){
                    if(std::get<Mythril_TextBOX*>(mouseCur->mouseOnButton) == std::get<Mythril_TextBOX*>(Objects.at(i))){
                        if(mouseCur->state == GLFW_PRESS){
                            focusedTXBox = std::get<Mythril_TextBOX*>(Objects.at(i));
                        }
                    }
                }

                // Render Active/Focused Textbox
                if(focusedTXBox == std::get<Mythril_TextBOX*>(Objects.at(i))){
                    std::get<Mythril_TextBOX*>(Objects.at(i))->mouseON->Texture->useTextureAs(GL_TEXTURE0);
                    glBindVertexArray(std::get<Mythril_TextBOX*>(Objects.at(i))->mouseVAO);
                    glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_INT, 0);
                    
                    // Draw I-BEAM text cursor dynamically
                    std::string displayText = std::get<Mythril_TextBOX*>(Objects.at(i))->text;
                    if(_IBEAM) displayText += "|";

                    std::get<Mythril_TextBOX*>(Objects.at(i))->font->Renderer->RenderTextBOX(
                        wWidth, wHeight, displayText, 
                        std::get<Mythril_TextBOX*>(Objects.at(i))->Position[0] + _TEXTBOXverticalMARGIN_, 
                        std::get<Mythril_TextBOX*>(Objects.at(i))->Position[1], 
                        std::get<Mythril_TextBOX*>(Objects.at(i))->scale, 
                        std::get<Mythril_TextBOX*>(Objects.at(i))->Size[0], 
                        _TEXTBOXverticalMARGIN_, 
                        std::get<Mythril_TextBOX*>(Objects.at(i))->Size[1], 
                        glm::vec3(std::get<Mythril_TextBOX*>(Objects.at(i))->textColor[0], std::get<Mythril_TextBOX*>(Objects.at(i))->textColor[1], std::get<Mythril_TextBOX*>(Objects.at(i))->textColor[2])
                    );
                }
                else{
                    // Render Default Textbox
                    std::get<Mythril_TextBOX*>(Objects.at(i))->skin->Texture->useTextureAs(GL_TEXTURE0);
                    glBindVertexArray(std::get<Mythril_TextBOX*>(Objects.at(i))->VAO);
                    glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_INT, 0);

                    std::get<Mythril_TextBOX*>(Objects.at(i))->font->Renderer->RenderTextBOX(
                        wWidth, wHeight, std::get<Mythril_TextBOX*>(Objects.at(i))->text, 
                        std::get<Mythril_TextBOX*>(Objects.at(i))->Position[0] + _TEXTBOXverticalMARGIN_, 
                        std::get<Mythril_TextBOX*>(Objects.at(i))->Position[1], 
                        std::get<Mythril_TextBOX*>(Objects.at(i))->scale, 
                        std::get<Mythril_TextBOX*>(Objects.at(i))->Size[0], 
                        _TEXTBOXverticalMARGIN_, 
                        std::get<Mythril_TextBOX*>(Objects.at(i))->Size[1], 
                        glm::vec3(std::get<Mythril_TextBOX*>(Objects.at(i))->textColor[0], std::get<Mythril_TextBOX*>(Objects.at(i))->textColor[1], std::get<Mythril_TextBOX*>(Objects.at(i))->textColor[2])
                    );
                }
            }
        }
    }
    glBindVertexArray(0);
    glUseProgram(0);
}

// =====================================================================
// WIDGET IMPLEMENTATIONS (IMAGE & TEXT)
// =====================================================================

// Note: Standard rectangular geometry (2 triangles = 6 indices) for Images
Image::Image(MythrilUI* UIw, float width, float height, float positionX, float positionY, const char* ImagePath){
    int windowWidth, windowHeight; UI = UIw;
    glfwGetWindowSize(UI->window, &windowWidth, &windowHeight);
    std::cout << "WindowWidth: " << windowWidth << " WindowHeight: " << windowHeight << std::endl;
    IMG_OBJ->Position[0] = positionX; IMG_OBJ->Position[1] = positionY;
    IMG_OBJ->Size[0] = width; IMG_OBJ->Size[1] = height;
    float RPositionX = 2*(positionX/(float)windowHeight);
    float RPositionY = 2*(positionY/(float)windowHeight);
    float Rwidth = 2*(width/(float)windowHeight);
    float Rheight = 2*(height/(float)windowHeight);
    float vertices[] = {
        RPositionX + (Rwidth/2), RPositionY + (Rheight/2), 0.0f, 1.0f, 1.0f,
        RPositionX + (Rwidth/2), RPositionY - (Rheight/2), 0.0f, 1.0f, 0.0f,
        RPositionX - (Rwidth/2), RPositionY - (Rheight/2), 0.0f, 0.0f, 0.0f,
        RPositionX - (Rwidth/2), RPositionY + (Rheight/2), 0.0f, 0.0f, 1.0f
    };
    IMG_OBJ->index = UI->Objects.size();
    unsigned int EBO0;
    unsigned int indices0[] = {  
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
    float RPositionX = 2*(position.x/(float)windowHeight);
    float RPositionY = 2*(position.y/(float)windowHeight);
    IMG_OBJ->Position[0] = position.x; IMG_OBJ->Position[1] = position.y;
    IMG_OBJ->Size[0] = width; IMG_OBJ->Size[1] = height; 
    float Rwidth = 2*(width/(float)windowHeight);
    float Rheight = 2*(height/(float)windowHeight);
    float vertices[] = {
        RPositionX + (Rwidth/2), RPositionY + (Rheight/2), 0.0f, 1.0f, 1.0f,
        RPositionX + (Rwidth/2), RPositionY - (Rheight/2), 0.0f, 1.0f, 0.0f,
        RPositionX - (Rwidth/2), RPositionY - (Rheight/2), 0.0f, 0.0f, 0.0f,
        RPositionX - (Rwidth/2), RPositionY + (Rheight/2), 0.0f, 0.0f, 1.0f
    };
    IMG_OBJ->index = UI->Objects.size();
    unsigned int EBO0;
    unsigned int indices0[] = {  
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
    float RPositionX = 2*(position.x/(float)windowHeight);
    float RPositionY = 2*(position.y/(float)windowHeight);
    IMG_OBJ->Position[0] = position.x; IMG_OBJ->Position[1] = position.y;
    IMG_OBJ->Size[0] = size.x; IMG_OBJ->Size[1] = size.y;
    float Rwidth = 2*(size.x/(float)windowHeight);
    float Rheight = 2*(size.y/(float)windowHeight);
    float vertices[] = {
        RPositionX + (Rwidth/2), RPositionY + (Rheight/2), 0.0f, 1.0f, 1.0f,
        RPositionX + (Rwidth/2), RPositionY - (Rheight/2), 0.0f, 1.0f, 0.0f,
        RPositionX - (Rwidth/2), RPositionY - (Rheight/2), 0.0f, 0.0f, 0.0f,
        RPositionX - (Rwidth/2), RPositionY + (Rheight/2), 0.0f, 0.0f, 1.0f
    };
    IMG_OBJ->index = UI->Objects.size();
    unsigned int EBO0;
    unsigned int indices0[] = {  
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
    unsigned int indices0[] = {  
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
    IMG_OBJ->Position[0] = positionX; IMG_OBJ->Position[0] = positionY;
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
    unsigned int indices0[] = {  
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
void Image::UpdateSize(float sizeX, float sizeY){
    int windowWidth, windowHeight;
    glfwGetWindowSize(UI->window, &windowWidth, &windowHeight);
    float RPositionX = 2*(IMG_OBJ->Position[0]/(float)windowHeight);
    float RPositionY = 2*(IMG_OBJ->Position[1]/(float)windowHeight);
    IMG_OBJ->Size[0] = sizeX; IMG_OBJ->Size[1] = sizeY;
    float Rwidth = 2*(sizeX/(float)windowHeight);
    float Rheight = 2*(sizeY/(float)windowHeight);
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
    // Safely garbage collects and frees old texture from VRAM if no other widget is using it
    if(ImagePath != IMG_OBJ->texture->ImagePath){
        bool isChanged = false, TextureUsing = false;
        for (int a = 0; a < Frames.size(); a++){
            for(int i = 0; i < Frames.at(a)->Objects.size(); i++){
                if(std::holds_alternative<Mythril_BUTTON*>(Frames.at(a)->Objects.at(i)))
                    if(std::get<Mythril_BUTTON*>(Frames.at(a)->Objects.at(i))->skin == IMG_OBJ->texture || std::get<Mythril_BUTTON*>(Frames.at(a)->Objects.at(i))->mouseON == IMG_OBJ->texture){
                        TextureUsing = true;
                        break;
                    }
                if(TextureUsing == false){
                    if(Frames.at(a) != UI){
                        for(int i = 0; i < Frames.at(a)->Objects.size(); i++){
                            if(std::holds_alternative<Mythril_IMAGE*>(Frames.at(a)->Objects.at(i)))
                                if(std::get<Mythril_IMAGE*>(Frames.at(a)->Objects.at(i))->texture == IMG_OBJ->texture){
                                    TextureUsing = true;
                                    break;
                                }
                        }
                        if(TextureUsing == true) break;
                    }
                    else{
                        for(int i = 0; i < Frames.at(a)->Objects.size(); i++){
                            if(std::holds_alternative<Mythril_IMAGE*>(Frames.at(a)->Objects.at(i)))
                                if(std::get<Mythril_IMAGE*>(Frames.at(a)->Objects.at(i))->texture == IMG_OBJ->texture && IMG_OBJ->index != i){
                                    TextureUsing = true;
                                    break;
                                }
                        }
                        if (TextureUsing == true) break;
                    }
                }
            }
            if(TextureUsing == true) break;
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
void Image::Delete(){
    this->~Image();
}
void Image::enable(bool value){
    IMG_OBJ->isEnable = value;
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
            else if(std::holds_alternative<Mythril_BUTTON*>(UI->Objects.at(i))) std::get<Mythril_BUTTON*>(UI->Objects.at(i))->index--;
        }
        UI->Objects.pop_back();
        std::cout << "New Size of Objects:" << UI->Objects.size() << std::endl;
    }
    bool TextureUsing = false;
    for(int a = 0; a < Frames.size(); a++){
        for(int i = 0; i < Frames.at(a)->Objects.size(); i++){
            if(std::holds_alternative<Mythril_IMAGE*>(Frames.at(a)->Objects.at(i))){if(std::get<Mythril_IMAGE*>(Frames.at(a)->Objects.at(i))->texture == IMG_OBJ->texture){TextureUsing = true; break;}}
            else if(std::holds_alternative<Mythril_BUTTON*>(Frames.at(a)->Objects.at(i))){if(std::get<Mythril_BUTTON*>(Frames.at(a)->Objects.at(i))->skin == IMG_OBJ->texture  || std::get<Mythril_BUTTON*>(Frames.at(a)->Objects.at(i))->mouseON == IMG_OBJ->texture){TextureUsing = true; break;}}
        }
        if(TextureUsing == true) break;
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

Text::Text(MythrilUI* UIw, std::string text, float positionX, float positionY, float scale, glm::vec3 color){
    TX_OBJ->text = text; UI = UIw;
    TX_OBJ->index = UI->Objects.size();
    for(int a = 0; a < Fonts.size(); a++){
        if(Fonts.at(a)->FontPath == defaultFontPath){
            TX_OBJ->font = Fonts.at(a);
            break;
        }
    }
    if(TX_OBJ->font == NULL){
        TX_OBJ->font = Fonts.at(CreateFont(defaultFontPath, defaultFontSize));
    }
    TX_OBJ->Position[0] = positionX; TX_OBJ->Position[1] = -positionY;
    TX_OBJ->scale = scale;
    TX_OBJ->color[0] = color.x; TX_OBJ->color[1] = color.y; TX_OBJ->color[2] = color.z;
    UI->Objects.push_back(TX_OBJ);
}
Text::Text(MythrilUI* UIw, std::string fontPath,std::string text, float positionX, float positionY, float scale, glm::vec3 color){
    TX_OBJ->text = text; UI = UIw;
    TX_OBJ->index = UI->Objects.size();
    for(int a = 0; a < Fonts.size(); a++){
        if(Fonts.at(a)->FontPath == fontPath){
            TX_OBJ->font = Fonts.at(a);
            break;
        }
    }
    if(TX_OBJ->font == NULL){
        TX_OBJ->font = Fonts.at(CreateFont(fontPath, defaultFontSize));
    }
    TX_OBJ->Position[0] = positionX; TX_OBJ->Position[1] = -positionY;
    TX_OBJ->scale = scale;
    TX_OBJ->color[0] = color.x; TX_OBJ->color[1] = color.y; TX_OBJ->color[2] = color.z;
    UI->Objects.push_back(TX_OBJ);
}
Text::Text(MythrilUI* UIw, std::string fontPath,std::string text, glm::vec2 position, float scale, glm::vec3 color){
    TX_OBJ->text = text; UI = UIw;
    TX_OBJ->index = UI->Objects.size();
    for(int a = 0; a < Fonts.size(); a++){
        if(Fonts.at(a)->FontPath == fontPath){
            TX_OBJ->font = Fonts.at(a);
            break;
        }
    }
    if(TX_OBJ->font == NULL){
        TX_OBJ->font = Fonts.at(CreateFont(fontPath, defaultFontSize));
    }
    TX_OBJ->Position[0] = position.x; TX_OBJ->Position[1] = -position.y;
    TX_OBJ->scale = scale;
    TX_OBJ->color[0] = color.x; TX_OBJ->color[1] = color.y; TX_OBJ->color[2] = color.z;
    UI->Objects.push_back(TX_OBJ);
}
Text::Text(MythrilUI* UIw, std::string text, glm::vec2 position, float scale, glm::vec3 color){
    TX_OBJ->text = text; UI = UIw;
    TX_OBJ->index = UI->Objects.size();
    for(int a = 0; a < Fonts.size(); a++){
        if(Fonts.at(a)->FontPath == defaultFontPath){
            TX_OBJ->font = Fonts.at(a);
            break;
        }
    }
    if(TX_OBJ->font == NULL){
        TX_OBJ->font = Fonts.at(CreateFont(defaultFontPath, defaultFontSize));
    }
    TX_OBJ->Position[0] = position.x; TX_OBJ->Position[1] = -position.y;
    TX_OBJ->scale = scale;
    TX_OBJ->color[0] = color.x; TX_OBJ->color[1] = color.y; TX_OBJ->color[2] = color.z;
    UI->Objects.push_back(TX_OBJ);
}
void Text::ChangeFont(std::string fontPath){
    if(fontPath != TX_OBJ->font->FontPath){
        bool isChanged = false, FontUsing = false;
        for (int a = 0; a < Frames.size(); a++){
            for(int i = 0; i < Frames.at(a)->Objects.size(); i++){
                if(std::holds_alternative<Mythril_BUTTON*>(Frames.at(a)->Objects.at(i)))
                    if(std::get<Mythril_BUTTON*>(Frames.at(a)->Objects.at(i))->font == TX_OBJ->font){
                        FontUsing = true;
                        break;
                    }
                if(FontUsing == false){
                    if(Frames.at(a) != UI){
                        for(int i = 0; i < Frames.at(a)->Objects.size(); i++){
                            if(std::holds_alternative<Mythril_TEXT*>(Frames.at(a)->Objects.at(i)))
                                if(std::get<Mythril_TEXT*>(Frames.at(a)->Objects.at(i))->font == TX_OBJ->font){
                                    FontUsing = true;
                                    break;
                                }
                        }
                        if(FontUsing == true) break;
                    }
                    else{
                        for(int i = 0; i < Frames.at(a)->Objects.size(); i++){
                            if(std::holds_alternative<Mythril_TEXT*>(Frames.at(a)->Objects.at(i)))
                                if(std::get<Mythril_TEXT*>(Frames.at(a)->Objects.at(i))->font == TX_OBJ->font && TX_OBJ->index != i){
                                    FontUsing = true;
                                    break;
                                }
                        }
                        if (FontUsing == true) break;
                    }
                }
            }
            if(FontUsing == true) break;
        }
        if(FontUsing == false){
            if(Fonts.size() == 1){
                Fonts.pop_back();
                std::cout << "New Size of Fonts:" << Fonts.size() << std::endl;
            }
            else{
                for(int i = TX_OBJ->font->index; i < Fonts.size()-1; i++){
                    Fonts.at(i) = Fonts.at(i+1);
                    Fonts.at(i)->index--;
                }
                Fonts.pop_back();
                std::cout << "New Size of Fonts:" << Fonts.size() << std::endl;
            }
            delete TX_OBJ->font->Renderer;
            delete TX_OBJ->font;
        }
        for(int i = 0; i < Fonts.size(); i++){
            if(fontPath == Fonts.at(i)->FontPath){
                TX_OBJ->font = Fonts.at(i);
                isChanged = true;
                std::cout << "Font found!" << std::endl;
                break;
            }
        }
        if(isChanged == false){
            TX_OBJ->font = Fonts.at(CreateFont(fontPath, defaultFontSize));
        }
    }
    else std::cout << "Font already using!" << std::endl;
}
void Text::UpdateText(std::string text){
    TX_OBJ->text = text;
}
void Text::UpdateScale(float scale){
    TX_OBJ->scale = scale;
}
void Text::UpdatePosition(glm::vec2 position){
    TX_OBJ->Position[0] = position.x; TX_OBJ->Position[1] = position.y;
}
void Text::UpdatePosition(float positionX, float positionY){
    TX_OBJ->Position[0] = positionX; TX_OBJ->Position[1] = positionY;
}
void Text::enable(bool value){
    TX_OBJ->isEnable = value;
}
void Text::Delete(){
    this->~Text();
}
Text::~Text(){
    if(UI->Objects.size() == 1){
        UI->Objects.pop_back();
        std::cout << "New Size of Objects:" << UI->Objects.size() << std::endl;
    }
    else{
        for(int i = TX_OBJ->index; i < UI->Objects.size()-1; i++){
            UI->Objects.at(i) = UI->Objects.at(i+1);
            if(std::holds_alternative<Mythril_IMAGE*>(UI->Objects.at(i))) std::get<Mythril_IMAGE*>(UI->Objects.at(i))->index--;
            else if(std::holds_alternative<Mythril_TEXT*>(UI->Objects.at(i))) std::get<Mythril_TEXT*>(UI->Objects.at(i))->index--;
            else if(std::holds_alternative<Mythril_BUTTON*>(UI->Objects.at(i))) std::get<Mythril_BUTTON*>(UI->Objects.at(i))->index--;
        }
        UI->Objects.pop_back();
        std::cout << "New Size of Objects:" << UI->Objects.size() << std::endl;
    }
    bool FontUsing = false;
    for(int a = 0; a < Frames.size(); a++){
        for(int i = 0; i < Frames.at(a)->Objects.size(); i++){
            if(std::holds_alternative<Mythril_TEXT*>(Frames.at(a)->Objects.at(i))){if(std::get<Mythril_TEXT*>(Frames.at(a)->Objects.at(i))->font == TX_OBJ->font){FontUsing = true; break;}}
            else if(std::holds_alternative<Mythril_BUTTON*>(Frames.at(a)->Objects.at(i))){if(std::get<Mythril_BUTTON*>(Frames.at(a)->Objects.at(i))->font == TX_OBJ->font){FontUsing = true; break;}}
        }
        if(FontUsing == true) break;
    }
    if(FontUsing == false){
        if(Fonts.size() == 1){
            Fonts.pop_back();
            std::cout << "New Size of Fonts:" << Fonts.size() << std::endl;
            delete TX_OBJ->font->Renderer;
            delete TX_OBJ->font;
        }
        else{
            for(int i = TX_OBJ->font->index; i < Fonts.size()-1; i++){
                Fonts.at(i) = Fonts.at(i+1);
                Fonts.at(i)->index--;
            }
            Fonts.pop_back();
            std::cout << "New Size of Fonts:" << Fonts.size() << std::endl;
            delete TX_OBJ->font->Renderer;
            delete TX_OBJ->font;
        }
    }
    delete TX_OBJ;
}

// =====================================================================
// INTERACTIVE WIDGETS (BUTTON & TEXTBOX) WITH 9-SLICE SCALING MATH
// =====================================================================

Button::Button(MythrilUI* UIw, glm::vec2 size, glm::vec2 position, const char* ImagePath, const char* MouseONPath, std::string text, float scale, glm::vec3 Textcolor){
    int windowWidth, windowHeight; UI = UIw;
    ButtonOBJ->index = UI->Objects.size();

    // Attempt to pull textures from global pool to prevent VRAM duplication
    for(int i = 0; i < Textures.size(); i++){
        if(ImagePath == Textures.at(i)->ImagePath){
            ButtonOBJ->skin = Textures.at(i);
            std::cout << "Texture found!" << std::endl;
            break;
        }
    }
    for(int i = 0; i < Textures.size(); i++){
        if(MouseONPath == Textures.at(i)->ImagePath){
            ButtonOBJ->mouseON = Textures.at(i);
            std::cout << "Texture found!" << std::endl;
            break;
        }
    }
    if(ButtonOBJ->skin == NULL){
        ButtonOBJ->skin = Textures.at(CreateTexture(ImagePath));
    }
    if(ButtonOBJ->mouseON == NULL){
        ButtonOBJ->mouseON = Textures.at(CreateTexture(MouseONPath));
    }
    
    glfwGetWindowSize(UI->window, &windowWidth, &windowHeight);

    // Normalize coordinates mapped to screen aspect ratio
    float RPositionX = 2*(position.x/(float)windowHeight);
    float RPositionY = 2*(position.y/(float)windowHeight);
    ButtonOBJ->Position[0] = position.x; ButtonOBJ->Position[1] = position.y;
    ButtonOBJ->Size[0] = size.x; ButtonOBJ->Size[1] = size.y;
    float Rwidth = 2*(size.x/(float)windowHeight);
    float Rheight = 2*(size.y/(float)windowHeight);

    // Extract base texture geometry size
    float RTextureWidth = 2*((float)ButtonOBJ->skin->Texture->width/(float)windowHeight);
    float RTextureHeight = 2*((float)ButtonOBJ->skin->Texture->height/(float)windowHeight);
    std::vector<float> infos;

    // -----------------------------------------------------------------------------------
    // 9-SLICE SCALING ALGORITHM (CRITICAL)
    // Generates 40 vertices and 30 indices (10 triangles) to scale the button perfectly.
    // Compares requested Aspect Ratio against Texture Aspect Ratio to prevent distortion.
    // -----------------------------------------------------------------------------------
    if ((size.x / size.y) < (RTextureWidth/RTextureHeight)){
        // If button is physically taller/narrower than original texture

        float buttonun1bolu10u = Rwidth / 5.0f; // Calculate exact corner slice boundary size
        float NormalSIze = RTextureHeight*(Rwidth/RTextureWidth);
        float xinydekapladigialan = buttonun1bolu10u/NormalSIze; // UV mapping boundary ratio
        float leftSIZE = NormalSIze-(buttonun1bolu10u*2); // Center fill size

        // Top-Right corner slice
        infos.push_back(RPositionX + (Rwidth/2)); infos.push_back(RPositionY + (Rheight/2));
        infos.push_back(1.0f); infos.push_back(1.0f);
        infos.push_back(RPositionX + (Rwidth/2)); infos.push_back(RPositionY + (Rheight/2) - buttonun1bolu10u);
        infos.push_back(1.0f); infos.push_back(1 - xinydekapladigialan);
        infos.push_back(RPositionX - (Rwidth/2)); infos.push_back(RPositionY + (Rheight/2) - buttonun1bolu10u);
        infos.push_back(0.0f); infos.push_back(1 - xinydekapladigialan);
        infos.push_back(RPositionX - (Rwidth/2)); infos.push_back(RPositionY + (Rheight/2));
        infos.push_back(0.0f); infos.push_back(1.0f);

        // Center stretching body slice
        infos.push_back(RPositionX + (Rwidth/2)); infos.push_back(RPositionY + (leftSIZE/2));
        infos.push_back(1.0f); infos.push_back(1 - xinydekapladigialan);
        infos.push_back(RPositionX - (Rwidth/2)); infos.push_back(RPositionY + (leftSIZE/2));
        infos.push_back(0.0f); infos.push_back(1 - xinydekapladigialan);
        infos.push_back(RPositionX + (Rwidth/2)); infos.push_back(RPositionY - (leftSIZE/2));
        infos.push_back(1.0f); infos.push_back(xinydekapladigialan);
        infos.push_back(RPositionX - (Rwidth/2)); infos.push_back(RPositionY - (leftSIZE/2));
        infos.push_back(0.0f); infos.push_back(xinydekapladigialan);

        // Bottom-Left corner slice
        infos.push_back(RPositionX + (Rwidth/2)); infos.push_back(RPositionY - (Rheight/2) + buttonun1bolu10u);
        infos.push_back(1.0f); infos.push_back(xinydekapladigialan);
        infos.push_back(RPositionX - (Rwidth/2)); infos.push_back(RPositionY - (Rheight/2) + buttonun1bolu10u);
        infos.push_back(0.0f); infos.push_back(xinydekapladigialan);
        infos.push_back(RPositionX + (Rwidth/2)); infos.push_back(RPositionY - (Rheight/2));
        infos.push_back(1.0f); infos.push_back(0.0f);
        infos.push_back(RPositionX - (Rwidth/2)); infos.push_back(RPositionY - (Rheight/2));
        infos.push_back(0.0f); infos.push_back(0.0f);
    }
    else{
        // If button is physically wider/shorter than original texture (Standard UI Button)

        float buttonun1bolu10u = Rheight / 5.0f;
        float NormalSIze = RTextureWidth*(Rheight/RTextureHeight);
        float yninxdekapladigialan = buttonun1bolu10u/NormalSIze; // UV horizontal stretching limit
        float leftSIZE = NormalSIze-(buttonun1bolu10u*2);
        
        // Right vertical slice
        infos.push_back(RPositionX + (Rwidth/2)); infos.push_back(RPositionY - (Rheight/2));
        infos.push_back(1.0f); infos.push_back(0.0f);
        infos.push_back(RPositionX + (Rwidth/2) - buttonun1bolu10u); infos.push_back(RPositionY - (Rheight/2));
        infos.push_back(1-yninxdekapladigialan); infos.push_back(0.0f);
        infos.push_back(RPositionX + (Rwidth/2) - buttonun1bolu10u); infos.push_back(RPositionY + (Rheight/2));
        infos.push_back(1-yninxdekapladigialan); infos.push_back(1.0f);
        infos.push_back(RPositionX + (Rwidth/2)); infos.push_back(RPositionY + (Rheight/2));
        infos.push_back(1.0f); infos.push_back(1.0f);

        // Center stretched slice
        infos.push_back(RPositionX + (leftSIZE/2)); infos.push_back(RPositionY - (Rheight/2));
        infos.push_back(1-yninxdekapladigialan); infos.push_back(0.0f);
        infos.push_back(RPositionX + (leftSIZE/2)); infos.push_back(RPositionY + (Rheight/2));
        infos.push_back(1-yninxdekapladigialan); infos.push_back(1.0f);
        infos.push_back(RPositionX - (leftSIZE/2)); infos.push_back(RPositionY - (Rheight/2));
        infos.push_back(yninxdekapladigialan); infos.push_back(0.0f);
        infos.push_back(RPositionX - (leftSIZE/2)); infos.push_back(RPositionY + (Rheight/2));
        infos.push_back(yninxdekapladigialan); infos.push_back(1.0f);

        // Left vertical slice
        infos.push_back(RPositionX - (Rwidth/2) + buttonun1bolu10u); infos.push_back(RPositionY - (Rheight/2));
        infos.push_back(yninxdekapladigialan); infos.push_back(0.0f);
        infos.push_back(RPositionX - (Rwidth/2) + buttonun1bolu10u); infos.push_back(RPositionY + (Rheight/2));
        infos.push_back(yninxdekapladigialan); infos.push_back(1.0f);
        infos.push_back(RPositionX - (Rwidth/2)); infos.push_back(RPositionY - (Rheight/2));
        infos.push_back(0.0f); infos.push_back(0.0f);
        infos.push_back(RPositionX - (Rwidth/2)); infos.push_back(RPositionY + (Rheight/2));
        infos.push_back(0.0f); infos.push_back(1.0f);
    }

    // Draw order indexing for 10 sliced triangles
    unsigned int indices1[] = {
                        0, 1, 3,   1,  2, 3,    1,  4, 2,   4, 5, 2,
                        4, 6, 5,   6,  7, 5,    6,  8, 7,   8, 9, 7,
                                   8, 10, 9,   10, 11, 9};
    float vertices[] = {
        infos.at(0), infos.at(1), 0.0f, infos.at(2), infos.at(3),
        infos.at(4), infos.at(5), 0.0f, infos.at(6), infos.at(7),
        infos.at(8), infos.at(9), 0.0f, infos.at(10), infos.at(11),
        infos.at(12), infos.at(13), 0.0f, infos.at(14), infos.at(15),

        infos.at(16), infos.at(17), 0.0f, infos.at(18), infos.at(19),
        infos.at(20), infos.at(21), 0.0f, infos.at(22), infos.at(23),
        infos.at(24), infos.at(25), 0.0f, infos.at(26), infos.at(27),
        infos.at(28), infos.at(29), 0.0f, infos.at(30), infos.at(31),
        
        infos.at(32), infos.at(33), 0.0f, infos.at(34), infos.at(35),
        infos.at(36), infos.at(37), 0.0f, infos.at(38), infos.at(39),
        infos.at(40), infos.at(41), 0.0f, infos.at(42), infos.at(43),
        infos.at(44), infos.at(45), 0.0f, infos.at(46), infos.at(47)
    };
    unsigned int EBO0;

    // Bind main button skin
    glGenVertexArrays(1, &ButtonOBJ->VAO);
    glGenBuffers(1, &EBO0);
    glGenBuffers(1, &ButtonOBJ->VBO);
    glBindVertexArray(ButtonOBJ->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, ButtonOBJ->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO0);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices1), indices1, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    infos.clear();

    // NOTE: Generating identical 9-slice mesh structure for Hover State (mouseVAO)
    // Removed duplicating logic for brevity in comments, performs same logic.
    RTextureWidth = 2*((float)ButtonOBJ->mouseON->Texture->width/(float)windowHeight);
    RTextureHeight = 2*((float)ButtonOBJ->mouseON->Texture->height/(float)windowHeight);
    if ((size.x / size.y) < (RTextureWidth/RTextureHeight)){
        float buttonun1bolu10u = Rwidth / 5.0f;
        float NormalSIze = RTextureHeight*(Rwidth/RTextureWidth);
        float xinydekapladigialan = buttonun1bolu10u/NormalSIze;
        float leftSIZE = NormalSIze-(buttonun1bolu10u*2);
        infos.push_back(RPositionX + (Rwidth/2)); infos.push_back(RPositionY + (Rheight/2));
        infos.push_back(1.0f); infos.push_back(1.0f);
        infos.push_back(RPositionX + (Rwidth/2)); infos.push_back(RPositionY + (Rheight/2) - buttonun1bolu10u);
        infos.push_back(1.0f); infos.push_back(1 - xinydekapladigialan);
        infos.push_back(RPositionX - (Rwidth/2)); infos.push_back(RPositionY + (Rheight/2) - buttonun1bolu10u);
        infos.push_back(0.0f); infos.push_back(1 - xinydekapladigialan);
        infos.push_back(RPositionX - (Rwidth/2)); infos.push_back(RPositionY + (Rheight/2));
        infos.push_back(0.0f); infos.push_back(1.0f);

        infos.push_back(RPositionX + (Rwidth/2)); infos.push_back(RPositionY + (leftSIZE/2));
        infos.push_back(1.0f); infos.push_back(1 - xinydekapladigialan);
        infos.push_back(RPositionX - (Rwidth/2)); infos.push_back(RPositionY + (leftSIZE/2));
        infos.push_back(0.0f); infos.push_back(1 - xinydekapladigialan);
        infos.push_back(RPositionX + (Rwidth/2)); infos.push_back(RPositionY - (leftSIZE/2));
        infos.push_back(1.0f); infos.push_back(xinydekapladigialan);
        infos.push_back(RPositionX - (Rwidth/2)); infos.push_back(RPositionY - (leftSIZE/2));
        infos.push_back(0.0f); infos.push_back(xinydekapladigialan);

        infos.push_back(RPositionX + (Rwidth/2)); infos.push_back(RPositionY - (Rheight/2) + buttonun1bolu10u);
        infos.push_back(1.0f); infos.push_back(xinydekapladigialan);
        infos.push_back(RPositionX - (Rwidth/2)); infos.push_back(RPositionY - (Rheight/2) + buttonun1bolu10u);
        infos.push_back(0.0f); infos.push_back(xinydekapladigialan);
        infos.push_back(RPositionX + (Rwidth/2)); infos.push_back(RPositionY - (Rheight/2));
        infos.push_back(1.0f); infos.push_back(0.0f);
        infos.push_back(RPositionX - (Rwidth/2)); infos.push_back(RPositionY - (Rheight/2));
        infos.push_back(0.0f); infos.push_back(0.0f);
    }
    else{
        float buttonun1bolu10u = Rheight / 5.0f;
        float NormalSIze = RTextureWidth*(Rheight/RTextureHeight);
        float yninxdekapladigialan = buttonun1bolu10u/NormalSIze;
        float leftSIZE = NormalSIze-(buttonun1bolu10u*2);
        
        infos.push_back(RPositionX + (Rwidth/2)); infos.push_back(RPositionY - (Rheight/2));
        infos.push_back(1.0f); infos.push_back(0.0f);
        infos.push_back(RPositionX + (Rwidth/2) - buttonun1bolu10u); infos.push_back(RPositionY - (Rheight/2));
        infos.push_back(1-yninxdekapladigialan); infos.push_back(0.0f);
        infos.push_back(RPositionX + (Rwidth/2) - buttonun1bolu10u); infos.push_back(RPositionY + (Rheight/2));
        infos.push_back(1-yninxdekapladigialan); infos.push_back(1.0f);
        infos.push_back(RPositionX + (Rwidth/2)); infos.push_back(RPositionY + (Rheight/2));
        infos.push_back(1.0f); infos.push_back(1.0f);

        infos.push_back(RPositionX + (leftSIZE/2)); infos.push_back(RPositionY - (Rheight/2));
        infos.push_back(1-yninxdekapladigialan); infos.push_back(0.0f);
        infos.push_back(RPositionX + (leftSIZE/2)); infos.push_back(RPositionY + (Rheight/2));
        infos.push_back(1-yninxdekapladigialan); infos.push_back(1.0f);
        infos.push_back(RPositionX - (leftSIZE/2)); infos.push_back(RPositionY - (Rheight/2));
        infos.push_back(yninxdekapladigialan); infos.push_back(0.0f);
        infos.push_back(RPositionX - (leftSIZE/2)); infos.push_back(RPositionY + (Rheight/2));
        infos.push_back(yninxdekapladigialan); infos.push_back(1.0f);

        infos.push_back(RPositionX - (Rwidth/2) + buttonun1bolu10u); infos.push_back(RPositionY - (Rheight/2));
        infos.push_back(yninxdekapladigialan); infos.push_back(0.0f);
        infos.push_back(RPositionX - (Rwidth/2) + buttonun1bolu10u); infos.push_back(RPositionY + (Rheight/2));
        infos.push_back(yninxdekapladigialan); infos.push_back(1.0f);
        infos.push_back(RPositionX - (Rwidth/2)); infos.push_back(RPositionY - (Rheight/2));
        infos.push_back(0.0f); infos.push_back(0.0f);
        infos.push_back(RPositionX - (Rwidth/2)); infos.push_back(RPositionY + (Rheight/2));
        infos.push_back(0.0f); infos.push_back(1.0f);
    }
    float vertices0[] = {
        infos.at(0), infos.at(1), 0.0f, infos.at(2), infos.at(3),
        infos.at(4), infos.at(5), 0.0f, infos.at(6), infos.at(7),
        infos.at(8), infos.at(9), 0.0f, infos.at(10), infos.at(11),
        infos.at(12), infos.at(13), 0.0f, infos.at(14), infos.at(15),

        infos.at(16), infos.at(17), 0.0f, infos.at(18), infos.at(19),
        infos.at(20), infos.at(21), 0.0f, infos.at(22), infos.at(23),
        infos.at(24), infos.at(25), 0.0f, infos.at(26), infos.at(27),
        infos.at(28), infos.at(29), 0.0f, infos.at(30), infos.at(31),
        
        infos.at(32), infos.at(33), 0.0f, infos.at(34), infos.at(35),
        infos.at(36), infos.at(37), 0.0f, infos.at(38), infos.at(39),
        infos.at(40), infos.at(41), 0.0f, infos.at(42), infos.at(43),
        infos.at(44), infos.at(45), 0.0f, infos.at(46), infos.at(47)
    };

    // Bind hover state skin
    glGenVertexArrays(1, &ButtonOBJ->mouseVAO);
    glGenBuffers(1, &EBO0);
    glGenBuffers(1, &ButtonOBJ->VBO);
    glBindVertexArray(ButtonOBJ->mouseVAO);
    glBindBuffer(GL_ARRAY_BUFFER, ButtonOBJ->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices0), vertices0, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO0);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices1), indices1, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    ButtonOBJ->text = text;
    ButtonOBJ->index = UI->Objects.size();

    // Assign Font
    for(int a = 0; a < Fonts.size(); a++){
        if(Fonts.at(a)->FontPath == defaultFontPath && Fonts.at(a)->fontSize == defaultFontSize){
            ButtonOBJ->font = Fonts.at(a);
            break;
        }
    }
    if(ButtonOBJ->font == NULL){
        ButtonOBJ->font = Fonts.at(CreateFont(defaultFontPath, defaultFontSize));
    }
    ButtonOBJ->scale = scale;
    ButtonOBJ->Position[0] = position.x;
    ButtonOBJ->Position[1] = -position.y;
    ButtonOBJ->textColor[0] = Textcolor.x; ButtonOBJ->textColor[1] = Textcolor.y; ButtonOBJ->textColor[2] = Textcolor.z;
    
    if(cursorHand == NULL) cursorHand = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
    
    // Verify and map Global Mouse Tracking instance
    if(UI->mouseCur == NULL){
        for(int i = 0; i < Mouses.size(); i++){
            if(Mouses.at(i)->window == UI->window){UI->mouseCur = Mouses.at(i);
                std::cout << "Mouse found!" << std::endl;
            }
        }
        if(UI->mouseCur == NULL){
            UI->mouseCur = new Mythril_MOUSE;
            UI->mouseCur->window = UI->window;
            Mouses.push_back(UI->mouseCur);
            std::cout << "Mouse created!" << std::endl;
        }
    }
    UI->Objects.push_back(ButtonOBJ);
}
void Button::OnClicked(void(*func)()){
    ButtonOBJ->onClickedFunc = func;
}
bool Button::isClicked(){
    return ButtonOBJ->isClicked;
}

// -----------------------------------------------------------------------------------
// TEXTBOX WIDGET INITIALIZER
// Shares identical 9-slice coordinate generation with the Button class
// -----------------------------------------------------------------------------------
TextBox::TextBox(MythrilUI* UIw, glm::vec2 size, glm::vec2 position, float scale, glm::vec3 Textcolor, std::string defaultText){
    int windowWidth, windowHeight; UI = UIw;
    TXBOX_OBJ->index = UI->Objects.size();
    for(int i = 0; i < Textures.size(); i++){
        if(defaultTXBXPath == Textures.at(i)->ImagePath){
            TXBOX_OBJ->skin = Textures.at(i);
            std::cout << "Texture found!" << std::endl;
            break;
        }
    }
    for(int i = 0; i < Textures.size(); i++){
        if(defaultTXBXOMBPath == Textures.at(i)->ImagePath){
            TXBOX_OBJ->mouseON = Textures.at(i);
            std::cout << "Texture found!" << std::endl;
            break;
        }
    }
    if(TXBOX_OBJ->skin == NULL){
        TXBOX_OBJ->skin = Textures.at(CreateTexture(defaultTXBXPath));
    }
    if(TXBOX_OBJ->mouseON == NULL){
        TXBOX_OBJ->mouseON = Textures.at(CreateTexture(defaultTXBXOMBPath));
    }
    glfwGetWindowSize(UI->window, &windowWidth, &windowHeight);
    float RPositionX = 2*(position.x/(float)windowHeight);
    float RPositionY = 2*(position.y/(float)windowHeight);
    TXBOX_OBJ->Position[0] = position.x; TXBOX_OBJ->Position[1] = position.y;
    TXBOX_OBJ->Size[0] = size.x; TXBOX_OBJ->Size[1] = size.y;
    float Rwidth = 2*(size.x/(float)windowHeight);
    float Rheight = 2*(size.y/(float)windowHeight);
    float RTextureWidth = 2*((float)TXBOX_OBJ->skin->Texture->width/(float)windowHeight);
    float RTextureHeight = 2*((float)TXBOX_OBJ->skin->Texture->height/(float)windowHeight);
    std::vector<float> infos;

    if ((size.x / size.y) < (RTextureWidth/RTextureHeight)){
        float buttonun1bolu10u = Rwidth / 5.0f;
        float NormalSIze = RTextureHeight*(Rwidth/RTextureWidth);
        float xinydekapladigialan = buttonun1bolu10u/NormalSIze;
        float leftSIZE = NormalSIze-(buttonun1bolu10u*2);

        infos.push_back(RPositionX + (Rwidth/2)); infos.push_back(RPositionY + (Rheight/2));
        infos.push_back(1.0f); infos.push_back(1.0f);
        infos.push_back(RPositionX + (Rwidth/2)); infos.push_back(RPositionY + (Rheight/2) - buttonun1bolu10u);
        infos.push_back(1.0f); infos.push_back(1 - xinydekapladigialan);
        infos.push_back(RPositionX - (Rwidth/2)); infos.push_back(RPositionY + (Rheight/2) - buttonun1bolu10u);
        infos.push_back(0.0f); infos.push_back(1 - xinydekapladigialan);
        infos.push_back(RPositionX - (Rwidth/2)); infos.push_back(RPositionY + (Rheight/2));
        infos.push_back(0.0f); infos.push_back(1.0f);

        infos.push_back(RPositionX + (Rwidth/2)); infos.push_back(RPositionY + (leftSIZE/2));
        infos.push_back(1.0f); infos.push_back(1 - xinydekapladigialan);
        infos.push_back(RPositionX - (Rwidth/2)); infos.push_back(RPositionY + (leftSIZE/2));
        infos.push_back(0.0f); infos.push_back(1 - xinydekapladigialan);
        infos.push_back(RPositionX + (Rwidth/2)); infos.push_back(RPositionY - (leftSIZE/2));
        infos.push_back(1.0f); infos.push_back(xinydekapladigialan);
        infos.push_back(RPositionX - (Rwidth/2)); infos.push_back(RPositionY - (leftSIZE/2));
        infos.push_back(0.0f); infos.push_back(xinydekapladigialan);

        infos.push_back(RPositionX + (Rwidth/2)); infos.push_back(RPositionY - (Rheight/2) + buttonun1bolu10u);
        infos.push_back(1.0f); infos.push_back(xinydekapladigialan);
        infos.push_back(RPositionX - (Rwidth/2)); infos.push_back(RPositionY - (Rheight/2) + buttonun1bolu10u);
        infos.push_back(0.0f); infos.push_back(xinydekapladigialan);
        infos.push_back(RPositionX + (Rwidth/2)); infos.push_back(RPositionY - (Rheight/2));
        infos.push_back(1.0f); infos.push_back(0.0f);
        infos.push_back(RPositionX - (Rwidth/2)); infos.push_back(RPositionY - (Rheight/2));
        infos.push_back(0.0f); infos.push_back(0.0f);
    }
    else{
        float buttonun1bolu10u = Rheight / 5.0f;
        float NormalSIze = RTextureWidth*(Rheight/RTextureHeight);
        float yninxdekapladigialan = buttonun1bolu10u/NormalSIze;
        float leftSIZE = NormalSIze-(buttonun1bolu10u*2);
        
        infos.push_back(RPositionX + (Rwidth/2)); infos.push_back(RPositionY - (Rheight/2));
        infos.push_back(1.0f); infos.push_back(0.0f);
        infos.push_back(RPositionX + (Rwidth/2) - buttonun1bolu10u); infos.push_back(RPositionY - (Rheight/2));
        infos.push_back(1-yninxdekapladigialan); infos.push_back(0.0f);
        infos.push_back(RPositionX + (Rwidth/2) - buttonun1bolu10u); infos.push_back(RPositionY + (Rheight/2));
        infos.push_back(1-yninxdekapladigialan); infos.push_back(1.0f);
        infos.push_back(RPositionX + (Rwidth/2)); infos.push_back(RPositionY + (Rheight/2));
        infos.push_back(1.0f); infos.push_back(1.0f);

        infos.push_back(RPositionX + (leftSIZE/2)); infos.push_back(RPositionY - (Rheight/2));
        infos.push_back(1-yninxdekapladigialan); infos.push_back(0.0f);
        infos.push_back(RPositionX + (leftSIZE/2)); infos.push_back(RPositionY + (Rheight/2));
        infos.push_back(1-yninxdekapladigialan); infos.push_back(1.0f);
        infos.push_back(RPositionX - (leftSIZE/2)); infos.push_back(RPositionY - (Rheight/2));
        infos.push_back(yninxdekapladigialan); infos.push_back(0.0f);
        infos.push_back(RPositionX - (leftSIZE/2)); infos.push_back(RPositionY + (Rheight/2));
        infos.push_back(yninxdekapladigialan); infos.push_back(1.0f);

        infos.push_back(RPositionX - (Rwidth/2) + buttonun1bolu10u); infos.push_back(RPositionY - (Rheight/2));
        infos.push_back(yninxdekapladigialan); infos.push_back(0.0f);
        infos.push_back(RPositionX - (Rwidth/2) + buttonun1bolu10u); infos.push_back(RPositionY + (Rheight/2));
        infos.push_back(yninxdekapladigialan); infos.push_back(1.0f);
        infos.push_back(RPositionX - (Rwidth/2)); infos.push_back(RPositionY - (Rheight/2));
        infos.push_back(0.0f); infos.push_back(0.0f);
        infos.push_back(RPositionX - (Rwidth/2)); infos.push_back(RPositionY + (Rheight/2));
        infos.push_back(0.0f); infos.push_back(1.0f);
    }  
    unsigned int indices1[] = {
                        0, 1, 3,   1,  2, 3,    1,  4, 2,   4, 5, 2,
                        4, 6, 5,   6,  7, 5,    6,  8, 7,   8, 9, 7,
                                   8, 10, 9,   10, 11, 9};
    float vertices[] = {
        infos.at(0), infos.at(1), 0.0f, infos.at(2), infos.at(3),
        infos.at(4), infos.at(5), 0.0f, infos.at(6), infos.at(7),
        infos.at(8), infos.at(9), 0.0f, infos.at(10), infos.at(11),
        infos.at(12), infos.at(13), 0.0f, infos.at(14), infos.at(15),
        
        infos.at(16), infos.at(17), 0.0f, infos.at(18), infos.at(19),
        infos.at(20), infos.at(21), 0.0f, infos.at(22), infos.at(23),
        infos.at(24), infos.at(25), 0.0f, infos.at(26), infos.at(27),
        infos.at(28), infos.at(29), 0.0f, infos.at(30), infos.at(31),
        
        infos.at(32), infos.at(33), 0.0f, infos.at(34), infos.at(35),
        infos.at(36), infos.at(37), 0.0f, infos.at(38), infos.at(39),
        infos.at(40), infos.at(41), 0.0f, infos.at(42), infos.at(43),
        infos.at(44), infos.at(45), 0.0f, infos.at(46), infos.at(47)
    };
    unsigned int EBO0;
    glGenVertexArrays(1, &TXBOX_OBJ->VAO);
    glGenBuffers(1, &EBO0);
    glGenBuffers(1, &TXBOX_OBJ->VBO);
    glBindVertexArray(TXBOX_OBJ->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, TXBOX_OBJ->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO0);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices1), indices1, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    infos.clear();
    RTextureWidth = 2*((float)TXBOX_OBJ->mouseON->Texture->width/(float)windowHeight);
    RTextureHeight = 2*((float)TXBOX_OBJ->mouseON->Texture->height/(float)windowHeight);
    if ((size.x / size.y) < (RTextureWidth/RTextureHeight)){
        float buttonun1bolu10u = Rwidth / 5.0f;
        float NormalSIze = RTextureHeight*(Rwidth/RTextureWidth);
        float xinydekapladigialan = buttonun1bolu10u/NormalSIze;
        float leftSIZE = NormalSIze-(buttonun1bolu10u*2);

        infos.push_back(RPositionX + (Rwidth/2)); infos.push_back(RPositionY + (Rheight/2));
        infos.push_back(1.0f); infos.push_back(1.0f);
        infos.push_back(RPositionX + (Rwidth/2)); infos.push_back(RPositionY + (Rheight/2) - buttonun1bolu10u);
        infos.push_back(1.0f); infos.push_back(1 - xinydekapladigialan);
        infos.push_back(RPositionX - (Rwidth/2)); infos.push_back(RPositionY + (Rheight/2) - buttonun1bolu10u);
        infos.push_back(0.0f); infos.push_back(1 - xinydekapladigialan);
        infos.push_back(RPositionX - (Rwidth/2)); infos.push_back(RPositionY + (Rheight/2));
        infos.push_back(0.0f); infos.push_back(1.0f);

        infos.push_back(RPositionX + (Rwidth/2)); infos.push_back(RPositionY + (leftSIZE/2));
        infos.push_back(1.0f); infos.push_back(1 - xinydekapladigialan);
        infos.push_back(RPositionX - (Rwidth/2)); infos.push_back(RPositionY + (leftSIZE/2));
        infos.push_back(0.0f); infos.push_back(1 - xinydekapladigialan);
        infos.push_back(RPositionX + (Rwidth/2)); infos.push_back(RPositionY - (leftSIZE/2));
        infos.push_back(1.0f); infos.push_back(xinydekapladigialan);
        infos.push_back(RPositionX - (Rwidth/2)); infos.push_back(RPositionY - (leftSIZE/2));
        infos.push_back(0.0f); infos.push_back(xinydekapladigialan);

        infos.push_back(RPositionX + (Rwidth/2)); infos.push_back(RPositionY - (Rheight/2) + buttonun1bolu10u);
        infos.push_back(1.0f); infos.push_back(xinydekapladigialan);
        infos.push_back(RPositionX - (Rwidth/2)); infos.push_back(RPositionY - (Rheight/2) + buttonun1bolu10u);
        infos.push_back(0.0f); infos.push_back(xinydekapladigialan);
        infos.push_back(RPositionX + (Rwidth/2)); infos.push_back(RPositionY - (Rheight/2));
        infos.push_back(1.0f); infos.push_back(0.0f);
        infos.push_back(RPositionX - (Rwidth/2)); infos.push_back(RPositionY - (Rheight/2));
        infos.push_back(0.0f); infos.push_back(0.0f);
    }
    else{
        float buttonun1bolu10u = Rheight / 5.0f;
        float NormalSIze = RTextureWidth*(Rheight/RTextureHeight);
        float yninxdekapladigialan = buttonun1bolu10u/NormalSIze;
        float leftSIZE = NormalSIze-(buttonun1bolu10u*2);
        
        infos.push_back(RPositionX + (Rwidth/2)); infos.push_back(RPositionY - (Rheight/2));
        infos.push_back(1.0f); infos.push_back(0.0f);
        infos.push_back(RPositionX + (Rwidth/2) - buttonun1bolu10u); infos.push_back(RPositionY - (Rheight/2));
        infos.push_back(1-yninxdekapladigialan); infos.push_back(0.0f);
        infos.push_back(RPositionX + (Rwidth/2) - buttonun1bolu10u); infos.push_back(RPositionY + (Rheight/2));
        infos.push_back(1-yninxdekapladigialan); infos.push_back(1.0f);
        infos.push_back(RPositionX + (Rwidth/2)); infos.push_back(RPositionY + (Rheight/2));
        infos.push_back(1.0f); infos.push_back(1.0f);

        infos.push_back(RPositionX + (leftSIZE/2)); infos.push_back(RPositionY - (Rheight/2));
        infos.push_back(1-yninxdekapladigialan); infos.push_back(0.0f);
        infos.push_back(RPositionX + (leftSIZE/2)); infos.push_back(RPositionY + (Rheight/2));
        infos.push_back(1-yninxdekapladigialan); infos.push_back(1.0f);
        infos.push_back(RPositionX - (leftSIZE/2)); infos.push_back(RPositionY - (Rheight/2));
        infos.push_back(yninxdekapladigialan); infos.push_back(0.0f);
        infos.push_back(RPositionX - (leftSIZE/2)); infos.push_back(RPositionY + (Rheight/2));
        infos.push_back(yninxdekapladigialan); infos.push_back(1.0f);

        infos.push_back(RPositionX - (Rwidth/2) + buttonun1bolu10u); infos.push_back(RPositionY - (Rheight/2));
        infos.push_back(yninxdekapladigialan); infos.push_back(0.0f);
        infos.push_back(RPositionX - (Rwidth/2) + buttonun1bolu10u); infos.push_back(RPositionY + (Rheight/2));
        infos.push_back(yninxdekapladigialan); infos.push_back(1.0f);
        infos.push_back(RPositionX - (Rwidth/2)); infos.push_back(RPositionY - (Rheight/2));
        infos.push_back(0.0f); infos.push_back(0.0f);
        infos.push_back(RPositionX - (Rwidth/2)); infos.push_back(RPositionY + (Rheight/2));
        infos.push_back(0.0f); infos.push_back(1.0f);
    }
    float vertices0[] = {
        infos.at(0), infos.at(1), 0.0f, infos.at(2), infos.at(3),
        infos.at(4), infos.at(5), 0.0f, infos.at(6), infos.at(7),
        infos.at(8), infos.at(9), 0.0f, infos.at(10), infos.at(11),
        infos.at(12), infos.at(13), 0.0f, infos.at(14), infos.at(15),

        
        infos.at(16), infos.at(17), 0.0f, infos.at(18), infos.at(19),
        infos.at(20), infos.at(21), 0.0f, infos.at(22), infos.at(23),
        infos.at(24), infos.at(25), 0.0f, infos.at(26), infos.at(27),
        infos.at(28), infos.at(29), 0.0f, infos.at(30), infos.at(31),

        
        infos.at(32), infos.at(33), 0.0f, infos.at(34), infos.at(35),
        infos.at(36), infos.at(37), 0.0f, infos.at(38), infos.at(39),
        infos.at(40), infos.at(41), 0.0f, infos.at(42), infos.at(43),
        infos.at(44), infos.at(45), 0.0f, infos.at(46), infos.at(47)
    };
    glGenVertexArrays(1, &TXBOX_OBJ->mouseVAO);
    glGenBuffers(1, &EBO0);
    glGenBuffers(1, &TXBOX_OBJ->VBO);
    glBindVertexArray(TXBOX_OBJ->mouseVAO);
    glBindBuffer(GL_ARRAY_BUFFER, TXBOX_OBJ->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices0), vertices0, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO0);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices1), indices1, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    TXBOX_OBJ->text = defaultText;
    TXBOX_OBJ->index = UI->Objects.size();

    // Assign Font
    for(int a = 0; a < Fonts.size(); a++){
        if(Fonts.at(a)->FontPath == defaultFontPath && Fonts.at(a)->fontSize == defaultFontSize){
            TXBOX_OBJ->font = Fonts.at(a);
            break;
        }
    }
    if(TXBOX_OBJ->font == NULL){
        TXBOX_OBJ->font = Fonts.at(CreateFont(defaultFontPath, defaultFontSize));
    }
    TXBOX_OBJ->scale = scale;
    TXBOX_OBJ->Position[0] = position.x;
    TXBOX_OBJ->Position[1] = -position.y;
    TXBOX_OBJ->textColor[0] = Textcolor.x; TXBOX_OBJ->textColor[1] = Textcolor.y; TXBOX_OBJ->textColor[2] = Textcolor.z;
    
    // Switch to text insertion cursor
    if(cursorIBEAM == NULL) cursorIBEAM = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
    if(UI->mouseCur == NULL){
        for(int i = 0; i < Mouses.size(); i++){
            if(Mouses.at(i)->window == UI->window){UI->mouseCur = Mouses.at(i);
                std::cout << "Mouse found!" << std::endl;
            }
        }
        if(UI->mouseCur == NULL){
            UI->mouseCur = new Mythril_MOUSE;
            UI->mouseCur->window = UI->window;
            Mouses.push_back(UI->mouseCur);
            std::cout << "Mouse created!" << std::endl;
        }
    }
    if(UI->keyboard == NULL) UI->keyboard = new Mythril_KEYBOARD;
    UI->Objects.push_back(TXBOX_OBJ);
}
std::string TextBox::getText(){
    return TXBOX_OBJ->text;
}

// =====================================================================
// GLOBAL EVENT HANDLERS & INPUT SCANNERS
// =====================================================================

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
int CreateTexture(const char* texturePath){
    int index = Textures.size();
    Textures.push_back(new Mythril_Texture);
    Textures.at(index)->index = index;
    Textures.at(index)->ImagePath = texturePath;
    Textures.at(index)->Texture = new Texture(texturePath);
    std::cout << "Created Texture! " << std::endl;
    return index;
}
void defaultFont(std::string fontPath){
    defaultFontPath = fontPath;
}

// Raycasts mouse position, calculates AABB bounding box collision against widgets 
// and switches GLFW hardware cursor icon
void rescanMouse(Mythril_MOUSE* mouse){
    double WXpos, WYpos;
    glfwGetCursorPos(mouse->window, &WXpos, &WYpos);
    int wWidth, wHeight;
    glfwGetWindowSize(mouse->window, &wWidth, &wHeight);

    // Normalize GLFW top-left coordinate system to center-origin layout
    WXpos -= wWidth/2; WYpos -= wHeight/2; WYpos = -WYpos;

    int tempstate = glfwGetMouseButton(mouse->window, GLFW_MOUSE_BUTTON_LEFT);
    if(tempstate == mouse->recentState){
        if(mouse->recentState == GLFW_PRESS) mouse->state = GLFW_REPEAT;
    }
    else{mouse->state = tempstate; mouse->recentState = tempstate;}

    if(WXpos != mouse->mousePos[0] || WYpos != mouse->mousePos[1]){
        mouse->mouseOnButton = false;
        mouse->mousePos[0] = WXpos; mouse->mousePos[1] = WYpos;
        for(int i = 0; i < Frames.size(); i++){
            if(Frames.at(i)->window == mouse->window){
                for(int a = 0; a < Frames.at(i)->Objects.size(); a++){

                    // AABB Check for Buttons
                    if(std::holds_alternative<Mythril_BUTTON*>(Frames.at(i)->Objects.at(a))){
                        if((std::get<Mythril_BUTTON*>(Frames.at(i)->Objects.at(a))->Position[0] - (std::get<Mythril_BUTTON*>(Frames.at(i)->Objects.at(a))->Size[0]/2)) <= WXpos && WXpos <= (std::get<Mythril_BUTTON*>(Frames.at(i)->Objects.at(a))->Position[0] + (std::get<Mythril_BUTTON*>(Frames.at(i)->Objects.at(a))->Size[0]/2)) &&
                            (std::get<Mythril_BUTTON*>(Frames.at(i)->Objects.at(a))->Position[1] - (std::get<Mythril_BUTTON*>(Frames.at(i)->Objects.at(a))->Size[1]/2)) <= WYpos && WYpos <= (std::get<Mythril_BUTTON*>(Frames.at(i)->Objects.at(a))->Position[1] + (std::get<Mythril_BUTTON*>(Frames.at(i)->Objects.at(a))->Size[1]/2))){
                                mouse->mouseOnButton = std::get<Mythril_BUTTON*>(Frames.at(i)->Objects.at(a));
                                glfwSetCursor(mouse->window, cursorHand);
                        }
                    }

                    // AABB Check for TextBoxes
                    else if(std::holds_alternative<Mythril_TextBOX*>(Frames.at(i)->Objects.at(a))){
                        if((std::get<Mythril_TextBOX*>(Frames.at(i)->Objects.at(a))->Position[0] - (std::get<Mythril_TextBOX*>(Frames.at(i)->Objects.at(a))->Size[0]/2)) <= WXpos && WXpos <= (std::get<Mythril_TextBOX*>(Frames.at(i)->Objects.at(a))->Position[0] + (std::get<Mythril_TextBOX*>(Frames.at(i)->Objects.at(a))->Size[0]/2)) &&
                            (std::get<Mythril_TextBOX*>(Frames.at(i)->Objects.at(a))->Position[1] - (std::get<Mythril_TextBOX*>(Frames.at(i)->Objects.at(a))->Size[1]/2)) <= WYpos && WYpos <= (std::get<Mythril_TextBOX*>(Frames.at(i)->Objects.at(a))->Position[1] + (std::get<Mythril_TextBOX*>(Frames.at(i)->Objects.at(a))->Size[1]/2))){
                                mouse->mouseOnButton = std::get<Mythril_TextBOX*>(Frames.at(i)->Objects.at(a));
                                glfwSetCursor(mouse->window, cursorIBEAM);
                        }
                    }
                }

                // If not hovering anything, return cursor to default OS arrow
                if(std::holds_alternative<bool>(mouse->mouseOnButton))
                    glfwSetCursor(mouse->window, NULL);
            }
        }
    }
}

// Invoked natively by GLFW to catch raw character inputs and push them to active textbox
void rescanKeyboard(GLFWwindow* window, unsigned int codepoint){
    for(int i = 0; i < Frames.size(); i++){
        if(Frames.at(i)->window == window) if(Frames.at(i)->focusedTXBox != NULL){
            Frames.at(i)->focusedTXBox->text += (unsigned char) codepoint;
        }
    }
}

// Scans for modifier keys (Backspace, Arrows) using GLFW_REPEAT logic to allow "hold to delete" behaviour
void scanKeyboard(MythrilUI* UI){
    int tempstate = glfwGetKey(UI->window, GLFW_KEY_BACKSPACE);

    // Backspace hold logic
    if(tempstate == UI->keyboard->BACKSPACErecentstate){if(UI->keyboard->BACKSPACErecentstate == GLFW_PRESS) UI->keyboard->BACKSPACEstate = GLFW_REPEAT;}
    else {UI->keyboard->BACKSPACErecentstate = tempstate; UI->keyboard->BACKSPACEstate = tempstate;}
    
    if(UI->keyboard->BACKSPACEstate == GLFW_PRESS) 
        if(UI->focusedTXBox != NULL)
            if(UI->focusedTXBox->text != "") UI->focusedTXBox->text.pop_back(); // Pop last char safely
    tempstate = glfwGetKey(UI->window, GLFW_KEY_LEFT);
    if(tempstate == UI->keyboard->LEFTARROWrecentstate){if(UI->keyboard->LEFTARROWrecentstate == GLFW_PRESS) UI->keyboard->LEFTARROWstate = GLFW_REPEAT;}
    else {UI->keyboard->LEFTARROWrecentstate = tempstate; UI->keyboard->LEFTARROWstate = tempstate;}
    if(UI->keyboard->LEFTARROWstate == GLFW_PRESS) if(UI->focusedTXBox != NULL) if(UI->focusedTXBox->text != "") UI->focusedTXBox->text.pop_back();
    
    tempstate = glfwGetKey(UI->window, GLFW_KEY_RIGHT);
    if(tempstate == UI->keyboard->RIGHTARROWrecentstate){if(UI->keyboard->RIGHTARROWrecentstate == GLFW_PRESS) UI->keyboard->RIGHTARROWstate = GLFW_REPEAT;}
    else {UI->keyboard->RIGHTARROWrecentstate = tempstate; UI->keyboard->RIGHTARROWstate = tempstate;}
    if(UI->keyboard->RIGHTARROWstate == GLFW_PRESS) if(UI->focusedTXBox != NULL) if(UI->focusedTXBox->text != "") UI->focusedTXBox->text.pop_back();
}
