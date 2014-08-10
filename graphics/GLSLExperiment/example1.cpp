// Draws colored cube  

#include "Angel.h"

using namespace std;

#pragma region declare variable

void display( void );
void keyboard( unsigned char key, int x, int y );
void init(int argc, char **argv);
void addToBuffer(vec4 *vertecies, vec4 *colors, int nfaces);
void enableBuffer(int index, int size);
void allocateMatrix(GLfloat* array, Angel::mat4 matrix);
vec4* allocRandomColor(int nfaces);
void loadSTLfile(string _filename, vec4** _facebuffer, int* _nfacebuffer);
mat4 getCenteringMatrix(vec4* _points, int _npoints);
mat4 getScalingMatrix(vec4* _points, int _npoints);
void loadSTLfile(string _filename, vec4** _facebuffer, int* _nfacebuffer);
void loadVertecies();

GLuint g_program;

int width = 0;
int height = 0;

GLuint *g_buffers;
int g_bufferSize, g_nfaces, g_allocatedBufferSize;
mat4 g_nomMat, g_centerMat;
mat4 g_scaleMat, g_transMat;
vec2 g_prevMousePos = NULL;
int g_mouseState, g_mouseButton, g_mouseFlow = 0;
//mouse flow == 0 when no button pressed
//mouse flow == 1 when the left button first pressed
//mouse flow returns to 0 when left button released
#pragma endregion

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Angel::mat4 perspectiveMat = Angel::Perspective((GLfloat)45.0, (GLfloat)width / (GLfloat)height, (GLfloat)0.1, (GLfloat) 100.0);

	float viewMatrixf[16];
	allocateMatrix(viewMatrixf, perspectiveMat);
	Angel::mat4 modelMat = Angel::identity();
	modelMat = modelMat * g_nomMat * g_centerMat *g_transMat;
	float modelMatrixf[16];
	allocateMatrix(modelMatrixf, modelMat);

	GLuint modelMatrix = glGetUniformLocationARB(g_program, "model_matrix");
	glUniformMatrix4fv( modelMatrix, 1, GL_FALSE, modelMatrixf );
	GLuint viewMatrix = glGetUniformLocationARB(g_program, "projection_matrix");
	glUniformMatrix4fv( viewMatrix, 1, GL_FALSE, viewMatrixf);

	glFlush();

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_DEPTH_TEST);
	
	enableBuffer(0, g_nfaces);
	glDrawArrays(GL_TRIANGLES, 0, g_nfaces * 3);

	glDisable(GL_DEPTH_TEST);
	glutSwapBuffers();

}

// keyboard handler
void keyboard( unsigned char key, int x, int y )
{
    switch ( key ) {
    case 033:
        exit( EXIT_SUCCESS );
        break;
    }
}

void mouseFunc(int button, int state, int x, int y) {
	g_mouseButton = button;
	g_mouseState = state;
	if (state == GLUT_UP) {
		g_mouseFlow = 0;
	}
}

void motionMouseFunc(int x, int y) {

	if (g_mouseFlow == 0) {
		g_prevMousePos = vec2((double)x, (double)y);
		g_mouseFlow = 1;
	}
	else if (g_mouseState == GLUT_DOWN) {
		if (g_mouseButton == GLUT_LEFT_BUTTON) {
			int transX = x - g_prevMousePos[0];
			int transY = y - g_prevMousePos[1];
			display();
			g_transMat *= Angel::Translate(vec4(transX, -transY, 0, 1));
			g_prevMousePos = vec2((double)x, (double)y);
		}
	}


}
void init(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512, 512);

	width = 512;
	height = 512;

	glutCreateWindow("display");

	glewInit();
}

int main(int argc, char **argv) {
	srand(1);
	init(argc, argv);

	loadVertecies();
	glClearColor(1, 1, 1, 1);

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouseFunc);
	glutMotionFunc(motionMouseFunc);
	glutMainLoop();

	return 0;
}

#pragma region buffers operation
void initBuffer(int buffer_size) {

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	g_buffers = (GLuint*)malloc(sizeof(GLuint)* buffer_size);
	g_allocatedBufferSize = buffer_size;
	g_bufferSize = 0;

	glGenBuffers(buffer_size, g_buffers);

	g_scaleMat = Angel::identity();
	g_transMat = Angel::identity();
}

void enableBuffer(int index, int size) {
	glBindBuffer(GL_ARRAY_BUFFER, g_buffers[index]);
	GLuint vPosition = glGetAttribLocation(g_program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(g_program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vec4)* 3 * size));
}

void loadVertecies() {
	int nfaces;
	vec4 *vertecies;
	loadSTLfile("liver.stl", &vertecies, &nfaces);
	g_nfaces = nfaces;
	vec4 *colors = allocRandomColor(nfaces);

	//for (int i = 0; i < 10; i++) {
	//	cout << colors[i] << endl;
	//}
	initBuffer(1);

	g_program = InitShader("vshader1.glsl", "fshader1.glsl");
	glUseProgram(g_program);

	addToBuffer(vertecies, colors, nfaces);

	g_nomMat = getScalingMatrix(vertecies, nfaces * 3);
	g_centerMat = getCenteringMatrix(vertecies, nfaces * 3);

	/*for (int i = 0; i < nfaces; i++) {
		cout << vertecies[i][2] << endl;
	}*/
}

vec4* allocRandomColor(int nfaces) {
	vec4 *colors = (vec4*)malloc(sizeof(vec4)* nfaces * 3);
	for (int i = 0; i < nfaces * 3; i++) {
		float r1 = rand() / (float)RAND_MAX, r2 = rand() / (float)RAND_MAX, r3 = rand() / (float)RAND_MAX;
		//cout << r1 << r2 << r3 << endl;
		colors[i] = vec4(r1, r2, r3, 1);
	}

	return colors;
}
#pragma endregion

#pragma region vertecies loading

void addToBuffer(vec4 *vertecies, vec4 *colors, int nfaces) {
	if (g_bufferSize < g_allocatedBufferSize) {

		glBindBuffer(GL_ARRAY_BUFFER, g_buffers[g_bufferSize]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec4)* nfaces * 3* 2, NULL, GL_STATIC_DRAW);

		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec4)* nfaces * 3, vertecies);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4)* nfaces * 3, sizeof(vec4)* nfaces * 3, colors);

		g_bufferSize++;
	}
	else {
		cout << "exceed buffer size limit" << endl;
	}
}

void loadSTLfile(string _filename, vec4** _facebuffer, int* _nfacebuffer) {
	int filenamelength = _filename.length();
	char *pathname = (char *)malloc(sizeof(char)* filenamelength);
	for (int i = 0; i < filenamelength; i++) {
		pathname[i] = _filename[i];
	}
	pathname[filenamelength] = '\0';
	int TriangleNum;

	FILE *fp = fopen(pathname, "rb");
	printf("%s\n", pathname);
	fseek(fp, 80 * sizeof(char), 0);
	fread(&TriangleNum, sizeof(int), 1, fp);
	//cout << TriangleNum << endl;
	float data;
	float col[2];

	float **Triangle = (float**)malloc(sizeof(float*)* 12);

	for (int j = 0; j < 12; j++)
	{
		Triangle[j] = new float[TriangleNum];
	}

	for (int i = 0; i < TriangleNum; i++)
	{
		for (int j = 0; j < 12; j++)
		{
			fread(&data, sizeof(float), 1, fp);
			Triangle[j][i] = data;
		}
		fread(&col, sizeof(char), 2, fp);
	}
	fclose(fp);
	int factor = 1;
	vec4* facebuffer = (vec4*)malloc(sizeof(vec4)* TriangleNum * 3);
	for (int i = 0; i < TriangleNum; i++) {
		vec4 points[3];
		for (int j = 0; j < 3; j++) {
			points[j] = vec4(Triangle[j * 3 + 3][i] / factor, Triangle[j * 3 + 4][i] / factor, Triangle[j * 3 + 5][i] / factor, 1);
		}
		facebuffer[i * 3] = points[0];
		facebuffer[i * 3 + 1] = points[1];
		facebuffer[i * 3 + 2] = points[2];
	}

	*_facebuffer = facebuffer;
	*_nfacebuffer = TriangleNum;
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

#pragma endregion

#pragma region matrix calculator
/**
points: an array of coordinates of the vertecies
return an matrix that centers the 3d model
*/
mat4 getCenteringMatrix(vec4* _points, int _npoints) {
	GLfloat minX = 0, minY = 0, minZ = 0, maxX = 0, maxY = 0, maxZ = 0;
	find3DBounds(_points, _npoints, &minX, &minY, &minZ, &maxX, &maxY, &maxZ);
	return Angel::Translate(-(maxX + minX) / 2, -(maxY + minY) / 2, -(maxZ + minZ) / 2);
}

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

mat4 arrayToMat(double* array) {
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

bool gluInvertMatrix(const double m[16], double invOut[16])
{
	double inv[16], det;
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

	det = (double)1.0 / det;

	for (i = 0; i < 16; i++)
		invOut[i] = inv[i] * det;

	return true;
}

void gluInvertMatrix(mat4 m, mat4 &inv) {

	double a[16];

	GLfloat *aa = (GLfloat*)malloc(sizeof(GLfloat)* 16);
	allocateMatrix(aa, m);

	for (int i = 0; i < 16; i++) {
		a[i] = aa[i];
	}

	free(aa);

	double invOut[16];

	gluInvertMatrix(a, invOut);
	inv = arrayToMat(invOut);

}

#pragma endregion