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

typedef DE_ARRAY_DECLARE(de_video_mode_t, de_video_mode_array_t);

typedef enum de_core_flags_t {
	DE_CORE_FLAGS_BORDERLESS = DE_BIT(0)
} de_core_flags_t;

typedef struct de_engine_params_t {
	de_video_mode_t video_mode;
	const char* data_path;
	uint32_t flags;
} de_engine_params_t;

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

void* de_core_get_user_pointer(de_core_t* core);

void de_core_set_user_pointer(de_core_t* core, void* ptr);

/**
 * @brief Returns current gui subsystem of the core.
 */
de_gui_t* de_core_get_gui(de_core_t* core);

/**
 * @brief Returns current sound subsystem of the core.
 */
de_sound_context_t* de_core_get_sound_context(de_core_t* core);

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

de_scene_t* de_core_get_first_scene(de_core_t* core);

/**
 * @brief Registers new resource. Useful to register dynamic resource. Every registered
 * resource will be automatically serialized.
 */
void de_core_add_resource(de_core_t* core, de_resource_t* resource);

/**
 * @brief Request external resource of specified type. If resource was loaded previously,
 * increases reference count and returns pointer to resource, if not - loads resource.
 */
de_resource_t* de_core_request_resource(de_core_t* core, de_resource_type_t type, const de_path_t* path, uint32_t flags);

de_resource_t* de_core_find_resource_of_type(de_core_t* core, de_resource_type_t type, const de_path_t* path);

/**
 * @brief Visit core with specified object visitor.
 *
 * Important notes: When visitor is set for read, then call of this function will
 * destroy every scene, every scene node, every resource (unless it marked
 * DE_RESOURCE_FLAG_PERSISTENT) and then will load everything into clean core. In
 * other words in this case core will be purged and then filled in with data from
 * specified file.
 */
bool de_core_visit(de_object_visitor_t* visitor, de_core_t* core);

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
 * @brief Enumerates available video modes. Result array must be freed.
 */
de_video_mode_array_t de_enum_video_modes();