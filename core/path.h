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

typedef struct de_path_t {
	de_str8_t str;	
} de_path_t;

void de_path_init(de_path_t* path);

void de_path_free(de_path_t* path);

void de_path_clear(de_path_t* path);

/**
 * @brief Initializes path as string view. Useful to temporarily use existing string as path.
 */
void de_path_as_str8_view(de_path_t* path, de_str8_t* str);

const char* de_path_cstr(const de_path_t* path);

void de_path_append_cstr(de_path_t* path, const char* utf8str);

void de_path_append_str8(de_path_t* path, const de_str8_t* str);

void de_path_append_str_view(de_path_t* path, const de_str8_view_t* view);

bool de_path_eq(const de_path_t* a, const de_path_t* b);

/**
 * @brief Extracts extension from file path with dot.
 *
 * Example: "baz/bar/foo.bar"
 *                      ^^^^ -> .bar
 */
void de_path_extension(const de_path_t* p, de_str8_view_t* ext);

/**
 * @brief Extracts only name from file path without extension.
 *
 * Example: "baz/bar/foo.bar"
 *                   ^^^ -> foo
 */
void de_path_file_name(const de_path_t* p, de_str8_view_t* name);
