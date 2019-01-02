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

/**
* @brief Engine creation parameters
*/
typedef struct de_engine_params_s
{
	unsigned int width;  /**< Width of the client area of the window */
	unsigned int height; /**< Height of the client area of the window */
} de_engine_params_t;

struct de_core_t
{
	struct
	{
	#ifdef _WIN32
		HGLRC gl_context;
		HWND window;
		HDC device_context;

		HWND dummy_window;
		HGLRC dummy_context;
		HDC dummy_dc;
	#else
		Display* display;
		Window window;
		GLXContext glContext;
	#endif
	} platform;

	de_engine_params_t params;   /**< Initialization parameters */
	de_bool_t running;              /**< DE_TRUE if engine is running */
	de_bool_t keyboard_focus;             /**< DE_TRUE if rendering window is in focus */
	de_bool_t keys[DE_KEY_Count];   /**< State of keyboard keys: DE_TRUE or DE_FALSE */
	de_bool_t mouse_buttons[5];     /**< State of mouse buttons: DE_TRUE or DE_FALSE */
	int mouse_wheel;             /**< Mouse wheel speed */
	de_vec2_t mouse_pos;         /**< Mouse position in window coordinates */
	de_vec2_t mouse_vel;         /**< Mouse velocity vector */

	DE_LINKED_LIST_DECLARE(de_scene_t, scenes);

	de_renderer_t* renderer;
	de_gui_t* gui;

	size_t alloc_count;

	DE_LINKED_LIST_DECLARE(de_font_t, fonts);
};

/**
* @brief Creates window and initializes OpenGL
*/
de_core_t* de_init(const de_engine_params_t* params);

/**
* @brief Destroys window and OpenGL context
*/
void de_shutdown(de_core_t* core);

/**
* @brief Returns DE_TRUE if engine is running
*/
de_bool_t de_is_running(de_core_t* core);

/**
 * @brief Stops engine.
 */
void de_stop(de_core_t* core);

/**
* @brief Updates message queue from OS. No need to call directly!
*/
void de_poll_messages(de_core_t* core);

/**
 * @brief Performs one tick of update
 */
void de_update(de_core_t* core, float dt);

/**
* Platform-specific function prototypes.
* Each of below functions must be implemented for each platform.
* Below functions is private, and not exported into dynamic library!
*/

typedef void(*de_proc)(void);

void de_engine_platform_init(de_core_t* core);
void de_engine_platform_shutdown(de_core_t* core);
void de_engine_platform_message_queue(de_core_t* core);
de_proc de_engine_platform_get_proc_address(const char *name);
void de_engine_platform_swap_buffers(de_core_t* core);
void de_engine_platform_message_box(const char * msg);

