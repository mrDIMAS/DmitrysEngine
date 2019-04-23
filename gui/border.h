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

typedef struct de_gui_border_descriptor_t {
	de_color_t stroke_color;
	de_gui_thickness_t thickness;
} de_gui_border_descriptor_t;

/**
* @brief
*/
typedef struct de_gui_border_t {
	de_color_t stroke_color;
	de_gui_thickness_t thickness;
} de_gui_border_t;

struct de_gui_node_dispatch_table_t* de_gui_border_get_dispatch_table();

/**
* @brief
* @param node
* @param color
*/
void de_gui_border_set_stroke_color(de_gui_node_t* node, const de_color_t* color);

/**
* @brief
* @param node
* @param r
* @param g
* @param b
* @param a
*/
void de_gui_border_set_stroke_color_rgba(de_gui_node_t* node, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

void de_gui_border_set_thickness_uniform(de_gui_node_t* node, float thickness);

void de_gui_border_set_thickness(de_gui_node_t* node, float left, float top, float right, float bottom);