#01 - Core initialization.
Core (`de_core_t`) is the main part of the engine, it handles main render window and its event loop, initializes other subsystems (renderer, GUI, Sound context, etc.). It manages all resources (textures, models, fonts, sound buffers, etc.) giving you the unified way to work with resources. Before use the engine, you must initialize the core. 

```c
int main(int argc, char** argv) {
	
}
```
