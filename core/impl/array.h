/* Internals functions. Do not use directly! Use macro instead. */
void de_array_grow_(void** data, size_t* size, size_t* capacity, size_t item_size, size_t n) {
	*size += n;
	if (*size >= *capacity && *size > 0) {
		*capacity = *capacity * 2u + n;
		*data = de_realloc(*data, *capacity * item_size);
	}
}

void de_array_reserve_(void** data, size_t* size, size_t* capacity, size_t item_size, size_t new_capacity) {
	*capacity = new_capacity;
	if (*size > *capacity) {
		*size = *capacity;
	}
	*data = de_realloc(*data, *capacity * item_size);
}