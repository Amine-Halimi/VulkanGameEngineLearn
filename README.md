# VulkanGameEngine

## Introduction

This is a game engine made using Vulkan following the guide from Brendan Galea (found at https://www.youtube.com/@BrendanGalea)
This is is a personal project to learn how to use Vulkan and 3D graphics. This will eventually implement HLSL instead of GLSL as the shader language.

## Build

### Use project

This project uses Visual Studio to build. Make sure you have a version of Visual Studio

The main entry point of this project is Main.cpp

Make sure you run the project using the Run button set in the Local Windows Debugger on Visual Studio.

### Compile shaders
You need to have the Vulkan SDK to compile the shaders in glsl. In the bin folder, you will find the glslc.exe, which is used to create spv files for the engine to use. 

A. Have a Vulkan SDK on your computer.
B. Navigate to the installion folder of the VulkanSDK binaries file (for example C:\VulkanSDK\1.3.296.0\Bin\ and find the file "glslc.exe".
C. Copy this file's absolute path.
D. Open the CompileShader.bat file. Paste the path of glslc.exe into the variable "glslcLocation".
E. Run the CompileShader.bat file.

### Running the program
In the US Keyboard layout
Camera Position:
- Forward: W
- Backward: S
- Left: A
- Right: D
- Up: E
- Down: Q

Camera View:
- Hold Left-Mouse Button and move the mouse.
### Thanks
- Great thanks to Brendan Galea for making this tutorial series and providing code and models: https://www.youtube.com/@BrendanGalea
- Thanks to Berk Gedik for providing the Backpack models and texture: https://sketchfab.com/3d-models/survival-guitar-backpack-799f8c4511f84fab8c3f12887f7e6b36
