# Dmitry's Engine

Experimental C99 cross-platform single-compilation-unit (SCU) 3D game engine with absolute minimum of external dependencies.

Please note, that this engine is at early development stage and some features maybe very brittle. 

## Projects on this engine
* [Shooter](https://github.com/mrDIMAS/Shooter) - 3d shooter game that I writing using this engine.

## Features
* Single compilation unit - no need to build as separate library: just add `de_main.h` and `de_main.c` to your project.
* C99 with full compatibility with C++.
* FBX support - both ASCII and binary
* Scene graph with these nodes:
	* Base (pivot)
	* Mesh
	* Light
	* Camera
* Automatic assets (resources) management 
	* Textures
	* Models
	* Sound buffers
* Modern rendering techniques, renderer based on OpenGL 3.3 Core.
    * Deferred shading
    * Normal mapping
	* Skeletal animation
* Sound
	* 2D + 3D 
	* WAV format support 
	* Streaming for large sounds 
* TTF fonts support (compound characters support is not implemented yet)
* GUI (inspired by WPF) with these widgets
	* Window
	* Text box
	* Text block
	* Border
	* Button
	* Scroll bar
	* Scroll viewer
	* Scroll content presenter
	* Canvas
	* Grid
	* Slide selector	
* TGA image loading
* Position-based physics
* Built-in save/load functionality via object visitor.
* Documentation - almost every function of the engine has description in Doxygen format.
* Easy to hack and modify

## Planned features
* Renderer improvements
	* Materials (probably PBR)
	* Shadows
	* Frustum culling
	* Performance optimizations
	* Levels of details (LODs)
	* Particle systems
* GUI improvements
	* Styles		
	* More widgets
* Async resource loading
* Stability	
* Support more 3D formats (3ds for example)
* Compound character support in fonts.
* Path finding
* Terrain
* Editor

## Dependencies
- miniz_tinfl - to decompress FBX compressed data.

## Code statistics
- ~18000 lines of code
- Pure C99

## Supported compilers
### Compiling as C
If compiling as C, you will need C99-compatible compiler:
- Windows:  Visual Studio 2013 or higher, mingw 4.8.1 or higher
- Linux: GCC 4.5 or higher

If you using lower version of Visual Studio, then you should compile engine as C++, because lower versions supports only C89.
### Compiling as C++
If compling as C++, you will need C++98 or higher compiler.

## How to build?
### Windows

Required packages: None.

Options for linker: `opengl32; dsound; gdi32; dxguid`

### Linux
Required packages: `libx11-dev, mesa-common-dev, libgl1-mesa-dev, libxrandr-dev.`

Options for linker: `-lGL -lpthread -lasound -lX11 -lXrandr`

## Why C?
* Very fast compilation compared to C++
* Very simple - what you have is just plain memory and procedures that operates on it.
* Strict ABI
* Why not one of those other fancy languages? Let Benjamin Franklin says for me: "He who sacrifices freedom for security deserves neither."

And of course:

![Why](pics/why.png?raw=true "Why")

## Why engine called in that way?
At very beginning engine was called Dark Engine, but then I found that there is already one engine with that name (Thief game series using it). Then I started to looking at some fancy adjectives that could fit into `de` prefix that I was already using for my functions, and I haven't found anything suitable. And then I said - "fuck it, let it be like Dmitry's Engine, I'm too tired of fancy names".
