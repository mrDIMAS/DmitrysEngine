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

/* Type-safe array */

/* Declares array as a struct */
#define DE_ARRAY_DECLARE(Type, Name) \
	struct { \
		Type* data; \
		size_t size; \
		size_t _capacity; \
	} Name \

/* Initializes array. Can be omitted if memory block containing array will filled with zeros */
#define DE_ARRAY_INIT(a) de_array_init_((void**)&(a).data, &(a).size, &(a)._capacity)

#define DE_ARRAY_GROW(a, n) de_array_grow_((void**)&(a).data, &(a).size, &(a)._capacity, sizeof(*(a).data), n);

/* Appends new item into array. Array capacity will be increased if needed */
#define DE_ARRAY_APPEND(a, item) \
	do { \
		DE_ARRAY_GROW(a, 1); \
		(a).data[(a).size - 1] = (item); \
	} while(0)

#define DE_ARRAY_MOVE(a, b) de_array_move_((void**)&(a).data, &(a).size, &(a)._capacity, (void**)&(b).data, &(b).size, &(b)._capacity)

/* Returns memory to OS */
#define DE_ARRAY_FREE(a) de_array_free_((void**)&(a).data, &(a).size, &(a)._capacity)

/* Fail-safe insertion (out-of-bounds check) with auto-grow of array */
#define DE_ARRAY_INSERT(a, pos, item) de_array_insert_((void**)&(a).data, &(a).size, &(a)._capacity, sizeof(*(a).data), (void*)&item, pos);

#define DE_ARRAY_RESERVE(a, new_capacity) de_array_reserve_((void**)&(a).data, &(a).size, &(a)._capacity, sizeof(*(a).data), new_capacity);

/* Sorts array using quick sort */
#define DE_ARRAY_QSORT(a, cmp) qsort((a).data, (a).size, sizeof(*(a).data), cmp)

#define DE_ARRAY_BSEARCH(a, key, cmp) bsearch(key, (a).data, (a).size, sizeof(*(a).data), cmp)

/* Returns size of array's content */
#define DE_ARRAY_SIZE_BYTES(a) ((a).size * sizeof(*(a).data))

/* Clears array (by resetting its size) and prepares it for re-use */
#define DE_ARRAY_CLEAR(a) (a).size = 0;

/* Reverses array members order */
#define DE_ARRAY_REVERSE(a) de_array_reverse_((void**)&(a).data, &(a).size, sizeof(*(a).data))

/* Makes array memory block size equals to real size of array's content */
#define DE_ARRAY_COMPACT(a) \
	do { \
		a._capacity = a.size; \
		a.data = de_realloc(a.data, a._capacity * sizeof(*a.data)); \
	} while(0) 

/* Returns index of item if it is in array. Returns size of array if item is not found */
#define DE_ARRAY_FIND(a, item, outIndex) \
	do { \
		for(outIndex = 0; outIndex < a.size; ++outIndex) { \
			if(a.data[outIndex] == item) { \
				break; \
			} \
		} \
	} while(0) 

/* Removes element at specified position */
#define DE_ARRAY_REMOVE_AT(a, itemIndex) \
	do { \
		size_t i; \
		if((int)itemIndex >= 0 && itemIndex < a.size) { \
			--a.size; \
			for(i = itemIndex; i < a.size; ++i) { \
				a.data[i] = a.data[i + 1]; \
			} \
		} \
	} while(0)

/* Tries to find and remove specified item */
#define DE_ARRAY_REMOVE(a, item) \
	do { \
		size_t itemIndex; \
		DE_ARRAY_FIND(a, item, itemIndex); \
		DE_ARRAY_REMOVE_AT(a, itemIndex); \
	} while(0)

#define DE_ARRAY_LAST(a) a.data[a.size-1]

#define DE_ARRAY_FIRST(a) a.data[0]

#define DE_ARRAY_AT(a, n) a.data[n]

/* Removes duplicates using cmp function as comparator */
#define DE_ARRAY_REMOVE_DUPLICATES(a, cmp)  \
	do { \
		for (int i = 0; i < intersections.size; ++i) { \
			for (int j = i + 1; j < intersections.size; ++j) { \
				if (cmp(&intersections.data[i], &intersections.data[j])) { \
					DE_ARRAY_REMOVE_AT(intersections, j); \
				} \
			} \
		} \
	} while(0) 

/* Internals functions. Do not use directly! Use macro instead. Macro desined to automatically 
 * calculate size of array elements and required pointers to array elements. */
void de_array_init_(void** data, size_t* size, size_t* capacity);
void de_array_grow_(void** data, size_t* size, size_t* capacity, size_t item_size, size_t n);
void de_array_reserve_(void** data, size_t* size, size_t* capacity, size_t item_size, size_t new_capacity);
void de_array_insert_(void** data, size_t* size, size_t* capacity, size_t item_size, void* item, size_t pos);
void de_array_free_(void** data, size_t* size, size_t* capacity);
void de_array_reverse_(void** data, size_t* size, size_t item_size);
void* de_array_find_(const void* data, const size_t* size, size_t item_size, const void* search_data, size_t search_data_size);
void de_array_move_(void** src_data, size_t* src_size, size_t* src_capacity, void** dest_data, size_t* dest_size, size_t* dest_capacity);