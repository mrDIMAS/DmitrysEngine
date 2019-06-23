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
 * @brief 2D float vector
 */
struct de_vec2_t {
	float x; /**< X coordinate of the vector */
	float y; /**< Y coordinate of the vector */
};

/**
* @brief Initializes vector
* @param out output vector
* @param x coordinate
* @param y coordinate
* @return @out
*/
de_vec2_t* de_vec2_set(de_vec2_t* out, float x, float y);

/**
* @brief Sets all components to zero
* @param out output vector
* @return @out
*/
de_vec2_t* de_vec2_zero(de_vec2_t* out);

/**
* @brief Adds two vectors and puts result into @out
* @param out output vector
* @param a input vector
* @param b input vector
* @return @out
*/
de_vec2_t* de_vec2_add(de_vec2_t* out, const de_vec2_t* a, const de_vec2_t* b);

/**
* @brief Subtracts @b from @a, puts result into @out
* @param out output vector
* @param a input vector
* @param b input vector
* @return @out
*/
de_vec2_t* de_vec2_sub(de_vec2_t* out, const de_vec2_t* a, const de_vec2_t* b);

/**
* @brief
* @param out
* @param a
* @param b
* @return
*/
de_vec2_t* de_vec2_div(de_vec2_t* out, const de_vec2_t* a, const de_vec2_t* b);

/**
* @brief
* @param out
* @param a
* @param b
* @return
*/
de_vec2_t* de_vec2_mul(de_vec2_t* out, const de_vec2_t* a, const de_vec2_t* b);

/**
* @brief
* @param out
* @param a
* @param s
* @return
*/
de_vec2_t* de_vec2_scale(de_vec2_t* out, const de_vec2_t* a, float s);

/**
* @brief
* @param out
* @param a
* @return
*/
de_vec2_t* de_vec2_negate(de_vec2_t* out, const de_vec2_t* a);

/**
* @brief
* @param a
* @param b
* @return
*/
float de_vec2_dot(const de_vec2_t* a, const de_vec2_t* b);

/**
* @brief
* @param a
* @return
*/
float de_vec2_len(const de_vec2_t* a);

/**
* @brief
* @param a
* @return
*/
float de_vec2_sqr_len(const de_vec2_t* a);

/**
* @brief
* @param out
* @param a
* @return
*/
de_vec2_t* de_vec2_normalize(de_vec2_t* out, const de_vec2_t* a);

/**
* @brief
* @param out
* @param a
* @param b
* @param t
* @return
*/
de_vec2_t* de_vec2_lerp(de_vec2_t* out, const de_vec2_t* a, const de_vec2_t* b, float t);

/**
* @brief
* @param a
* @param b
* @return
*/
float de_vec2_distance(const de_vec2_t* a, const de_vec2_t* b);


/**
* @brief
* @param out
* @param b
* @return
*/
void de_vec2_perp(de_vec2_t* out, const de_vec2_t* a);

/**
* @brief
* @param a
* @param b
* @return
*/
float de_vec2_sqr_distance(const de_vec2_t* a, const de_vec2_t* b);

/**
* @brief
* @param a
* @param b
* @return
*/
float de_vec2_angle(const de_vec2_t* a, const de_vec2_t* b);

/**
* @brief
* @param out
* @param a
* @param b
* @return
*/
de_vec2_t* de_vec2_middle(de_vec2_t* out, const de_vec2_t* a, const de_vec2_t* b);

/**
* @brief
* @param a
* @param vMin
* @param vMax
*/
void de_vec2_min_max(const de_vec2_t* a, de_vec2_t* vMin, de_vec2_t* vMax);

/**
* @brief
* @param a
* @param b
* @return
*/
int de_vec2_approx_equals(const de_vec2_t* a, const de_vec2_t* b);

/**
* @brief
* @param a
* @param b
* @return
*/
int de_vec2_equals(const de_vec2_t* a, const de_vec2_t* b);