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

typedef struct de_tga_header_t {
	char idLength;
	char colorMapType;
	char dataTypeCode;
	short colorMapOrigin;
	short colorMapLength;
	char colorMapDepth;
	short xOrigin;
	short yOrigin;
	short width;
	short height;
	char bitsPerPixel;
	char imageDescriptor;
} de_tga_header_t;


bool de_image_load_tga(const char* filename, de_image_t* img) {
	FILE* file;
	size_t i, byte_count;
	unsigned char temp;
	de_tga_header_t header;

	file = fopen(filename, "rb");

	if (!file) {
		de_log("TGA Loader: Unable to open file %s", filename);
		return false;
	}

#define READ_VAR(var) fread(&var, sizeof(var), 1, file)
	/* Read TGA header */
	READ_VAR(header.idLength);
	READ_VAR(header.colorMapType);
	READ_VAR(header.dataTypeCode);
	READ_VAR(header.colorMapOrigin);
	READ_VAR(header.colorMapLength);
	READ_VAR(header.colorMapDepth);
	READ_VAR(header.xOrigin);
	READ_VAR(header.yOrigin);
	READ_VAR(header.width);
	READ_VAR(header.height);
	READ_VAR(header.bitsPerPixel);
	READ_VAR(header.imageDescriptor);
#undef READ_VAR

	/* Create new texture */
	img->width = header.width;
	img->height = header.height;
	img->byte_per_pixel = header.bitsPerPixel / 8;

	/* Read pixels */
	byte_count = header.width * header.height * img->byte_per_pixel;
	img->data = (char*)de_malloc(byte_count);
	if (fread(img->data, 1, byte_count, file) != byte_count) {
		de_log("TGA Loader: File %s is corrupted", filename);
		de_free(img->data);
		fclose(file);
		return false;
	}

	fclose(file);

	/* Swap red and blue to get RGB/RGBA image from BGR/ABGR */
	for (i = 0; i < byte_count; i += img->byte_per_pixel) {
		temp = img->data[i];
		img->data[i] = img->data[i + 2];
		img->data[i + 2] = temp;
	}

	de_image_flip_y(img);

	return true;
}


void de_image_flip_y(de_image_t* img) {
	int x, y_src, y_dest, k;
	char* new_data = (char*)de_malloc(img->byte_per_pixel * img->width * img->height);

	for (y_src = (int)img->height - 1, y_dest = 0; y_src >= 0; --y_src, ++y_dest) {
		for (x = 0; x < (int)img->width; ++x) {
			for (k = 0; k < (int)img->byte_per_pixel; ++k) {
				size_t src_index = (y_src * img->width + x) * img->byte_per_pixel + k;
				size_t dst_index = (y_dest * img->width + x) * img->byte_per_pixel + k;
				new_data[dst_index] = img->data[src_index];
			}
		}
	}

	de_free(img->data);
	img->data = new_data;
}