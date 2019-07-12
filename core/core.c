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
	de_core_config_t params;
	bool is_running;
	bool being_visited; /**< Indicates that core correctly prepared for visit. */
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

static void de_core_clean(de_core_t* core)
{
	/* destroy every scene with all their contents */
	while (core->scenes.head) {
		de_scene_free(core->scenes.head);
	}
	de_sound_context_clear(core->sound_context);
	for (size_t i = 0; i < core->resources.size; ++i) {
		de_resource_t* res = core->resources.data[i];
		if (!(res->flags & DE_RESOURCE_FLAG_PERSISTENT) && !(res->flags & DE_RESOURCE_FLAG_INTERNAL)) {
			/* If this error triggered then you have unreleased resource in your *game* code.
			 * If resource intended to be persistent (like UI textures), then you forgot to
			 * set such flag. In other cases most likely that you forgot to call some
			 * yourgameentity_free method. This is required check because if resource does
			 * not released properly then you most likely will have memory leaks or event
			 * worse - memory corruption. Recheck your code and make sure that create/free \
			 * calls match. */
			de_fatal_error("core cleanup: expecting resource '%s' to have 'persistent' flag set, but it doesn't!",
				de_path_cstr(&res->source));
		}
	}
}

void de_core_begin_visit(de_core_t* core)
{
	DE_ASSERT(core->being_visited == false);
	de_sound_context_lock(core->sound_context);
	core->being_visited = true;
}

void de_core_end_visit(de_core_t* core)
{
	DE_ASSERT(core->being_visited == true);
	core->being_visited = false;
	de_sound_context_unlock(core->sound_context);
}

bool de_core_visit(de_object_visitor_t* visitor, de_core_t* core)
{
	bool result = true;
	DE_ASSERT(core->being_visited);
	/* make sure to deserialize everything on "clean" core */
	if (visitor->is_reading) {
		de_core_clean(core);
	}
	DE_ARRAY_DECLARE(de_resource_t*, visited_resources);
	DE_ARRAY_INIT(visited_resources);
	if (!visitor->is_reading) {
		/* copy only serializable resources to temp collection */
		for (size_t i = 0; i < core->resources.size; ++i) {
			de_resource_t* res = core->resources.data[i];
			if (!(res->flags & DE_RESOURCE_FLAG_PERSISTENT) && !(res->flags & DE_RESOURCE_FLAG_INTERNAL)) {
				DE_ARRAY_APPEND(visited_resources, res);
			}
		}
	}
	result &= DE_OBJECT_VISITOR_VISIT_POINTER_ARRAY(visitor, "Resources", visited_resources, de_resource_visit);
	/* append serialized resources back to core collection of resources */
	if (visitor->is_reading) {
		for (size_t i = 0; i < visited_resources.size; ++i) {
			de_resource_t* res = visited_resources.data[i];
			DE_ARRAY_APPEND(core->resources, res);
		}
	}
	if (visitor->is_reading) {
		/* now we can load resources. this step is deferred from actual resource serialization
		 * because internally resource can request other resources (models for example can request
		 * textures assigned to materials) and to properly proccess a request we at first deserialize
		 * resource structures so they can be requested and only then we doing actual loading */
		for (size_t i = 0; i < visited_resources.size; ++i) {
			de_resource_t* res = visited_resources.data[i];
			de_resource_load(res);
		}
	}
	DE_ARRAY_FREE(visited_resources);
	result &= de_sound_context_visit(visitor, core->sound_context);
	result &= DE_OBJECT_VISITOR_VISIT_INTRUSIVE_LINKED_LIST(visitor, "Scenes", core->scenes, de_scene_t, de_scene_visit);
	return result;
}

de_core_t* de_core_init(const de_core_config_t* params)
{
	de_core_t* core = DE_NEW(de_core_t);
	de_log("Dmitry's Engine - Logging Started");
	core->params = *params;
	core->is_running = true;
	DE_LINKED_LIST_INIT(core->scenes);

	double last_time = de_time_get_seconds();
	de_core_platform_init(core);
	de_log("platform initialized in %f seconds", de_time_get_seconds() - last_time);

	last_time = de_time_get_seconds();
	core->sound_context = de_sound_context_create(core);
	de_log("sound context initialized in %f seconds", de_time_get_seconds() - last_time);

	last_time = de_time_get_seconds();
	core->renderer = de_renderer_init(core);
	de_log("renderer initialized in %f seconds", de_time_get_seconds() - last_time);

	last_time = de_time_get_seconds();
	core->gui = de_gui_init(core);
	de_log("gui initialized in %f seconds", de_time_get_seconds() - last_time);

	return core;
}

void de_core_shutdown(de_core_t* core)
{
	while (core->scenes.head) {
		de_scene_free(core->scenes.head);
	}
	de_sound_context_free(core->sound_context);
	de_gui_shutdown(core->gui);
	de_renderer_free(core->renderer);
	/* Notify about unreleased resources */
	for (size_t i = 0; i < core->resources.size; ++i) {
		de_resource_t* res = core->resources.data[i];
		de_log("unrealeased resource found -> mem leaks. details:\n\tpath: %s\n\tref count: %d",
			de_path_cstr(&res->source), res->ref_count);
	}
	de_core_platform_shutdown(core);
	DE_ARRAY_FREE(core->resources);
	DE_ARRAY_FREE(core->events_queue);
	de_free(core);
	de_log("Engine shutdown successful!");
	de_log_close();
}

bool de_core_is_running(de_core_t* core)
{
	return core->is_running;
}

void de_core_stop(de_core_t* core)
{
	core->is_running = false;
}

de_video_mode_t de_core_get_current_video_mode(de_core_t* core)
{
	return core->params.video_mode;
}

void* de_core_get_user_pointer(de_core_t* core)
{
	return core->user_pointer;
}

void de_core_set_user_pointer(de_core_t* core, void* ptr)
{
	core->user_pointer = ptr;
}

de_renderer_t* de_core_get_renderer(de_core_t* core)
{
	return core->renderer;
}

de_gui_t* de_core_get_gui(de_core_t* core)
{
	return core->gui;
}

void de_core_set_video_mode(de_core_t* core, const de_video_mode_t* vm)
{
	de_core_platform_set_video_mode(core, vm);

	de_renderer_notify_video_mode_changed(core->renderer, vm);

	de_log("core: video mode has changed to %dx%d@%d", vm->width, vm->height, vm->bits_per_pixel);
}

de_sound_context_t* de_core_get_sound_context(de_core_t* core)
{
	return core->sound_context;
}

void de_core_push_event(de_core_t* core, const de_event_t* evt)
{
	DE_ARRAY_APPEND(core->events_queue, *evt);
}

bool de_core_poll_event(de_core_t* core, de_event_t* evt)
{
	de_core_platform_poll_events(core);
	if (core->events_queue.size) {
		*evt = core->events_queue.data[0];
		DE_ARRAY_REMOVE_AT(core->events_queue, 0);
		return true;
	}
	return false;
}

de_scene_t* de_core_get_first_scene(de_core_t* core)
{
	return core->scenes.head;
}

void de_core_add_resource(de_core_t* core, de_resource_t* resource)
{
	for (size_t i = 0; i < core->resources.size; ++i) {
		if (core->resources.data[i] == resource) {
			de_log("trying to add already registered resource!");
			return;
		}
	}
	DE_ARRAY_APPEND(core->resources, resource);
}

de_resource_t* de_core_find_resource_of_type(de_core_t* core, de_resource_type_t type, const de_path_t* path)
{
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
	return NULL;
}

de_resource_t* de_core_request_resource(de_core_t* core, de_resource_type_t type, const de_path_t* path)
{
	de_resource_t* res = de_core_find_resource_of_type(core, type, path);
	if (res) {
		return res;
	}
	de_str8_view_t ext;
	de_path_extension(path, &ext);
	switch (type) {
		case DE_RESOURCE_TYPE_TEXTURE:
			if (de_str8_view_eq_cstr(&ext, ".tga")) {
				res = de_resource_create(core, path, type);
			}
			break;
		case DE_RESOURCE_TYPE_SOUND_BUFFER:
			if (de_str8_view_eq_cstr(&ext, ".wav")) {
				res = de_resource_create(core, path, type);
			}
			break;
		case DE_RESOURCE_TYPE_MODEL:
			if (de_str8_view_eq_cstr(&ext, ".fbx")) {
				res = de_resource_create(core, path, type);
			}
			break;
		default:
			de_log("unsupported resource type");
			return NULL;
	}

	if (res && !de_resource_load(res)) {
		/* Hackery hack: adding ref and then immediately release to prevent
		 * triggering of assert. This required because when resource failed to load it
		 * still has ref count == 0 and de_resource_release does not allow to release
		 * unreferenced resources. */
		de_resource_add_ref(res);
		de_resource_release(res);
		DE_ARRAY_REMOVE(core->resources, res);
		return NULL;
	}

	return res;
}

de_resource_t* de_core_request_resource_with_flags(de_core_t* core, de_resource_type_t type, const de_path_t* path, de_resource_flags_t flags)
{
	de_resource_t* res = de_core_request_resource(core, type, path);
	if (res) {
		de_resource_set_flags(res, flags);
	}
	return res;
}

/* Include platform-specific implementation */
#  ifdef _WIN32
#    include "platform/win32.c"
#  else
#    include "platform/x11.c"
#  endif