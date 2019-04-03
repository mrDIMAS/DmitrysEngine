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

bool de_config_parse_file(de_config_t* cfg, const de_path_t* path) {
	DE_ASSERT(path);
	DE_ASSERT(cfg);
	size_t size;
	char* src = de_load_file_into_memory(de_path_cstr(path), &size);
	if (src) {
		return de_config_parse(cfg, src, size);
	}
	return false;
}

/**
 * @brief Moves read pointer until find any of specified characters and sets read pointer on next
 * symbol right after last found. Returns false if end or source has reached.
 */
static bool de_config_move_after(size_t* i, const char* src, size_t len, const char* chars) {
	while (!strchr(chars, src[*i])) {
		if (++(*i) >= len) {
			return false;
		}
	}
	if (++(*i) >= len) {
		return false;
	}
	return true;
}

/**
* @brief Moves read pointer while it points to any of specified characters.
* Returns false if end has reached.
*/
static bool de_config_ignore_while_contains_any(size_t* i, const char* src, size_t len, const char* chars) {
	while (strchr(chars, src[*i])) {
		if (++(*i) >= len) {
			return false;
		}
	}
	return true;
}

bool de_config_parse(de_config_t* cfg, char* src, size_t len) {
	DE_ASSERT(src);
	DE_ASSERT(cfg);
	size_t i = 0;
	bool result = false;
	cfg->data = src;
	DE_ARRAY_INIT(cfg->entries);
	while (true) {
		if (!de_config_ignore_while_contains_any(&i, src, len, "\t ")) {
			break;
		}

		/* ignore comments */
		if (src[i] == '#') {
			de_config_move_after(&i, src, len, "\n");
		}

		if (!de_config_ignore_while_contains_any(&i, src, len, "\t ")) {
			break;
		}

		/* read name */
		DE_ARRAY_GROW(cfg->entries, 1);
		de_config_entry_t* entry = &DE_ARRAY_LAST(cfg->entries);
		entry->name = src + i;
		while (isalnum(src[i])) {
			++i;
			if (i >= len) {
				de_log("unexpected end of source while reading name");
				entry->name = NULL;
				entry->value = NULL;
				goto end;
			}
		}
		src[i++] = 0;
		if (i >= len) {
			goto end;
		}

		/* find = */
		if (!de_config_move_after(&i, src, len, "=")) {
			de_log("unexpected end of source while looking for =");
			break;
		}

		/* find opening quote */
		if (!de_config_move_after(&i, src, len, "\"")) {
			de_log("unexpected end of source");
			break;
		}

		/* read value */
		entry->value = src + i;
		while (src[i] != '\"') {
			++i;
			if (i >= len) {
				de_log("unexpected end of source");
				entry->name = NULL;
				entry->value = NULL;
				goto end;
			}
		}
		src[i++] = 0;
		if (i >= len) {
			goto end;
		}

		/* find end */
		if (!de_config_move_after(&i, src, len, ";")) {
			de_log("unexpected end of source");
			break;
		}
	}
	result = true;
end:
	return result;
}

de_config_entry_t* de_config_find(de_config_t* cfg, const char* name) {
	DE_ASSERT(cfg);
	DE_ASSERT(name);
	for (size_t i = 0; i < cfg->entries.size; ++i) {
		de_config_entry_t* entry = cfg->entries.data + i;
		if (strcmp(name, entry->name) == 0) {
			return entry;
		}
	}
	return NULL;
}

bool de_config_get_float(de_config_t* cfg, const char* name, float* f) {
	DE_ASSERT(cfg);
	DE_ASSERT(name);
	DE_ASSERT(f);
	de_config_entry_t* entry = de_config_find(cfg, name);
	if (entry) {
		*f = (float)atof(entry->value);
		if (errno == ERANGE) {
			return false;
		}
	}
	return true;
}

bool de_config_get_int32(de_config_t* cfg, const char* name, int32_t* i) {
	DE_ASSERT(cfg);
	DE_ASSERT(name);
	DE_ASSERT(i);
	de_config_entry_t* entry = de_config_find(cfg, name);
	if (entry) {
		*i = atoi(entry->value);
		if (errno == ERANGE) {
			return false;
		}
	}
	return true;
}

bool de_config_get_int64(de_config_t* cfg, const char* name, int64_t* i) {
	DE_ASSERT(cfg);
	DE_ASSERT(name);
	DE_ASSERT(i);
	de_config_entry_t* entry = de_config_find(cfg, name);
	if (entry) {
		*i = atoll(entry->value);
		if (errno == ERANGE) {
			return false;
		}
	}
	return true;
}

bool de_config_get_str(de_config_t* cfg, const char* name, const char** str) {
	DE_ASSERT(cfg);
	DE_ASSERT(name);
	DE_ASSERT(str);
	de_config_entry_t* entry = de_config_find(cfg, name);
	if (entry) {
		*str = entry->value;
		return true;
	}
	return true;
}

void de_config_free(de_config_t* cfg) {
	DE_ASSERT(cfg);
	de_free(cfg->data);
	DE_ARRAY_FREE(cfg->entries);
}

void de_config_print(de_config_t* cfg) {
	DE_ASSERT(cfg);
	for (size_t i = 0; i < cfg->entries.size; ++i) {
		de_config_entry_t* entry = cfg->entries.data + i;
		printf("%s = %s\n", entry->name, entry->value);
	}
}