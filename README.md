# Avalanche

This is an ongoing personal game engine project still in early stage of development. The purpose of this project is to learn the Vulkan API and test out computer graphics knowledges.

***

## Getting Started
Visual Studio 2019 and later is recommended, Windows is currently the only supported platform.

<ins>**1. Downloading the repository:**</ins>

Start by cloning the repository with `git clone --recursive https://github.com/Kanagawa-Wave/Avalanche`.

If the repository was cloned non-recursively previously, use `git submodule update --init` to clone the necessary submodules.

<ins>**2. Build the project:**</ins>

1. Make sure you've download and installed the latest Vulkan SDK from [LunarG](https://www.lunarg.com/vulkan-sdk/).
2. Run the [GenerateProjectFiles.bat](https://github.com/Kanagawa-Wave/Avalanche/blob/main/GenerateProjectFiles.bat) script file to generate a visual studio solution (defaults to vs2022, you need to edit the command if other versions of Visual Studio is used)
3. Build the project in Visual Studio

***

## Feature List
### Implemented
- Model loading
- Editor interface based on ImGui
- Blinn-Phong shading
- Entity-Component System based on entt
- Abstraction layer for Vulkan API
### Short-term goals (Before Fall 2024)
- Shadow Maps
- PBR support
- Cubemaps
- Post-processing
- Assimp model loading
- Deferred Rendering
### Long-term goals
- Reflections (SSR, Planar)
- Ambient Occulsion (SSAO, GTAO)
- Mesh shading pipeline
- Vulkan Ray tracing
