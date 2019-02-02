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
	de_renderer_t* renderer;
	DE_LINKED_LIST_ITEM(struct de_texture_t);
	char* name;             /**< Name for procedural texture, or file name for textures loaded from file */
	unsigned int id;        /**< OpenGL texture id */
	int ref_count;          /**< Use count of texture, texture is shared object */
	int width;              /**< Width of texture */
	int height;             /**< Height of texture */
	int depth;              /**< Depth of volume texture */
	char* pixels;           /**< Texture pixels */
	int byte_per_pixel;     /**< Count of bytes per pixel */
	de_texture_type_t type; /**< Type of texture */
	bool need_upload;     /**< Indicates that texture needs to be uploaded to GPU */
};

/**
* @brief Increases reference count of texture
* @param tex pointer to texture
*/
void de_texture_add_ref(de_texture_t* tex);

/**
* @brief Decreases reference count of texture, if reference count is zero - frees texture
* @param tex pointer to texture
*/
void de_texture_release(de_texture_t* tex);
