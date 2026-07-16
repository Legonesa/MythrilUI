# MythrilUI 🛡️

A custom, hardware-accelerated 2D User Interface framework built from scratch in C++ using **OpenGL 3.3 (GLFW / GLAD)**. 

Named after the mythical metal known for being incredibly lightweight yet stronger than steel, **MythrilUI** bypasses standard complex layout systems to offer low-level control over rendering pipelines. It features custom aspect-ratio-corrected geometry calculations, interactive states, and an advanced text rendering engine powered by **FreeType**.

---

## ⚠️ Project Status & Ongoing Modularization

> **Crucial Note:** This framework is actively under development. 
> While we are progressively refactoring the monolithic codebase into a fully modular design, the transition is ongoing:
> * **Fully Modularized:** `Image` and `Text` widgets have been successfully decoupled into their own standalone source and header files (`src/` and `include/`).
> * **In-Transition:** The core `Button` and `TextBox` interactive widgets, along with the mouse/keyboard polling systems, are currently preserved inside the `monolithic/` core. Decoupling them into separate modules is a top priority on our development roadmap.

---

## 🚀 Key Features

* **Aspect-Ratio Aware Geometry:** Implements custom vertex-coordinate scaling equations for buttons and text boxes, ensuring UI textures do not stretch or warp when resized.
* **Advanced Font Engine:** Powered by **FreeType**. Supports multi-line rendering, dynamic text alignment, automatic word-wrapping within bound boxes, and smart ellipsis (`...`) clipping.
* **Stateful Input System:** Low-level keyboard and mouse polling with standard cursor changes (e.g., pointing hand for buttons, I-beam for textboxes).
* **Modern Polymorphic Batching:** Manages UI elements recursively using type-safe variants (`std::variant`) to render images, text, and buttons in a single draw hierarchy.
* **GPU Utilities:** Lightweight, custom abstraction layers for OpenGL shaders, textures, and transformations.

---

## 📁 Directory Structure

The current layout of the project reflects our ongoing architectural migration from a monolithic engine to modular widgets:

```text
.
├── include/                   # Modular header files (.hpp)
│   ├── MythrilUI.hpp          # Core UI window manager
│   ├── MythrilUITYPE.hpp      # Object type definitions & global pools
│   ├── Mythril_IMAGE.hpp      # Standalone Image widget wrapper
│   ├── Mythril_TEXT.hpp       # Standalone Text widget wrapper
│   ├── shader.hpp             # GLSL shader compiler
│   ├── text.hpp               # FreeType layout manager
│   ├── texture.hpp            # Image loader (stb_image wrapper)
│   └── transform.hpp          # Projection and widescreen matrix generator
├── src/                       # Modular source files (.cpp)
│   ├── main.cpp               # Sample application & main render loop
│   ├── MythrilUI.cpp          # Event polling and layout setup
│   ├── Mythril_IMAGE.cpp      # Standalone Image implementation
│   ├── Mythril_TEXT.cpp       # Standalone Text implementation
│   ├── shader.cpp             # Shader loaders
│   ├── text.cpp               # FreeType glyph texture generator
│   ├── texture.cpp            # STB implementation
│   └── transform.cpp          # GLM matrix calculations
├── monolithic/                # Interactive Monolithic Core 🧠
│   ├── MythrilUI_.cpp         # Contains complex 9-slice Button, TextBox, and I/O polling logic
│   └── MythrilUI_.hpp         # Internal variant wrappers for the monolithic components
├── shaders/                   # GLSL Pipeline
├── textures/                  # Default UI skin assets
├── fonts/                     # Default Open-Source fonts (Roboto)
└── README.md
```

## 🏗️ Technical Highlights
### 1. Aspect-Ratio Corrected Scaling (No Distortion)
To prevent button and texture warping on widescreen monitors, MythrilUI dynamically computes 12 custom vertices and indices on construction:

```c++
// Sliced layout calculation for aspect-ratio matching
float buttonun1bolu10u = Rwidth / 5.0f;
float NormalSIze = RTextureHeight * (Rwidth / RTextureWidth);
float xinydekapladigialan = buttonun1bolu10u / NormalSIze;
```
This enables flexible rectangular buttons while preserving the exact scale of corner details.

### 2. Custom Word Wrapping with FreeType
Instead of simple character printing, the text engine calculates dynamic line wraps based on box margins:

```c++
if (getTextWidth(tmpword, scale) < maxX) {
    // Computes layout bounds, injects ellipsis if text exceeds maximum heights
}
```

## 💻 Getting Started
### Prerequisites
Make sure you have the following libraries installed on your system:

 - GLFW & GLAD (OpenGL 3.3 Core Profile)

 - GLM (OpenGL Mathematics)

 - FreeType (Font engine)

### Compilation
Compile the files using a C++ compiler with C++17 support:

```Bash
g++ -std=c++17 -Iinclude src/*.cpp -lglfw -lglad -lfreetype -o MythrilUIApp
```

### 🛠️ Quick Start (Usage Example)

MythrilUI is designed to be incredibly simple to instantiate. Once you have your standard GLFW window set up, creating and rendering UI elements takes only a few lines of code:

```cpp
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <MythrilUI.hpp>
#include <Mythril_TEXT.hpp>

int main() {
    // 1. Standard GLFW & GLAD Initialization
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(1280, 720, "MythrilUI Example", NULL, NULL);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // 2. Initialize the MythrilUI Main Frame
    MythrilUI mainFrame(window);

    // 3. Create UI Widgets (Automatically bound to mainFrame)
    // Parameters: parent, text, X, Y, scale, RGB color
    Text greetingText(&mainFrame, "Hello, MythrilUI!", 0, 0, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));

    // 4. Main Render Loop
    while(!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Update widget properties dynamically if needed
        // greetingText.UpdateText("New Text!");

        // Render the entire UI tree in a single call
        mainFrame.RenderUI();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}
```
## 📚 API Reference (Function Guide)

MythrilUI is designed with an intuitive, object-oriented API. Below is a quick reference guide for the core classes and their available public methods.

### 🖥️ MythrilUI (Main Core)
The engine manager that handles the render queue, input polling, and shader states.
* **`MythrilUI(GLFWwindow* Window)`** Initializes the UI manager and binds it to the active GLFW window.
* **`void RenderUI()`** The core loop function. Processes mouse/keyboard inputs, calculates aspect-ratio matrices, and draws all widgets. Must be placed inside your main render loop.

---

### 🖼️ Image
A modular widget for rendering static 2D textures. Supports multiple overloaded constructors for different `glm::vec2` or primitive float coordinates.
* **Constructor:** `Image(MythrilUI* UI, float width, float height, float x, float y, const char* ImagePath)`
* **`void UpdateSize(float x, float y)` / `UpdateSize(glm::vec2 size)`**
  Dynamically recalculates the quad vertices to resize the image without breaking the aspect ratio.
* **`void UpdatePosition(float x, float y)` / `UpdatePosition(glm::vec2 pos)`**
  Moves the image to a new layout coordinate.
* **`void UpdateTexture(const char* ImagePath)`**
  Changes the displayed texture. Includes a smart garbage collector that frees the old texture from VRAM if no other widget is using it.
* **`void enable(bool value)`**
  Toggles the visibility of the image (true = visible, false = hidden).
* **`void Delete()`**
  Safely removes the image from the render queue and frees associated memory.

---

### 🔤 Text
A modular widget that renders FreeType characters dynamically.
* **Constructor:** `Text(MythrilUI* UI, std::string text, float x, float y, float scale, glm::vec3 color)`
* **`void UpdateText(std::string text)`**
  Changes the current string displayed on the screen.
* **`void ChangeFont(std::string fontPath)`**
  Switches the font face. Automatically manages FreeType face caches to prevent memory leaks.
* **`void UpdateScale(float scale)`**
  Adjusts the size multiplier of the font.
* **`void UpdatePosition(float x, float y)` / `UpdatePosition(glm::vec2 pos)`**
  Updates the baseline layout position.
* **`void enable(bool value)`**
  Toggles text visibility.
* **`void Delete()`**
  Removes the text from the render queue and cleans up resources.

---

### 🔘 Button (Interactive)
A stateful widget that utilizes a 9-slice algorithm to prevent texture distortion on resize. Handles its own hover and click states.
* **Constructor:** `Button(MythrilUI* UI, glm::vec2 size, glm::vec2 pos, const char* skinPath, const char* hoverSkinPath, std::string text, float scale, glm::vec3 textColor)`
* **`void OnClicked(void(*func)())`**
  Binds a function pointer (callback). The bound function will trigger automatically when the user clicks the button.
  *Example:* `myButton.OnClicked(myCustomFunction);`
* **`bool isClicked()`**
  Returns `true` if the button was clicked during the current frame. Useful if you prefer polling over callbacks.

---

### ⌨️ TextBox (Interactive)
An interactive input field with I-beam cursor rendering, keyboard polling, and text constraint margins.
* **Constructor:** `TextBox(MythrilUI* UI, glm::vec2 size, glm::vec2 pos, float scale, glm::vec3 textColor, std::string defaultText)`
* **`std::string getText()`**
  Retrieves the real-time string value currently written inside the input box by the user.

## 📄 License
This project is licensed under the MIT License - see the LICENSE file for details.
