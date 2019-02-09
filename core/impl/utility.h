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

char * de_load_file_into_memory(const char * path, size_t* out_size) {
	size_t file_size;
	size_t content_size;
	char* content;
	FILE* file;

	/* try to open file */
	file = fopen(path, "rb");
	if (!file) {
		de_fatal_error("unable to read file: %s", path);
	}

	/* get file size */
	fseek(file, 0, SEEK_END);
	file_size = ftell(file);
	content_size = file_size + 1;
	fseek(file, 0, SEEK_SET);

	/* read file */
	content = (char*)de_malloc(content_size);
	if (fread(content, sizeof(char), file_size, file) != file_size) {
		de_fatal_error("file %s is corrupted", path);
	}

	if (out_size) {
		*out_size = file_size;
	}

	/* close file and write trailing zero at end of content array */
	fclose(file);
	content[file_size] = '\0';

	return content;
}


void de_convert_to_c_array(const char* source, const char* dest) {
	size_t i;
	size_t size;
	FILE* out;
	unsigned char* data;

	data = (unsigned char*)de_load_file_into_memory(source, &size);

	out = fopen(dest, "w");

	fprintf(out, "static const unsigned char array[] = {\n");
	for (i = 0; i < size; ++i) {
		fprintf(out, "%u, ", data[i]);

		if (i % 20 == 0) {
			fprintf(out, "\n\t");
		}
	}

	fprintf(out, "\n};");

	de_free(data);
	fclose(out);
}

/**
 * @brief Extracts only name from file path without extension.
 *
 * Example: "baz/bar/foo.bar" path will result in "foo" string.
 */
void de_file_extract_name(const char* path, char* buffer, size_t buffer_size) {
	size_t i;
	size_t size_limit = buffer_size - 1;
	const char* dot_pos, *slash_pos;
	const char *c;

	if (!path || !buffer || buffer_size == 0) {
		return;
	}

	/* look for slash first */
	slash_pos = strrchr(path, '/');
	if (!slash_pos) {
		/* look for windows-style slashes */
		slash_pos = strrchr(path, '\\');

		if (!slash_pos) {
			/* no slashes, so only filename present */
			slash_pos = path;
		}
	}

	/* look for dot */
	dot_pos = strrchr(path, '.');
	if (!dot_pos) {
		/* no extension - just copy full name */
		size_t count, length;

		length = strlen(slash_pos);
		count = length < buffer_size ? length : buffer_size;

		strncpy(buffer, slash_pos, count);
	} else {
		/* copy everything between slash and dot */
		c = path;
		i = 0;
		while (c < dot_pos) {
			if (i >= size_limit) {
				break;
			}
			buffer[i++] = *c;
			++c;
		}
		buffer[i] = '\0';
	}
}

/**
 * @brief Extracts extension from file path with dot.
 *
 * Example: "baz/bar/foo.bar" will result in ".bar" string.
 */
void de_file_extract_extension(const char* path, char* buffer, size_t buffer_size) {
	const char* dot_pos;

	dot_pos = strrchr(path, '.');

	if (dot_pos) {
		size_t count, length;

		length = strlen(dot_pos);
		count = length < buffer_size ? length : buffer_size;

		strncpy(buffer, dot_pos, count);
		buffer[count] = '\0';
	}
}

/**
 * @brief Tests for file system utilities.
 */
void de_file_tests(void) {

}