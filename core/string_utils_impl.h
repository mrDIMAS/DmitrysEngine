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

#define MASKBITS 0x3F
#define MASKBYTE 0x80
#define MASK2BYTES 0xC0
#define MASK3BYTES 0xE0
#define MASK4BYTES 0xF0
#define MASK5BYTES 0xF8
#define MASK6BYTES 0xFC

int de_utf8_to_utf32(const char* inString, size_t in_str_len, uint32_t* out, int bufferSize) {
	size_t i;
	int n = 0;
	unsigned char * in = (unsigned char *)inString;

	for (i = 0; i < in_str_len; ) {
		uint32_t ch = 0;

		if ((in[i] & MASK6BYTES) == MASK6BYTES) {
			/* 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx */
			ch = ((in[i] & 0x01) << 30) | ((in[i + 1] & MASKBITS) << 24) | ((in[i + 2] & MASKBITS) << 18) | ((in[i + 3] & MASKBITS) << 12) | ((in[i + 4] & MASKBITS) << 6) | (in[i + 5] & MASKBITS);
			i += 6;
		} else if ((in[i] & MASK5BYTES) == MASK5BYTES) {
			/* 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx */
			ch = ((in[i] & 0x03) << 24) | ((in[i + 1] & MASKBITS) << 18) | ((in[i + 2] & MASKBITS) << 12) | ((in[i + 3] & MASKBITS) << 6) | (in[i + 4] & MASKBITS);
			i += 5;
		} else if ((in[i] & MASK4BYTES) == MASK4BYTES) {
			/* 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */
			ch = ((in[i] & 0x07) << 18) | ((in[i + 1] & MASKBITS) << 12) | ((in[i + 2] & MASKBITS) << 6) | (in[i + 3] & MASKBITS);
			i += 4;
		} else if ((in[i] & MASK3BYTES) == MASK3BYTES) {
			/* 1110xxxx 10xxxxxx 10xxxxxx */
			ch = ((in[i] & 0x0F) << 12) | ((in[i + 1] & MASKBITS) << 6) | (in[i + 2] & MASKBITS);
			i += 3;
		} else if ((in[i] & MASK2BYTES) == MASK2BYTES) {
			/* 110xxxxx 10xxxxxx */
			ch = ((in[i] & 0x1F) << 6) | (in[i + 1] & MASKBITS);
			i += 2;
		} else if (in[i] < MASKBYTE) {
			/* 0xxxxxxx */
			ch = in[i];
			i += 1;
		} else {
			return n;
		}

		out[n++] = ch;

		if (n >= bufferSize) {
			return n - 1;
		}
	}

	return n;
}

#undef MASKBITS
#undef MASKBYTE
#undef MASK2BYTES
#undef MASK3BYTES
#undef MASK4BYTES
#undef MASK5BYTES
#undef MASK6BYTES

char* de_str_format(const char* format, ...) {
	static char format_buffer[32768];
	va_list argument_list;
	va_start(argument_list, format);
	vsprintf(format_buffer, format, argument_list);
	va_end(argument_list);
	return format_buffer;
}