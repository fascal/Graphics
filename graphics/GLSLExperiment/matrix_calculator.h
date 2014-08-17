#ifndef MATRIX_CALCULATOR
#define MATRIX_CALCULATOR
#include "Angel.h"

void allocateMatrix(GLfloat* array, Angel::mat4 matrix);
mat4 getCenteringMatrix(vec4* _points, int _npoints);
mat4 getScalingMatrix(vec4* _points, int _npoints);
mat4 arrayToMat(GLfloat* array);
bool gluInvertMatrix(const GLfloat m[16], GLfloat invOut[16]);
void gluInvertMatrix(mat4 m, mat4 &inv);
mat4 negativeMatrix(mat4 _matrix);
void find3DBounds(vec4* _points, int _npoints, GLfloat *_minX, GLfloat *_minY, GLfloat *_minZ, GLfloat *_maxX, GLfloat *_maxY, GLfloat *_maxZ);
void findBounds(GLfloat* _array, int _size, GLfloat* _min, GLfloat* _max);
void allocateMatrix(GLdouble* array, Angel::mat4 matrix);
#endif // !MATRIX_CALCULATOR

