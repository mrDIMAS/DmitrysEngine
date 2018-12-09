char * de_load_file_into_memory(const char * path)
{
	size_t file_size;
	size_t content_size;
	char* content;
	FILE* file;

	/* try to open file */
	file = fopen(path, "rb");
	if (!file)
	{
		de_error("unable to read file: %s", path);
	}

	/* get file size */
	fseek(file, 0, SEEK_END);
	file_size = ftell(file);
	content_size = file_size + 1;
	fseek(file, 0, SEEK_SET);

	/* read file */
	content = de_malloc(content_size);
	if (fread(content, sizeof(char), file_size, file) != file_size)
	{
		de_error("file %s is corrupted", path);
	}

	/* close file and write trailing zero at end of content array */
	fclose(file);
	content[file_size] = '\0';

	return content;
}
