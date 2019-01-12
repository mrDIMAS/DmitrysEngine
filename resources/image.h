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

typedef struct de_rgb8_t
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
} de_rgb8_t;

typedef struct de_rgba8_t
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
} de_rgba8_t;

typedef struct de_image_t
{
	char* data;
	unsigned int width;
	unsigned int height;
	unsigned int byte_per_pixel;
} de_image_t;

/**
 * @brief Loads image from tga file. Compressed images not supported yet!
 */
bool de_image_load_tga(const char* filename, de_image_t* img);

/**
 * @brief Flips image upside down.
 */
void de_image_flip_y(de_image_t* img);