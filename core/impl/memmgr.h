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

static int de_alloc_count;

void* de_malloc(size_t size)
{
	void* mem;

	mem = malloc(size);

	if (!mem)
	{
		de_error("Failed to allocate %d bytes of memory!", size);
	}

	++de_alloc_count;

	return mem;
}

void* de_calloc(size_t count, size_t size)
{
	void* mem;

	mem = calloc(count, size);

	if (!mem)
	{
		de_error("Failed to allocate %d bytes of clean memory!", count * size);
	}

	++de_alloc_count;

	return mem;
}

void* de_realloc(void* ptr, size_t size)
{
	void* mem;

	if (ptr == NULL && size > 0)
	{
		++de_alloc_count;
	}

	mem = realloc(ptr, size);

	if (size != 0)
	{
		if (!mem)
		{
			de_error("Failed to reallocate %d bytes of memory!", size);
		}
	}
	else
	{
		--de_alloc_count;
	}

	return mem;
}

void de_free(void* ptr)
{
	if (ptr)
	{
		--de_alloc_count;
	}
	free(ptr);
}

size_t de_get_alloc_count()
{
	return de_alloc_count;
}

void de_zero(void* data, size_t size)
{
	memset(data, 0, size);
}