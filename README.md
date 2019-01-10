# Dmitry's Engine

Experimental C89 cross-platform header-only 3D game engine with absolute minimum of external dependencies.

![Why](pics/why.png?raw=true "Why")

Please note, that this engine is at very early development stage and some features maybe very brittle. In the `./test` folder you can find the game that I writing using this engine.

## Features
- Header only - no need to build as separate library, just `#include "de_main.h"` so extremely fast compilation.
- C89 - full compatibility with C++.
- FBX support - both ASCII and binary
- Modern rendering techniques, renderer based on OpenGL 3.3 Core.
- TTF fonts support (compound characters support is not implemented yet)
- GUI (inspired by WPF)
- TGA image loading
- Position-based physics
- Skeletal animation
- Documentation - almost every function of the engine has description in Doxygen format.

## Planned features (by priorities)
- Fix compatibility with C++ (now it not compiles)
- Materials (probably PBR)
- More GUI widgets (there are still no text input nodes)
- Stability
- Sound 
- Particle systems
- Support more 3D formats (3ds for example)
- Compound character support in fonts.

## Dependencies (header only)
- miniz_tinfl - to decompress FBX compressed data.

## Code statistics
- ~16000 lines of code
- Pure C89
- Header-only

## Why engine called in that way?
At very beginning engine was called Dark Engine, but then I found that there is already one engine with that name (Thief game series using it). Then I started to looking at some fancy adjectives that could fit into `de` prefix that I was already using for my functions, and I haven't found anything suitable. And then I said - "fuck it, let it be like Dmitry's Engine, I'm too tired of fancy names".
