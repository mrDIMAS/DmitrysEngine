/**
* @brief
*/
typedef struct de_gui_border_t
{
	de_color_t stroke_color;
	de_gui_thickness_t thickness;
} de_gui_border_t;

de_gui_node_t* de_gui_border_create(de_gui_t* gui);

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