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
* @brief Loads file as a null-terminated string.
* @param path file to load
* @param out_size total bytes count, can be NULL
*/
char* de_load_file_into_memory(const char * path, size_t* out_size);

/**
 * @brief Converts a file 'source' as array to file 'dest'
 *
 * Will create a file 'dest' with such content:
 *
 * static const char array[] = {
 *     123, 234, 12, 0, 34, 16, ...
 * };
 *
 * Can be used to pack any file into a source code file. Useful when
 * you need to store some resource as data in your executable file.
 */
void de_convert_to_c_array(const char* source, const char* dest);

/**
 * @brief Extracts only name from file path without extension.
 * 
 * Example: "baz/bar/foo.bar" path will result in "foo" string.
 */
void de_file_extract_name(const char* path, char* buffer, size_t buffer_size);

/**
 * @brief Extracts extension from file path with dot.
 * 
 * Example: "baz/bar/foo.bar" will result in ".bar" string.
 */
void de_file_extract_extension(const char* path, char* buffer, size_t buffer_size);

/**
 * @brief Tests for file system utilities.
 */
void de_file_tests(void);