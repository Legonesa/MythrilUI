#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <shader.hpp>
#include <transform.hpp>
#include <texture.hpp>
#include <MythrilUI.hpp>
#include <Mythril_TEXT.hpp>
#include <Mythril_IMAGE.hpp>
#include <iostream>

using namespace std;

// =====================================================================
// DEDICATED GPU TRIGGER
// Forces laptops with dual GPUs to use the high-performance dedicated GPU 
// (NVIDIA/AMD) instead of the integrated Intel chip.
// =====================================================================
extern "C" {
__declspec(dllexport) unsigned long NvOptimusEnablement = 1;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

// Function Prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void onclickedfunction();
void RenderText();
void showFPS();

// Global Window Configurations
int SCR_WIDTH = 1280;
int SCR_HEIGHT = 720;
double fps = 0.0;
bool show = false;
Transform trans;
glm::mat4 wideScreenMat;

int main(){
    // 1. Initialize GLFW and configure OpenGL context (Version 3.3 Core Profile)
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, false);

#ifndef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // 2. Create Window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "MythEngine (Modular)", NULL, NULL);
    glfwSetKeyCallback(window, key_callback);
    if(window == NULL){
        cout << "Failed to creat GLFW window!" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 3. Initialize GLAD (Loads OpenGL function pointers)
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        cout << "Failed to initialize GLAD!" << endl;
        return -1;
    }

    // 4. Initialize Modular MythrilUI Frame
    MythrilUI mainFrame(window);
    Text FPS_text(&mainFrame, "0", -600, 330, 0.5f, glm::vec3(0.0f, 1.0f, 0.0f));

    // Calculate Aspect Ratio matrix to prevent distortion
    cout << "ObjectCount: " << mainFrame.Objects.size() << endl;

    trans.setValue(0, 0, ((float)SCR_HEIGHT/(float)SCR_WIDTH));
    wideScreenMat = trans.getTransform();

    // Disable V-Sync for uncapped framerate testing
    glfwSwapInterval(0);

    // =====================================================================
    // MAIN RENDER LOOP
    // =====================================================================
    while(!glfwWindowShouldClose(window)){
        // Clear screen with a background color
        glClearColor(0.7f, 0.5f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Update Frame Time Metrics
        showFPS();
        FPS_text.UpdateText(to_string((int)fps));
        
        // Draw all modular UI elements
        mainFrame.RenderUI();

        // Swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glfwTerminate();
    return 0;
}

// Quick exit on ESC key
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
        exit(-1);
    }
}

// Callback example
void onclickedfunction(){
    cout << "Button Clicked! Height:" << SCR_HEIGHT << endl;
}

// Adjusts viewport and updates aspect ratio matrix when window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    trans.setValue(0, 0, ((float)height / (float)width));
    wideScreenMat = trans.getTransform();
    glViewport(0, 0, width, height);
}

// Simple FPS counter using GLFW timer
int nbFrames = 0;
double lastTime = 0.0;
void showFPS()
{
     double currentTime = glfwGetTime();
     double delta = currentTime - lastTime;
     nbFrames++;
     if ( delta >= 1.0 ){
         fps = double(nbFrames) / delta;
         nbFrames = 0;
         lastTime = currentTime;
     }
}