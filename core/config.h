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

typedef struct de_config_entry_t {
	char* name;
	char* value;
} de_config_entry_t;

typedef struct de_config_t {
	char* data;
	DE_ARRAY_DECLARE(de_config_entry_t, entries);
} de_config_t;

/**
 * @brief Loads file into memory and then parses it.
 */
bool de_config_parse_file(de_config_t* cfg, const de_path_t* path);

/**
 * @brief Parses given source string.
 *
 * IMPORTANT: src must be allocated on heap!
 */
bool de_config_parse(de_config_t* cfg, char* src, size_t len);

/**
 * @brief Frees memory used by specified config.
 */
void de_config_free(de_config_t* cfg);

/**
 * @brief Prints config as set of strings "name = value".
 */
void de_config_print(de_config_t* cfg);

/**
* @brief Performs linear search of value by specified name.
*/
de_config_entry_t* de_config_find(de_config_t* cfg, const char* name);

/**
 * @brief Tries to get float value by given name. Returns true on success.
 */
bool de_config_get_float(de_config_t* cfg, const char* name, float* f);

/**
 * @brief Tries to get 32 bit integer value by given name. Returns true on success.
 */
bool de_config_get_int32(de_config_t* cfg, const char* name, int32_t* i);

/**
 * @brief Tries to get 64 bit integer value by given name. Returns true on success.
 */
bool de_config_get_int64(de_config_t* cfg, const char* name, int64_t* i);

/**
 * @brief Tries to get string value by given name. Returns true on success.
 */
bool de_config_get_str(de_config_t* cfg, const char* name, const char** str);

/**
 * @brief Tests for config.
 */
void de_config_test(void);