void de_str_init(de_string_t* str)
{
	DE_ARRAY_INIT(*str);
	DE_ARRAY_APPEND(*str, '\0');
}

void de_str_from_utf8(de_string_t * str, const char * cstr)
{
	size_t i;
	size_t len;
	if (!str->data)
	{
		DE_ARRAY_INIT(*str);
	}
	DE_ARRAY_CLEAR(*str);
	len = strlen(cstr);
	for (i = 0; i < len; ++i)
	{
		DE_ARRAY_APPEND(*str, cstr[i]);
	}
	DE_ARRAY_APPEND(*str, '\0');
}

void de_str_free(de_string_t * str)
{
	DE_ARRAY_FREE(*str);
}

char* de_str_copy(const char* src)
{
	char* str;
	char* p;
	int len = 0;

	while (src[len])
	{
		len++;
	}
	str = (char*)de_malloc(len + 1);
	p = str;
	while (*src)
	{
		*p++ = *src++;
	}
	*p = '\0';
	return str;
}

char* de_str_format(const char* format, ...)
{
	static char format_buffer[32768];
	va_list argument_list;
	va_start(argument_list, format);
	vsprintf(format_buffer, format, argument_list);
	va_end(argument_list);
	return format_buffer;
}

#define MASKBITS 0x3F
#define MASKBYTE 0x80
#define MASK2BYTES 0xC0
#define MASK3BYTES 0xE0
#define MASK4BYTES 0xF0
#define MASK5BYTES 0xF8
#define MASK6BYTES 0xFC

int de_utf8_to_utf32(const char* inString, uint32_t* out, int bufferSize)
{
	int i = 0;
	int n = 0;
	unsigned char * in = (unsigned char *)inString;
	int size = strlen(inString);

	for (i = 0; i < size; )
	{
		uint32_t ch = 0;

		if ((in[i] & MASK6BYTES) == MASK6BYTES)
		{
			/* 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx */
			ch = ((in[i] & 0x01) << 30) | ((in[i + 1] & MASKBITS) << 24) | ((in[i + 2] & MASKBITS) << 18) | ((in[i + 3] & MASKBITS) << 12) | ((in[i + 4] & MASKBITS) << 6) | (in[i + 5] & MASKBITS);
			i += 6;
		}
		else if ((in[i] & MASK5BYTES) == MASK5BYTES)
		{
			/* 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx */
			ch = ((in[i] & 0x03) << 24) | ((in[i + 1] & MASKBITS) << 18) | ((in[i + 2] & MASKBITS) << 12) | ((in[i + 3] & MASKBITS) << 6) | (in[i + 4] & MASKBITS);
			i += 5;
		}
		else if ((in[i] & MASK4BYTES) == MASK4BYTES)
		{
			/* 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */
			ch = ((in[i] & 0x07) << 18) | ((in[i + 1] & MASKBITS) << 12) | ((in[i + 2] & MASKBITS) << 6) | (in[i + 3] & MASKBITS);
			i += 4;
		}
		else if ((in[i] & MASK3BYTES) == MASK3BYTES)
		{
			/* 1110xxxx 10xxxxxx 10xxxxxx */
			ch = ((in[i] & 0x0F) << 12) | ((in[i + 1] & MASKBITS) << 6) | (in[i + 2] & MASKBITS);
			i += 3;
		}
		else if ((in[i] & MASK2BYTES) == MASK2BYTES)
		{
			/* 110xxxxx 10xxxxxx */
			ch = ((in[i] & 0x1F) << 6) | (in[i + 1] & MASKBITS);
			i += 2;
		}
		else if (in[i] < MASKBYTE)
		{
			/* 0xxxxxxx */
			ch = in[i];
			i += 1;
		}
		else
		{
			return n;
		}

		out[n++] = ch;

		if (n >= bufferSize)
		{
			return n - 1;
		}
	}

	return n;
}

void de_tokenize_string(const char* str, de_string_array_t* tokens, const char* delim)
{
	static char buffer[1024];
	char* token;
	strcpy(buffer, str);
	DE_ARRAY_CLEAR(*tokens);
	token = strtok(buffer, delim);
	while (token)
	{
		DE_ARRAY_APPEND(*tokens, token);
		token = strtok(NULL, delim);
	}
}