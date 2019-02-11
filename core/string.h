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

/* String slice, portion of some string. NOT compatible with strlen and others.
 * Be careful with string views: if source string will be changed then ALL views
 * will become invalid! So do NOT store views for long period of time: use them
 * as soon as possible. */
typedef struct de_str8_view_t {
	const char* data;
	size_t len;
} de_str8_view_t;

typedef DE_ARRAY_DECLARE(de_str8_view_t, de_str8_view_array_t);

/**
 * @brief Small helper to create string view.
 */
void de_str8_view_set(de_str8_view_t* view, const char* data, size_t len);

/**
 * @brief Small helper to create string view from c string.
 */
void de_str8_view_set_cstr(de_str8_view_t* view, const char* data);

/**
 * @brief Prints string view into console.
 */
void de_str8_view_print(de_str8_view_t* view);

/**
 * @brief Creates new string from given view.
 */
void de_str8_view_to_string(const de_str8_view_t* view, de_str8_t* str);

/**
 * @brief Compares string view with any null-terminated string.
 */
bool de_str8_view_eq_cstr(const de_str8_view_t* view, const char* utf8str);

/**
 * @brief Compares string view with str8.
 */
bool de_str8_view_eq_str8(const de_str8_view_t* view, const de_str8_t* str);

/**
 * @brief Compares two string views.
 */
bool de_str8_view_eq(const de_str8_view_t* view, const de_str8_view_t* other);

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
 * @brief Appends UTF-8 null terminated string.
 */
void de_str8_append_cstr(de_str8_t* str, const char* utf8str);

/**
 * @brief Appends UTF8 string.
 */
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
 * @brief Searches for a given substring in a given string. Fills @out_view with a params of first occurrence.
 */
void de_str8_find(const de_str8_t* str, size_t offset, const de_str8_view_t* substr, de_str8_view_t* out_view);

/**
 * @brief Fills out string view with a requested sub-string range [offset, offset + count].
 * 
 * Note: Function is non-sensitive for out-of-bounds arguments.
 */
void de_str8_substr(const de_str8_t* str, de_str8_view_t* view, size_t offset, size_t count);

/**
 * @brief Replaces all substrings in a string with a given string.
 * 
 * Note: @what and @with must NOT be views of @str!
 */
void de_str8_replace(de_str8_t* str, const de_str8_view_t* what, const de_str8_view_t* with);

/**
 * @brief Reads string from file. 
 * 
 * Note: If @len == 0, reads everything until null-terminator. This usage is potentially 
 * dangerous and slower than reading string with known length. Use with caution!  
 */
size_t de_str8_fread(de_str8_t* str, FILE* file, size_t len);

/**
 * @brief Copies @src to @dest
 */
void de_str8_copy(const de_str8_t* src, de_str8_t* dest);

/**
 * @brief Returns array of string views for tokens. Array must be freed!
 */
de_str8_view_array_t de_str8_tokenize(const de_str8_t* str, const char* delim);

/**
 * @brief Safe sprintf. Does NOT accept string views.
 */
void de_str8_format(de_str8_t* str, const char* format, ...);

/**
 * @brief Internal tests.
 */
void de_str8_tests(void);