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

typedef DE_ARRAY_DECLARE(char, de_string_t);
typedef DE_ARRAY_DECLARE(uint32_t, de_string32_t);
typedef DE_ARRAY_DECLARE(char*, de_string_array_t);

void de_str_init(de_string_t* str);

void de_str_from_utf8(de_string_t* str, const char* cstr);

void de_str_free(de_string_t* str);

/**
* @brief Creates copy of string on heap. You have to call @de_free to delete string
* @param src string
* @return copy of string
*/
char* de_str_copy(const char* src);

/**
* @brief Acts like sprintf, but works with internal fixed-size buffer
* @param format format line
* @return formatted string
*/
char* de_str_format(const char* format, ...);

int de_utf8_to_utf32(const char* inString, uint32_t* out, int bufferSize);

/* Note: Uses internal fixed-size string to tokenize string. This function is NOT reentrant! */
void de_tokenize_string(const char* str, de_string_array_t* tokens, const char* delim);