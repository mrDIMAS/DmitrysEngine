# Dmitry's Engine

Experimental C89 cross-platform header-only 3D game engine with absolute minimum of external dependencies.

Please note, that this engine is at very early development stage and some features maybe very brittle. In the `./test` folder you can find the game that I writing using this engine.

## Features
- Header only - no need to build as separate project, just `#include "de_main.h"` so extremely fast compilation.
- FBX support (for now without skinning)
- Modern rendering techniques (still lacking, currently contains only deferred shading)
- TTF fonts support (compound characters support is not implemented yet)
- GUI (inspired by WPF)
- TGA image loading
- Position-based physics

## Planned features (by priorities)
- Fix compatibility with C++ (now it not compiles)
- More complete FBX support (skinning, etc.)
- More GUI widgets (there are still no text input nodes)
- Stability
- Sound 
- Support more 3D formats (3ds for example)
