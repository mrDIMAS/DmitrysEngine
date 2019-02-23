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

void de_str32_init(de_str32_t* str) {
	DE_ARRAY_INIT(str->str);
}

void de_str32_free(de_str32_t* str) {
	DE_ARRAY_FREE(str->str);
}

void de_str32_move(de_str32_t* src, de_str32_t* dest) {
	DE_ARRAY_MOVE(src->str, dest->str);
}

void de_str32_insert(de_str32_t* str, int pos, uint32_t unicode) {
	DE_ARRAY_INSERT(str->str, pos, unicode);	
}

void de_str32_append(de_str32_t* str, uint32_t unicode) {
	de_str32_insert(str, str->str.size, unicode);
}

void de_str32_clear(de_str32_t* str) {
	DE_ARRAY_CLEAR(str->str);
}

void de_str32_remove(de_str32_t* str, int pos, int amount) {
	uint32_t* data = str->str.data;
	int size = (int)str->str.size;
	pos = pos < 0 ? 0 : pos;
	amount = (pos + amount) >= size ? size - pos : amount;
	memmove(data + pos, data + pos + amount, sizeof(*data) * (size - (pos + amount)));
	str->str.size -= amount;
}

size_t de_str32_length(de_str32_t* str) {
	return str->str.size;
}

const uint32_t* de_str32_get_data(const de_str32_t* str) {
	return str->str.data;
}

uint32_t de_str32_at(const de_str32_t* str, size_t i) {
	return str->str.data[i];
}

void de_str32_set_utf8(de_str32_t* str, const de_str8_view_t* view) {
	int reserved_size = view->len * 4;
	de_str32_clear(str);
	DE_ARRAY_GROW(str->str, reserved_size);
	str->str.size = (size_t)de_utf8_to_utf32(view->data, view->len, str->str.data, reserved_size);
}

void de_str32_append_utf8(de_str32_t* str, const de_str8_view_t* view) {
	size_t old_size, decoded;
	int reserved_size = view->len * 4;
	old_size = str->str.size;
	DE_ARRAY_GROW(str->str, reserved_size);
	decoded = (size_t)de_utf8_to_utf32(view->data, view->len, str->str.data + old_size, reserved_size);
	str->str.size = old_size + decoded;
}

void de_str32_append_cstr(de_str32_t* str, const char* cstr) {
	de_str8_view_t view;
	de_str8_view_set_cstr(&view, cstr);
	de_str32_append_utf8(str, &view);
}