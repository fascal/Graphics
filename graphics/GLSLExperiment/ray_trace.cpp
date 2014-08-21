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

bool isFiniteNumber(GLfloat x) {
	return (x <= FLT_MAX && x >= -FLT_MAX);
}

bool isAbnormalTriangle(vec4 v1, vec4 v2, vec4 v3) {
	return (v3.y - v1.y) * (v2.x - v1.x) == (v3.x - v1.x) * (v2.y - v1.y);
}

bool isTwoPointClose(vec4 v1, vec4 v2) {
	if (sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2) + pow(v1.z - v2.z, 2)) < 300) {
		cout << "HI" << endl;
	}
	return sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2) + pow(v1.z - v2.z, 2)) < 3;
}
bool PointInTriangleVec4_2(vec4 pt, vec4 p1, vec4 p2, vec4 p3) {
	bool b1, b2, b3;
	vec2 p21 = vec2(p1.x, p1.y), p22 = vec2(p2.x, p2.y), p23 = vec2(p3.x, p3.y),
		p2t = vec2(pt.x, pt.y);
	b1 = PointInTriangle(p2t, p21, p22, p23);
	vec2 p212 = vec2(p1.y, p1.z), p222 = vec2(p2.y, p2.z), p232 = vec2(p3.y, p3.z),
		p2t2 = vec2(pt.y, pt.z);
	b2 = PointInTriangle(p2t2, p212, p222, p232);
	vec2 p213 = vec2(p1.z, p1.x), p223 = vec2(p2.z, p2.x), p233 = vec2(p3.z, p3.x),
		p2t3 = vec2(pt.z, pt.x);
	b3 = PointInTriangle(p2t3, p213, p223, p233);
	cout << b1 << b2 << b3 << endl;
	if (abs(p1.x - p2.x) < 0.0001) {
		b2 = true;
	}
	if (abs(p1.y - p2.y) < 0.0001) {
		b3 = true;
	}
	if (abs(p1.z - p2.z) < 0.0001) {
		b1 = true;
	}

	cout << b1 << b2 << b3 << endl;
	return b1 && b2 && b3;

}

vec4 get_normal2(vec4 a, vec4 b, vec4 c) {
	vec3 ab = vec3(b.x - a.x, b.y - a.y, b.z - a.z);
	cout << "ab: " << ab << endl;
	vec3 ac = vec3(c.x - a.x, c.y - a.y, c.z - a.z);
	cout << "ac: " << ac << endl;
	vec3 n = Angel::cross(ab, ac);
	GLfloat d = sqrt(pow(n.x, 2) + pow(n.y, 2) + pow(n.z, 2));
	n /= d;
	cout << "vec4: " << vec4(n.x, n.y, n.z, 1) << endl;
	return vec4(n.x, n.y, n.z, 1);
}

int ray_intersection_for_index(vec4 *triangle_vertecies, vec4 *normals,
	int ntriangles, vec3 ray_wor, vec4 eye_pos) {
	bool is_initial_state = true;
	vec3 closest_intersect = NULL;
	GLfloat t_minimal = 0;
	vec3 eye_pos_v3;
	int index = -1;
	cout << "----------------" << endl;
	for (int i = 0; i < ntriangles; i++) {
		vec3 norm = vec3(normals[i].x, normals[i].y, normals[i].z);
		GLfloat dot_product = dot(ray_wor, norm);
		if (dot_product == 0 || !isfinite(dot_product) || isAbnormalTriangle(triangle_vertecies[i * 3], triangle_vertecies[i * 3 + 1], triangle_vertecies[i * 3 + 2])) {
			continue;
		}
		
		vec3 a = vec3(triangle_vertecies[i * 3].x, triangle_vertecies[i * 3].y,
			triangle_vertecies[i * 3].z);
		GLfloat distance = -Angel::dot(a, norm);
		eye_pos_v3 = vec3(eye_pos.x, eye_pos.y, eye_pos.z);
		GLfloat t = -((dot(eye_pos_v3, norm) + distance)
			/ dot_product);

		//cout << "t = " << t << endl;
		vec3 intersect_point = eye_pos_v3 + ray_wor * t;

		if (t <= 0) {
			continue;
		}
		//cout << "hi" << endl;
		//if (isTwoPointClose(intersect_point, triangle_vertecies[i * 3]) &&
		//	isTwoPointClose(intersect_point, triangle_vertecies[i * 3 + 1]) &&
		//	isTwoPointClose(intersect_point, triangle_vertecies[i * 3 + 2])) {
		//	cout << "intersect point: " << intersect_point << endl
		//		<< "point1: " << triangle_vertecies[i * 3] << endl
		//		<< "point2: " << triangle_vertecies[i * 3 + 1] << endl
		//		<< "point3: " << triangle_vertecies[i * 3 + 2] << endl;
		//}
		//else {
		//	cout << intersect_point << triangle_vertecies[i * 3] << endl;
		//}

		if (PointInTriangleVec4(intersect_point, triangle_vertecies[i * 3],
			triangle_vertecies[i * 3 + 1], triangle_vertecies[i * 3 + 2])) {
			cout << "intersect point: " << intersect_point << endl
				<< "point1: " << triangle_vertecies[i * 3] << endl
				<< "point2: " << triangle_vertecies[i * 3 + 1] << endl
				<< "point3: " << triangle_vertecies[i * 3 + 2] << endl;
			cout << "eye_pos_v3: " << eye_pos_v3 << endl;
			cout << "ray_wor: " << ray_wor << endl;
			cout << "normal: " << norm << endl;
			get_normal2(triangle_vertecies[i * 3], triangle_vertecies[i * 3 + 1], triangle_vertecies[i * 3 + 2]);
			cout << "t = " << t << endl;
			cout << "dot(eye_pos_v3, norm): " << dot(eye_pos_v3, norm) << endl;
			cout << "distance: " << distance << endl;
			cout << "dot_product: " << dot_product << endl;
			cout << "t: " << t << endl;
		/*	PointInTriangleVec4_2(intersect_point, triangle_vertecies[i * 3],
				triangle_vertecies[i * 3 + 1], triangle_vertecies[i * 3 + 2]);*/
			//getchar();
			if (is_initial_state) {
				closest_intersect = intersect_point;
				t_minimal = t;
				is_initial_state = false;
			}
			else if (t < t_minimal) {
				closest_intersect = intersect_point;
				t_minimal = t;
			}
			index = i;
			//cout << "index: " << index << endl;
		}
	}
	return index;
}

GLfloat sign(vec2 p1, vec2 p2, vec2 p3)
{
	return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

bool PointInTriangle(vec2 pt, vec2 v1, vec2 v2, vec2 v3)
{
	bool b1, b2, b3;

	b1 = sign(pt, v1, v2) < 0.0f;
	b2 = sign(pt, v2, v3) < 0.0f;
	b3 = sign(pt, v3, v1) < 0.0f;

	return ((b1 == b2) && (b2 == b3));
}

bool isequal(GLfloat f1, GLfloat f2) {
	return abs(f1 - f2) < 0.0001;
}

bool isequal(GLfloat f1, GLfloat f2, GLfloat f3) {
	return isequal(f1, f2) && isequal(f2, f3);
}
bool PointInTriangleVec4(vec4 pt, vec4 p1, vec4 p2, vec4 p3) {
	bool b1, b2, b3;
	vec2 p21 = vec2(p1.x, p1.y), p22 = vec2(p2.x, p2.y), p23 = vec2(p3.x, p3.y),
		p2t = vec2(pt.x, pt.y);
	b1 = PointInTriangle(p2t, p21, p22, p23);
	vec2 p212 = vec2(p1.y, p1.z), p222 = vec2(p2.y, p2.z), p232 = vec2(p3.y, p3.z),
		p2t2 = vec2(pt.y, pt.z);
	b2 = PointInTriangle(p2t2, p212, p222, p232);
	vec2 p213 = vec2(p1.z, p1.x), p223 = vec2(p2.z, p2.x), p233 = vec2(p3.z, p3.x),
		p2t3 = vec2(pt.z, pt.x);
	b3 = PointInTriangle(p2t3, p213, p223, p233);

	//if (abs(p1.x - p2.x) < 0.0001) {
	//	b2 = true;
	//}
	//if (abs(p1.y - p2.y) < 0.0001) {
	//	b3 = true;
	//}
	//if (abs(p1.z - p2.z) < 0.0001) {
	//	b1 = true;
	//}
	if (isequal(p1.x, p2.x, p3.x)) {
		return b2;
	}
	else if (isequal(p1.y, p2.y, p3.y)) {
		return b3;
	}
	else if (isequal(p1.z, p2.z, p3.z)) {
		return b1;
	}
	else {
		return b1 && b2 && b3;
	}
	
}
