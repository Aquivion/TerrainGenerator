## 3D Landscape Generator – C++ & OpenGL

A Bachelor Thesis by Tom Bazgar (FH Südwestfalen, 2016)

### 📖 Overview

This project demonstrates procedural generation of 3D landscapes using the Perlin Noise algorithm.
The system is written in C++, rendered with OpenGL, and uses SDL2 for window and input handling.
A custom GUI (also OpenGL-based) allows real-time manipulation of Noise- and terrain-parameters.

The goal of the thesis was to show how complex natural environments such as mountains, valleys, heightmaps, and texture patterns can be generated algorithmically at runtime — instead of manually creating assets.

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

Standard Perlin Noise

Billowy Noise

Ridgid Noise

Cosinus Noise (experimental)

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
