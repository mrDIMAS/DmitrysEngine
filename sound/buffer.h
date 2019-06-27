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
	DE_SOUND_BUFFER_FLAGS_NONE = 0, /**< Sound data will be uploaded directly to RAM as solid chunk. */
	DE_SOUND_BUFFER_FLAGS_STREAM = DE_BIT(0), /**< Useful for huge sounds like music. Data will be loaded piece by piece when needed. */

	/* 
	 * Internal flags. Do not use. 
	 */

	/** Sound source will raise this flag, which indicates that next block must be uploaded */
	DE_SOUND_BUFFER_FLAGS_UPLOAD_NEXT_BLOCK = DE_BIT(1) 
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
	bool need_reload; /**< Indicates that buffer must be (re)loaded. Until true, sound source won't play such buffer! */
};

/**
 * @brief Internal. Performs streaming for streaming buffers.
 */
void de_sound_buffer_update(de_sound_buffer_t* buf);

/**
 * @brief Internal. Swaps read and stream write pointers, used for streaming.
 */
void de_sound_buffer_swap_pointers(de_sound_buffer_t* buf);

/**
 * @brief Sets specified flags. 
 * 
 * Important notes: Setting DE_SOUND_BUFFER_FLAGS_STREAM flag will cause full reload of sound buffer!
 * Reload will be deferred and performed on next update of sound context. Only one sound will use this 
 * buffer, other sound sources uses same buffer will be reset and will use NULL as buffer. You've been 
 * warned!
 */
void de_sound_buffer_set_flags(de_sound_buffer_t* buf, uint32_t flags);

/**
 * @brief Resets specified flags.
 */
void de_sound_buffer_reset_flags(de_sound_buffer_t* buf, uint32_t flags);

/**
 * @brief Rewinds streaming buffer. Immediately uploads two blocks of data so may cause lag.
 * 
 * Thread-safe.
 */
void de_sound_buffer_rewind(de_sound_buffer_t* buf);

/**
 * @brief Schedules sound buffer for reload. Actual reload will be performed on update
 * of sound context. In other words this function only raises flag, and do nothing more!
 * 
 * Thread-safe.
 */
void de_sound_buffer_reload(de_sound_buffer_t* buf);

de_resource_dispatch_table_t* de_sound_buffer_get_dispatch_table();