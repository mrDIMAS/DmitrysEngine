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

struct de_core_t {
	/* All fields are private. Do not access directly! */
	de_renderer_t* renderer;
	de_sound_context_t* sound_context;
	de_gui_t* gui;
	DE_LINKED_LIST_DECLARE(de_scene_t, scenes);
	DE_LINKED_LIST_DECLARE(de_font_t, fonts);
	de_engine_params_t params;
	bool is_running;
	void* user_pointer;
	DE_ARRAY_DECLARE(de_event_t, events_queue);
	DE_ARRAY_DECLARE(de_resource_t*, resources);
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
};

static void de_core_clean(de_core_t* core) {	
	/* destroy every scene with all their contents */
	while (core->scenes.head) {
		de_scene_free(core->scenes.head);
	}
	for (size_t i = 0; i < core->resources.size; ++i) {
		de_resource_t* res = core->resources.data[i];
		if (!(res->flags & DE_RESOURCE_FLAG_PERSISTENT) && !(res->flags & DE_RESOURCE_FLAG_INTERNAL)) {
			/* force destroy a resource, at this moment we do not care if some still owns it
			 * because at this time owner is already dead. */
			while (res->ref_count) {
				de_resource_release(res);
			}
		}
	}
}

bool de_core_visit(de_object_visitor_t* visitor, de_core_t* core) {
	bool result = true;
	de_sound_context_lock(core->sound_context);
	/* make sure to deserialize everything on "clean" core */
	if (visitor->is_reading) {		
		de_core_clean(core);
	}
	result &= DE_OBJECT_VISITOR_VISIT_POINTER_ARRAY(visitor, "Resources", core->resources, de_resource_visit);
	result &= de_sound_context_visit(visitor, core->sound_context);
	result &= DE_OBJECT_VISITOR_VISIT_INTRUSIVE_LINKED_LIST(visitor, "Scenes", core->scenes, de_scene_t, de_scene_visit);
	de_sound_context_unlock(core->sound_context);
	return result;
}

de_core_t* de_core_init(const de_engine_params_t* params) {
	de_core_t* core;
	core = DE_NEW(de_core_t);
	de_log("Dmitry's Engine - Logging Started");
	core->params = *params;
	core->is_running = true;
	de_core_platform_init(core);
	DE_LINKED_LIST_INIT(core->scenes);
	core->sound_context = de_sound_context_create(core);
	core->renderer = de_renderer_init(core);
	core->gui = de_gui_init(core);
	return core;
}

void de_core_shutdown(de_core_t* core) {
	while (core->scenes.head) {
		de_scene_free(core->scenes.head);
	}
	de_sound_context_free(core->sound_context);
	for (size_t i = 0; i < core->resources.size; ++i) {
		de_resource_t* res = core->resources.data[i];
		if (de_resource_release(res) != 0) {
			de_log("unrealeased resource found -> mem leaks");
		}
	}
	DE_ARRAY_FREE(core->events_queue);
	de_gui_shutdown(core->gui);
	de_renderer_free(core->renderer);
	de_core_platform_shutdown(core);
	de_free(core);
	de_log("Engine shutdown successful!");
	de_log_close();
}

bool de_core_is_running(de_core_t* core) {
	return core->is_running;
}

void de_core_stop(de_core_t* core) {
	core->is_running = false;
}

unsigned int de_core_get_window_width(de_core_t* core) {
	return core->params.video_mode.width;
}

unsigned int de_core_get_window_height(de_core_t* core) {
	return core->params.video_mode.height;
}

void* de_core_get_user_pointer(de_core_t* core) {
	return core->user_pointer;
}

void de_core_set_user_pointer(de_core_t* core, void* ptr) {
	core->user_pointer = ptr;
}

de_renderer_t* de_core_get_renderer(de_core_t* core) {
	return core->renderer;
}

de_gui_t* de_core_get_gui(de_core_t* core) {
	return core->gui;
}

de_sound_context_t* de_core_get_sound_context(de_core_t* core) {
	return core->sound_context;
}

void de_core_push_event(de_core_t* core, const de_event_t* evt) {
	DE_ARRAY_APPEND(core->events_queue, *evt);
}

bool de_core_poll_event(de_core_t* core, de_event_t* evt) {
	de_core_platform_poll_events(core);
	if (core->events_queue.size) {
		*evt = core->events_queue.data[0];
		DE_ARRAY_REMOVE_AT(core->events_queue, 0);
		return true;
	}
	return false;
}

de_scene_t* de_core_get_first_scene(de_core_t* core) {
	return core->scenes.head;
}

void de_core_add_resource(de_core_t* core, de_resource_t* resource) {
	for (size_t i = 0; i < core->resources.size; ++i) {
		if (core->resources.data[i] == resource) {
			de_log("trying to add already registered resource!");
			return;
		}
	}
	DE_ARRAY_APPEND(core->resources, resource);
}

de_resource_t* de_core_request_resource(de_core_t* core, de_resource_type_t type, const de_path_t* path) {
	for (size_t i = 0; i < core->resources.size; ++i) {
		de_resource_t* res = core->resources.data[i];
		if (de_path_eq(&res->source, path)) {
			if (res->type != type) {
				de_log("warning: resource %s is found but types mismatch: requested '%s' got '%s'",
					de_path_cstr(path), de_resource_type_to_cstr(type), de_resource_type_to_cstr(res->type));
			}
			return res;
		}
	}
	de_str8_view_t ext;
	de_path_extension(path, &ext);	
	de_resource_t* res = de_resource_create(core, type, 0);	
	de_path_copy(path, &res->source);
	switch (type) {
		case DE_RESOURCE_TYPE_TEXTURE:
			if (de_str8_view_eq_cstr(&ext, ".tga")) {

			}
			break;
		case DE_RESOURCE_TYPE_SOUND_BUFFER:
			if (de_str8_view_eq_cstr(&ext, ".wav")) {
				de_sound_buffer_init(&res->s.sound_buffer, core->sound_context, 0);
				de_sound_buffer_load_file(&res->s.sound_buffer, de_path_cstr(path));
				return res;
			}
			break;
		case DE_RESOURCE_TYPE_MODEL:
			if (de_str8_view_eq_cstr(&ext, ".fbx")) {				
				de_model_load(&res->s.model, path);
				return res;
			}
			break;
		default:
			de_log("unsupported resource type");
			return NULL;
	}
	return NULL;
}

/* Include platform-specific implementation */
#  ifdef _WIN32
#    include "platform/win32.c"
#  else
#    include "platform/x11.c"
#  endif