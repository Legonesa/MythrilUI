#include <text.hpp>

// =====================================================================
// FREETYPE FONT COMPILER
// =====================================================================
TextRendeer::TextRendeer(std::string font, unsigned int fontSize){
    if(TextShader == NULL) TextShader = new Shader("shaders/TextVShader.glsl", "shaders/TextFShader.glsl");
    TextShader->use();
    TextShader->setInt("text", 0);
    fontSIZE = fontSize;

    // Configure dynamic Quad VBO for character placement
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    Characters.clear();
    
    // Initialize FreeType Library and load face
    FT_Library ft;    
    if (FT_Init_FreeType(&ft))
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    FT_Face face;
    if (FT_New_Face(ft, font.c_str(), 0, &face))
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    FT_Set_Pixel_Sizes(face, 0, fontSize);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

    // Load first 128 ASCII characters into OpenGL Textures
    for (GLubyte c = 0; c < 128; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
            );

        // Texture Filtering specific to sharp fonts
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        // Cache character metadata metrics
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}


// =====================================================================
// STANDARD SINGLE LINE RENDER
// =====================================================================
void TextRendeer::RenderText(unsigned int width, unsigned int height, std::string text, float x, float y, float scale, glm::vec3 color){
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    TextShader->use();

    // Standard Orthographic projection for 2D UI space
    glm::mat4 matrix = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f);
    TextShader->setMat4("projection", matrix);
    TextShader->setVec2("Middle", width, height);
    TextShader->setVec3("textColor", color);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    std::string::const_iterator c;

    // Pivot alignment correction
    x -= this->getTextWidth(text, scale)/2;
    y -= this->fontSIZE*scale/2;

    // Iterate string and draw textured quads letter by letter
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y + (Characters['H'].Bearing.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        // Quad geometry mapping
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 0.0f },
            { xpos,     ypos,       0.0f, 0.0f },

            { xpos,     ypos + h,   0.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 0.0f }
        };
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Move layout cursor for the next glyph
        x += (ch.Advance >> 6) * scale;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glUseProgram(0);
}

// =====================================================================
// MULTI-LINE BOUNDING BOX RENDER WITH ELLIPSIS CLIPPING
// Parses string into words and wraps lines dynamically within margins.
// Appends "..." if height limits are reached.
// =====================================================================
void TextRendeer::RenderText(unsigned int width, unsigned int height, std::string text, float x, float y, float scale, float maxX, float maxY, glm::vec3 color){
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    TextShader->use();
    glm::mat4 matrix = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f);
    TextShader->setMat4("projection", matrix);
    TextShader->setVec2("Middle", width, height);
    TextShader->setVec3("textColor", color);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    std::string::const_iterator c;
    std::vector<std::string> words;

    float BaseX = x;
    x -= this->getTextWidth(text, scale)/2;
    y -= (this->fontSIZE*scale*GetLineCount(text, scale, maxX, maxY, &words))/2;
    float BaseY = y;

    std::string tmpword = "";

    // Core wrapping algorithm iterating through words
    for (int i = 0; i < words.size(); i++){
        if(tmpword != "") tmpword += " " + words.at(i);
        else tmpword += words.at(i);

        // Check if the current line fits the maximum horizontal box width
        if(getTextWidth(tmpword, scale) < maxX){
                if(i != words.size()-1){
                    // Lookahead: Will the next word overflow?
                    if(getTextWidth(tmpword + " " +words.at(i+1), scale) > maxX){
                        x = BaseX - this->getTextWidth(tmpword, scale)/2;

                        // Check if adding a new line exceeds the vertical box limit
                        if(maxY < (y + this->fontSIZE*scale) - BaseY){
                            tmpword += "..."; // Inject ellipsis to clip the text elegantly
                        }

                        // Render line characters
                        for (c = tmpword.begin(); c != tmpword.end(); c++){
                            Character ch = Characters[*c];

                            float xpos = x + ch.Bearing.x * scale;
                            float ypos = y + (Characters['H'].Bearing.y - ch.Bearing.y) * scale;

                            float w = ch.Size.x * scale;
                            float h = ch.Size.y * scale;
                            float vertices[6][4] = {
                                { xpos,     ypos + h,   0.0f, 1.0f },
                                { xpos + w, ypos,       1.0f, 0.0f },
                                { xpos,     ypos,       0.0f, 0.0f },

                                { xpos,     ypos + h,   0.0f, 1.0f },
                                { xpos + w, ypos + h,   1.0f, 1.0f },
                                { xpos + w, ypos,       1.0f, 0.0f }
                            };
                            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
                            glBindBuffer(GL_ARRAY_BUFFER, VBO);
                            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
                            glBindBuffer(GL_ARRAY_BUFFER, 0);
                            glDrawArrays(GL_TRIANGLES, 0, 6);
                            x += (ch.Advance >> 6) * scale;
                        }
                        tmpword = "";
                        // Terminate drawing completely if vertical boundary exceeded
                        if(maxY < (y + this->fontSIZE*scale) - BaseY){
                            break;
                        }
                        y += this->fontSIZE*scale; // Move baseline down
                    }
                    else continue;
                }
                // Handle final line processing ... (similar logic execution continues)
                // [Logic execution omitted for comment brevity but works exactly the same]
                else{
                    x = BaseX - this->getTextWidth(tmpword, scale)/2;
                    if(maxY < (y + this->fontSIZE*scale) - BaseY){
                        tmpword += "...";
                    }
                    for (c = tmpword.begin(); c != tmpword.end(); c++)
                    {
                        Character ch = Characters[*c];

                        float xpos = x + ch.Bearing.x * scale;
                        float ypos = y + (Characters['H'].Bearing.y - ch.Bearing.y) * scale;

                        float w = ch.Size.x * scale;
                        float h = ch.Size.y * scale;
                        float vertices[6][4] = {
                            { xpos,     ypos + h,   0.0f, 1.0f },
                            { xpos + w, ypos,       1.0f, 0.0f },
                            { xpos,     ypos,       0.0f, 0.0f },

                            { xpos,     ypos + h,   0.0f, 1.0f },
                            { xpos + w, ypos + h,   1.0f, 1.0f },
                            { xpos + w, ypos,       1.0f, 0.0f }
                        };
                        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
                        glBindBuffer(GL_ARRAY_BUFFER, VBO);
                        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
                        glBindBuffer(GL_ARRAY_BUFFER, 0);
                        glDrawArrays(GL_TRIANGLES, 0, 6);
                        x += (ch.Advance >> 6) * scale;
                    }
                    tmpword = "";
                    if(maxY < (y + this->fontSIZE*scale) - BaseY){
                        break;
                    }
                    y += this->fontSIZE*scale/2;
                }
            }

            else{
                x = BaseX - this->getTextWidth(tmpword, scale)/2;
                if(maxY < (y + this->fontSIZE*scale) - BaseY){
                    tmpword += "...";
                }
                for (c = tmpword.begin(); c != tmpword.end(); c++)
                {
                    Character ch = Characters[*c];

                    float xpos = x + ch.Bearing.x * scale;
                    float ypos = y + (Characters['H'].Bearing.y - ch.Bearing.y) * scale;

                    float w = ch.Size.x * scale;
                    float h = ch.Size.y * scale;
                    float vertices[6][4] = {
                        { xpos,     ypos + h,   0.0f, 1.0f },
                        { xpos + w, ypos,       1.0f, 0.0f },
                        { xpos,     ypos,       0.0f, 0.0f },

                        { xpos,     ypos + h,   0.0f, 1.0f },
                        { xpos + w, ypos + h,   1.0f, 1.0f },
                        { xpos + w, ypos,       1.0f, 0.0f }
                    };
                    glBindTexture(GL_TEXTURE_2D, ch.TextureID);
                    glBindBuffer(GL_ARRAY_BUFFER, VBO);
                    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
                    glBindBuffer(GL_ARRAY_BUFFER, 0);
                    glDrawArrays(GL_TRIANGLES, 0, 6);
                    x += (ch.Advance >> 6) * scale;
                }
                tmpword = "";
                if(maxY < (y + this->fontSIZE*scale) - BaseY){
                    break;
                }
                y += this->fontSIZE*scale;
            }
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glUseProgram(0);
}

// Render dynamic input text (Shifts text left when it overflows right bounds)
void TextRendeer::RenderTextBOX(unsigned int width, unsigned int height, std::string text, float x, float y, float scale, float maxX, float MaxY, float _TEXTBOXverticalMARGIN_ , glm::vec3 color){
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    TextShader->use();
    glm::mat4 matrix = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f);
    TextShader->setMat4("projection", matrix);
    TextShader->setVec2("Middle", width, height);
    TextShader->setVec3("textColor", color);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);
    std::string::const_iterator c;
    x -= (maxX - _TEXTBOXverticalMARGIN_) /2;
    y -= (this->fontSIZE*scale)/2;
    std::string tmpword = "";
    for(int i = text.size()-1; i >= 0; i--){
        tmpword = text.at(i) + tmpword;
        if(getTextWidth(tmpword, scale) > maxX - (2*_TEXTBOXverticalMARGIN_)) break;
    }
    for (c = tmpword.begin(); c != tmpword.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y + (Characters['H'].Bearing.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 0.0f },
            { xpos,     ypos,       0.0f, 0.0f },

            { xpos,     ypos + h,   0.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 0.0f }
        };
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        x += (ch.Advance >> 6) * scale;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glUseProgram(0);
}

// Utility: Sums up advance metrics to return absolute screen-pixel width of string
float TextRendeer::getTextWidth(std::string text, float scale){
    float x = 0;
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];
        x += (ch.Advance >> 6) * scale;
    }
    return x;
}

// Utility: Splits text and calculates total needed lines before rendering
int TextRendeer::GetLineCount(std::string text, float scale, float maxX, float maxY, std::vector<std::string>* words){
    int linecount= 0;
    std::string tmpword = "";
    float y = 0;
    for (int i = 0; i < text.size(); i++){
        if(text.at(i) != ' '){
            tmpword += text.at(i);
        }
        else{
            words->push_back(tmpword);
            tmpword = "";
        }
    }
    words->push_back(tmpword); tmpword = "";
    for (int i = 0; i < words->size(); i++){
        if(tmpword != "") tmpword += " " + words->at(i);
        else tmpword += words->at(i);

        if(getTextWidth(tmpword, scale) < maxX){
            if(i != words->size()-1){
                if(getTextWidth(tmpword + " " + words->at(i+1), scale) > maxX){
                    linecount ++;
                    if(maxY < (y + this->fontSIZE*scale)){
                        break;
                    }
                    y += this->fontSIZE*scale;
                    tmpword = "";
                }
                else continue;
            }
            else{
                linecount ++;
                if(maxY < (y + this->fontSIZE*scale)){
                    break;
                }
                y += this->fontSIZE*scale;
                tmpword = "";
            }
        }
        else{
            linecount ++;
            if(maxY < (y + this->fontSIZE*scale)){
                break;
            }
            y += this->fontSIZE*scale;
            tmpword = "";
        }

    }
    return linecount;
}
