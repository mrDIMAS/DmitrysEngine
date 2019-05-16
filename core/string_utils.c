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

int de_utf8_to_utf32(const char* in, size_t in_length, uint32_t* out, int out_length)
{
	int n = 0;

	for (size_t i = 0; i < in_length; ) {
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

		if (n >= out_length) {
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

int de_utf8_to_utf16(const char* in, int in_length, uint16_t* out, int out_length)
{
	uint16_t* out_start = out;
	const char* in_end = in + in_length;
	uint16_t* out_end = out + (out_length / 2);
	while (in < in_end) {
		int trailing;
		uint32_t c;
		uint32_t d = *in++;
		if (d < 0x80) {
			c = d;
			trailing = 0;
		} else if (d < 0xC0) {
			/* Trailing byte in leading position */
			return -2;
		} else if (d < 0xE0) {
			c = d & 0x1F;
			trailing = 1;
		} else if (d < 0xF0) {
			c = d & 0x0F;
			trailing = 2;
		} else if (d < 0xF8) {
			c = d & 0x07;
			trailing = 3;
		} else {
			/* Not valid for UTF-16 */
			return -2;
		}

		if (in_end - in < trailing) {
			break;
		}

		for (; trailing; trailing--) {
			if ((in >= in_end) || (((d = *in++) & 0xC0) != 0x80)) {
				break;
			}
			c <<= 6;
			c |= d & 0x3F;
		}

		if (c < 0x10000) {
			if (out >= out_end) {
				break;
			}
			*out++ = (uint16_t)c;
		} else if (c < 0x110000) {
			if (out + 1 >= out_end) {
				break;
			}
			c -= 0x10000;
			*out++ = (uint16_t)(0xD800 | (c >> 10));
			*out++ = (uint16_t)(0xDC00 | (c & 0x03FF));
		} else {
			break;
		}
	}

	return (ptrdiff_t)(out - out_start);
}

int de_utf16_to_utf32(const uint16_t* in, size_t in_length, uint32_t* out, int out_length)
{
	const uint16_t* begin = in;
	const uint16_t* end = in + in_length;
	const uint32_t* out_end = out + out_length;
	while (in < end) {
		const uint16_t uc = *in++;
		if ((uc - 0xd800u) < 2048u) {
			if ((uc & 0xfffffc00) == 0xd800 && in < end && (*in & 0xfffffc00) == 0xdc00) {
				if (out + 1 >= out_end) {
					break;
				}
				*out++ = (uc << 10) + *in - 0x35fdc00;
				++in;
			} else {
				return 0;
			}
		} else {
			if(out + 1 >= out_end) {
				break;
			}
			*out++ = uc;
		}
	}
	return (ptrdiff_t)(in - begin);
}