// Draws colored cube  

#include "Angel.h"

using namespace std;

int width = 0;
int height = 0;

void display( void );
void keyboard( unsigned char key, int x, int y );
void init(int argc, char **argv);
void addToBuffer(vec4 *vertecies, vec4 *colors, int nfaces);
void enableBuffer(int index, int size);
void allocateMatrix(GLfloat* array, Angel::mat4 matrix);
vec4* allocRandomColor(int nfaces);
void loadSTLfile(string _filename, vec4** _facebuffer, int* _nfacebuffer);

GLuint g_program;


//GLuint buffer[2];

GLuint *g_buffers;
int g_bufferSize;
int g_allocatedBufferSize;

int g_nfaces;
void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Angel::mat4 perspectiveMat = Angel::Perspective((GLfloat)45.0, (GLfloat)width / (GLfloat)height, (GLfloat)0.1, (GLfloat) 100.0);

	float viewMatrixf[16];
	allocateMatrix(viewMatrixf, perspectiveMat);
	Angel::mat4 modelMat = Angel::identity();
	float s = 1;
	modelMat = modelMat * Angel::Translate(0.0, 0.0, -10.0f) * Angel::RotateY(45.0f) * Angel::RotateX(35.0f) * Angel::Scale(s, s, s);
	float modelMatrixf[16];
	allocateMatrix(modelMatrixf, modelMat);

	GLuint modelMatrix = glGetUniformLocationARB(g_program, "model_matrix");
	glUniformMatrix4fv( modelMatrix, 1, GL_FALSE, modelMatrixf );
	GLuint viewMatrix = glGetUniformLocationARB(g_program, "projection_matrix");
	glUniformMatrix4fv( viewMatrix, 1, GL_FALSE, viewMatrixf);

	glFlush();

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_DEPTH_TEST);
	
	enableBuffer(0, 1);
	glDrawArrays(GL_TRIANGLES, 0, g_nfaces);

	glDisable(GL_DEPTH_TEST);
	glutSwapBuffers();

}
//void display( void )
//{
//
//    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );     // clear the window
//
//	Angel::mat4 perspectiveMat = Angel::Perspective((GLfloat)45.0, (GLfloat)width/(GLfloat)height, (GLfloat)0.1, (GLfloat) 100.0);
//
//	float viewMatrixf[16];
//	allocateMatrix(viewMatrixf, perspectiveMat);
//	
//	Angel::mat4 modelMat = Angel::identity();
//	modelMat = modelMat * Angel::Translate(0.0, 0.0, -2.0f) * Angel::RotateY(45.0f) * Angel::RotateX(35.0f);
//	float modelMatrixf[16];
//	allocateMatrix(modelMatrixf, modelMat);
//	
//	// set up projection matricies
//	GLuint modelMatrix = glGetUniformLocationARB(g_program, "model_matrix");
//	glUniformMatrix4fv( modelMatrix, 1, GL_FALSE, modelMatrixf );
//	GLuint viewMatrix = glGetUniformLocationARB(g_program, "projection_matrix");
//	glUniformMatrix4fv( viewMatrix, 1, GL_FALSE, viewMatrixf);
//
//    glFlush(); // force output to graphics hardware
//
//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//	glEnable(GL_DEPTH_TEST);
//
//	enableBuffer(0, 1);
//	glDrawArrays(GL_TRIANGLES, 0, 3);
//
//	enableBuffer(1, 1);
//	glDrawArrays(GL_TRIANGLES, 0, 3);
//	glDisable(GL_DEPTH_TEST);
//	// use this call to double buffer
//	glutSwapBuffers();
//	// you can implement your own buffers with textures
//}

void enableBuffer(int index, int size) {
	glBindBuffer(GL_ARRAY_BUFFER, g_buffers[index]);
	GLuint vPosition = glGetAttribLocation(g_program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(g_program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vec4)* 3 * size));
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

void init(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512, 512);

	width = 512;
	height = 512;

	glutCreateWindow("display");

	glewInit();


}

void genTestTriangle() {
	int nfaces = 3;
	vec4 *vertecies = (vec4*)malloc(sizeof(vec4)* nfaces);
	vec4 *colors = (vec4*)malloc(sizeof(vec4)* nfaces);

	vertecies[0] = vec4(0, 0, 0, 1);
	vertecies[1] = vec4(0.5, 0.5, 0, 1);
	vertecies[2] = vec4(0.5, -0.5, 0, 1);

	colors[0] = vec4(0, 0, 0, 1);
	colors[1] = vec4(0, 0, 0, 1);
	colors[2] = vec4(0, 0, 0, 1);

	GLuint *vao;
	glGenVertexArrays(2, vao);
	glBindVertexArray(vao[0]);

	g_buffers = (GLuint*)malloc(sizeof(GLuint)* 2);
	g_allocatedBufferSize = 2;
	g_bufferSize = 0;

	glGenBuffers(2, g_buffers);

	addToBuffer(vertecies, colors, nfaces);

	g_program = InitShader("vshader1.glsl", "fshader1.glsl");
	glUseProgram(g_program);

	vec4 *vertecies2 = (vec4*)malloc(sizeof(vec4)* nfaces);
	vertecies2[0] = vec4(0, 0, 0, 1);
	vertecies2[1] = vec4(-0.5, -0.5, 0, 1);
	vertecies2[2] = vec4(-0.5, 0.5, 0, 1);
	addToBuffer(vertecies2, colors, nfaces);


}

void initBuffer(int buffer_size) {

	GLuint *vao;
	glGenVertexArrays(2, vao);
	glBindVertexArray(vao[0]);

	g_buffers = (GLuint*)malloc(sizeof(GLuint)* buffer_size);
	g_allocatedBufferSize = buffer_size;
	g_bufferSize = 0;

	glGenBuffers(buffer_size, g_buffers);
}

void loadVertecies() {
	int nfaces;
	vec4 *vertecies;
	loadSTLfile("liver.stl", &vertecies, &nfaces);
	g_nfaces = nfaces;
	vec4 *colors = allocRandomColor(nfaces);

	initBuffer(1);

	g_program = InitShader("vshader1.glsl", "fshader1.glsl");
	glUseProgram(g_program);

	addToBuffer(vertecies, colors, nfaces);

	//for (int i = 0; i < 10; i++) {
	//	cout << vertecies[i] << endl;
	//}
}

vec4* allocRandomColor(int nfaces) {
	vec4 *colors = (vec4*)malloc(sizeof(vec4)* nfaces * 3);
	for (int i = 0; i < nfaces * 3; i++) {
		colors[i] = (float)rand() / RAND_MAX;
	}

	return colors;
}
int main(int argc, char **argv) {
	srand(1);
	init(argc, argv);
	
	//genTestTriangle();
	loadVertecies();
	glClearColor(1, 1, 1, 1);

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return 0;
}

void addToBuffer(vec4 *vertecies, vec4 *colors, int nfaces) {
	if (g_bufferSize < g_allocatedBufferSize) {

		glBindBuffer(GL_ARRAY_BUFFER, g_buffers[g_bufferSize]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec4)* nfaces * 2, NULL, GL_STATIC_DRAW);

		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec4)* nfaces, vertecies);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4)* nfaces, sizeof(vec4)* nfaces, colors);

		g_bufferSize++;
	}
	else {
		cout << "exceed buffer size limit" << endl;
	}
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
	cout << TriangleNum << endl;
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

	vec4* facebuffer = (vec4*)malloc(sizeof(vec4)* TriangleNum * 3);
	for (int i = 0; i < TriangleNum; i++) {
		vec4 points[3];
		for (int j = 0; j < 3; j++) {
			points[j] = vec4(Triangle[j * 3 + 3][i] / 200, Triangle[j * 3 + 4][i] / 200, Triangle[j * 3 + 5][i] / 200, 1);
		}
		facebuffer[i * 3] = points[0];
		facebuffer[i * 3 + 1] = points[1];
		facebuffer[i * 3 + 2] = points[2];
	}

	*_facebuffer = facebuffer;
	*_nfacebuffer = TriangleNum;
}