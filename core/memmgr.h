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

/**
 * @brief Allocates raw memory. On failure raises error
 * @param size requested size
 */
void* de_malloc(size_t size);

/**
 * @brief Allocates clean (filled with zeroes) memory. On failure raises error
 * @param count count of objects
 * @param size size per object
 */
void* de_calloc(size_t count, size_t size);

/**
 * @brief Reallocates memory. On failure raises error
 * @param ptr pointer
 */
void* de_realloc(void* ptr, size_t size);

/**
 * @brief Returns memory to OS
 * @param ptr pointer
 */
void de_free(void* ptr);

/**
 * @brief Returns total number of active memory allocations
 */
size_t de_get_alloc_count(void);

/**
 * Allocates memory for structure. Memory is filled with zeroes
 */
#define DE_NEW(type) (type*)de_calloc(1, sizeof(type))

 /**
	* Calculates static array size
	*/
#define DE_ARRAY_SIZE(array) (sizeof(array) / (sizeof(*array)))
