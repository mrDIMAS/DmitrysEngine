# 01 - Core initialization.

Core (`de_core_t`) is the main part of the engine, it handles main render window and its event loop, initializes other subsystems (renderer, GUI, Sound context, etc.). It manages all resources (textures, models, fonts, sound buffers, etc.) giving you the unified way to work with resources. Before use the engine, you must initialize the core. Here is very simple way to initialize it.

```c
#include "de_main.h"

int main(int argc, char** argv)
{
	/* Fill config using designated initializer. */
	const de_core_config_t config = {
		.video_mode = {
			.width = 800,
			.height = 600,
		}
	};

	/* Initialize core. */
	de_core_t* core = de_core_init(&config);

	/* Cleanup. */
	de_core_shutdown(core);
}
```

If you run this example you'll see a window for a moment and then it will be closed. To prevent this you should add event handling loop.

```c
#include "de_main.h"

int main(int argc, char** argv)
{
	/* Fill config using designated initializer. */
	const de_core_config_t config = {
		.video_mode = {
			.width = 800,
			.height = 600,
		}
	};

	/* Initialize core. */
	de_core_t* core = de_core_init(&config);

	/* Run main loop. */
	while (de_core_is_running(core)) {
		de_event_t evt;
		if (de_core_poll_event(core, &evt)) {
			switch (evt.type) {
				case DE_EVENT_TYPE_CLOSE:
					/* Stop engine if user closes main window. */
					de_core_stop(core);
					break;
				case DE_EVENT_TYPE_KEY_DOWN:
					/* Esc will stop engine. */
					if (evt.s.key.key == DE_KEY_ESC) {
						de_core_stop(core);
					}
					break;
			}
		}
	}

	/* Cleanup. */
	de_core_shutdown(core);
}
```

Now all you see is a window filled with white color, it can be closed by hitting Esc key or by close button on the window. 