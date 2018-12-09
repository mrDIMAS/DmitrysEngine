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

typedef struct de_glyph_t
{
	float bitmap_top;
	float bitmap_left;
	int bitmap_width;
	int bitmap_height;
	de_color_t* pixels;
	float advance;
	char has_outline;
	de_vec2_t texCoords[4];
} de_glyph_t;

typedef struct de_font_charmap_entry_t
{
	uint32_t unicode;
	uint32_t glyph_index;
} de_font_charmap_entry_t;

typedef struct de_font_t
{
	DE_LINKED_LIST_ITEM(struct de_font_t);
	float size;
	int glyph_count;
	de_glyph_t* glyphs;
	int glyphsSorted;
	float ascender;
	float descender;
	float line_gap;
	de_texture_t* texture;
	DE_ARRAY_DECLARE(de_font_charmap_entry_t, charmap);
} de_font_t;

/**
 * @brief Loads font from TTF file
 */
de_font_t* de_font_load_ttf(const char * ttf, float height, const int* char_set, int char_count);

/**
 * @brief Frees resources associated with font
 */
void de_font_free(de_font_t* font);

de_glyph_t* de_font_get_glyph(const de_font_t* font, uint32_t code);