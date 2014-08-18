#include "ray_trace.h"

using namespace std;
vec4 calculate_ray(mat4 perspective_mat, mat4 model_mat) {

	//vec4 ray_clip = vec4(0, 0, -1, 1);
	//mat4 inv_proj_mat;
	//gluInvertMatrix(perspective_mat, inv_proj_mat);
	//vec4 ray_eye = inv_proj_mat * ray_clip;
	//mat4 inv_view_mat;
	//gluInvertMatrix(model_mat, inv_view_mat);
	//vec4 ray = inv_view_mat * ray_eye;
	//vec3 ray_wor = vec3(ray.x, ray.y, ray.z);
	//cout << "ray vector " << ray_wor << endl;
	//GLfloat d = sqrt(pow(ray.x, 2) + pow(ray.y, 2) + pow(ray.z, 2));
	//ray_wor /= d;
	//return ray_wor;

	vec4 ray = vec4(0, 0, -1, 1);
	mat4 inv_mat;
	gluInvertMatrix(perspective_mat * model_mat, inv_mat);
	cout << "pespective mat:" << endl;
	cout << perspective_mat << endl;
	cout << "model mat:" << endl;
	cout << model_mat << endl;
	cout << "product:" << endl;
	cout << perspective_mat * model_mat << endl;
	cout << "inverse prodcut:" << endl;
	cout << inv_mat << endl;
	cout << "init ray:" << endl;
	cout << ray << endl;
	
	

	ray = inv_mat * ray;
	cout << "raw ray vector: " << ray << endl;
	GLfloat d = sqrt(pow(ray.x, 2) + pow(ray.y, 2) + pow(ray.z, 2));
	ray /= d;
	cout << "normalized ray vector: " << ray << endl;
	return vec3(ray.x, ray.y, ray.z);
}

vec4 calculate_eye_position(mat4 perspective_mat, mat4 model_mat, int x, int y, int width, int height) {
	GLfloat xx = (2.0f * x) / width - 1;
	GLfloat yy = 1 - (2.0f * y) / height;
	vec4 eye_pos = vec4(xx, yy, 0, 1);
	
	mat4 inv_mat;
	gluInvertMatrix(perspective_mat * model_mat, inv_mat);
	eye_pos = inv_mat * eye_pos;
	return eye_pos;
}

vec4 unproject_vertex(mat4 mat, vec4 vertex) {
	mat4 inv_mat;
	gluInvertMatrix(mat, inv_mat);
	return inv_mat * vertex;
}

/*
calculate intersection with casting ray and traingle meshes
cosume an array of vertecies, array of normal vectors,
an integer for the number of triangles, a 3d vector
and the eye position
return the intersection coordinate in 3d vector
*/
vec3 ray_intersection(vec4 *triangle_vertecies, vec4 *normals,
	int ntriangles, vec3 ray_wor, vec4 eye_pos) {
	bool is_initial_state = true;
	vec3 closest_intersect = NULL;
	GLfloat t_minimal = 0;
	vec3 eye_pos_v3;
	for (int i = 0; i < ntriangles; i++) {
		vec3 norm = vec3(normals[i].x, normals[i].y, normals[i].z);
		vec3 a = vec3(triangle_vertecies[i].x, triangle_vertecies[i].y,
			triangle_vertecies[i].z);
		GLfloat distance = -Angel::dot(a, norm);
		eye_pos_v3 = vec3(eye_pos.x, eye_pos.y, eye_pos.z);
		GLfloat t = -((dot(eye_pos_v3, norm) + distance)
			/ dot(ray_wor, norm));
		//cout << "t = " << t << endl;
		vec3 intersect_point = eye_pos_v3 + ray_wor * t;

		if (t <= 0) {
			continue;
		}
		if (is_initial_state) {
			closest_intersect = intersect_point;
			t_minimal = t;
			is_initial_state = false;
		}
		else if (t < t_minimal) {
			closest_intersect = intersect_point;
			t_minimal = t;
		}
	}
	cout << "t = " << t_minimal << endl;
	cout << "intersect point:" << (eye_pos_v3 + ray_wor * t_minimal) << endl;;
	return closest_intersect;
}

