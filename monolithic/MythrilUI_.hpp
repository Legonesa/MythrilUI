#ifndef MYTHRILUI_HPP
#define MYTHRILUI_HPP

#include <text.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <variant>
#include <transform.hpp>

// =====================================================================
// GLOBAL CONSTANTS & CONFIGURATIONS
// =====================================================================
static std::string defaultFontPath = "fonts/Roboto-Regular.ttf";
static const char* defaultTXBXPath = "textures/TXBox0.png";
static const char* defaultTXBXOMBPath = "textures/TXBox1.png";
static unsigned int defaultFontSize = 48;

// Cursor blinking and input parameters
static double _lastTime_ = 0.0;
static bool _IBEAM = false;                     // Blinking state for the text input cursor
static double _IBEAMspeed_ = 0.43f;             // Interval speed for the blink (in seconds)
static float _TEXTBOXverticalMARGIN_= 20.0f;    // Padding inside textbox (left/right)
static float _TEXTBOXhorizontalMARGIN_= 20.0f;  // Padding inside textbox (top/bottom)

// Global GLFW Cursors for interactive hover states
static GLFWcursor* cursorHand = NULL;
static GLFWcursor* cursorIBEAM = NULL;
static Shader* UIShader = NULL;

// =====================================================================
// MONOLITHIC INTERNAL STRUCTURES
// =====================================================================
struct Font
{
    unsigned int index;
    TextRendeer* Renderer;
    std::string FontPath;
    unsigned int fontSize = defaultFontSize;
};

struct Mythril_Texture
{
    Texture* Texture;
    const char* ImagePath;
    int index;
};

struct Mythril_IMAGE
{
    unsigned int index;
    unsigned int VAO, VBO;
    bool isEnable = true;
    float Position[2];
    float Size[2];
    Mythril_Texture* texture = NULL;
};

struct Mythril_TEXT
{
    Font* font = NULL;
    std::string text = "";
    bool isEnable = true;
    unsigned int index;
    float Position[2];
    float scale;
    float color[3];
};

struct Mythril_TextBOX
{
    Font* font = NULL;
    std::string text = "";
    unsigned int VAO, VBO, mouseVAO, index; // mouseVAO holds geometry for hover state
    float scale;
    float color[3] = {255, 255, 255};
    float textColor[3] = {0, 0, 0};
    float Position[2];
    float Size[2];
    Mythril_Texture* skin = NULL;      // Default background texture
    Mythril_Texture* mouseON = NULL;   // Focused/Hovered background texture
    bool isFocused = false;         // Determines if keyboard input writes to this box
    bool isEnable = true;
};

struct Mythril_BUTTON
{
    Font* font = NULL;
    std::string text = "";
    bool isEnable = true;
    unsigned int VAO, mouseVAO, VBO, index;
    float scale;
    float Position[2];
    float textColor[3] = {255, 255, 255};
    float Size[2];
    Mythril_Texture* skin = NULL;
    Mythril_Texture* mouseON = NULL;
    bool isClicked = false;
    void (*onClickedFunc)() = NULL; // Callback pointer for click events
};

// Tracks mouse position and collision states
struct Mythril_MOUSE
{
    int state, recentState;
    GLFWwindow* window;
    double mousePos[2] = {NULL, NULL};
    std::variant<Mythril_TextBOX*, Mythril_BUTTON*, bool> mouseOnButton; // Dynamic reference to hovered widget
};

// Tracks keyboard states specifically for repeat-delays (e.g., holding Backspace)
struct Mythril_KEYBOARD
{
    int BACKSPACErecentstate = 0;
    int BACKSPACEstate = 0;
    int LEFTARROWrecentstate = 0;
    int LEFTARROWstate = 0;
    int RIGHTARROWrecentstate = 0;
    int RIGHTARROWstate = 0;
};

// =====================================================================
// MONOLITHIC MANAGER & CLASSES
// =====================================================================

class MythrilUI
{
public:
    GLFWwindow* window;
    Mythril_MOUSE* mouseCur = NULL;
    Mythril_KEYBOARD* keyboard = NULL;
    Mythril_TextBOX* focusedTXBox = NULL;
    // Core render queue containing all active widgets
    std::vector<std::variant<Mythril_IMAGE*, Mythril_TEXT*, Mythril_BUTTON*, Mythril_TextBOX*>> Objects;
    
    MythrilUI(GLFWwindow* Window);
    
    // Executes logic, polling, and draw calls for the frame
    void RenderUI();
    //void Delete();
    //~MythrilUI();
};

class Image
{
private:
    MythrilUI* UI;
    Mythril_IMAGE* IMG_OBJ = new Mythril_IMAGE;
public:
    Image(MythrilUI* UIw, float width, float height, float positionX, float positionY, const char* ImagePath);
    Image(MythrilUI* UIw, float width, float height, glm::vec2 position, const char* ImagePath);
    Image(MythrilUI* UIw, glm::vec2 size, glm::vec2 position, const char* ImagePath);
    Image(MythrilUI* UIw, glm::vec2 position, const char* ImagePath);
    Image(MythrilUI* UIw, float positionX, float positionY, const char* ImagePath);
    void UpdateSize(glm::vec2 size);
    void UpdateSize(float sizeX, float sizeY);
    void UpdatePosition(glm::vec2 position);
    void UpdatePosition(float positionX, float positionY);
    void UpdateTexture(const char* ImagePath);
    void enable(bool value);
    void Delete();
    ~Image();
};

class Text
{
private:
    MythrilUI* UI;
    Mythril_TEXT* TX_OBJ = new Mythril_TEXT;
public:
    Text(MythrilUI* UIw, std::string text, float positionX, float positionY, float scale, glm::vec3 color);
    Text(MythrilUI* UIw, std::string fontPath, std::string text, float positionX, float positionY, float scale, glm::vec3 color);
    Text(MythrilUI* UIw, std::string fontPath, std::string text, glm::vec2 position, float scale, glm::vec3 color);
    Text(MythrilUI* UIw, std::string text, glm::vec2 position, float scale, glm::vec3 color);
    void ChangeFont(std::string fontPath);
    void UpdateText(std::string text);
    void UpdateScale(float scale);
    void UpdatePosition(glm::vec2 position);
    void UpdatePosition(float positionX, float positionY);
    void enable(bool value);
    void Delete();
    ~Text();
};

class Button
{
private:
    MythrilUI* UI;
    Mythril_BUTTON* ButtonOBJ = new Mythril_BUTTON;
public:
    Button(MythrilUI* UIw, glm::vec2 size, glm::vec2 position, const char* ImagePath, const char* MouseONPath, std::string text, float scale, glm::vec3 Textcolor);
    void OnClicked(void(*func)());
    bool isClicked();
};

class TextBox
{
private:
    MythrilUI* UI;
    Mythril_TextBOX* TXBOX_OBJ = new Mythril_TextBOX;
public:
    TextBox(MythrilUI* UIw, glm::vec2 size, glm::vec2 position, float scale, glm::vec3 Textcolor, std::string defaultText = "");
    std::string getText();
};

// =====================================================================
// GLOBAL RESOURCE POOLS & HELPERS
// =====================================================================

static std::vector<Font*> Fonts;
static std::vector<Mythril_Texture*> Textures;
static std::vector<Mythril_MOUSE*> Mouses;
static std::vector<MythrilUI*> Frames;

void defaultFont(std::string fontPath);
void rescanMouse(Mythril_MOUSE* mouse);
void rescanKeyboard(GLFWwindow* window, unsigned int codepoint);
void scanKeyboard(MythrilUI* UI);
int CreateFont(std::string fontPath, unsigned int fontSize);
int CreateTexture(const char* texturePath);


#endif