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
typedef struct de_gui_line_point_t {
	de_vec2_t pos;
	de_color_t color;
} de_gui_line_point_t;

/**
* @brief
*/
typedef struct de_gui_polyline_t {
	DE_ARRAY_DECLARE(de_gui_line_point_t, points);
	float thickness;
} de_gui_polyline_t;

/**
* @brief
*/
typedef enum de_gui_draw_command_type_t {
	DE_GUI_DRAW_COMMAND_TYPE_GEOMETRY, /**< Command describes geometry of UI element */
	DE_GUI_DRAW_COMMAND_TYPE_CLIP      /**< Command describes clipping geometry */
} de_gui_draw_command_type_t;

/**
* @brief
*/
typedef struct de_gui_draw_command_t {
	de_gui_draw_command_type_t type;
	de_texture_t* texture;
	size_t index_offset;
	size_t triangle_count;
	uint8_t nesting; /**< Nesting level of draw command. This value used for clipping by stencil buffer. Root value is 1 */
} de_gui_draw_command_t;

/**
* @brief
*/
typedef struct de_gui_vertex_t {
	de_vec2_t pos;
	de_vec2_t tex_coord;
	de_color_t color;
} de_gui_vertex_t;

typedef DE_ARRAY_DECLARE(de_gui_vertex_t, de_gui_vertex_buffer_t);
typedef DE_ARRAY_DECLARE(int, de_gui_index_buffer_t);
typedef DE_ARRAY_DECLARE(de_gui_draw_command_t, de_gui_command_buffer_t);

/**
* @brief
*/
struct de_gui_draw_list_t {
	de_gui_vertex_buffer_t vertex_buffer;
	de_gui_index_buffer_t index_buffer;
	de_gui_command_buffer_t commands;
	DE_ARRAY_DECLARE(int, clip_cmd_stack);
	DE_ARRAY_DECLARE(float, opacity_stack);
	/* state machine fields - these fields are persistent between de_gui_draw_list_commit calls */
	size_t triangles_to_commit;
	uint8_t current_nesting;
};

/**
* @brief Adds new clipping geometry to command buffer. Stores its index in stack of clipping geometries.
* Pop commands count should be "pop_cmd_count = push_cmd_count - 1"
* Draw => Clip1 => Draw => Clip2 => Draw => Draw => Clip1
*		    ^                 ^		                  ^
*		   Push              Push                     Pop
*/
void de_gui_draw_list_commit_clip_geom(de_gui_draw_list_t* draw_list, de_gui_node_t* n);

/**
* @brief Pops out previous clipping command from stack. Commits clipping geometry from stack top to draw list.
* @param draw_list
*/
void de_gui_draw_list_revert_clip_geom(de_gui_draw_list_t* draw_list);

/**
* @brief
* @param draw_list
* @param type
* @param tex
* @param n
*/
void de_gui_draw_list_commit(de_gui_draw_list_t* draw_list, de_gui_draw_command_type_t type, de_texture_t* tex, de_gui_node_t* n);

/**
* @brief
* @param draw_list
* @param pos
* @param size
* @param clr
* @param tex_coords
*/
void de_gui_draw_list_push_rect_filled(de_gui_draw_list_t* draw_list, const de_vec2_t* pos, const de_vec2_t* size, const de_color_t* clr, const de_vec2_t* tex_coords);

/**
* @brief
* @param draw_list
* @param pos
* @param size
* @param thickness
* @param clr
*/
void de_gui_draw_list_push_rect(de_gui_draw_list_t* draw_list, const de_vec2_t* pos, const de_vec2_t* size, float thickness, const de_color_t* clr);

void de_gui_draw_list_push_rect_vary(de_gui_draw_list_t* draw_list, const de_vec2_t* pos, const de_vec2_t* size, const de_gui_thickness_t* thickness, const de_color_t* clr);

/**
* @brief
* @param draw_list
* @param a
* @param b
* @param thickness
* @param clr
*/
void de_gui_draw_list_push_line(de_gui_draw_list_t* draw_list, const de_vec2_t* a, const de_vec2_t* b, float thickness, const de_color_t* clr);

/**
* @brief
* @param draw_list
* @return
*/
int de_gui_draw_list_get_next_index_origin(de_gui_draw_list_t* draw_list);

/**
* @brief
* @param draw_list
* @param n
* @return
*/
de_gui_vertex_t* de_gui_draw_list_reserve_vertices(de_gui_draw_list_t* draw_list, size_t n);

/**
* @brief
* @param draw_list
* @param a
* @param b
* @param c
*/
void de_gui_draw_list_ib_push_triangle(de_gui_draw_list_t* draw_list, int a, int b, int c);

/**
* @brief
* @param draw_list
* @param font
* @param utf32_text
* @param text_len
* @param pos
* @param clr
*/
void de_gui_draw_list_push_text(de_gui_draw_list_t* draw_list, const de_font_t* font, const uint32_t* utf32_text, size_t text_len, const de_vec2_t* pos, const de_color_t* clr);

/**
* @brief
* @param draw_list
* @param x
* @param y
* @param w
* @param h
* @param n
*/
void de_gui_draw_list_commit_clip_rect(de_gui_draw_list_t* draw_list, float x, float y, float w, float h, de_gui_node_t* n);

/**
* @brief
* @param draw_list
* @param nesting
*/
void de_gui_draw_list_set_nesting(de_gui_draw_list_t* draw_list, uint8_t nesting);

/**
* @brief
* @param polyline
*/
void de_gui_polyline_init(de_gui_polyline_t* polyline);

/**
* @brief
* @param polyline
* @param x
* @param y
* @param clr
*/
void de_gui_polyline_add_point(de_gui_polyline_t* polyline, float x, float y, const de_color_t* clr);

/**
* @brief
* @param polyline
* @param draw_list
*/
void de_gui_polyline_draw(de_gui_polyline_t* polyline, de_gui_draw_list_t* draw_list);

/**
* @brief
* @param polyline
*/
void de_gui_polyline_free(de_gui_polyline_t* polyline);

void de_gui_draw_list_clear(de_gui_draw_list_t* draw_list);