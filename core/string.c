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

de_str8_view_array_t de_str8_tokenize(const de_str8_t* str, const char* delim) {
	size_t i;
	de_str8_view_t view;
	de_str8_view_array_t tokens;
	DE_ARRAY_INIT(tokens);
	view.data = str->str.data;
	view.len = 0;
	for (i = 0; i < str->str.size; ++i) {
		char c = str->str.data[i];
		if (strchr(delim, c)) {
			DE_ARRAY_APPEND(tokens, view);
			while (i < str->str.size) {
				if (strchr(delim, str->str.data[i])) {
					++i;
				} else {
					view.data = str->str.data + i;
					view.len = 0;
					break;
				}
			}
		}
		++view.len;
	}
	/* move array */
	return tokens;
}

void de_str8_format(de_str8_t* str, const char* format, ...) {
	int n;
	va_list argument_list;
	de_str8_clear(str);
	/* count how much memory is needed */
	va_start(argument_list, format);
	n = vsnprintf(0, 0, format, argument_list);
	if (n < 0) {
		return;
	}
	va_end(argument_list);
	/* reserve */
	DE_ARRAY_GROW(str->str, n);
	/* print */
	va_start(argument_list, format);
	vsnprintf(str->str.data, str->str.size, format, argument_list);
	va_end(argument_list);
}

void de_str8_view_set(de_str8_view_t* view, const char* data, size_t len) {
	view->data = data;
	view->len = len;
}

void de_str8_view_set_cstr(de_str8_view_t* view, const char* data) {
	view->data = data;
	view->len = strlen(data);
}

void de_str8_view_print(de_str8_view_t* view) {
	size_t i;
	for (i = 0; i < view->len; ++i) {
		putchar(view->data[i]);
	}
}

void de_str8_view_to_string(const de_str8_view_t* view, de_str8_t* str) {
	de_str8_clear(str);
	de_str8_append_str_view(str, view);
}

bool de_str8_view_eq_cstr(const de_str8_view_t* view, const char* utf8str) {
	if (!view || !utf8str || !view->data) {
		return false;
	}
	return strncmp(view->data, utf8str, view->len) == 0;
}

bool de_str8_view_eq_str8(const de_str8_view_t* view, const de_str8_t* str) {
    size_t count;
	if (!view || !str || !str->str.data || !view->data) {
		return false;
	}
	count = view->len > str->str.size ? str->str.size : view->len;
	return strncmp(view->data, str->str.data, count);
}

bool de_str8_view_eq(const de_str8_view_t* view, const de_str8_view_t* other) {
    size_t count;
	if (!view || !other || !other->data || !view->data) {
		return false;
	}
	count = view->len > other->len ? other->len : view->len;
	return strncmp(view->data, other->data, count);
}

void de_str8_init(de_str8_t* str) {
	if (!str) {
		return;
	}
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

void de_str8_clear(de_str8_t* str) {
	if (str->str.data) {
		str->str.data[0] = 0;
	}
	str->str.size = 1;
}

void de_str8_move(de_str8_t* src, de_str8_t* dest) {
	DE_ARRAY_MOVE(src->str, dest->str);
}

void de_str8_append_cstr(de_str8_t* str, const char* utf8str) {
	de_str8_view_t view;
	view.data = utf8str;
	view.len = strlen(utf8str);
	de_str8_append_str_view(str, &view);
}

void de_str8_append_str8(de_str8_t* str, const de_str8_t* other) {
	de_str8_append_cstr(str, other->str.data);
}

void de_str8_free(de_str8_t * str) {
	DE_ARRAY_FREE(str->str);
}

void de_str8_append_str_view(de_str8_t* str, const de_str8_view_t* view) {
    size_t i;
	if (!view->data || view->len == 0) {
		return;
	}
	i = str->str.size ? str->str.size - 1 : 0;
	DE_ARRAY_GROW(str->str, view->len - 1);
	memcpy(str->str.data + i, view->data, view->len);
	DE_ARRAY_APPEND(str->str, '\0');
}

size_t de_str8_length(const de_str8_t* str) {
	/* array length without null-terminator */
	return str->str.size ? str->str.size - 1 : 0;
}

bool de_str8_eq(const de_str8_t* str, const char* utf8str) {
	if (!str->str.data || !utf8str) {
		return false;
	}
	return strcmp(str->str.data, utf8str) == 0;
}

bool de_str8_eq_str8(const de_str8_t* str, const de_str8_t* other) {
	return strcmp(str->str.data, other->str.data) == 0;
}

const char* de_str8_cstr(const de_str8_t* str) {
	return str->str.data;
}

void de_str8_find(const de_str8_t* str, size_t offset, const de_str8_view_t* substr, de_str8_view_t* out_view) {
	size_t i, k, n, len;
	len = de_str8_length(str);
	for (i = offset; i < len; ++i) {
		bool is_substr = true;
		for (k = i, n = 0; k < len && n < substr->len; ++k, ++n) {
			char a = str->str.data[k];
			char b = substr->data[n];
			if (a != b) {
				is_substr = false;
				break;
			}
		}
		if (is_substr && n == substr->len) {
			out_view->data = str->str.data + i;
			out_view->len = substr->len;
			return;
		} 
	}
	out_view->data = NULL;
	out_view->len = 0;
}

void de_str8_substr(const de_str8_t* str, de_str8_view_t* view, size_t offset, size_t count) {
	size_t len = de_str8_length(str);
	if (len == 0) {
		view->data = NULL;
		view->len = 0;
	} else {
		if (offset >= len) {
			view->data = str->str.data + (len - 1);
			view->len = 0;
		} else {
			view->data = str->str.data + offset;
			if (offset + count >= len) {
				view->len = len - offset;
			} else {
				view->len = count;
			}
		}
	}
}

void de_str8_replace(de_str8_t* str, const de_str8_view_t* what, const de_str8_view_t* with) {
	char* s;
	size_t offset = 0;
	while (1) {
		de_str8_view_t substr;
		de_str8_find(str, offset, what, &substr);
		if (!substr.data) {
			break;
		} else {
			int substr_index;
			s = (char*)substr.data;
			substr_index = (ptrdiff_t)(s - str->str.data);
			/* enough space */
			if (what->len > with->len) {				
				/* replace */
				memcpy(s, with->data, with->len);
				/* place rest near replacement */
				memmove(s + with->len, s + substr.len, str->str.size - substr_index);
			} else {
				/* not enough space, grow array */
				DE_ARRAY_GROW(str->str, with->len - what->len);
				/* move null-terminator */
				str->str.data[str->str.size - 1] = '\0'; 
				/* move to free place */
				memmove(s + with->len, s + substr.len, str->str.size - substr_index);
				/* replace */
				memcpy(s, with->data, with->len);				
			}			
			offset = substr_index + with->len;
		}
	}
}

size_t de_str8_fread(de_str8_t* str, FILE* file, size_t len) {
	size_t read;
	de_str8_free(str);
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
		DE_ARRAY_GROW(str->str, len);
		read = fread(str->str.data, 1, len, file);
	}
	DE_ARRAY_APPEND(str->str, '\0');
	return read;
}

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

void de_str8_tests(void) {
	size_t i;
	de_str8_t str;

	/* de_str8_tokenize */
	{
		de_str8_view_array_t tokens;
		de_str8_init(&str);
		de_str8_set(&str, "Some;;;;;;;;; ,,string,, ,with ,,some; ,,tokens");
		tokens = de_str8_tokenize(&str, " ,;");
		for (i = 0; i < tokens.size; ++i) {
			de_str8_view_t* view = tokens.data + i;
			de_str8_view_print(view);
		}
		de_str8_free(&str);
		DE_ARRAY_FREE(tokens);
	}

	/* de_str8_replace */
	{
		de_str8_view_t what, with;
		de_str8_init(&str);
		de_str8_view_set_cstr(&what, "some");
		{
			de_str8_set(&str, "some string with some tokens");
			de_str8_view_set_cstr(&with, "foobar");
			de_str8_replace(&str, &what, &with);
			printf("%s", de_str8_cstr(&str));
		}
		{
			de_str8_set(&str, "some string with some tokens some");
			de_str8_view_set_cstr(&with, "foo");
			de_str8_replace(&str, &what, &with);
			printf("%s", de_str8_cstr(&str));
		}

		de_str8_free(&str);
	}

	/* de_str8_format */
	{
		de_str8_init(&str);
		de_str8_format(&str, "Some numbers: %d %f foo", 123, 456.789);
		printf("%s", de_str8_cstr(&str));
		de_str8_free(&str);
	}
}