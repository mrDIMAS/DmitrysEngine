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

static FILE* de_log_file;

void de_log_open(const char* filename)
{
	if (!de_log_file) {
		de_log_file = fopen(filename, "w");
	}
}

static void de_write_log(const char* message, bool error)
{
	time_t rawtime;
	struct tm* timeinfo;

	/* Get time stamp */
	time(&rawtime);
	timeinfo = localtime(&rawtime);

	if (de_log_file) {
		fprintf(de_log_file, "[%dh:%dm:%ds] %s\n", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, message);
		fflush(de_log_file);
	}

	/* Duplicate message into standard streams, but without time stamps */
	if (error) {
		fprintf(stderr, "Error: %s\n", message);
	} else {
		fprintf(stdout, "%s\n", message);
		fflush(stdout);
	}
}

void de_log(const char* message, ...)
{
	static char format_buffer[65536];
	va_list argument_list;
	va_start(argument_list, message);
	vsnprintf(format_buffer, sizeof(format_buffer), message, argument_list);
	va_end(argument_list);
	de_write_log(format_buffer, false);
}

void de_fatal_error(const char* message, ...)
{
	static char format_buffer[65536];
	va_list argument_list;
	va_start(argument_list, message);
	vsnprintf(format_buffer, sizeof(format_buffer), message, argument_list);
	va_end(argument_list);
	de_write_log(format_buffer, true);
	de_message_box(format_buffer, "Fatal Error");
#ifdef _MSC_VER
	__debugbreak();
#else
	__asm__("int $3");
#endif
	exit(EXIT_FAILURE);
}

void de_log_close(void)
{
	if (de_log_file) {
		fclose(de_log_file);
	}
}