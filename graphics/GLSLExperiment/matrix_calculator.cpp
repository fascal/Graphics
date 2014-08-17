#include "matrix_calculator.h"

/*
assign the matrix of matrix form to column major array form
*/
void allocateMatrix(GLfloat* array, Angel::mat4 matrix) {
	int i = 0, j = 0, m;
	for (m = 0; m < 16; m++) {
		array[m] = matrix[i][j];
		i++;
		if (i == 4) {
			i = 0;
			j++;
		}
	}
}

void allocateMatrix(GLdouble* array, Angel::mat4 matrix) {
	int i = 0, j = 0, m;
	for (m = 0; m < 16; m++) {
		array[m] = matrix[i][j];
		i++;
		if (i == 4) {
			i = 0;
			j++;
		}
	}
}

/**
points: an array of coordinates of the vertecies
return an matrix that centers the 3d model
*/
mat4 getCenteringMatrix(vec4* _points, int _npoints) {
	GLfloat minX = 0, minY = 0, minZ = 0, maxX = 0, maxY = 0, maxZ = 0;
	find3DBounds(_points, _npoints, &minX, &minY, &minZ, &maxX, &maxY, &maxZ);
	return Angel::Translate(-(maxX + minX) / 2, -(maxY + minY) / 2, -(maxZ + minZ) / 2);
}

/*
points: an array of coordiante of the vertecies
return an matrix that scale the 3d model within [-1, 1] on each dimension
*/
mat4 getScalingMatrix(vec4* _points, int _npoints) {
	GLfloat minX = 0, minY = 0, minZ = 0, maxX = 0, maxY = 0, maxZ = 0;
	find3DBounds(_points, _npoints, &minX, &minY, &minZ, &maxX, &maxY, &maxZ);
	GLfloat maxDiff = maxX - minX;
	if (maxY - minY > maxDiff) {
		maxDiff = maxY - minY;
	}

	if (maxZ - minZ > maxDiff) {
		maxDiff = maxZ - minZ;
	}
	return Angel::Scale(1 / maxDiff, 1 / maxDiff, 1 / maxDiff);
}

/*
assign the matrix of array form to mat4 form
*/
mat4 arrayToMat(GLfloat* array) {
	int m;
	int i = 0, j = 0;
	mat4 mat;
	for (m = 0; m < 16; m++) {
		mat[i][j] = array[m];
		i++;
		if (i == 4) {
			i = 0;
			j++;
		}
	}
	return mat;
}

bool gluInvertMatrix(const GLfloat m[16], GLfloat invOut[16])
{
	GLfloat inv[16], det;
	int i;

	inv[0] = m[5] * m[10] * m[15] -
		m[5] * m[11] * m[14] -
		m[9] * m[6] * m[15] +
		m[9] * m[7] * m[14] +
		m[13] * m[6] * m[11] -
		m[13] * m[7] * m[10];

	inv[4] = -m[4] * m[10] * m[15] +
		m[4] * m[11] * m[14] +
		m[8] * m[6] * m[15] -
		m[8] * m[7] * m[14] -
		m[12] * m[6] * m[11] +
		m[12] * m[7] * m[10];

	inv[8] = m[4] * m[9] * m[15] -
		m[4] * m[11] * m[13] -
		m[8] * m[5] * m[15] +
		m[8] * m[7] * m[13] +
		m[12] * m[5] * m[11] -
		m[12] * m[7] * m[9];

	inv[12] = -m[4] * m[9] * m[14] +
		m[4] * m[10] * m[13] +
		m[8] * m[5] * m[14] -
		m[8] * m[6] * m[13] -
		m[12] * m[5] * m[10] +
		m[12] * m[6] * m[9];

	inv[1] = -m[1] * m[10] * m[15] +
		m[1] * m[11] * m[14] +
		m[9] * m[2] * m[15] -
		m[9] * m[3] * m[14] -
		m[13] * m[2] * m[11] +
		m[13] * m[3] * m[10];

	inv[5] = m[0] * m[10] * m[15] -
		m[0] * m[11] * m[14] -
		m[8] * m[2] * m[15] +
		m[8] * m[3] * m[14] +
		m[12] * m[2] * m[11] -
		m[12] * m[3] * m[10];

	inv[9] = -m[0] * m[9] * m[15] +
		m[0] * m[11] * m[13] +
		m[8] * m[1] * m[15] -
		m[8] * m[3] * m[13] -
		m[12] * m[1] * m[11] +
		m[12] * m[3] * m[9];

	inv[13] = m[0] * m[9] * m[14] -
		m[0] * m[10] * m[13] -
		m[8] * m[1] * m[14] +
		m[8] * m[2] * m[13] +
		m[12] * m[1] * m[10] -
		m[12] * m[2] * m[9];

	inv[2] = m[1] * m[6] * m[15] -
		m[1] * m[7] * m[14] -
		m[5] * m[2] * m[15] +
		m[5] * m[3] * m[14] +
		m[13] * m[2] * m[7] -
		m[13] * m[3] * m[6];

	inv[6] = -m[0] * m[6] * m[15] +
		m[0] * m[7] * m[14] +
		m[4] * m[2] * m[15] -
		m[4] * m[3] * m[14] -
		m[12] * m[2] * m[7] +
		m[12] * m[3] * m[6];

	inv[10] = m[0] * m[5] * m[15] -
		m[0] * m[7] * m[13] -
		m[4] * m[1] * m[15] +
		m[4] * m[3] * m[13] +
		m[12] * m[1] * m[7] -
		m[12] * m[3] * m[5];

	inv[14] = -m[0] * m[5] * m[14] +
		m[0] * m[6] * m[13] +
		m[4] * m[1] * m[14] -
		m[4] * m[2] * m[13] -
		m[12] * m[1] * m[6] +
		m[12] * m[2] * m[5];

	inv[3] = -m[1] * m[6] * m[11] +
		m[1] * m[7] * m[10] +
		m[5] * m[2] * m[11] -
		m[5] * m[3] * m[10] -
		m[9] * m[2] * m[7] +
		m[9] * m[3] * m[6];

	inv[7] = m[0] * m[6] * m[11] -
		m[0] * m[7] * m[10] -
		m[4] * m[2] * m[11] +
		m[4] * m[3] * m[10] +
		m[8] * m[2] * m[7] -
		m[8] * m[3] * m[6];

	inv[11] = -m[0] * m[5] * m[11] +
		m[0] * m[7] * m[9] +
		m[4] * m[1] * m[11] -
		m[4] * m[3] * m[9] -
		m[8] * m[1] * m[7] +
		m[8] * m[3] * m[5];

	inv[15] = m[0] * m[5] * m[10] -
		m[0] * m[6] * m[9] -
		m[4] * m[1] * m[10] +
		m[4] * m[2] * m[9] +
		m[8] * m[1] * m[6] -
		m[8] * m[2] * m[5];

	det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

	if (det == 0)
		return false;

	det = (GLfloat)1.0 / det;

	for (i = 0; i < 16; i++)
		invOut[i] = inv[i] * det;

	return true;
}

/*
assign the inverse matrix of m to inv
*/
void gluInvertMatrix(mat4 m, mat4 &inv) {

	GLfloat a[16];

	GLfloat *aa = (GLfloat*)malloc(sizeof(GLfloat)* 16);
	allocateMatrix(aa, m);

	for (int i = 0; i < 16; i++) {
		a[i] = aa[i];
	}

	free(aa);

	GLfloat invOut[16];

	gluInvertMatrix(a, invOut);
	inv = arrayToMat(invOut);

}

//comsume a matrix for translation and return its 
//reverse translation matrix
mat4 negativeMatrix(mat4 _matrix) {
	mat4 mat = _matrix;
	int i;
	for (i = 0; i < 3; i++) {
		mat[i][3] = -_matrix[i][3];
	}

	return mat;
}

void findBounds(GLfloat* _array, int _size, GLfloat* _min, GLfloat* _max) {
	int i;
	GLfloat min = 0, max = 0;
	for (i = 0; i < _size; i++) {
		if (i == 0) {
			min = _array[i];
			max = _array[i];
		}
		else {
			if (_array[i] < min) {
				min = _array[i];
			}
			else if (_array[i] > max) {
				max = _array[i];
			}
		}
	}

	*_min = min;
	*_max = max;
}

/*
find bounds for all dimension for given coordinate array
*/
void find3DBounds(vec4* _points, int _npoints, GLfloat *_minX, GLfloat *_minY, GLfloat *_minZ, GLfloat *_maxX, GLfloat *_maxY, GLfloat *_maxZ) {
	GLfloat *coordX, *coordY, *coordZ;
	coordX = (GLfloat*)malloc(sizeof(GLfloat)* _npoints);
	coordY = (GLfloat*)malloc(sizeof(GLfloat)* _npoints);
	coordZ = (GLfloat*)malloc(sizeof(GLfloat)* _npoints);

	int i;
	for (i = 0; i < _npoints; i++) {
		coordX[i] = _points[i].x;
		coordY[i] = _points[i].y;
		coordZ[i] = _points[i].z;
	}

	GLfloat minX, minY, minZ, maxX, maxY, maxZ;

	findBounds(coordX, _npoints, &minX, &maxX);
	findBounds(coordY, _npoints, &minY, &maxY);
	findBounds(coordZ, _npoints, &minZ, &maxZ);

	*_minX = minX;
	*_minY = minY;
	*_minZ = minZ;
	*_maxX = maxX;
	*_maxY = maxY;
	*_maxZ = maxZ;

}