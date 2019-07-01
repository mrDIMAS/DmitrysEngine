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

void de_fbx_rdbuf_init(de_fbx_rdbuf_t* rdbuf)
{
	rdbuf->chunk_read_cursor = 0;
	rdbuf->chunk_size = 0;
	rdbuf->eof = false;
}

char de_fbx_rdbuf_next_symbol(FILE* file, de_fbx_rdbuf_t* rdbuf)
{
	if (rdbuf->chunk_read_cursor >= rdbuf->chunk_size) {
		rdbuf->chunk_size = fread(rdbuf->chunk, sizeof(char), sizeof(rdbuf->chunk), file);
		rdbuf->chunk_read_cursor = 0;

		if (rdbuf->chunk_size != sizeof(rdbuf->chunk)) {
			rdbuf->eof = 1;
		}
	}
	return rdbuf->chunk[rdbuf->chunk_read_cursor++];
}

bool de_fbx_rdbuf_is_eof(de_fbx_rdbuf_t* rdbuf)
{
	return rdbuf->eof && rdbuf->chunk_read_cursor >= rdbuf->chunk_size;
}

bool de_fbx_is_space(char c)
{
	return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v';
}

de_fbx_node_t* de_fbx_ascii_load_file(const char* filename, de_fbx_buffer_t* data_buf)
{
	int line_size = 0;
	char buffer[32768];
	de_fbx_rdbuf_t rdbuf;
	char name[512];
	int name_length = 0;
	char value[512];
	int value_length = 0;	
	de_fbx_node_t* node = NULL;	
	int i;

	FILE* file = fopen(filename, "rb");

	if (!file) {
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	de_fbx_buffer_init(data_buf, ftell(file));
	fseek(file, 0, SEEK_SET);

	de_fbx_node_t* root = de_fbx_create_node("__ROOT__");
	de_fbx_node_t* parent = root;

	de_fbx_rdbuf_init(&rdbuf);

	/* Read line by line */
	while (!de_fbx_rdbuf_is_eof(&rdbuf)) {
		/* Read line, trim spaces (but leave spaces in quotes) */
		line_size = 0;
		bool read_all = false;
		for (;;) {
			char symbol = de_fbx_rdbuf_next_symbol(file, &rdbuf);

			if (de_fbx_rdbuf_is_eof(&rdbuf)) {
				break;
			}

			if (symbol == '\n') {
				break;
			} else if (symbol == '"') {
				read_all = !read_all;
			} else {
				if (read_all || !de_fbx_is_space(symbol)) {
					buffer[line_size++] = (char)symbol;
				}
			}
		}
		buffer[line_size] = '\0';

		/* Ignore comments */
		if (buffer[0] == ';') {
			continue;
		}

		if (line_size == 0) {
			continue;
		}

		/* Parse string */
		bool read_value = false;
		name_length = 0;
		for (i = 0; i < line_size; ++i) {
			char symbol = buffer[i];
			if (i == 0) {
				if (symbol == '-' || isdigit(symbol)) {
					read_value = true;
				}
			}
			if (symbol == ':' && !read_value) {
				read_value = true;
				name[name_length] = '\0';
				node = de_fbx_create_node(name);
				name_length = 0;
				if (parent) {
					node->parent = parent;
					DE_ARRAY_APPEND(parent->children, node);
				}
			} else if (symbol == '{') {
				parent = node;

				if (value_length) {
					char* attrib;
					value[value_length] = '\0';
					attrib = (char*)de_fbx_buffer_alloc(data_buf, value_length + 1);
					memcpy(attrib, value, value_length + 1);
					DE_ARRAY_APPEND(node->attributes, attrib);
					value_length = 0;
				}
			} else if (symbol == '}') {
				parent = parent->parent;
			} else if (symbol == ',' || (i == line_size - 1)) {
				char* attrib;
				if (symbol != ',') {
					value[value_length++] = symbol;
				}
				value[value_length] = '\0';
				attrib = (char*)de_fbx_buffer_alloc(data_buf, value_length + 1);
				memcpy(attrib, value, value_length + 1);
				DE_ARRAY_APPEND(node->attributes, attrib);
				value_length = 0;
			} else {
				if (!read_value) {
					name[name_length++] = symbol;
				} else {
					value[value_length++] = symbol;
				}
			}
		}

		read_value = false;
	}

#if DE_FBX_VERBOSE
	de_log("FBX: %s is parsed!", filename);
#endif

	return root;
}

de_fbx_reference_t de_fbx_get_reference(const char* str)
{
	if (strcmp(str, "Direct") == 0) {
		return DE_FBX_REFERENCE_DIRECT;
	} else if (strcmp(str, "IndexToDirect") == 0 || strcmp(str, "Index") == 0) {
		return DE_FBX_REFERENCE_INDEX_TO_DIRECT;
	}
	return DE_FBX_REFERENCE_UNKNOWN;
}