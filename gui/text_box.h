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

typedef void(*de_gui_text_changed_event_t)(de_gui_node_t* node);

/**
* @brief
*/
typedef struct de_gui_text_box_t {
	bool multiline;
	de_str32_t str;
	de_font_t* font;
	de_gui_vertical_alignment_t ver_alignment;
	de_gui_horizontal_alignment_t hor_alignment;
	DE_ARRAY_DECLARE(de_gui_text_line_t, lines);
	float total_lines_height;
	int caret_offset; /* relative offset from begin of line */
	int caret_line; /* index of line under caret */
	bool show_caret;
	size_t blink_interval; /* in frames (with fixed update at 60 fps, 60 fps = 1s)*/
	size_t blink_timer;
	size_t caret_visible;
	de_gui_text_changed_event_t text_changed;
} de_gui_text_box_t;

struct de_gui_node_dispatch_table_t* de_gui_text_box_get_dispatch_table(void);
