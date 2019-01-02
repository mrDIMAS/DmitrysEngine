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

char* de_base64_encode(const void* data, size_t size, size_t* out_size)
{
	size_t i, k;
	char* bytes;
	char* buffer;
	int padding;
	size_t encoded_size;

	static const uint8_t encoding_table[] =
	{
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
		'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
		'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
		'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
		'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
		'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
		'w', 'x', 'y', 'z', '0', '1', '2', '3',
		'4', '5', '6', '7', '8', '9', '+', '/'
	};

	encoded_size = 4 * ((size + 2) / 3);

	buffer = (char*)de_malloc(encoded_size + 1);

	bytes = (char*)data;

	for (i = 0, k = 0; i < size; )
	{
		uint8_t b0 = i < size ? bytes[i++] : 0;
		uint8_t b1 = i < size ? bytes[i++] : 0;
		uint8_t b2 = i < size ? bytes[i++] : 0;

		buffer[k++] = encoding_table[b0 >> 2];
		buffer[k++] = encoding_table[((b0 & 0x03) << 4) | (b1 >> 4)];
		buffer[k++] = encoding_table[((b1 & 0x0F) << 2) | (b2 >> 6)];
		buffer[k++] = encoding_table[b2 & 0x3F];
	}

	padding = size % 3;

	if (padding == 1)
	{
		buffer[k - 2] = '=';
		buffer[k - 1] = '=';
	}
	else if (padding == 2)
	{
		buffer[k - 1] = '=';
	}

	buffer[k] = '\0';

	if (out_size)
	{
		*out_size = encoded_size;
	}

	return buffer;
}

void* de_base64_decode(const char* data, size_t size, size_t* out_size)
{
	char* bytes;
	size_t i, k;
	size_t data_size;
	static const uint8_t decode_table[] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 62, 0, 0, 0, 63, 52, 53,
		54, 55, 56, 57, 58, 59, 60, 61, 0, 0,
		0, 0, 0, 0, 0, 0, 1, 2, 3, 4,
		5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
		15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
		25, 0, 0, 0, 0, 0, 0, 26, 27, 28,
		29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
		39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
		49, 50, 51, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0,
	};

	data_size = size / 4 * 3;

	if (data[size - 1] == '=')
	{
		--data_size;
	}

	if (data[size - 2] == '=')
	{
		--data_size;
	}

	bytes = (char*)de_malloc(data_size);

	for (i = 0, k = 0; i < size; )
	{
		uint8_t b0 = data[i] == '=' ? 0 : decode_table[data[i]];
		++i;

		uint8_t b1 = data[i] == '=' ? 0 : decode_table[data[i]];
		++i;

		uint8_t b2 = data[i] == '=' ? 0 : decode_table[data[i]];
		++i;

		uint8_t b3 = data[i] == '=' ? 0 : decode_table[data[i]];
		++i;

		if (k < data_size)
		{
			bytes[k++] = (b0 << 2) | (b1 >> 4);
		}
		if (k < data_size)
		{
			bytes[k++] = (b1 << 4) | (b2 >> 2);
		}
		if (k < data_size)
		{
			bytes[k++] = (b2 << 6) | b3;
		}
	}

	*out_size = data_size;

	return bytes;
}

void de_base64_test(void)
{
	int i;
	const char* str[] = {
		"abc",
		"abcd",
		"abcde",
	};

	const char* cmp[] = {
		"YWJj",
		"YWJjZA==",
		"YWJjZGU=",
	};

	/* encode test */
	for (i = 0; i < 3; ++i)
	{
		size_t size = strlen(str[i]);
		char* buffer = de_base64_encode(str[i], size, NULL);
		assert(strcmp(buffer, cmp[i]) == 0);
		de_free(buffer);
	}

	/* decode test */
	for (i = 0; i < 3; ++i)
	{
		size_t decoded_size;
		void* decoded = de_base64_decode(cmp[i], strlen(cmp[i]), &decoded_size);
		assert(memcmp(decoded, str[i], decoded_size) == 0);
		de_free(decoded);
	}
}