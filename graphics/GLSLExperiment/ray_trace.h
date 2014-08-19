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
vec4 unproject_vertex(mat4 mat, vec4 vertex);
int ray_intersection_for_index(vec4 *triangle_vertecies, vec4 *normals,
	int ntriangles, vec3 ray_wor, vec4 eye_pos);
GLfloat sign(vec2 p1, vec2 p2, vec2 p3);
bool PointInTriangle(vec2 pt, vec2 v1, vec2 v2, vec2 v3);
bool PointInTriangleVec4(vec4 pt, vec4 p1, vec4 p2, vec4 p3);
#endif