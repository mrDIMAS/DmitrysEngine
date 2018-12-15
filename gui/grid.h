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

/**
* @brief
*/
typedef enum de_gui_size_mode_t
{
	/* size of row/column is strictly defined by desired size */
	DE_GUI_SIZE_MODE_STRICT,
	/* grid row/column will take size of most large element */
	DE_GUI_SIZE_MODE_AUTO,
	/* grid row/column size calculated automatically without respect of elements
	* children nodes will be automatically sized to size of row/column */
	DE_GUI_SIZE_MODE_STRETCH
} de_gui_size_mode_t;

/**
* @brief
*/
typedef struct de_gui_grid_column_t
{
	de_gui_size_mode_t size_mode;
	float desired_width;
	float actual_width;
	float x;
} de_gui_grid_column_t;

/**
* @brief
*/
typedef struct de_gui_grid_row_t
{
	de_gui_size_mode_t size_mode;
	float desired_height;
	float actual_height;
	float y;
} de_gui_grid_row_t;

/**
* @brief
*/
typedef struct de_gui_grid_t
{
	DE_ARRAY_DECLARE(de_gui_grid_column_t, columns);
	DE_ARRAY_DECLARE(de_gui_grid_row_t, rows);
	de_bool_t draw_borders;
} de_gui_grid_t;

de_gui_node_t* de_gui_grid_create(de_gui_t* gui);

/**
* @brief
* @param node
* @param desired_height
* @param size_mode
*/
void de_gui_grid_add_row(de_gui_node_t* node, float desired_height, de_gui_size_mode_t size_mode);

/**
* @brief
* @param node
* @param desired_width
* @param size_mode
*/
void de_gui_grid_add_column(de_gui_node_t* node, float desired_width, de_gui_size_mode_t size_mode);

/**
* @brief
* @param grid
* @param prev_child
* @param column
* @return
*/
size_t de_gui_grid_find_child_on_column(de_gui_node_t* grid, size_t prev_child, size_t column);

/**
* @brief
* @param grid
* @param prev_child
* @param row
* @return
*/
size_t de_gui_grid_find_child_on_row(de_gui_node_t* grid, size_t prev_child, size_t row);

/**
* @brief
* @param grid
* @param state
*/
void de_gui_grid_enable_draw_borders(de_gui_node_t* grid, de_bool_t state);

/**
* @brief
* @param grid
*/
void de_gui_grid_clear_columns(de_gui_node_t* grid);

/**
* @brief
* @param grid
*/
void de_gui_grid_clear_rows(de_gui_node_t* grid);