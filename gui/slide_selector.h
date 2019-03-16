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


typedef void(*de_gui_item_text_getter)(void* items, int n, char* out_buffer, int out_buffer_size);
typedef void(*de_gui_selection_changed)(de_gui_node_t* node, int new_item);

typedef struct de_gui_slide_selector_t {
	de_gui_node_t* current_item;
	int item_count;
	int selection_index;
	void* items;
	void* selection;
	de_gui_item_text_getter get_item_text;
	de_gui_selection_changed selection_changed;
} de_gui_slide_selector_t;

struct de_gui_dispatch_table_t* de_gui_slide_selector_get_dispatch_table(void);

void de_gui_slide_selector_set_items(de_gui_node_t* node, void* items, int item_count, de_gui_item_text_getter getter);

void* de_gui_slide_selector_get_selection(de_gui_node_t* node);

void de_gui_slide_selector_set_selection_changed(de_gui_node_t* node, de_gui_selection_changed callback);