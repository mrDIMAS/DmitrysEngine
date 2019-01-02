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


/*=======================================================================================*/
void de_pool_init(de_pool_t* pool, size_t item_size, size_t initial_capacity, de_pool_object_destructor_t obj_destructor)
{
	pool->records = NULL;
	pool->free_stack = NULL;
	pool->free_stack_top = 0;
	pool->spawned_count = 0;
	pool->item_size = item_size;
	pool->record_size = item_size + sizeof(de_stamp_t);
	pool->capacity = initial_capacity;
	pool->global_stamp = DE_POOL_STAMP_FREE + 1;
	pool->destructor = obj_destructor;
	de_pool_realloc_memory(pool, 0);
}

/*=======================================================================================*/
void de_pool_realloc_memory(de_pool_t* pool, size_t old_capacity)
{
	int i;
	pool->records = (char*)de_realloc(pool->records, pool->record_size * pool->capacity);
	memset(pool->records + old_capacity, 0, pool->record_size * (pool->capacity - old_capacity));
	pool->free_stack = (size_t*)de_realloc(pool->free_stack, pool->capacity * sizeof(size_t));
	for (i = pool->capacity - 1; i >= (int)old_capacity; --i)
	{
		assert((unsigned)pool->free_stack_top < pool->capacity);
		pool->free_stack[pool->free_stack_top++] = i;
	}
}

/*=======================================================================================*/
static de_stamp_t de_pool_internal_get_stamp(de_pool_t* pool, int index)
{
	return *((de_stamp_t*)(pool->records + pool->record_size * index));
}

/*=======================================================================================*/
static void de_pool_internal_set_stamp(de_pool_t* pool, int index, de_stamp_t stamp)
{
	*((de_stamp_t*)(pool->records + pool->record_size * index)) = stamp;
}

/*=======================================================================================*/
static void* de_pool_internal_get_object(de_pool_t* pool, int index)
{
	return pool->records + pool->record_size * index + sizeof(de_stamp_t);
}

/*=======================================================================================*/
de_ref_t de_pool_spawn(de_pool_t* pool)
{
	void* obj;
	de_ref_t ref;
	if (pool->spawned_count >= pool->capacity)
	{
		size_t old_capacity = pool->capacity;
		pool->capacity = pool->capacity * 2 + 1;
		de_pool_realloc_memory(pool, old_capacity);
	}
	assert(pool->free_stack_top > 0);
	ref.index = pool->free_stack[--pool->free_stack_top];
	ref.stamp = pool->global_stamp++;
	obj = de_pool_internal_get_object(pool, ref.index);
	memset(obj, 0, pool->item_size);
	de_pool_internal_set_stamp(pool, ref.index, ref.stamp);
	++pool->spawned_count;
	return ref;
}

/*=======================================================================================*/
void de_pool_return(de_pool_t* pool, de_ref_t ref)
{
	if (de_pool_is_valid_ref(pool, ref))
	{
		if (de_pool_internal_get_stamp(pool, ref.index) != DE_POOL_STAMP_FREE)
		{
			if (pool->destructor)
			{
				pool->destructor(de_pool_internal_get_object(pool, ref.index));
			}
			de_pool_internal_set_stamp(pool, ref.index, DE_POOL_STAMP_FREE);
			--pool->spawned_count;
			assert((unsigned)pool->free_stack_top < pool->capacity);
			pool->free_stack[pool->free_stack_top++] = ref.index;
		}
	}
}

/*=======================================================================================*/
void de_pool_clear(de_pool_t* pool)
{
	if (pool->destructor)
	{
		size_t i;
		for (i = 0; i < pool->capacity; ++i)
		{
			if (de_pool_internal_get_stamp(pool, i) != DE_POOL_STAMP_FREE)
			{
				pool->destructor(de_pool_internal_get_object(pool, i));
			}
		}
	}
	de_free(pool->records);
	de_free(pool->free_stack);
	pool->free_stack_top = 0;
	pool->records = NULL;
	pool->capacity = 0;
	pool->spawned_count = 0;
	pool->global_stamp = DE_POOL_STAMP_FREE + 1;
}

/*=======================================================================================*/
de_bool_t de_pool_is_valid_ref(de_pool_t* pool, de_ref_t ref)
{
	return (unsigned)ref.index < pool->capacity && de_pool_internal_get_stamp(pool, ref.index) == ref.stamp;
}

/*=======================================================================================*/
void* de_pool_get_ptr(de_pool_t* pool, de_ref_t ref)
{
	return de_pool_is_valid_ref(pool, ref) ? de_pool_internal_get_object(pool, ref.index) : NULL;
}