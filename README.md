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

 -GLFW & GLAD (OpenGL 3.3 Core Profile)

 -GLM (OpenGL Mathematics)

 -FreeType (Font engine)

### Compilation
Compile the files using a C++ compiler with C++17 support:

```Bash
g++ -std=c++17 -Iinclude src/*.cpp -lglfw -lglad -lfreetype -o MythrilUIApp
```
## 📄 License
This project is licensed under the MIT License - see the LICENSE file for details.
