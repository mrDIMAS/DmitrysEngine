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


/*=======================================================================================*/
de_core_t* de_core_init(const de_engine_params_t* params)
{
	de_core_t* core;

	core = DE_NEW(de_core_t);

	de_log_open("dengine.log");
	de_log("Dmitry's Engine - Logging Started");

	core->params = *params;
	core->running = true;

	/* Call platform-specific initialization routine */
	de_engine_platform_init(core);

	DE_ARRAY_INIT(core->scenes);

	core->renderer = de_renderer_init(core);

	core->gui = de_gui_init(core);

	return core;
}

/*=======================================================================================*/
void de_core_shutdown(de_core_t* core)
{
	de_gui_shutdown(core->gui);
	de_renderer_free(core->renderer);
	de_engine_platform_shutdown(core);
	de_free(core);
	de_log("Engine shutdown successful!");
	de_log_close();
}

/*=======================================================================================*/
static void de_core_poll_messages(de_core_t* core)
{
	de_engine_platform_message_queue(core);
}

/*=======================================================================================*/
bool de_core_is_running(de_core_t* core)
{
	return core->running;
}

/*=======================================================================================*/
void de_core_stop(de_core_t* core)
{
	core->running = false;
}

/*=======================================================================================*/
int de_is_key_pressed(de_core_t* core, enum de_key key)
{
	return core->keys[key];
}

/*=======================================================================================*/
int de_is_key_released(de_core_t* core, enum de_key key)
{
	return !core->keys[key];
}

/*=======================================================================================*/
void de_get_mouse_velocity(de_core_t* core, de_vec2_t* vel)
{
	*vel = core->mouse_vel;
}

/*=======================================================================================*/
int de_is_mouse_pressed(de_core_t* core, enum de_mouse_button btn)
{
	return core->mouse_buttons[btn];
}

/*=======================================================================================*/
void de_get_mouse_pos(de_core_t* core, de_vec2_t* pos)
{
	pos->x = core->mouse_pos.x;
	pos->y = core->mouse_pos.y;
}

/*=======================================================================================*/
void de_core_update(de_core_t* core, float dt)
{
	de_scene_t* scene;

	de_core_poll_messages(core);

	de_gui_update(core->gui);

	de_physics_step(core, dt);

	DE_LINKED_LIST_FOR_EACH(core->scenes, scene)
	{
		de_scene_update(scene, dt);
	}
}

/*=======================================================================================*/
unsigned int de_core_get_window_width(de_core_t* core)
{
	return core->params.width;
}

/*=======================================================================================*/
unsigned int de_core_get_window_height(de_core_t* core)
{
	return core->params.height;
}

/*=======================================================================================*/
de_renderer_t* de_core_get_renderer(de_core_t* core)
{
	return core->renderer;
}

/*=======================================================================================*/
de_gui_t* de_core_get_gui(de_core_t* core)
{
	return core->gui;
}