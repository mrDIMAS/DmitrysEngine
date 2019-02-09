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

/* Inspired by WPF */

/* Set to non-zero to enable visual debugging of GUI */
#define DE_GUI_ENABLE_GUI_DEBUGGING 1

typedef struct de_gui_node_t de_gui_node_t;
typedef struct de_gui_draw_list_t de_gui_draw_list_t;

typedef struct de_gui_thickness_t {
	float left;
	float top;
	float right;
	float bottom;
} de_gui_thickness_t;

/**
* @brief
*/
typedef enum de_gui_vertical_alignment_t {
	DE_GUI_VERTICAL_ALIGNMENT_TOP,    /**< Top alignment */
	DE_GUI_VERTICAL_ALIGNMENT_CENTER, /**< Center alignment */
	DE_GUI_VERTICAL_ALIGNMENT_BOTTOM, /**< Bottom alignment */
	DE_GUI_VERTICAL_ALIGNMENT_STRETCH /**< Top alignment with vertical stretch */
} de_gui_vertical_alignment_t;

/**
* @brief
*/
typedef enum de_gui_horizontal_alignment_t {
	DE_GUI_HORIZONTAL_ALIGNMENT_LEFT,   /**< Left alignment */
	DE_GUI_HORIZONTAL_ALIGNMENT_CENTER, /**< Center alignment */
	DE_GUI_HORIZONTAL_ALIGNMENT_RIGHT,  /**< Right alignment */
	DE_GUI_HORIZONTAL_ALIGNMENT_STRETCH /**< Left alignment with horizontal stretch */
} de_gui_horizontal_alignment_t;

typedef void(*de_gui_callback_func_t)(de_gui_node_t*, void*);

typedef struct de_gui_callback_t {
	void* user_data;
	de_gui_callback_func_t func;
} de_gui_callback_t;

#if DE_DISABLE_ASSERTS
#  define DE_ASSERT_GUI_NODE_TYPE(node, expected_type) 
#else
#  define DE_ASSERT_GUI_NODE_TYPE(node, expected_type) \
	if(node->type != expected_type) de_fatal_error("Node must be " #expected_type " type!")
#endif

#include "gui/draw.h"
#include "gui/button.h"
#include "gui/border.h"
#include "gui/canvas.h"
#include "gui/grid.h"
#include "gui/scroll_bar.h"
#include "gui/scroll_content_presenter.h"
#include "gui/scroll_viewer.h"
#include "gui/text.h"
#include "gui/text_box.h"
#include "gui/window.h"
#include "gui/slide_selector.h"

/**
 * @brief
 *
 * Custom types should use DE_GUI_NODE_USER_CONTROL as origin point.
 */
typedef enum de_gui_node_type_t {
	DE_GUI_NODE_BASE, /**< Default node type */
	DE_GUI_NODE_TEXT,
	DE_GUI_NODE_BORDER,
	DE_GUI_NODE_WINDOW,
	DE_GUI_NODE_BUTTON,
	DE_GUI_NODE_SCROLL_BAR,
	DE_GUI_NODE_SCROLL_VIEWER,
	DE_GUI_NODE_TEXT_BOX,
	DE_GUI_NODE_GRID,                    /**< Automatically arranges children by rows and columns */
	DE_GUI_NODE_CANVAS,                  /**< Allows user to directly set position and size of a node */
	DE_GUI_NODE_SCROLL_CONTENT_PRESENTER, /**< Allows user to scroll content */
	DE_GUI_NODE_DRAWING,
	DE_GUI_NODE_SLIDE_SELECTOR,
	DE_GUI_NODE_USER_CONTROL /* Use this as start index for type for your custom controls */
} de_gui_node_type_t;

/**
 * @brief
 */
typedef enum de_gui_routed_event_type_t {
	DE_GUI_ROUTED_EVENT_MOUSE_DOWN,
	DE_GUI_ROUTED_EVENT_MOUSE_UP,
	DE_GUI_ROUTED_EVENT_MOUSE_ENTER,
	DE_GUI_ROUTED_EVENT_MOUSE_LEAVE,
	DE_GUI_ROUTED_EVENT_MOUSE_MOVE,
	DE_GUI_ROUTED_EVENT_LOST_FOCUS,
	DE_GUI_ROUTED_EVENT_GOT_FOCUS,
	DE_GUI_ROUTED_EVENT_KEY_DOWN,
	DE_GUI_ROUTED_EVENT_KEY_UP,
	DE_GUI_ROUTED_EVENT_TEXT,
} de_gui_routed_event_type_t;

/**
 * @brief Routed event. Typed union.
 */
typedef struct de_gui_routed_event_args_t {
	de_gui_routed_event_type_t type;
	bool handled; /* if set to true, further routing will be stopped */
	de_gui_node_t* source; /* node which initiated this event */
	union {
		struct {
			de_vec2_t pos;
		} mouse_move;
		struct {
			de_vec2_t pos;
			enum de_mouse_button button;
		} mouse_down;
		struct {
			de_vec2_t pos;
			enum de_mouse_button button;
		} mouse_up;
		struct {
			uint32_t unicode;
		} text;
		struct {
			enum de_key key;
			int alt : 1;
			int control : 1;
			int shift : 1;
			int system : 1;
		} key;
	} s;
} de_gui_routed_event_args_t;

/**
 * @brief
 */
typedef enum de_gui_node_visibility_t {
	DE_GUI_NODE_VISIBILITY_HIDDEN,    /**< Node will be invisible, but its bounds will participate in layout */
	DE_GUI_NODE_VISIBILITY_COLLAPSED, /**< Node will be invisible and no space will be reserved for it in layout */
	DE_GUI_NODE_VISIBILITY_VISIBLE    /**< Node will be visible */
} de_gui_node_visibility_t;

#define DE_MEMBER_SIZE(type, member) sizeof(((type *)0)->member)


#define DE_GUI_NODE_DESIRED_POSITION_PROPERTY "DesiredPosition"
#define DE_GUI_NODE_ACTUAL_POSITION_PROPERTY "ActualPosition"
#define DE_GUI_NODE_SCREEN_POSITION_PROPERTY "ScreenPosition"

#define DE_GUI_BUTTON_HOVERED_COLOR_PROPERTY "HoveredColor"
#define DE_GUI_BUTTON_PRESSED_COLOR_PROPERTY "PressedColor"
#define DE_GUI_BUTTON_NORMAL_COLOR_PROPERTY "NormalColor"

/**
 * @brief
 */
typedef struct de_gui_margin_t {
	float left;   /**< Margin for left side */
	float top;    /**< Margin for top side */
	float right;  /**< Margin for right side */
	float bottom; /**< Margin for bottom side */
} de_gui_margin_t;

/** Events */
typedef void(*de_mouse_down_event_t)(de_gui_node_t*, de_gui_routed_event_args_t*);
typedef void(*de_mouse_up_event_t)(de_gui_node_t*, de_gui_routed_event_args_t*);
typedef void(*de_mouse_move_event_t)(de_gui_node_t*, de_gui_routed_event_args_t*);
typedef void(*de_mouse_enter_event_t)(de_gui_node_t*, de_gui_routed_event_args_t*);
typedef void(*de_mouse_leave_event_t)(de_gui_node_t*, de_gui_routed_event_args_t*);
typedef void(*de_lost_focus_event_t)(de_gui_node_t*, de_gui_routed_event_args_t*);
typedef void(*de_got_focus_event_t)(de_gui_node_t*, de_gui_routed_event_args_t*);
typedef void(*de_text_event_t)(de_gui_node_t*, de_gui_routed_event_args_t*);
typedef void(*de_key_down_event_t)(de_gui_node_t*, de_gui_routed_event_args_t*);
typedef void(*de_key_up_event_t)(de_gui_node_t*, de_gui_routed_event_args_t*);

#define DE_DECLARE_PROPERTY_SETTER(type__, field__, passed_name__, field_name__, value__, data_size__, object__) \
	if (strcmp(field_name__, passed_name__) == 0) { \
		if (data_size == DE_MEMBER_SIZE(type__, field__)) { \
			memcpy(&object__->field__, value__, data_size__); \
		} \
	}

#define DE_DECLARE_PROPERTY_GETTER(type__, field__, passed_name__, field_name__, value__, data_size__, object__) \
	if (strcmp(field_name__, passed_name__) == 0) { \
		if (data_size == DE_MEMBER_SIZE(type__, field__)) { \
			memcpy(value__, &object__->field__, data_size__); \
      return true; \
		} \
	}

typedef struct de_gui_dispatch_table_t {
	void(*deinit)(de_gui_node_t* n); /* required */
	void(*measure)(de_gui_node_t* n, const de_vec2_t* constraint); /* optional */
	void(*layout_children)(de_gui_node_t* n); /* optional */
	void(*update)(de_gui_node_t* n);/* optional */
	void(*render)(de_gui_draw_list_t* dl, de_gui_node_t* n, uint8_t nesting);/* optional */
	bool(*get_property)(de_gui_node_t* n, const char* name, void* value, size_t data_size);/* optional */
	bool(*set_property)(de_gui_node_t* n, const char* name, const void* value, size_t data_size);/* optional */
	bool(*parse_property)(de_gui_node_t* n, const char* name, const char* value);/* optional */	
} de_gui_dispatch_table_t;

/**
 * @brief GUI node. Tagged union (https://en.wikipedia.org/wiki/Tagged_union)
 */
struct de_gui_node_t {	
	de_gui_node_type_t type;                            /**< Actual type of the node */
	de_gui_dispatch_table_t* dispatch_table;            /**< Table of pointers to type-related functions (vtable) */
	de_vec2_t desired_local_position;                   /**< Desired position relative to parent node */
	de_vec2_t actual_local_position;                    /**< Position that will be used in layout */
	de_vec2_t screen_position;                          /**< Screen position of the node */
	de_vec2_t desired_size;                             /**< Desired size of the node */
	de_vec2_t actual_size;                              /**< Size that will be used in layout */
	de_color_t color;                                   /**< Overlay color of the node */
	size_t row;                                         /**< Index of row to which this node belongs */
	size_t column;                                      /**< Index of column to which this node belongs */
	de_gui_vertical_alignment_t vertical_alignment;     /**< Vertical alignment */
	de_gui_horizontal_alignment_t horizontal_alignment; /**< Horizontal alignment */
	de_gui_margin_t margin;                             /**< Margin (four sides) */
	de_gui_node_visibility_t visibility;                /**< Current visibility state */
	float opacity;                                      /**< Opacity. Inherited property - children will have same opacity */
	uint16_t tab_index;                                 /**< Index for keyboard navigation */
	struct de_gui_node_t* parent;                       /**< Pointer to parent node */
	DE_ARRAY_DECLARE(struct de_gui_node_t*, children);  /**< Array of children nodes */
	DE_ARRAY_DECLARE(int, geometry);                    /**< Array of indices to draw command in draw list */
	bool is_hit_test_visible;                      /**< Will this control participate in hit-test */
	void* user_data; /**< Pointer to any data. Useful to pass data to callbacks */

	/* Specialization (type-specific data) */
	union {
		de_gui_border_t border;
		de_gui_text_t text;
		de_gui_button_t button;
		de_gui_grid_t grid;
		de_gui_scroll_bar_t scroll_bar;
		de_gui_canvas_t canvas;
		de_gui_scroll_viewer_t scroll_viewer;
		de_gui_scroll_content_presenter_t scroll_content_presenter;
		de_gui_window_t window;
		de_gui_text_box_t text_box;
		de_gui_slide_selector_t slide_selector;
		void* user_control;
	} s;

	/* Public events */
	de_mouse_down_event_t mouse_down;
	de_mouse_up_event_t mouse_up;
	de_mouse_move_event_t mouse_move;
	de_mouse_enter_event_t mouse_enter;
	de_mouse_leave_event_t mouse_leave;
	de_lost_focus_event_t lost_focus;
	de_got_focus_event_t got_focus;
	de_text_event_t text_entered;
	de_key_down_event_t key_down;
	de_key_up_event_t key_up;
	bool is_focused;
	bool is_mouse_over;

	/* intrusive linked list */
	DE_LINKED_LIST_ITEM(struct de_gui_node_t);
	de_gui_t* gui;
};

/**
 * @brief
 */
struct de_gui_t {
	DE_LINKED_LIST_DECLARE(de_gui_node_t, nodes);
	de_core_t* core;
	de_gui_draw_list_t draw_list;
	size_t text_buffer_size;
	uint32_t* text_buffer; /**< Temporary buffer to convert utf8 -> utf32. do NOT store pointer to this memory */
	de_font_t* default_font;
	de_gui_node_t* captured_node; /**< Pointer to node that captured mouse input. */
	de_gui_node_t* keyboard_focus;
	de_gui_node_t* picked_node;
	de_gui_node_t* prev_picked_node;
	de_vec2_t prev_mouse_pos;
	size_t tab_width; /* count of spaces per tab */
};

/**
 * @brief Initializes GUI. For internal use only!
 */
de_gui_t* de_gui_init(de_core_t* core);

/**
 * @brief Shutdowns GUI. For internal use only!
 */
void de_gui_shutdown(de_gui_t* gui);

/**
 * @brief Calculates screen positions of node. Acts recursively to child nodes.
 * @param node
 */
void de_gui_node_update_transform(de_gui_node_t* node);

/**
 * @brief Allocates memory and initializes common node data.
 * @param type
 * @return
 *
 * You must use this function only when you need to create your own control!
 * DO NOT USE THIS DIRECTLY IN USER CODE! USE de_gui_NODETYPE_create()
 *
 * dispatch_table - MUST be a pointer to static de_gui_dispatcher_entry_t!
 */
de_gui_node_t* de_gui_node_alloc(de_gui_t* gui, de_gui_node_type_t type, de_gui_dispatch_table_t* dispatch_table);

/**
 * @brief Sets desired local position of a node. Actual position can be different and depends on layout.
 */
void de_gui_node_set_desired_local_position(de_gui_node_t* node, float x, float y);

/**
 * @brief Sets desired size of a node. Actual size can be different. It depends on alignment settings and type of layout.
 */
void de_gui_node_set_desired_size(de_gui_node_t* node, float w, float h);

bool de_gui_node_set_property(de_gui_node_t* n, const char* name, const void* value, size_t data_size);

bool de_gui_node_parse_property(de_gui_node_t* n, const char* name, const char* value);

bool de_gui_node_get_property(de_gui_node_t* n, const char* name, void* value, size_t data_size);

void de_gui_node_set_hit_test_visible(de_gui_node_t* n, bool visibility);

/**
 * @brief
 * @param node
 * @param color
 */
void de_gui_node_set_color(de_gui_node_t* node, const de_color_t* color);

/**
 * @brief
 * @param node
 * @param r
 * @param g
 * @param b
 * @param a
 */
void de_gui_node_set_color_rgba(de_gui_node_t* node, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

/**
 * @brief
 * @param node
 * @param parent
 * @return
 */
bool de_gui_node_attach(de_gui_node_t* node, de_gui_node_t* parent);

/**
 * @brief
 * @param node
 */
void de_gui_node_detach(de_gui_node_t* node);

/**
 * @brief
 * @param node
 */
void de_gui_node_free(de_gui_node_t* node);

/**
 * @brief
 * @param node
 * @param row
 */
void de_gui_node_set_row(de_gui_node_t* node, uint16_t row);

/**
 * @brief
 * @param node
 * @param col
 */
void de_gui_node_set_column(de_gui_node_t* node, uint16_t col);

/**
 * @brief
 * @param node
 * @param margin
 */
void de_gui_node_set_margin_uniform(de_gui_node_t* node, float margin);

/**
 * @brief
 * @param node
 * @param left
 * @param top
 * @param right
 * @param bottom
 */
void de_gui_node_set_margin(de_gui_node_t* node, float left, float top, float right, float bottom);

/**
 * @brief
 * @param node
 * @param type
 * @return
 */
de_gui_node_t* de_gui_node_find_parent_of_type(de_gui_node_t* node, de_gui_node_type_t type);

/**
* @brief
* @param node
* @param type
* @return
*/
de_gui_node_t* de_gui_node_find_direct_child_of_type(de_gui_node_t* node, de_gui_node_type_t type);

/**
* @brief
* @param node
* @param type
* @return
*/
de_gui_node_t* de_gui_node_find_child_of_type(de_gui_node_t* node, de_gui_node_type_t type);

/**
 * @brief
 * @param node
 */
void de_gui_node_capture_mouse(de_gui_node_t* node);

/**
 * @brief
 * @param node
 */
void de_gui_node_release_mouse_capture(de_gui_node_t* node);

/**
 * @brief
 * @param node
 * @param evt
 */
void de_gui_node_set_mouse_down(de_gui_node_t* node, de_mouse_down_event_t evt);

/**
 * @brief
 * @param node
 * @param evt
 */
void de_gui_node_set_mouse_up(de_gui_node_t* node, de_mouse_up_event_t evt);

/**
 * @brief
 * @param node
 * @param evt
 */
void de_gui_node_set_mouse_move(de_gui_node_t* node, de_mouse_move_event_t evt);

/**
 * @brief
 * @param node
 * @param vis
 */
void de_gui_node_set_visibility(de_gui_node_t* node, de_gui_node_visibility_t vis);

/**
 * @brief
 * @param node
 * @param va
 */
void de_gui_node_set_vertical_alignment(de_gui_node_t* node, de_gui_vertical_alignment_t va);

/**
 * @brief
 * @param node
 * @param ha
 */
void de_gui_node_set_horizontal_alignment(de_gui_node_t* node, de_gui_horizontal_alignment_t ha);

/**
 * @brief
 */
de_gui_draw_list_t* de_gui_render(de_gui_t* gui);

void de_gui_update(de_gui_t* gui);


bool de_gui_process_event(de_gui_t* gui, const de_event_t* evt);

de_gui_node_t* de_gui_hit_test(de_gui_t* gui, float x, float y);

void de_gui_node_set_user_data(de_gui_node_t* node, void* user_data) {
	node->user_data = user_data;
}

void de_gui_node_measure(de_gui_node_t* node, const de_vec2_t* constraint);