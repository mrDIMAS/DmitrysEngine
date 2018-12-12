# Dmitry's Engine

Experimental C89 cross-platform header-only game engine with absolute minimum of external dependencies.

Please note, that this engine is at very early development stage and some features maybe very brittle. In the `./test` folder you can find the game that I writing using this engine.

## Features
- Header only - no need to build as separate project, just `#include "de_main.h"` so extremely fast compilation.
- FBX support (for now without animations)
- Modern rendering techniques (still lacking, currently contains only deferred shading)
- TTF fonts support (compound characters support is not implemented yet)
- GUI (inspired by WPF)
- TGA image loading

## Planned features (by priorities)
- More complete FBX support (key frame animations, skinning, etc.)
- Stability
- Sound 
- Support more 3D formats (3ds for example)
