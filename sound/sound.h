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

/**
 * Sound library. 
 * 
 * If you familiar with OpenAL, then you won't get any troubles understanding how
 * library works. Main concepts are:
 * - Listener
 * - Buffer
 * - Sound source  * 
 *
 * Important notes: Sound system internally uses floats to store samples.
 * Each sample is normalized to [-1; 1] range so it is independent of
 * output device bit-depth.
 * You may say: "Hey, what the fuck, floats eats too much memory and slow
 * for mixing?!". That is true only for ancient hardware, modern CPU's
 * are very fast at floating point operations and modern computers have enough
 * memory to store float samples.
 * Floats giving great flexibility in calculations, so we'll stick with them.
 * 
 * Notes about mixing and output device feeding.
 * 
 * Output device playbacks special buffer which consists of two parts of the same size.
 * While device playing one part, mixer thread mixes all audible sounds into other part
 * and then when playback device wants more data, mixer threads "feeds" it with already
 * mixed data and cycle starts over. So changes in output audio can be applied only with 
 * fixed time interval which defined by output buffer size. Unlike graphics, updating
 * panning and other sound properties can be done less frequently, even 10 Hz update 
 * frequency sounds pretty fine. Thus being said, you should NOT call de_sound_context_update
 * too frequently, because update frequncy cannot exceed frequency mentioned above, but you 
 * will lose precious CPU resources on redundant work.
 * 
 * Signal flow scheme:
 * 
 * Device part:                   _______________________________________________________________
 *                                |                                                              |
 *                                V                                                              |
 * ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||  <-- Audio device data     |
 * ^                              |                                 |                            |
 * |                              |                                 |                            |
 * |_____________________         |  Signaling playback positions   |                            |
 *                      |         |     (triggers copying)          |                            |
 * Mixer thread:        |         V                                 |                             
 *                      |__(<<copy to device)____    ________ (copy to device>>)_________________|
 *                                               |  |
 * |||||||||||||||||||||||||||||||||     <-- Output buffer
 * ^......(mix until end)......^^^^^
 * |            
 * + Adder
 * |------------------------
 * |            |          |
 * Source1 	Source2 ... SourceN
 *    ^        ^           ^
 *    |        |           |_______________________________________________________
 *    |        |________________________                                           |
 *    |_______                          |                                          |
 *            |                         |                                          |
 * Buffer1: |||||||||||||||||||||||||||||||||           Buffer2: |||||||||||||||||||||||||||||||||
 *                                      
 *                             BufferN: |||||||||||||||||||||||||||||||||
 */

typedef enum de_sound_channel_type_t {
	DE_SOUND_CHANNEL_LEFT,
	DE_SOUND_CHANNEL_RIGHT,
	DE_SOUND_MAX_CHANNELS
} de_sound_channel_type_t;

#include "sound/listener.h"
#include "sound/device.h"
#include "sound/decoder.h"
#include "sound/buffer.h"
#include "sound/source.h"
#include "sound/context.h"