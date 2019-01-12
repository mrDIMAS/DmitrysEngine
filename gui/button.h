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
* @brief
*/
typedef struct de_gui_button_t
{
	de_color_t normal_color;
	de_color_t pressed_color;
	de_color_t hover_color;
	struct de_gui_node_t* border;
	struct de_gui_node_t* text;
	de_gui_callback_t click;	
	bool was_pressed;
} de_gui_button_t;

de_gui_node_t* de_gui_button_create(de_gui_t* gui);

/**
* @brief
* @param node
*/
void de_gui_button_set_click(de_gui_node_t* node, de_gui_callback_func_t click, void* user_data);

/**
* @brief
* @param node
* @return
*/
de_gui_node_t* de_gui_button_get_text(de_gui_node_t* node);

/**
* @brief
* @param node
* @return
*/
de_gui_node_t* de_gui_button_get_border(de_gui_node_t* node);
