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

typedef struct de_pointer_pair_t
{
	intptr_t last;
	void * current;
} de_pointer_pair_t;

typedef struct de_serializer_t
{
	FILE * file;
	bool write;
	DE_ARRAY_DECLARE(de_pointer_pair_t, pointerPairs);
} de_serializer_t;

de_serializer_t * de_serializer_create(const char * filename, bool write);
void de_serializer_free(de_serializer_t * s);

// typical usage:
//  value = serialize_xxx(serializer, value);
// for pointers use macro SERIALIZE_POINTER
// for arrays use macro SERIALIZE_ARRAY
int de_serialize_int(de_serializer_t * s, int i);
float de_serialize_float(de_serializer_t * s, float f);
bool de_serialize_bool(de_serializer_t * s, bool b);
short de_serialize_short(de_serializer_t * s, short shrt);
unsigned char de_serialize_byte(de_serializer_t * s, unsigned char b);
de_vec3_t de_serialize_vec3(de_serializer_t * s, de_vec3_t v);
char * de_serialize_heap_string(de_serializer_t * s, char * str);
void * de_serialize_pointer(de_serializer_t * s, void * p, int size);
void * de_serialize_binary(de_serializer_t * s, size_t size, void * bin);

// helpers
#define DE_SERIALIZE_ARRAY(s, array, count) de_serialize_pointer(s, array, count * sizeof(*array))
#define DE_SERIALIZE_POINTER(s, pointer) de_serialize_pointer(s, pointer, sizeof(*pointer))
