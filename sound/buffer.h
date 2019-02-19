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

typedef enum de_sound_format_t {
	DE_SOUND_FORMAT_MONO,
	DE_SOUND_FORMAT_STEREO
} de_sound_format_t;

typedef enum de_sound_buffer_flags_t {
	DE_SOUND_BUFFER_FLAGS_STREAM = DE_BIT(0), /**< Useful for huge sounds like music */
	DE_SOUND_BUFFER_FLAGS_UPLOAD_NEXT_BLOCK = DE_BIT(1), /**< Sound source will raise this flag,
														 which indicates that next block must be
														 uploaded */
} de_sound_buffer_flags_t;

/** 
 * Data in buffer is NON-INTERLEAVED, which for left (L) and right (R) channels means:
 * LLLLLLLLLLLLLL RRRRRRRRRRRRRR 
 * So basically data split into chunks for each channel.
 * 
 * Important notes about streaming:
 * When buffer is streaming, data size doubles.
 * |LLLLLLLLLLLLLL RRRRRRRRRRRRRR|LLLLLLLLLLLLLL RRRRRRRRRRRRRR|
 * ^                             ^
 * read_ptr                      stream_write_ptr
 * 
 * So while you read data from buffer by read_ptr, other part will be filled with new 
 * portion of data. When read_ptr will reach stream_write_ptr, they'll be swapped and
 * reading will be performed from new loaded data, while old data will be filled with 
 * new portion of data, this process will continue until end of file and when eof is 
 * reached, streaming will be started from beginning of a file.
 */
struct de_sound_buffer_t {
	de_sound_context_t* ctx;
	de_sound_format_t format;
	float* data; /**< Data of buffer. Do NOT read from this pointer directly! Use read_ptr! */
	float* read_ptr; /**< Pointer to beginning of actual block of samples */	
	float* stream_write_ptr; /**< Pointer to beginning of block for streaming. */
	size_t sample_per_channel; /**< Count of samples per each channel (left, right, etc.) */
	size_t total_sample_per_channel; /**< Total amount of samples in the buffer. */
	size_t channel_count; /**< Total amount of channels */
	uint32_t flags; /**< Special flags for buffer (i.e. streaming) */
	de_sound_decoder_t* decoder; /**< Valid only for streaming buffers */
};

/**
 * @brief Creates new sound buffer. Thread-safe.
 */
de_sound_buffer_t* de_sound_buffer_create(de_sound_context_t* ctx, uint32_t flags);

/**
 * @brief Uploads content of file into buffer. Thread-safe.
 * 
 * If used with DE_SOUND_BUFFER_FLAGS_STREAM flag, then reads small portion from file 
 * and keeps it open until @de_sound_buffer_free is called. 
 * 
 * IMPORTANT: Streaming buffers can ONLY be used with only one sound source!
 */
void de_sound_buffer_load_file(de_sound_buffer_t* buf, const char* file);

/**
 * @brief Destroys sound buffer. Thread-safe.
 */
void de_sound_buffer_free(de_sound_buffer_t* buf);

/**
 * @brief Internal. Performs streaming for streaming buffers.
 */
void de_sound_buffer_update(de_sound_buffer_t* buf);

/**
 * @brief Internal. Swaps read and stream write pointers, used for streaming.
 */
void de_sound_buffer_swap_pointers(de_sound_buffer_t* buf);

void de_sound_buffer_set_flags(de_sound_buffer_t* buf, uint32_t flags);

void de_sound_buffer_reset_flags(de_sound_buffer_t* buf, uint32_t flags);