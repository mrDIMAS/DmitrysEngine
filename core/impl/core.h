/* Copyright (c) 2017-2018 Dmitry Stepanov a.k.a mr.DIMAS
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

de_engine_t* de_engine;

void de_engine_init_renderer();

/*=======================================================================================*/
int de_init(const de_engine_params_t* params)
{
	de_engine = DE_NEW(de_engine_t);

	de_log_open("dengine.log");
	de_log("Dmitry's Engine - Logging Started");

	de_engine->params = *params;
	de_engine->running = DE_TRUE;

	/* Call platform-specific initialization routine */
	de_engine_platform_init();

	DE_ARRAY_INIT(de_engine->scenes);

	de_engine_init_renderer();

	de_gui_init(&de_engine->gui);

	return DE_TRUE;
}

/*=======================================================================================*/
void de_shutdown()
{
	de_gui_shutdown(&de_engine->gui);
	de_engine_platform_shutdown();
	de_free(de_engine);
	de_log("Engine shutdown successful!");
	de_log_close();
}

/*=======================================================================================*/
void de_poll_messages()
{
	de_engine_platform_message_queue();
}

/*=======================================================================================*/
de_bool_t de_is_running()
{
	return de_engine->running;
}

/*=======================================================================================*/
void de_stop()
{
	de_engine->running = DE_FALSE;
}

/*=======================================================================================*/
int de_is_key_pressed(enum de_key key)
{
	return de_engine->keys[key];
}

/*=======================================================================================*/
int de_is_key_released(enum de_key key)
{
	return !de_engine->keys[key];
}

/*=======================================================================================*/
void de_get_mouse_velocity(de_vec2_t* vel)
{
	*vel = de_engine->mouse_vel;
}

/*=======================================================================================*/
void de_set_framerate_limit(int limit)
{
	de_engine->renderer.frame_rate_limit = limit;
}

int de_is_mouse_pressed(enum de_mouse_button btn)
{
	return de_engine->mouse_buttons[btn];
}

void de_get_mouse_pos(de_vec2_t* pos)
{
	pos->x = de_engine->mouse_pos.x;
	pos->y = de_engine->mouse_pos.y;
}