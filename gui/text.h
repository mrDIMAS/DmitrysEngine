/**
* @brief
*/
typedef enum de_gui_text_alignment_t {
	DE_GUI_TA_LEFT,
	DE_GUI_TA_CENTER
} de_gui_text_alignment_t;

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
	de_gui_text_alignment_t alignment;
	DE_ARRAY_DECLARE(de_gui_text_line_t, lines);
	float total_lines_height;
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
void de_gui_text_set_text(de_gui_node_t* node, const char* utf8str);

/**
* @brief
* @param node
* @param alignment
*/
void de_gui_text_set_alignment(de_gui_node_t* node, de_gui_text_alignment_t alignment);

