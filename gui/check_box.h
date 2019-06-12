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

typedef void(*de_check_box_checked_changed_t)(de_gui_node_t* node, bool new_value);

typedef struct de_gui_check_box_descriptor_t {
	bool checked;
	de_check_box_checked_changed_t checked_changed;
} de_gui_check_box_descriptor_t;

typedef struct de_gui_check_box_t {
	de_gui_node_t* border;
	de_gui_node_t* check_mark;
	bool checked;
	de_check_box_checked_changed_t checked_changed;
} de_gui_check_box_t;

struct de_gui_node_dispatch_table_t* de_gui_check_box_get_dispatch_table();

void de_gui_check_box_set_value(de_gui_node_t* node, bool value);

bool de_gui_check_box_get_value(de_gui_node_t* node);