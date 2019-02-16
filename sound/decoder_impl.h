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
	char chunk_id[4];
	uint32_t chunk_size;
	char format[4];
	/* fmt  chunk */
	char fmt_chunk_id[4];
	uint32_t fmt_chunk_size;
	uint16_t audio_format;
	uint16_t num_channels;
	uint32_t sample_rate;
	uint32_t byte_rate;
	uint16_t block_align;
	uint16_t bits_per_sample;
	/* data chunk */
	char data_chunk_id[4];
	uint32_t data_chunk_size;
} de_wav_header_t;

bool de_wav_read_header(FILE* f, de_wav_header_t* wav) {
	fread(wav->chunk_id, 1, sizeof(wav->chunk_id), f);
	if (strncmp(wav->chunk_id, "RIFF", sizeof(wav->chunk_id)) != 0) {
		return false;
	}
	fread(&wav->chunk_size, 1, sizeof(wav->chunk_size), f);
	fread(wav->format, 1, sizeof(wav->format), f);
	if (strncmp(wav->format, "WAVE", sizeof(wav->format)) != 0) {
		return false;
	}
	fread(wav->fmt_chunk_id, 1, sizeof(wav->fmt_chunk_id), f);
	if (strncmp(wav->fmt_chunk_id, "fmt ", sizeof(wav->fmt_chunk_id)) != 0) {
		return false;
	}
	fread(&wav->fmt_chunk_size, 1, sizeof(wav->fmt_chunk_size), f);
	fread(&wav->audio_format, 1, sizeof(wav->audio_format), f);
	if (wav->audio_format != 1) {
		de_log("wav: compressed formats not supported!");
		return false;
	}
	fread(&wav->num_channels, 1, sizeof(wav->num_channels), f);
	fread(&wav->sample_rate, 1, sizeof(wav->sample_rate), f);
	fread(&wav->byte_rate, 1, sizeof(wav->byte_rate), f);
	fread(&wav->block_align, 1, sizeof(wav->block_align), f);
	fread(&wav->bits_per_sample, 1, sizeof(wav->bits_per_sample), f);
	fread(&wav->data_chunk_id, 1, sizeof(wav->data_chunk_id), f);
	if (strncmp(wav->data_chunk_id, "data", sizeof(wav->data_chunk_id)) != 0) {
		return false;
	}
	fread(&wav->data_chunk_size, 1, sizeof(wav->data_chunk_size), f);
	return true;
}

de_sound_decoder_t* de_sound_decoder_init(const char* filename) {
	de_path_t path;
	de_str8_view_t ext;
	de_sound_decoder_t* dec = DE_NEW(de_sound_decoder_t);
	de_path_init(&path);
	de_path_append_cstr(&path, filename);
	de_path_extension(&path, &ext);
	de_zero(dec, sizeof(*dec));
	dec->file = fopen(filename, "rb");
	if (de_str8_view_eq_cstr(&ext, ".wav")) {
		de_wav_header_t wav;
		if (!de_wav_read_header(dec->file, &wav)) {
			de_log("decoder: invalid or unsupported wav file %s!", filename);
		}
		dec->channel_count = wav.num_channels;
		dec->sample_rate = wav.sample_rate;
		dec->sample_per_channel = wav.data_chunk_size / wav.block_align;
		dec->type = DE_SOUND_DECODER_TYPE_WAV;
		dec->source_byte_per_sample = wav.bits_per_sample / 8;		
		/* next goes data which will be obtained by de_sound_decoder_get_next_block */
	} else {
		de_log("decoder: unsupported format of file %s", filename);
	}
	de_path_free(&path);
	return dec;
}

int de_sound_decoder_get_next_block(de_sound_decoder_t* dec, float* out_data, size_t sample_per_channel) {	
	int readed_bytes;
	uint8_t chunk[64];
	size_t i, k;	
	int actual_sample_count = -1;
	int sample_chunk_size = dec->source_byte_per_sample * dec->channel_count;
	switch (dec->type) {
		case DE_SOUND_DECODER_TYPE_WAV:
			for (i = 0; i < sample_per_channel; ++i) {				
				/* read sample chunk */
				readed_bytes = fread(chunk, 1, sample_chunk_size, dec->file);
				if (readed_bytes != sample_chunk_size) {
					break;
				} 
				/* convert interleaved i8/i16 data non-interleaved normalized float */
				for (k = 0; k < (size_t)dec->channel_count; ++k) {
					switch (dec->source_byte_per_sample) {
						case 1:
							out_data[k * sample_per_channel + i] = ((int8_t*)chunk)[k] / 255.0f;
							break;
						case 2:
							out_data[k * sample_per_channel + i] = ((int16_t*)chunk)[k] / 32767.0f;
							break;
						default:
							break;
					}
				}
			}			
			break;
		default:
			break;
	}
	return actual_sample_count;
}

void de_sound_decoder_rewind(de_sound_decoder_t* dec) {
	rewind(dec->file);
}

void de_sound_decoder_free(de_sound_decoder_t* dec) {
	fclose(dec->file);
	de_free(dec);
}