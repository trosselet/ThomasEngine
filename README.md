# Custom DirectX 12 Game Engine

## Overview
This project is a custom-built game engine developed using **DirectX 12**, designed with a modular and scalable architecture. The engine is divided into several core parts: **Render**, **Engine**, **Tools**, and **Gameplay**, allowing for clean separation of concerns and easier extensibility.

---

## Architecture

### 1. Render Module
The **Render** module is responsible for all low-level graphics operations using DirectX 12.

#### Features:
- DirectX 12 device and swapchain management
- Command queue and command list handling
- Descriptor heaps and resource binding
- Pipeline State Objects (PSO)
- Shader management (HLSL)
- Render passes
- Shadow mapping support
- GPU resource synchronization

This layer is designed to be as close to the GPU as possible, providing high performance and flexibility.

---

### 2. Engine Module
The **Engine** module contains the core systems of the engine, including the **Entity Component System (ECS)**.

#### Features:
- Custom ECS implementation:
  - Lightweight entity IDs
  - Component storage in contiguous arrays
  - Fast iteration over entities
- Transform system
- Scene management
- Resource management (textures, meshes, shaders)
- Memory handling and lifetime management

The ECS design ensures high performance and scalability, making it suitable for complex simulations and large scenes.

---

### 3. Tools Module
The **Tools** module provides utilities to improve development workflow and debugging.

#### Features:
- Debug rendering (lines, bounding boxes, etc.)
- Asset loading (models, textures)
- Editor utilities (optional)
- Logging system
- Profiling tools

This module is designed to assist developers during production without impacting runtime performance.

---

### 4. Gameplay Module
The **Gameplay** module handles game-specific logic and behavior.

#### Features:
- C++ scripting system:
  - Scripts attached to entities
  - Update lifecycle (Init / Update / Destroy)
- Game logic separation from engine core
- Easy extension for custom gameplay systems

The scripting system allows developers to create gameplay behavior directly in C++ while keeping the engine core clean and reusable.

---

## Build System

The project uses **CMake** for configuration and building.

### Requirements:
- CMake (3.x or higher)
- Visual Studio (with C++ support)
- DirectX 12 compatible GPU
- Windows 10/11

### Build Instructions:

```bash
# Clone the repository
git clone <repository_url>
cd <project_folder>

```

Then build with cmake.
