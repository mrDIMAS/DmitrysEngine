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

void de_array_init_(void** data, size_t* size, size_t* capacity)
{
	*data = NULL;
	*size = 0;
	*capacity = 0;
}

void* de_array_grow_(void** data, size_t* size, size_t* capacity, size_t item_size, size_t n)
{	
	const size_t old_size = *size;
	*size += n;
	if (*size >= *capacity && *size > 0) {
		*capacity = *capacity * 2u + n;
		*data = de_realloc(*data, *capacity * item_size);
	}
	return ((char*)*data) + old_size * item_size;
}

void de_array_reserve_(void** data, size_t* size, size_t* capacity, size_t item_size, size_t new_capacity)
{
	*capacity = new_capacity;
	if (*size > *capacity) {
		*size = *capacity;
	}
	*data = de_realloc(*data, *capacity * item_size);
}

void de_array_insert_(void** data, size_t* size, size_t* capacity, size_t item_size, void* item, size_t pos)
{
	size_t byte_count;
	char* bytes, *dest, *src;
	de_array_grow_(data, size, capacity, item_size, 1);
	if (pos >= *size) {
		if (*size == 0) {
			pos = 0;
		} else {
			pos = *size - 1;
		}
	}
	bytes = (char*)*data;
	src = bytes + pos * item_size;
	if (*size) {
		dest = src + item_size;
		byte_count = (*size - 1 - pos) * item_size;
		memmove(dest, src, byte_count);
	}
	memcpy(src, item, item_size);
}

void de_array_free_(void** data, size_t* size, size_t* capacity)
{
	de_free(*data);
	*data = NULL;
	*size = 0;
	*capacity = 0;
}

void de_array_reverse_(void** data, size_t* size, size_t item_size)
{
	char* swapBuffer = de_malloc(item_size);
	size_t i = *size - 1, j = 0;
	while (i > j) {
		void* right = (char*)*data + i;
		void* left = (char*)*data + j;
		memcpy(swapBuffer, right, item_size);
		memcpy(right, left, item_size);
		memcpy(left, swapBuffer, item_size);
		--i;
		++j;
	}
	de_free(swapBuffer);
}

void* de_array_find_(const void* data, const size_t* size, size_t item_size, const void* search_data, size_t search_data_size)
{
	size_t i;
	char* ptr;
	if (item_size != search_data_size) {
		return NULL; /* trying to search invalid item */
	}
	ptr = (char*)data;
	for (i = 0; i < *size; ++i) {
		if (memcmp(ptr, search_data, item_size) == 0) {
			return ptr;
		}
		ptr += item_size;
	}
	return NULL;
}

void de_array_move_(void** src_data, size_t* src_size, size_t* src_capacity, void** dest_data, size_t* dest_size, size_t* dest_capacity)
{
	de_array_free_(dest_data, dest_size, dest_capacity); /* prevent mem leaks */
	*dest_data = *src_data;
	*src_data = NULL;
	*dest_size = *src_size;
	*src_size = 0;
	*dest_capacity = *src_capacity;
	*src_capacity = 0;
}

void de_array_copy_(const void** src_data, size_t src_size, void** dest_data, size_t* dest_size, size_t* dest_capacity, size_t item_size)
{
	if (src_size > *dest_size) {
		de_array_grow_(dest_data, dest_size, dest_capacity, item_size, src_size - *dest_size);
	}
	memcpy(*dest_data, *src_data, src_size * item_size);
}