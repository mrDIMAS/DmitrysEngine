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
	DE_ARRAY_FREE(core->events_queue);
	de_sound_context_free(core->sound_context);
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

/* Include platform-specific implementation */
#  ifdef _WIN32
#    include "platform/impl/win32.h"
#  else
#    include "platform/impl/x11.h"
#  endif