# Dmitry's Engine

Experimental C89 cross-platform header-only 3D game engine with absolute minimum of external dependencies.

![Why](pics/why.png?raw=true "Why")

Please note, that this engine is at very early development stage and some features maybe very brittle. In the `./test` folder you can find the game that I writing using this engine.

## Features
- Header only - no need to build as separate library, just `#include "de_main.h"` so extremely fast compilation.
- C89 - full compatibility with C++.
- FBX support - both ASCII and binary
- Modern rendering techniques (still lacking, currently contains only deferred shading)
- TTF fonts support (compound characters support is not implemented yet)
- GUI (inspired by WPF)
- TGA image loading
- Position-based physics
- Skeletal animation

## Planned features (by priorities)
- Fix compatibility with C++ (now it not compiles)
- Materials (probably PBR)
- More GUI widgets (there are still no text input nodes)
- Stability
- Sound 
- Particle systems
- Support more 3D formats (3ds for example)

## Dependencies (header only)
- miniz_tinfl - to decompress FBX compressed data.

## Code statistics
- ~16000 lines of code
- Pure C89
- Header-only
