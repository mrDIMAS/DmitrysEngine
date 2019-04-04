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
* @brief Possible texture types
*/
typedef enum de_texture_type_e {
	DE_TEXTURE_TYPE_2D,
	DE_TEXTURE_TYPE_VOLUME,
	DE_TEXTURE_TYPE_CUBE
} de_texture_type_t;

/**
* @brief Common texture. Can be 2D, volume or cube
*/
struct de_texture_t {				
	unsigned int id; /**< OpenGL texture id */
	int width; /**< Width of texture */
	int height; /**< Height of texture */
	int depth; /**< Depth of volume texture */
	char* pixels; /**< Texture pixels */
	int byte_per_pixel; /**< Count of bytes per pixel */
	de_texture_type_t type; /**< Type of texture */
	bool need_upload; /**< Indicates that texture needs to be uploaded to GPU */
};

de_resource_dispatch_table_t* de_texture_get_dispatch_table(void);

/**
 * @brief Allocates pixels for rectangle image.
 */
void de_texture_alloc_pixels(de_texture_t* tex, int w, int h, size_t byte_per_pixel);