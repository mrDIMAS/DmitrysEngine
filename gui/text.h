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
typedef struct de_gui_text_line_t {
	size_t begin;
	size_t end;
	float width;
} de_gui_text_line_t;


/**
* @brief
*/
typedef struct de_gui_text_t {
	de_string32_t str;
	de_font_t* font;
	de_gui_vertical_alignment_t ver_alignment;
	de_gui_horizontal_alignment_t hor_alignment;
	DE_ARRAY_DECLARE(de_gui_text_line_t, lines);
	float total_lines_height;
	bool auto_size; /**< desired size will be determined automatically */
} de_gui_text_t;

de_gui_node_t* de_gui_text_create(de_gui_t* gui);

/**
* @brief
* @param node
* @param font
*/
void de_gui_text_set_font(de_gui_node_t* node, de_font_t* font);

/**
* @brief
* @param node
* @param utf8str
*/
void de_gui_text_set_text_utf8(de_gui_node_t* node, const char* utf8str);

void de_gui_text_set_text_utf32(de_gui_node_t* node, const uint32_t* utf32str);

/**
* @brief
* @param node
* @param alignment
*/
void de_gui_text_set_vertical_alignment(de_gui_node_t* node, de_gui_vertical_alignment_t alignment);

void de_gui_text_set_horizontal_alignment(de_gui_node_t* node, de_gui_horizontal_alignment_t alignment);