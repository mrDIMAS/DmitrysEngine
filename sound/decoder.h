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

typedef enum de_sound_decoder_type_t {
	DE_SOUND_DECODER_TYPE_WAV = 1
} de_sound_decoder_type_t;

struct de_sound_decoder_t {
	uint32_t channel_count;
	uint32_t sample_per_channel;
	uint32_t sample_rate;
	/* private */
	de_sound_decoder_type_t type;
	FILE* file;
	uint32_t source_byte_per_sample;
	size_t total_bytes;
	size_t bytes_readed;
};

/**
 * @brief Opens stream. Automatically detects type of data.
 */
de_sound_decoder_t* de_sound_decoder_init(const char* filename);

/**
 * @brief Reads next portion of decoded PCM data. Returns actual count of samples per channel.
 *
 * Returns actual sample count per channel that was readed.
 *
 * Explanation: We don't care about size in bytes of data since we using floats to store samples,
 * so this method expects amount of samples that it should read per channel in source stream.
 * For example, we have 2-channel with 44100 Hz sample rate source file and we want to read one
 * second of data from it we need to pass 44100 as @sample_per_channel argument. The function will
 * actually read 2*44100*internal_sample_size bytes from source and will convert it to
 * 2*44100 float samples. In this case @out_data must be 2*44100*sizeof(float)=352800 bytes long.
 * You may wonder why we do take care of sample rate of source? Answer is simple: sample rate will be
 * used as parameter to calculate playback speed of a sound source. Thus we will eliminate resampling
 * stage, it will be performed automatically.
 */
size_t de_sound_decoder_read(de_sound_decoder_t* dec, float* out_data, size_t sample_per_channel, size_t offset, size_t count);

/**
 * @brief Rewinds read head to beginning of the stream.
 */
void de_sound_decoder_rewind(de_sound_decoder_t* dec);

/**
 * @brief Frees stream.
 */
void de_sound_decoder_free(de_sound_decoder_t* dec);
