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

/* UTF32 string 
 * 
 * WARNING: Unlike de_str8_t does NOT contain null-terminator! */

/* Wrap-struct to prevent using string with DE_ARRAY_XXX functions
 * which can lead to undefined behaviour */
typedef struct de_str32_t {
	DE_ARRAY_DECLARE(uint32_t, str);
} de_str32_t;

/**
 * @brief Prepares string.
 */ 
void de_str32_init(de_str32_t* str);

/**
 * @brief Frees memory. Must be called for every string!
 */
void de_str32_free(de_str32_t* str);

/**
 * @brief Moves source string to destination. @src will be empty array after.
 * 
 * Notes: Automatically frees memory from @dest if such was allocated.
 */
void de_str32_move(de_str32_t* src, de_str32_t* dest);

/**
 * @brief Inserts single character into @pos
 * 
 * Performs out-of-bounds checks! @pos will be clamped to [0; size]
 */
void de_str32_insert(de_str32_t* str, int pos, uint32_t unicode);

/**
 * @brief Appends single character to the end of the string.
 */
void de_str32_append(de_str32_t* str, uint32_t unicode);

/**
 * @brief Remove @amount of characters from string starting from @pos
 * 
 * Performs out-of-bounds checks, remove range will be clamped to [0; size].
 */
void de_str32_remove(de_str32_t* str, int pos, int amount);

/**
 * @brief Sets size of string to zero. Does not reallocates memory!
 */
void de_str32_clear(de_str32_t* str);

/**
 * @brief Returns total count of characters in string. Unlike strlen this method is O(1).
 */
size_t de_str32_length(de_str32_t* str);

/**
 * @brief Returns pointer to immutable string data. 
 * 
 * WARNING: DO NOT modify internals of string using returned pointer!
 */
const uint32_t* de_str32_get_data(const de_str32_t* str);

/**
 * @brief Returns single character from string. 
 * 
 * WARNING: There is no out-of-bound checks!
 */
uint32_t de_str32_at(const de_str32_t* str, size_t i);

/**
 * @brief Sets string.
 */
void de_str32_set_utf8(de_str32_t* str, const de_str8_view_t* view);

/**
 * @brief Appends UTF8 string view.
 */
void de_str32_append_utf8(de_str32_t* str, const de_str8_view_t* view);

/**
 * @brief Appends null-terminated string as UTF8 string.
 */
void de_str32_append_cstr(de_str32_t* str, const char* cstr);

/**
 * @brief Inserts UTF-8 string at @pos by converting it to UTF-32.
 */

#if 0
void de_str32_insert_utf8(de_str32_t* str, const de_str8_view_t* view) {
	size_t old_size, decoded;
	int reserved_size = view->len * 4;
	old_size = str->str.size;
	DE_ARRAY_GROW(str->str, reserved_size);
	memmove(str->str.data + old_size + reserved_size, str->str.data + old_size, old_size -  )
	decoded = (size_t)de_utf8_to_utf32(view->data, view->len, str->str.data + old_size, reserved_size);
	str->str.size = old_size + decoded;
}
#endif