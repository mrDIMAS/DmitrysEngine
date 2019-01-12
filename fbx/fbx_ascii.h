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


typedef struct de_fbx_rdbuf_t
{
	char chunk[32768];
	size_t chunk_size;
	size_t chunk_read_cursor;
	bool eof;
} de_fbx_rdbuf_t;

void de_fbx_rdbuf_init(de_fbx_rdbuf_t* rdbuf);

char de_fbx_rdbuf_next_symbol(FILE* file, de_fbx_rdbuf_t* rdbuf);

bool de_fbx_rdbuf_is_eof(de_fbx_rdbuf_t* rdbuf);

/* custom isspace function, because standard implementation uses locale which is slow */
bool de_fbx_is_space(char c);

/**
 * @brief Reads ASCII FBX into node hierarchy and initializes data buffer.
 */
de_fbx_node_t* de_fbx_ascii_load_file(const char* filename, de_fbx_buffer_t* data_buf);

de_fbx_reference_t de_fbx_get_reference(const char* str);