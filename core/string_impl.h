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

void de_tokenize_string(const char* str, de_string_array_t* tokens, const char* delim) {
	static char buffer[1024];
	char* token;
	strcpy(buffer, str);
	DE_ARRAY_CLEAR(*tokens);
	token = strtok(buffer, delim);
	while (token) {
		DE_ARRAY_APPEND(*tokens, token);
		token = strtok(NULL, delim);
	}
}

void de_str8_init(de_str8_t* str) {
	DE_ARRAY_INIT(str->str);
	DE_ARRAY_APPEND(str->str, '\0');
}

void de_str8_set(de_str8_t* str, const char* cstr) {
	size_t len;
	de_str8_free(str);
	len = strlen(cstr);
	DE_ARRAY_GROW(str->str, len);
	memcpy(str->str.data, cstr, len);
	DE_ARRAY_APPEND(str->str, '\0');
}

void de_str8_move(de_str8_t* src, de_str8_t* dest) {
	DE_ARRAY_MOVE(src->str, dest->str);
}

void de_str8_free(de_str8_t * str) {
	DE_ARRAY_FREE(str->str);
}

size_t de_str8_length(const de_str8_t* str) {
	/* array length without null-terminator */
	return str->str.size ? str->str.size - 1 : 0;
}

bool de_str8_eq(de_str8_t* str, const char* utf8str) {
	if (!str->str.data || !utf8str) {
		return false;
	}
	return strcmp(str->str.data, utf8str) == 0;
}

bool de_str8_eq_str8(de_str8_t* str, de_str8_t* other) {
	return strcmp(str->str.data, other->str.data) == 0;
}

const char* de_str8_cstr(const de_str8_t* str) {
	return str->str.data;
}

size_t de_str8_fread(de_str8_t* str, FILE* file, size_t len) {
	size_t read;
	if (len == 0) {
		char c;
		read = 0;
		while (1) {
			fread(&c, 1, 1, file);
			if (c) {
				DE_ARRAY_APPEND(str->str, c);
				++read;
			} else {
				break;
			}
		}
	} else {
		de_str8_free(str);
		DE_ARRAY_GROW(str->str, len);
		read = fread(str->str.data, 1, len, file);
		DE_ARRAY_APPEND(str->str, '\0');
	}
	return read;
}

/**
* @brief Creates copy of string on heap. You have to call @de_free to delete string
* @param src string
* @return copy of string
*/
void de_str8_copy(const de_str8_t* src, de_str8_t* dest) {
	size_t len;
	DE_ASSERT(src);
	DE_ASSERT(dest);
	de_str8_free(dest);
	len = de_str8_length(src);
	DE_ARRAY_GROW(dest->str, len);
	memcpy(dest->str.data, src->str.data, len);
	DE_ARRAY_APPEND(dest->str, '\0');
}