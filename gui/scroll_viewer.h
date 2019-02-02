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
typedef struct de_gui_scroll_viewer_t {
	de_gui_node_t* border;                   /**< Background */
	de_gui_node_t* grid;                     /**< Main layout panel */
	de_gui_node_t* scroll_content_presenter; /**< Panel for content */
	de_gui_node_t* ver_scroll_bar;           /**< Vertical scroll bar */
	de_gui_node_t* hor_scroll_bar;           /**< Horizontal scroll bar */
	de_gui_node_t* content;
	bool auto_hide_ver_scroll_bar;
	bool auto_hide_hor_scroll_bar;
} de_gui_scroll_viewer_t;

de_gui_node_t* de_gui_scroll_viewer_create(de_gui_t* gui);

void de_gui_scroll_viewer_measure(de_gui_node_t* n);

/**
* @brief
* @param sv_node
* @param content
*/
void de_gui_scroll_viewer_set_content(de_gui_node_t* sv_node, de_gui_node_t* content);