## 3D Landscape Generator – C++ & OpenGL

### 📖 Overview

This project demonstrates procedural generation of 3D landscapes using the Perlin Noise algorithm.
The system is written in C++, rendered with OpenGL, and uses SDL2 for window and input handling.
A custom GUI (also OpenGL-based) allows real-time manipulation of Noise- and terrain-parameters.

<img width="798" height="614" alt="image" src="https://github.com/user-attachments/assets/2f7b5fdd-db5d-4276-ab7f-66428f48df57" />


### ✨ Features
🏞️ Procedural Terrain Generation

Fully custom Perlin Noise implementation (1D & 2D, layered/fractal).

Configurable:

Seed

Frequency & frequency multiplier

Amplitude

Number of layers (octaves)

Weighting of layers

Landscape size & vertex resolution

### 🌄 Rendering & Graphics

OpenGL 3.0 rendering pipeline

Custom Vertex, Fragment shaders for terrain & GUI

Normal map generation for real-time lighting

Additional seamless procedural texture generated from modified Perlin Noise

Perspective camera with movement & rotation (WASD + mouse look)

### 🧰 GUI (Custom-built)

Implemented entirely in OpenGL

Buttons, panels, labels, dynamic text rendering (via FreeType)

Real-time adjustment of terrain and noise parameters

Selection of Noise types:

#### Standard Perlin Noise
<img width="537" height="376" alt="image" src="https://github.com/user-attachments/assets/69b06fa5-8665-408e-b2cf-a6421c2d5e51" />

#### Billowy Noise
<img width="537" height="376" alt="image" src="https://github.com/user-attachments/assets/5be7d5ea-a44d-46b4-81d9-741efad95160" />

#### Ridgid Noise
<img width="567" height="407" alt="image" src="https://github.com/user-attachments/assets/31627b4e-132c-4b8b-a6d2-8ba03765f59b" />

#### Cosinus Noise (experimental)
<img width="584" height="388" alt="image" src="https://github.com/user-attachments/assets/99c185fb-8dea-4dfd-8b94-74f6a99ea4d3" />


### 👨‍💻 Technology Stack

C++11

OpenGL 3.0

SDL2 (window, keyboard/mouse, context creation)

GLEW (OpenGL extensions)

GLM (math library)

FreeType (font rendering)

### 🧪 Core Concepts Demonstrated

Procedural content generation (PCG)

Fractal noise & multi-layer Noise synthesis

Custom shader pipeline (GLSL)

Efficient GPU data handling via:

VBOs (Vertex Buffer Objects)

VAOs (Vertex Array Objects)

EBOs (Element Buffers)

Bilinear interpolation & smoothing kernels for Noise

Real-time terrain mesh generation

Camera system with relative mouse movement
