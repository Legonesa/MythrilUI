#include <iostream>
#include <MythrilUI.hpp>
#include <transform.hpp>

MythrilUI::MythrilUI(GLFWwindow* Window){
    window = Window;
    // Load UI Shaders automatically if not already loaded
    if(UIShader == NULL) UIShader = new Shader("shaders/UIvShader.glsl", "shaders/UIfShader.glsl");
}

void MythrilUI::RenderUI(){
    int wWidth, wHeight;
    glfwGetWindowSize(window, &wWidth, &wHeight);

    // Dynamically calculate screen aspect ratio to prevent widescreen stretching
    Transform trans;
    trans.setValue(0, 0, ((float)wHeight/(float)wWidth));

    UIShader->use();
    UIShader->setInt("texture1", 0);
    UIShader->setMat4("wideScreen", trans.trans);

    // Iterate through the type-safe variant vector and render components
    for(int i = 0; i < Objects.size(); i++){

        // 1. RENDER MODULAR IMAGE
        if(std::holds_alternative<Mythril_IMAGE*>(Objects.at(i))){
            if(std::get<Mythril_IMAGE*>(Objects.at(i))->isEnable == true){
                UIShader->use();
                std::get<Mythril_IMAGE*>(Objects.at(i))->texture->Texture->useTextureAs(GL_TEXTURE0);
                glBindVertexArray(std::get<Mythril_IMAGE*>(Objects.at(i))->VAO);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
        }
        // 2. RENDER MODULAR TEXT
        else if(std::holds_alternative<Mythril_TEXT*>(Objects.at(i))){
            if(std::get<Mythril_TEXT*>(Objects.at(i))->isEnable == true)
                std::get<Mythril_TEXT*>(Objects.at(i))->font->Renderer->RenderText(
                    wWidth, wHeight, 
                    std::get<Mythril_TEXT*>(Objects.at(i))->text, 
                    std::get<Mythril_TEXT*>(Objects.at(i))->Position[0], 
                    std::get<Mythril_TEXT*>(Objects.at(i))->Position[1], 
                    std::get<Mythril_TEXT*>(Objects.at(i))->scale, 
                    glm::vec3(std::get<Mythril_TEXT*>(Objects.at(i))->color[0], 
                    std::get<Mythril_TEXT*>(Objects.at(i))->color[1], 
                    std::get<Mythril_TEXT*>(Objects.at(i))->color[2])
                );
        }
        // 3. RENDER MODULAR BUTTON (If migrated)
        else if(std::holds_alternative<Mythril_BUTTON*>(Objects.at(i))){
            if(std::get<Mythril_BUTTON*>(Objects.at(i))->isEnable == true){
                if(std::holds_alternative<Mythril_Texture*>(std::get<Mythril_BUTTON*>(Objects.at(i))->skin)){
                    UIShader->use();
                    std::get<Mythril_Texture*>(std::get<Mythril_BUTTON*>(Objects.at(i))->skin)->Texture->useTextureAs(GL_TEXTURE0);
                    glBindVertexArray(std::get<Mythril_BUTTON*>(Objects.at(i))->VAO);
                    glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_INT, 0);

                    std::get<Mythril_BUTTON*>(Objects.at(i))->BUTTON_text->font->Renderer->RenderText(
                        wWidth, wHeight, 
                        std::get<Mythril_BUTTON*>(Objects.at(i))->BUTTON_text->text, 
                        std::get<Mythril_BUTTON*>(Objects.at(i))->BUTTON_text->Position[0], 
                        std::get<Mythril_BUTTON*>(Objects.at(i))->BUTTON_text->Position[1], 
                        std::get<Mythril_BUTTON*>(Objects.at(i))->BUTTON_text->scale, 
                        glm::vec3(std::get<Mythril_TEXT*>(Objects.at(i))->color[0], 
                        std::get<Mythril_TEXT*>(Objects.at(i))->color[1], 
                        std::get<Mythril_TEXT*>(Objects.at(i))->color[2])
                    );
                }
            }
        }
    }

    // Clear states after UI batch is finished
    glBindVertexArray(0);
    glUseProgram(0);
}