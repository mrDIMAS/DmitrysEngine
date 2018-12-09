#ifndef DE_VERTEX_H
#define DE_VERTEX_H

/**
* @brief Vertex type
*/
typedef struct de_vertex_s
{
	de_vec3_t position;         /**< Position of vertex in 3D space */
	de_vec3_t normal;           /**< Normal vector of vertex */
	de_vec2_t tex_coord;        /**< Texture coordinates of vertex */
	de_vec2_t second_tex_coord; /**< Texture coordinates of vertex */
	de_vec3_t tangent;          /**< Tangent vector of vertex */
	float bone_weights[4];      /**< Array of four bone weights */
	unsigned char bones[4];     /**< Indices of corresponding bone matrix */
} de_vertex_t;


#endif