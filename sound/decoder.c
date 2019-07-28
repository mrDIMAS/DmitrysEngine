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

/* http://soundfile.sapp.org/doc/WaveFormat/ */
typedef struct de_wav_header_t {
	int8_t chunk_id[4];
	uint32_t chunk_size;
	int8_t format[4];
	/* fmt  chunk */
	int8_t fmt_chunk_id[4];
	uint32_t fmt_chunk_size;
	uint16_t audio_format;
	uint16_t num_channels;
	uint32_t sample_rate;
	uint32_t byte_rate;
	uint16_t block_align;
	uint16_t bits_per_sample;
	/* data chunk */
	int8_t data_chunk_id[4];
	uint32_t data_chunk_size;
} de_wav_header_t;

/* Make sure that compiler didn't added some nasty padding. So we'll be able to read whole struct
 * as a single call. */
DE_STATIC_ASSERT(sizeof(de_wav_header_t) == 44U, invalid_wav_header_size);

bool de_wav_read_header(FILE* f, de_wav_header_t* wav)
{
	if (fread(wav->chunk_id, sizeof(wav->chunk_id), 1, f) != 1) {
		return false;
	}
	if (fread(&wav->chunk_size, sizeof(wav->chunk_size), 1, f) != 1) {
		return false;
	}
	if (fread(wav->format, sizeof(wav->format), 1, f) != 1) {
		return false;
	}
	if (fread(wav->fmt_chunk_id, sizeof(wav->fmt_chunk_id), 1, f) != 1) {
		return false;
	}
	if (fread(&wav->fmt_chunk_size, sizeof(wav->fmt_chunk_size), 1, f) != 1) {
		return false;
	}
	if (fread(&wav->audio_format, sizeof(wav->audio_format), 1, f) != 1) {
		return false;
	}
	if (fread(&wav->num_channels, sizeof(wav->num_channels), 1, f) != 1) {
		return false;
	}
	if (fread(&wav->sample_rate, sizeof(wav->sample_rate), 1, f) != 1) {
		return false;
	}
	if (fread(&wav->byte_rate, sizeof(wav->byte_rate), 1, f) != 1) {
		return false;
	}
	if (fread(&wav->block_align, sizeof(wav->block_align), 1, f) != 1) {
		return false;
	}
	if (fread(&wav->bits_per_sample, sizeof(wav->bits_per_sample), 1, f) != 1) {
		return false;
	}
	if (fread(&wav->data_chunk_id, sizeof(wav->data_chunk_id), 1, f) != 1) {
		return false;
	}
	if (strncmp((char*)wav->data_chunk_id, "data", sizeof(wav->data_chunk_id)) != 0) {
		return false;
	}
	if (fread(&wav->data_chunk_size, sizeof(wav->data_chunk_size), 1, f) != 1) {
		return false;
	}
	return true;
}

static bool de_wav_header_is_valid(de_wav_header_t* wav)
{
	if (strncmp((char*)wav->chunk_id, "RIFF", sizeof(wav->chunk_id)) != 0) {
		de_log("wav: invalid chunk id");
		return false;
	}

	if (strncmp((char*)wav->fmt_chunk_id, "fmt ", sizeof(wav->fmt_chunk_id)) != 0) {
		de_log("wav: invalid fmt chunk id");
		return false;
	}

	if (strncmp((char*)wav->format, "WAVE", sizeof(wav->format)) != 0) {
		de_log("wav: invalid format");
		return false;
	}

	if (wav->audio_format != 1) {
		de_log("wav: compressed formats not supported!");
		return false;
	}

	return true;
}

bool de_sound_decoder_is_source_valid(const de_path_t* path)
{
	FILE* file = fopen(de_path_cstr(path), "rb");

	if (!file) {
		return false;
	}

	de_wav_header_t wav;
	de_wav_read_header(file, &wav);
	if (!de_wav_header_is_valid(&wav)) {
		return false;
	} else {
		/* TODO: Add more checks for other supported parameters here. */
	}

	fclose(file);
	return true;
}

de_sound_decoder_t* de_sound_decoder_create(const char* filename)
{
	de_path_t path;
	de_path_init(&path);
	de_path_append_cstr(&path, filename);

	de_str8_view_t ext;
	de_path_extension(&path, &ext);

	de_sound_decoder_t* dec = DE_NEW(de_sound_decoder_t);
	dec->file = fopen(filename, "rb");
	if (de_str8_view_eq_cstr(&ext, ".wav")) {
		de_wav_header_t wav;
		de_wav_read_header(dec->file, &wav);
		dec->channel_count = wav.num_channels;
		dec->sample_rate = wav.sample_rate;
		dec->sample_per_channel = wav.data_chunk_size / wav.block_align;
		dec->type = DE_SOUND_DECODER_TYPE_WAV;
		dec->total_bytes = wav.data_chunk_size;
		dec->bytes_read = 0;
		dec->source_byte_per_sample = wav.bits_per_sample / 8;
		/* next goes data which will be obtained by de_sound_decoder_read */
	} else {
		de_log("decoder: unsupported format of file %s", filename);
	}

	de_path_free(&path);

	return dec;
}

size_t de_sound_decoder_read(de_sound_decoder_t* dec, float* out_data, size_t sample_per_channel, size_t offset, size_t count)
{
	size_t i = 0;
	int sample_chunk_size = dec->source_byte_per_sample * dec->channel_count;
	switch (dec->type) {
		case DE_SOUND_DECODER_TYPE_WAV:
			for (i = 0; i < count && dec->bytes_read < dec->total_bytes; ++i) {
				/* Read sample chunk */
				uint8_t chunk[64];
				size_t bytes_read = fread(chunk, 1, sample_chunk_size, dec->file);

				/* Convert interleaved i8/i16 data non-interleaved normalized float */
				for (size_t k = 0; k < (size_t)dec->channel_count; ++k) {
					const size_t channel_start = k * sample_per_channel;
					const size_t position = channel_start + offset + i;
					if (offset != 0) {
						offset = offset;
					}
					switch (dec->source_byte_per_sample) {
						case 1:
							out_data[position] = ((int8_t*)chunk)[k] / (float)INT8_MAX;
							break;
						case 2:
							out_data[position] = ((int16_t*)chunk)[k] / (float)INT16_MAX;
							break;
						default:
							/* Unsupported format. Keep silence here. */
							out_data[position] = 0;
							break;
					}
				}

				dec->bytes_read += bytes_read;
			}
			break;
		default:
			break;
	}
	return i;
}

void de_sound_decoder_rewind(de_sound_decoder_t* dec)
{
	rewind(dec->file);
	dec->bytes_read = 0;
	switch (dec->type) {
		case DE_SOUND_DECODER_TYPE_WAV:
			fseek(dec->file, sizeof(de_wav_header_t), SEEK_SET);
		default:
			break;
	}
}

void de_sound_decoder_free(de_sound_decoder_t* dec)
{
	fclose(dec->file);
	de_free(dec);
}