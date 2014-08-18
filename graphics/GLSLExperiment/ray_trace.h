#ifndef RAY_TRACE
#define RAY_TRACE
#include "Angel.h"
#include "matrix_calculator.h"
using namespace std;
vec3 ray_intersection(vec4 *triangle_vertecies, vec4 *normals,
	int ntriangles, vec3 ray_wor, vec4 eye_pos);
vec4 calculate_ray(mat4 perspective_mat, mat4 model_mat);
vec4 calculate_eye_position(mat4 perspective_mat, mat4 model_mat, int x, int y,
	int width, int height);
#endif