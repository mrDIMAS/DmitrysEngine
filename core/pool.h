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
 * Pool used to increase performance of memory allocations and reduce
 * memory fragmentation. The pool can grow and shrink as much as you
 * need. Current implementation allows to track objects by special pool
 * object references. Each reference contains special field called stamp
 * which allows you to check if reference points to the same object as
 * the object you obtained when called de_pool_spawn. This is very simple
 * mechanism of safe and fast memory management in C. The pool supports
 * object destructors which will be called when you will return an object
 * into pool.
 */

#define DE_POOL_STAMP_FREE 0

typedef int de_stamp_t;

typedef struct de_ref_t {
	int index; /**< Index of an object in a pool. */
	de_stamp_t stamp; /**< Unique identifier of an object in a pool. Used to check if reference is valid. */
} de_ref_t;

#define de_null_ref { -1, 0 }

typedef void(*de_pool_object_destructor_t)(void*);

typedef struct de_pool_t {
	char* records;
	size_t item_size;
	size_t record_size;
	size_t capacity;
	size_t spawned_count;
	de_stamp_t global_stamp;
	size_t* free_stack;
	int free_stack_top;
	de_pool_object_destructor_t destructor;
} de_pool_t;

void de_pool_init(de_pool_t* pool, size_t item_size, size_t initial_capacity, de_pool_object_destructor_t obj_destructor);

void de_pool_realloc_memory(de_pool_t* pool, size_t old_capacity);

de_ref_t de_pool_spawn(de_pool_t* pool);

void de_pool_return(de_pool_t* pool, de_ref_t ref);

void de_pool_clear(de_pool_t* pool);

bool de_pool_is_valid_ref(de_pool_t* pool, de_ref_t ref);

void* de_pool_get_ptr(de_pool_t* pool, de_ref_t ref);