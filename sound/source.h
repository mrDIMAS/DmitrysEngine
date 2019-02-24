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

typedef enum de_sound_source_status_t {
	DE_SOUND_SOURCE_STATUS_PLAYING,
	DE_SOUND_SOURCE_STATUS_STOPPED,
	DE_SOUND_SOURCE_STATUS_PAUSED
} de_sound_source_status_t;

typedef enum de_sound_source_type_t {
	DE_SOUND_SOURCE_TYPE_2D,
	DE_SOUND_SOURCE_TYPE_3D
} de_sound_source_type_t;

struct de_sound_source_t {
	de_sound_source_type_t type;
	de_sound_context_t* ctx;
	de_sound_buffer_t* buffer;
	de_sound_source_status_t status;
	double buf_read_pos; /**< Read position in the buffer. Differs from @playback_pos if buffer is streaming. */
	double playback_pos; /**< Real playback position */
	double current_sample_rate;
	float pan; 
	float pitch;
	float gain;
	bool loop;
	float radius; /**< Radius of sphere around sound source at which sound volume is half of initial. */
	float left_gain;
	float right_gain;
	/* spatial data for 3d sounds */
	de_vec3_t position;
};

/**
 * @brief Creates new sound source. Thread-safe.
 */
de_sound_source_t* de_sound_source_create(de_sound_context_t* ctx, de_sound_source_type_t type);

/**
 * @brief Frees all resources associated with sound source. Thread-safe.
 */
void de_sound_source_free(de_sound_source_t* src);

/**
 * @brief Sets new sound buffer for sound source. Thread-safe.
 */
void de_sound_source_set_buffer(de_sound_source_t* src, de_sound_buffer_t* buf);

/**
 * @brief Internal. Writes out final samples for sound device mixer.
 *
 * Note: Left and right channels only! 2.1, 5.1 and so on formats not supported!
 */
void de_sound_source_sample(de_sound_source_t* src, float* left, float* right);

/**
 * @brief Internal. Sound mixer uses this method to ensure that source should participate in mixing.
 *
 * Sources without buffers or stopped sources does not produce any samples and can be
 * discarded from mixing process.
 */
bool de_sound_source_can_produce_samples(de_sound_source_t* src);

/**
 * @brief Starts playing the sound. Thread-safe.
 */
void de_sound_source_play(de_sound_source_t* src);

/**
 * @brief Stops the sound, rewinds to beginning. Thread-safe.
 */
void de_sound_source_stop(de_sound_source_t* src);

/**
 * @brief Pauses the sound, playing can be continued by @de_sound_source_play. Thread-safe.
 */
void de_sound_source_pause(de_sound_source_t* src);

/**
 * @brief Sets actual type of sound (3d or 2d). Thread-safe.
 */
void de_sound_source_set_type(de_sound_source_t* src, de_sound_source_type_t type);

/**
 * @brief Internal. Calculates gain for each channel. This is relatively heavy method and
 * should not be called too frequently.
 */
void de_sound_source_update(de_sound_source_t* src);