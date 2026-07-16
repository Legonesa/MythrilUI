#include <iostream>
#include <MythrilUI.hpp>
#include <MythrilUITYPE.hpp>
#include <Mythril_TEXT.hpp>

// =====================================================================
// TEXT WIDGET CONSTRUCTORS
// =====================================================================
Text::Text(MythrilUI* UIw, std::string text, float positionX, float positionY, float scale, glm::vec3 color){
    TX_OBJ->text = text; UI = UIw;
    TX_OBJ->index = UI->Objects.size();
    
    // Look up default font in the global cache to prevent duplicate rendering buffers
    for(int a = 0; a < Fonts.size(); a++){
        if(Fonts.at(a)->FontPath == defaultFontPath){
            TX_OBJ->font = Fonts.at(a);
            break;
        }
    }

    // Load font via FreeType if it hasn't been loaded yet
    if(TX_OBJ->font == NULL){
        TX_OBJ->font = Fonts.at(CreateFont(defaultFontPath, defaultFontSize));
    }

    // Apply Y-axis inversion to match 2D Screen Space mapping
    TX_OBJ->Position[0] = positionX; TX_OBJ->Position[1] = -positionY;
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
Text::Text(MythrilUI* UIw, std::string fontPath, std::string text, float positionX, float positionY, float scale, glm::vec3 color){
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
    TX_OBJ->Position[0] = positionX; TX_OBJ->Position[1] = positionY;
    TX_OBJ->scale = scale;
    TX_OBJ->color[0] = color.x; TX_OBJ->color[1] = color.y; TX_OBJ->color[2] = color.z;
    UI->Objects.push_back(TX_OBJ);
}
Text::Text(MythrilUI* UIw, std::string fontPath, std::string text, glm::vec2 position, float scale, glm::vec3 color){
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
    TX_OBJ->Position[0] = position.x; TX_OBJ->Position[1] = position.y;
    TX_OBJ->scale = scale;
    TX_OBJ->color[0] = color.x; TX_OBJ->color[1] = color.y; TX_OBJ->color[2] = color.z;
    UI->Objects.push_back(TX_OBJ);
}

// =====================================================================
// DYNAMIC MODIFIERS
// =====================================================================
void Text::ChangeFont(std::string fontPath){
    // Smart Garbage Collection for Fonts (Same logic as Textures)
    if(fontPath != TX_OBJ->font->FontPath){
        bool isChanged = false, FontUsing = false;

        // Scan if other texts rely on this font face
        for(int i = 0; i < UI->Objects.size(); i++){
            if(std::holds_alternative<Mythril_TEXT*>(UI->Objects.at(i)))
                if(std::get<Mythril_TEXT*>(UI->Objects.at(i))->font == TX_OBJ->font && TX_OBJ->index != i){
                    FontUsing = true;
                }
        }

        // Free FreeType resources if isolated
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

        // Fetch new font face
        for(int i = 0; i < Fonts.size(); i++){
            if(fontPath == Fonts.at(i)->FontPath){
                TX_OBJ->font = Fonts.at(i);
                isChanged = true;
                std::cout << "Font found!" << std::endl;
                break;
            }
        }
        if(isChanged == false){
            TX_OBJ->font = Fonts.at(CreateFont(fontPath, 48));
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
        }
        UI->Objects.pop_back();
        std::cout << "New Size of Objects:" << UI->Objects.size() << std::endl;
    }
    bool FontUsing = false;
    for(int i = 0; i < UI->Objects.size(); i++){
        if(std::holds_alternative<Mythril_TEXT*>(UI->Objects.at(i))) if(std::get<Mythril_TEXT*>(UI->Objects.at(i))->font == TX_OBJ->font) FontUsing = true;
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
void defaultFont(std::string fontPath){
    defaultFontPath = fontPath;
}
void ChangeDefFontSize(unsigned int fontSize){
    defaultFontSize = fontSize;
}