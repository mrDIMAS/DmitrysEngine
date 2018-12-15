/* Copyright (c) 2017-2018 Dmitry Stepanov a.k.a mr.DIMAS
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

typedef struct de_animation_t de_animation_t;

/**
* @class de_keyframe_t
* @brief Keyframe
*/
typedef struct de_keyframe_t
{
	de_vec3_t position; /**< Position of keyframe */
	de_vec3_t scale;    /**< Scale of keyframe */
	de_quat_t rotation; /**< Rotation quaternion of keyframe */
	float time;         /**< Time of keyframe in seconds */
} de_keyframe_t;

/**
 * @class de_animation_track_t
 * @brief Animation track
 *
 * Set of keyframes and pointer to animated node. Defines animation of a node.
 */
struct de_animation_track_t
{
	de_animation_t* parent_animation;
	DE_ARRAY_DECLARE(de_keyframe_t, keyframes); /**< Array of keyframes */
	de_bool_t enabled;       /**< Is track enabled? */
	float max_time;       /**< Private. Length of animation. */
	de_node_t* node;
};

typedef enum de_animation_flags_t
{
	DE_ANIMATION_FLAG_ENABLED, /**< Is animation enabled? */
	DE_ANIMATION_FLAG_LOOPED   /**< Is animation looped */
} de_animation_flags_t;

/**
* @class de_animation_t
* @brief Animation
*
* Set of animation tracks that controls various properties of scene nodes.
*/
struct de_animation_t
{
	DE_LINKED_LIST_ITEM(struct de_animation_t);
	de_scene_t* parent_scene;
	DE_ARRAY_DECLARE(de_animation_track_t*, tracks);  /**< Array of pointers to animation tracks */
	de_animation_flags_t flags; /**< Bitset of flags */
	float speed;                /**< Animation playback speed */
	float length;               /**< Total animation length */
	float time_position;        /**< Current time of animation (playback position) */
	float weight;               /**< Weight of animation [0; 1]. Used for animation blending */
	float fade_step;            /**< Speed of weight fading. Used for animation blending */
};

/**
* @brief Writes out intepolated keyframe from animation track at specified time
* @param track pointer to animation track
* @param time time in seconds
* @param out_keyframe pointer to output intepolated keyframe
*/
void de_animation_track_get_keyframe(de_animation_track_t* track, float time, de_keyframe_t* out_keyframe);

/**
 * @brief Adds animations track to an animation. Every animation can contain any number of tracks.
 */
void de_animation_add_track(de_animation_t* anim, de_animation_track_t* track);

/**
 * @brief Creates new animation track and attaches it to animation.
 */
de_animation_track_t* de_animation_track_create(de_animation_t* anim);

/**
 * @brief Frees memory.
 */
void de_animation_track_free(de_animation_track_t* track);

/**
 * @brief Adds a keyframe to animation track.
 */
void de_animation_track_add_keyframe(de_animation_track_t* track, const de_keyframe_t* keyframe);

/**
 * @brief Frees memory.
 */
void de_animation_free(de_animation_t* anim);

/**
 * @brief
 */
void de_animation_update(de_animation_t* anim, float dt);

/**
 * @brief
 */
void de_animation_set_time_position(de_animation_t* anim, float time);

/**
 * @brief
 */
de_bool_t de_animation_is_flags_set(de_animation_t* anim, int flags);

/**
 * @brief
 */
void de_animation_set_flags(de_animation_t* anim, int flags);

/**
 * @brief
 */
void de_animation_reset_flags(de_animation_t* anim, int flags);

/**
 * @brief Clamps length of animation to longest track.
 */
void de_animation_clamp_length(de_animation_t* anim);