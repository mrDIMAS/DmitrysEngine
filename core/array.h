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

/* Declares array as a struct */
#define DE_ARRAY_DECLARE(Type, Name) \
	struct { \
		Type* data; \
		size_t size; \
		size_t _capacity; \
	} Name \

/*=======================================================================================*/
/* Initializes array. Can be omitted if memory block containing array will filled with zeros */
#define DE_ARRAY_INIT(a) memset(&(a), 0, sizeof(a))

/*=======================================================================================*/
#define DE_ARRAY_GROW(a, n) \
	do { \
		(a).size += n; \
		if((a).size >= (a)._capacity) { \
			(a)._capacity = (a)._capacity * 2u + (size_t)n; \
			(a).data = de_realloc((a).data, (a)._capacity * sizeof(*(a).data)); \
		} \
	} while(0)

#define de_array_reserve_n(a, n) (DE_ARRAY_GROW(a, n), (a).data - n)

/*=======================================================================================*/
/* Appends new item into array. Array capacity will be increased if needed */
#define DE_ARRAY_APPEND(a, item) \
	do { \
		DE_ARRAY_GROW(a, 1); \
		(a).data[(a).size - 1] = (item); \
	} while(0)

/*=======================================================================================*/
/* Returns memory to OS */
#define DE_ARRAY_FREE(a) \
	do { \
		de_free((a).data); \
		DE_ARRAY_INIT(a); \
	} while(0) \

/*=======================================================================================*/
#define DE_ARRAY_RESERVE(a, new_capacity) \
	do { \
		(a)._capacity = new_capacity; \
		if((a).size > (a)._capacity) { \
			(a).size = (a)._capacity; \
		}\
		(a).data = de_realloc((a).data, (a)._capacity * sizeof(*(a).data)); \
	} while(0)

/*=======================================================================================*/
/* Sorts array using quick sort */
#define DE_ARRAY_QSORT(a, cmp) qsort((a).data, (a).size, sizeof(*(a).data), cmp)

/*=======================================================================================*/
#define DE_ARRAY_BSEARCH(a, key, cmp) bsearch(key, (a).data, (a).size, sizeof(*(a).data), cmp)

/*=======================================================================================*/
/* Returns size of array's content */
#define DE_ARRAY_SIZE_BYTES(a) ((a).size * sizeof(*(a).data))

/*=======================================================================================*/
/* Clears array (by resetting its size) and prepares it for re-use */
#define DE_ARRAY_CLEAR(a) (a).size = 0;

/*=======================================================================================*/
/* Reverses array members order */
#define DE_ARRAY_REVERSE(a) \
	do { \
		char* swapBuffer = de_malloc(sizeof(*a.data)); \
		size_t i = a.size - 1, j = 0; \
		while(i > j) { \
			void* right = a.data + i; \
			void* left = a.data + j; \
			memcpy(swapBuffer, right, sizeof(*a.data)); \
			memcpy(right, left, sizeof(*a.data)); \
			memcpy(left, swapBuffer, sizeof(*a.data)); \
			--i; \
			++j; \
		} \
		de_free(swapBuffer); \
	} while(0)

/*=======================================================================================*/
/* Makes array memory block size equals to real size of array's content */
#define DE_ARRAY_COMPACT(a) \
	do { \
		a._capacity = a.size; \
		a.data = de_realloc(a.data, a._capacity * sizeof(*a.data)); \
	} while(0) 

/*=======================================================================================*/
/* Returns index of item if it is in array. Returns size of array if item is not found */
#define DE_ARRAY_FIND(a, item, outIndex) \
	do { \
		for(outIndex = 0; outIndex < a.size; ++outIndex) { \
			if(a.data[outIndex] == item) { \
				break; \
			} \
		} \
	} while(0) 

/*=======================================================================================*/
/* Removes element at specified position */
#define DE_ARRAY_REMOVE_AT(a, itemIndex) \
	do { \
		size_t i; \
		if(itemIndex >= 0 && itemIndex < a.size) { \
			--a.size; \
			for(i = itemIndex; i < a.size; ++i) { \
				a.data[i] = a.data[i + 1]; \
			} \
		} \
	} while(0)

/*=======================================================================================*/
/* Tries to find and remove specified item */
#define DE_ARRAY_REMOVE(a, item) \
	do { \
		size_t itemIndex; \
		DE_ARRAY_FIND(a, item, itemIndex); \
		DE_ARRAY_REMOVE_AT(a, itemIndex); \
	} while(0)

/*=======================================================================================*/
#define DE_ARRAY_LAST(a) a.data[a.size-1]

/*=======================================================================================*/
#define DE_ARRAY_FIRST(a) a.data[0]

/*=======================================================================================*/
#define DE_ARRAY_AT(a, n) a.data[n]

/*=======================================================================================*/
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

#define DE_ARRAY_INSERT(a, pos, item) \
    do { \
        if(a.size == 0) { \
            DE_ARRAY_APPEND(a, item); \
		} \
		if (pos >= 0 && pos < a.size) { \
			DE_ARRAY_GROW(a, 1); \
			memmove(a.data + pos + 1, a.data + pos, sizeof(*a.data) * (a.size - pos)); \
			memcpy(&item, a.data + pos, sizeof(item)); \
		} \
	} while(0)