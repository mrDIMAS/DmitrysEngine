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
typedef enum de_gui_scroll_bar_orientation_t
{
	DE_GUI_SCROLL_BAR_ORIENTATION_VERTICAL,
	DE_GUI_SCROLL_BAR_ORIENTATION_HORIZONTAL
} de_gui_scroll_bar_orientation_t;

typedef void(*de_scroll_bar_value_changed_event_t)(de_gui_node_t*);

/**
* @brief
*/
typedef struct de_gui_scroll_bar_t
{
	de_gui_node_t* border;                       /**< Background of scrollbar */
	de_gui_node_t* grid;                         /**< Lives on border */
	de_gui_node_t* canvas;                       /**< Lives on 2nd grid cell */
	de_gui_node_t* indicator;                    /**< Lives on canvas */
	de_gui_node_t* up_button;                    /**< Lives on 1st grid cell */
	de_gui_node_t* down_button;                  /**< Lives on 3rd grid cell */
	de_gui_scroll_bar_orientation_t orientation; /**< Orieintation: vertical or horizontal */
	float value;                                 /**< Current scroll value */
	float min;                                   /**< Maximum scroll value */
	float max;                                   /**< Minimum scroll value */
	float step;                                  /**< Increment/decrement step for value. Used when user clicks on arrows */
	de_bool_t is_dragging;                       /**< Indicates that indicator is being dragging */
	de_vec2_t offset;                            /**< Offset from left top corner of indicator to mouse position */
	de_scroll_bar_value_changed_event_t value_changed;
} de_gui_scroll_bar_t;

de_gui_node_t* de_gui_scroll_bar_create(de_gui_t* gui);

/**
* @brief
* @param node
* @param dir
*/
void de_gui_scroll_bar_set_direction(de_gui_node_t* node, de_gui_scroll_bar_orientation_t dir);

/**
* @brief
* @param node
* @param min
*/
void de_gui_scroll_bar_set_min_value(de_gui_node_t* node, float min);

/**
* @brief
* @param node
* @param max
*/
void de_gui_scroll_bar_set_max_value(de_gui_node_t* node, float max);

/**
* @brief
* @param node
* @param evt
*/
void de_gui_scroll_bar_set_value_changed(de_gui_node_t* node, de_scroll_bar_value_changed_event_t evt);