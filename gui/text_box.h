/**
* @brief
*/
typedef struct de_gui_text_box_t
{
	de_gui_node_t* border;
	de_bool_t multiline;
	de_gui_node_t* text_block;
	de_string_t text;
} de_gui_text_box_t;

de_gui_node_t* de_gui_text_box_create(de_gui_t* gui);
