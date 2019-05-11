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