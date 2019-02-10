/* Copyright (c) 2017-2019 Dmitry Stepanov a.k.a mr.DIMAS
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
* LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
* OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
* WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

typedef struct de_video_mode_t {
	unsigned int width;
	unsigned int height;
	unsigned int bits_per_pixel;
	bool fullscreen;
} de_video_mode_t;

typedef enum de_core_flags_t {
	DE_CORE_FLAGS_BORDERLESS = DE_BIT(0)
} de_core_flags_t;

typedef struct de_engine_params_t {
	de_video_mode_t video_mode;
	uint32_t flags;
} de_engine_params_t;

struct de_core_t {
	struct {
	#ifdef _WIN32
		HGLRC gl_context;
		HWND window;
		HDC device_context;
	#else
		Display* display;
		Window window;
		GLXContext glContext;
	#endif
	} platform;

	de_engine_params_t params; /**< Initialization parameters */
	bool running; /**< true if engine is running */
	bool keyboard_focus; /**< true if rendering window is in focus */
	DE_LINKED_LIST_DECLARE(de_scene_t, scenes);
	DE_LINKED_LIST_DECLARE(de_font_t, fonts);
	de_renderer_t* renderer;
	de_gui_t* gui;
	size_t alloc_count;
	DE_ARRAY_DECLARE(de_event_t, events_queue);
};

/**
* @brief Creates window and initializes OpenGL
*/
de_core_t* de_core_init(const de_engine_params_t* params);

/**
* @brief Destroys window and OpenGL context
*/
void de_core_shutdown(de_core_t* core);

/**
* @brief Returns true if engine is running
*/
bool de_core_is_running(de_core_t* core);

/**
 * @brief Stops engine.
 */
void de_core_stop(de_core_t* core);

/**
 * @brief Returns current width of render window.
 */
unsigned int de_core_get_window_width(de_core_t* core);

/**
 * @brief Returns current height of render window.
 */
unsigned int de_core_get_window_height(de_core_t* core);

/**
 * @brief Returns current renderer of the core.
 */
de_renderer_t* de_core_get_renderer(de_core_t* core);

/**
 * @brief Returns current gui subsystem of the core.
 */
de_gui_t* de_core_get_gui(de_core_t* core);

/**
 * @brief Pushes new event into event queue. Can be used to inject custom input
 * into message queue.
 */
void de_core_push_event(de_core_t* core, const de_event_t* evt);

/**
 * @brief Poll event from event events queue. Returns true if event was extracted.
 *
 * Typical usage:
 *
 * de_event_t evt;
 * while(de_core_poll_event(core, &evt)) {
 *    ..do stuff
 * }
 */
bool de_core_poll_event(de_core_t* core, de_event_t* evt);


/********************************************************************
* Platform-specific function prototypes.                            *
* Each of below functions must be implemented for each platform.    *
********************************************************************/

/**
 * @brief Initializes platform specific stuff.
 */
void de_core_platform_init(de_core_t* core);

/**
 * @brief Doing platform specific shutdown routine.
 */
void de_core_platform_shutdown(de_core_t* core);

/**
 * @brief Polls platform events.
 */
void de_core_platform_poll_events(de_core_t* core);

/**
 * @brief Retrieves OpenGL extension function pointer.
 */
de_proc de_core_platform_get_proc_address(const char *name);

/**
 * @brief Swaps buffers: back and front.
 */
void de_core_platform_swap_buffers(de_core_t* core);

/**
 * @brief Shows message box.
 */
void de_core_platform_message_box(de_core_t* core, const char * msg, const char* title);

/**
 * @brief Sets title of rendering window.
 */
void de_core_platform_set_window_title(de_core_t* core, const char* title);

/**
 * @brief Sets new video mode.
 */
void de_core_set_video_mode(de_core_t* core, const de_video_mode_t* vm);

/**
 * @brief Gets current desktop video mode.
 */
void de_get_desktop_video_mode(de_video_mode_t* vm);

/**
 * @brief Enumerates available video modes.
 * 
 * Typical usage:
 * 
 * int n = 0;
 * de_video_mode_t vm;
 * while(de_enum_video_modes(&vm, n++)) {
 *   ..do stuff
 * }
 * 
 * Note: Function may return duplicated videomodes! You have to filter them by yourself.
 */
bool de_enum_video_modes(de_video_mode_t* vm, int n);