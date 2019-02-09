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

/* UTF-8 String with null-terminator (compatible with strlen, strcpy and others) */

 /* Wrap-struct to prevent using string with DE_ARRAY_XXX functions
  * which can lead to undefined behaviour */
typedef struct de_str8_t {
	DE_ARRAY_DECLARE(char, str);
} de_str8_t;

/* String slice, portion of some string */
typedef struct de_str8_view_t {
	const char* data;
	size_t len;
} de_str8_view_t;

/**
 * @brief Small helper to create string view.
 */
void de_str8_view_set(de_str8_view_t* view, const char* data, size_t len);

/**
 * @brief Initializes string. Allocates 1 byte for null-terminator.
 */
void de_str8_init(de_str8_t* str);

/**
 * @brief Set string content as UTF8 null terminated string (or plain C string)
 */
void de_str8_set(de_str8_t* str, const char* cstr);

/**
 * @brief Clears string. Does not deallocates memory!
 */
void de_str8_clear(de_str8_t* str);

/**
 * @brief Moves @src string to @dest string. @src string will be empty!
 */
void de_str8_move(de_str8_t* src, de_str8_t* dest);

/**
 * @brief Frees allocated memory.
 */
void de_str8_free(de_str8_t * str);

/**
 * @brief Appends UTF-8 null terminated string into string.
 */
void de_str8_append_cstr(de_str8_t* str, const char* utf8str);

void de_str8_append_str8(de_str8_t* str, const de_str8_t* other);

/**
 * @brief Appends string view to string.
 */
void de_str8_append_str_view(de_str8_t* str, const de_str8_view_t* view);

/**
 * @brief Returns length of a string.
 */
size_t de_str8_length(const de_str8_t* str);

/**
 * @brief Checks strings for equality.
 */
bool de_str8_eq(de_str8_t* str, const char* utf8str);

/**
 * @brief Checks two utf8 strings for equality
 */
bool de_str8_eq_str8(de_str8_t* str, de_str8_t* other);

/**
 * @brief Returns internal data as plain C string.
 */
const char* de_str8_cstr(const de_str8_t* str);

/**
 * @brief Reads string from file. 
 * 
 * Note: If @len == 0, reads everything until null-terminator. This usage is potentially 
 * dangerous and slower than reading string with known length. Use with caution!  
 */
size_t de_str8_fread(de_str8_t* str, FILE* file, size_t len);

/**
* @brief Creates copy of string on heap. You have to call @de_free to delete string
* @param src string
* @return copy of string
*/
void de_str8_copy(const de_str8_t* src, de_str8_t* dest);

typedef DE_ARRAY_DECLARE(char*, de_string_array_t);

/* Note: Uses internal fixed-size string to tokenize string. This function is NOT reentrant! */
void de_tokenize_string(const char* str, de_string_array_t* tokens, const char* delim);