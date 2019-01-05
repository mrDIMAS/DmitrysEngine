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

static void de_fignore(FILE* f, size_t num)
{
	fseek(f, ftell(f) + num, SEEK_SET);
}

/**
 * @brief Small helper method to ensure that we have read all requested data.
 */
static de_bool_t de_fbx_fread(void* buffer, size_t size, FILE* f)
{
	return fread(buffer, 1, size, f) == size;
}

/**
 * Very inefficient for now. Every attribute allocates its own memory block. So even
 * to store 1 byte we doing de_malloc. TODO: Should be optimized!
 *
 * Perform reading using this specification:
 * https://code.blender.org/2013/08/fbx-binary-file-format-specification/
 **/
de_fbx_node_t* de_fbx_binary_read_node(de_fbx_buffer_t* data_buf, FILE* f)
{
	size_t i, k;
	char null_record[13];
	uint32_t null_record_position;
	uint32_t end_offset;
	uint32_t num_attrib;
	uint32_t attrib_list_len;
	uint8_t name_len;
	de_fbx_node_t* node;

	if (!de_fbx_fread(&end_offset, sizeof(end_offset), f))
	{
		de_log("FBX Binary: Unable to read end offset of a node!");

		return NULL;
	}

	/* Footer found. We're done */
	if (end_offset == 0)
	{
		return NULL;
	}

	node = DE_NEW(de_fbx_node_t);

	node->is_binary = DE_TRUE;

	if (!de_fbx_fread(&num_attrib, sizeof(num_attrib), f))
	{
		de_log("FBX Binary: Unable to read attribute count of a node!");

		return NULL;
	}

	if (!de_fbx_fread(&attrib_list_len, sizeof(attrib_list_len), f))
	{
		de_log("FBX Binary: Unable to read attribute list length of a node!");

		return NULL;
	}

	if (!de_fbx_fread(&name_len, sizeof(name_len), f))
	{
		de_log("FBX Binary: Unable to read name length of a node!");

		return NULL;
	}

	node->name = (char*)de_malloc(name_len + 1);
	if (!de_fbx_fread(node->name, name_len, f))
	{
		de_log("FBX Binary: Unable to read name of node!");

		return NULL;
	}
	node->name[name_len] = '\0';

	for (i = 0; i < num_attrib; ++i)
	{
		char type_code;
		size_t size = 0;

		fread(&type_code, sizeof(char), 1, f);

		/* Simple types */
		switch (type_code)
		{
		case 'Y':
			size = 2;
			break;
		case 'C':
			size = 1;
			break;
		case 'I':
			size = 4;
			break;
		case 'F':
			size = 4;
			break;
		case 'D':
			size = 8;
			break;
		case 'L':
			size = 8;
			break;
		default:
			break;
		}

		/* We have a simple type */
		if (size)
		{
			void* attrib = de_fbx_buffer_alloc(data_buf, size);
			if (!de_fbx_fread(attrib, size, f))
			{
				de_log("FBX Binary: Unable to read property data!");

				de_free(attrib);

				return NULL;
			}
			DE_ARRAY_APPEND(node->attributes, attrib);
		}
		else
		{
			/* Arrays */
			switch (type_code)
			{
			case 'f':
				size = 4;
				break;
			case 'd':
				size = 8;
				break;
			case 'l':
				size = 8;
				break;
			case 'i':
				size = 4;
				break;
			case 'b':
				size = 1;
				break;
			default:
				break;
			}

			/* We have an array */
			if (size)
			{
				uint32_t length;
				uint32_t encoding;
				uint32_t compressed_length;
				char* raw_array = NULL;

				if (!de_fbx_fread(&length, sizeof(length), f))
				{
					de_log("FBX Binary: Unable to read array length!");

					return NULL;
				}

				if (!de_fbx_fread(&encoding, sizeof(encoding), f))
				{
					de_log("FBX Binary: Unable to read array encoding!");

					return NULL;
				}

				if (!de_fbx_fread(&compressed_length, sizeof(compressed_length), f))
				{
					de_log("FBX Binary: Unable to read array compressed length!");

					return NULL;
				}

				if (encoding == 0)
				{
					size_t total_length = size * length;

					raw_array = de_malloc(total_length);
					if (!de_fbx_fread(raw_array, total_length, f))
					{
						de_log("FBX Binary: Unable to read plain array data!");

						de_free(raw_array);

						return NULL;
					}
				}
				else
				{
					void* compressed;
					size_t decompressed_length;

					compressed = de_malloc(compressed_length);
					if (!de_fbx_fread(compressed, compressed_length, f))
					{
						de_log("FBX Binary: Unable to read compressed array data!");

						de_free(compressed);

						return NULL;
					}

					raw_array = tinfl_decompress_mem_to_heap(compressed, compressed_length, &decompressed_length, TINFL_FLAG_PARSE_ZLIB_HEADER);
					if (!raw_array)
					{
						de_free(compressed);

						de_log("FBX Binary: Unable to decompress array data!");

						return NULL;
					}

					de_free(compressed);
				}

				/* Here we should add child node called "a" to make binary format compatible with ASCII-designed
				 * converter and unpack array into set of free-standing properties - we won't lose any performance
				 * because of custom fixed-size-buffer allocator */
				if (raw_array)
				{
					size_t n;
					de_fbx_node_t* a_node = DE_NEW(de_fbx_node_t);
					a_node->name = de_str_copy("a");
					a_node->parent = node;
					a_node->is_binary = DE_TRUE;
					for (k = 0, n = 0; k < length; ++k, n += size)
					{
						void* item = de_fbx_buffer_alloc(data_buf, size);
						memcpy(item, raw_array + n, size);
						DE_ARRAY_APPEND(a_node->attributes, item);
					}
					DE_ARRAY_APPEND(node->children, a_node);
				}

				de_free(raw_array);
			}
			else
			{
				/* We have a string (or raw data) */
				if (type_code == 'S' || type_code == 'R')
				{
					char* str;
					uint32_t length;
					uint32_t buffer_len;

					if (!de_fbx_fread(&length, sizeof(length), f))
					{
						de_log("FBX Binary: Unable to read string/raw length!");

						return NULL;
					}

					/* Strings or raw data will always contain null terminator */
					buffer_len = length + 1;

					str = de_fbx_buffer_alloc(data_buf, buffer_len);
					if (!de_fbx_fread(str, length, f))
					{
						de_log("FBX Binary: Unable to read string/raw data!");

						de_free(str);

						return NULL;
					}

					/* Make sure to add null terminator for strings */
					str[length] = '\0';

					DE_ARRAY_APPEND(node->attributes, str);
				}
				else
				{
					size_t pos = ftell(f);

					de_log("FBX Binary: corrupt data at pos %d", pos);

					return NULL;
				}
			}
		}
	}

	null_record_position = end_offset - 13;

	if (ftell(f) < end_offset)
	{
		de_bool_t null_record_correct = DE_TRUE;

		while (ftell(f) < null_record_position)
		{
			de_fbx_node_t* child = de_fbx_binary_read_node(data_buf, f);
			if (child == NULL)
			{
				return NULL;
			}
			child->parent = node;
			DE_ARRAY_APPEND(node->children, child);
		}

		/* Check if we have a null-record */
		fread(null_record, sizeof(char), sizeof(null_record), f);
		for (i = 0; i < sizeof(null_record); ++i)
		{
			if (null_record[i] != 0)
			{
				null_record_correct = DE_FALSE;
				break;
			}
		}

		if (!null_record_correct)
		{
			de_log("FBX: Binary - invalid null record!");

			return NULL;
		}
	}

	return node;
}

/**
 * You must check if file is binary before call this function!
 */
de_fbx_node_t* de_fbx_binary_load_file(const char* filename, de_fbx_buffer_t* data_buf)
{
	de_fbx_node_t* root;
	size_t total_length;
	unsigned int version;
	FILE* file = fopen(filename, "rb");

	if (!file)
	{
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	total_length = ftell(file);
	fseek(file, 0, SEEK_SET);

	de_fbx_buffer_init(data_buf, total_length * 5);

	/* Ignore all stuff until version */
	de_fignore(file, 23);

	/* read version */
	fread(&version, sizeof(char), 4, file);

	if (version < DE_FBX_VERSION_MIN || version > DE_FBX_VERSION_MAX)
	{
		de_log("FBX: Unable to load %s - unsupported version %d. Supported %d to %d", file, version, DE_FBX_VERSION_MIN, DE_FBX_VERSION_MAX);
		fclose(file);
		return NULL;
	}

	root = de_fbx_create_node("__ROOT__");

	while (ftell(file) < total_length)
	{
		de_fbx_node_t* root_child;

		root_child = de_fbx_binary_read_node(data_buf, file);
		if (!root_child)
		{
			break;
		}
		root_child->parent = root;
		DE_ARRAY_APPEND(root->children, root_child);
	}

	return root;
}