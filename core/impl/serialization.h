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
#define SERIALIZE_TYPE_AND_RETURN(s, data) \
  if (s->write) { \
    fwrite(&data, 1, sizeof(data), s->file); \
  } else { \
    fread(&data, 1, sizeof(data), s->file); \
  } \
  return data; \

/*=======================================================================================*/
de_serializer_t * de_serializer_create(const char * filename, bool write)
{
	de_serializer_t * s = DE_NEW(de_serializer_t);

	s->file = fopen(filename, write ? "wb" : "rb");
	s->write = write;

	return s;
}

/*=======================================================================================*/
void de_serializer_free(de_serializer_t * s)
{
	fclose(s->file);
	DE_ARRAY_FREE(s->pointerPairs);
	de_free(s);
}

/*=======================================================================================*/
int de_serialize_int(de_serializer_t * s, int i)
{
	SERIALIZE_TYPE_AND_RETURN(s, i);
}

/*=======================================================================================*/
float de_serialize_float(de_serializer_t * s, float f)
{
	SERIALIZE_TYPE_AND_RETURN(s, f);
}

/*=======================================================================================*/
bool de_serialize_bool(de_serializer_t * s, bool b)
{
	SERIALIZE_TYPE_AND_RETURN(s, b);
}

/*=======================================================================================*/
short de_serialize_short(de_serializer_t * s, short shrt)
{
	SERIALIZE_TYPE_AND_RETURN(s, shrt);
}

/*=======================================================================================*/
unsigned char de_serialize_byte(de_serializer_t * s, unsigned char b)
{
	SERIALIZE_TYPE_AND_RETURN(s, b);
}

/*=======================================================================================*/
de_vec3_t de_serialize_vec3(de_serializer_t * s, de_vec3_t v)
{
	SERIALIZE_TYPE_AND_RETURN(s, v);
}

/*=======================================================================================*/
char * de_serialize_heap_string(de_serializer_t * s, char * str)
{
	if (s->write)
	{
		if (str)
		{
			fwrite(str, 1, strlen(str) + 1, s->file);
		}
		else
		{
			const char * dummy = "";
			fwrite(dummy, 1, 1, s->file);
		}
	}
	else
	{
		str = NULL;
		char symbol;
		int symbolCount = 0;
		do
		{
			fread(&symbol, 1, 1, s->file);
			str = (char*)realloc(str, ++symbolCount);
			str[symbolCount - 1] = symbol;
		} while (symbol != '\0');
	}
	return str;
}

/*=======================================================================================*/
void * de_serialize_pointer(de_serializer_t * s, void * p, int size)
{
	if (s->write)
	{
		// just reinterpret pointer as integer and dump to file
		fwrite(&p, 1, sizeof(p), s->file);
	}
	else
	{
		/* read old pointer */
		void * ptr;
		fread(&ptr, sizeof(void*), 1, s->file);

		/* got NULL, return and don't bother to create object */
		if (ptr == NULL)
		{
			return NULL;
		}

		/* check for existing pointer pair - "last-current" */
		de_pointer_pair_t * existing = NULL;
		de_bool_t alreadyExists = DE_FALSE;
		for (size_t i = 0; i < s->pointerPairs.size; ++i)
		{
			if (s->pointerPairs.data[i].last == (intptr_t)ptr)
			{
				alreadyExists = DE_TRUE;
				existing = &s->pointerPairs.data[i];
				break;
			}
		}

		if (alreadyExists)
		{
			/* object already created, return pointer to current object's location */
			p = existing->current;
		}
		else
		{
			de_pointer_pair_t* pointer_pair;

			/* object doesn't exist, create new one */
			if (size)
			{
				p = de_malloc(size);
			}
			else
			{
				p = NULL;
			}

			/* add pointer pair to list */
			DE_ARRAY_GROW(s->pointerPairs, 1);
			pointer_pair = &DE_ARRAY_LAST(s->pointerPairs);
			pointer_pair->current = p;
			pointer_pair->last = (intptr_t)ptr;
		}
	}
	return p;
}

/*=======================================================================================*/
void * de_serialize_binary(de_serializer_t * s, size_t size, void * bin)
{
	size = de_serialize_int(s, size);
	if (s->write)
	{
		// dump to file
		fwrite(bin, 1, size, s->file);
	}
	else
	{
		// alloc and read
		bin = de_malloc(size);
		fread(bin, 1, size, s->file);
	}
	return bin;
}
