// Draws colored cube  

#include "Angel.h"


int width = 0;
int height = 0;

void display( void );
void keyboard( unsigned char key, int x, int y );
void init(int argc, char **argv);
void addToBuffer(vec4 *vertecies, vec4 *colors, int nfaces);

GLuint g_program;

using namespace std;
//GLuint buffer[2];

GLuint *g_buffers;
int g_bufferSize;
int g_allocatedBufferSize;
void display( void )
{

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );     // clear the window

	Angel::mat4 perspectiveMat = Angel::Perspective((GLfloat)45.0, (GLfloat)width/(GLfloat)height, (GLfloat)0.1, (GLfloat) 100.0);

	float viewMatrixf[16];
	viewMatrixf[0] = perspectiveMat[0][0];viewMatrixf[4] = perspectiveMat[0][1];
	viewMatrixf[1] = perspectiveMat[1][0];viewMatrixf[5] = perspectiveMat[1][1];
	viewMatrixf[2] = perspectiveMat[2][0];viewMatrixf[6] = perspectiveMat[2][1];
	viewMatrixf[3] = perspectiveMat[3][0];viewMatrixf[7] = perspectiveMat[3][1];

	viewMatrixf[8] = perspectiveMat[0][2];viewMatrixf[12] = perspectiveMat[0][3];
	viewMatrixf[9] = perspectiveMat[1][2];viewMatrixf[13] = perspectiveMat[1][3];
	viewMatrixf[10] = perspectiveMat[2][2];viewMatrixf[14] = perspectiveMat[2][3];
	viewMatrixf[11] = perspectiveMat[3][2];viewMatrixf[15] = perspectiveMat[3][3];
	
	Angel::mat4 modelMat = Angel::identity();
	modelMat = modelMat * Angel::Translate(0.0, 0.0, -2.0f) * Angel::RotateY(45.0f) * Angel::RotateX(35.0f);
	float modelMatrixf[16];
	modelMatrixf[0] = modelMat[0][0];modelMatrixf[4] = modelMat[0][1];
	modelMatrixf[1] = modelMat[1][0];modelMatrixf[5] = modelMat[1][1];
	modelMatrixf[2] = modelMat[2][0];modelMatrixf[6] = modelMat[2][1];
	modelMatrixf[3] = modelMat[3][0];modelMatrixf[7] = modelMat[3][1];

	modelMatrixf[8] = modelMat[0][2];modelMatrixf[12] = modelMat[0][3];
	modelMatrixf[9] = modelMat[1][2];modelMatrixf[13] = modelMat[1][3];
	modelMatrixf[10] = modelMat[2][2];modelMatrixf[14] = modelMat[2][3];
	modelMatrixf[11] = modelMat[3][2];modelMatrixf[15] = modelMat[3][3];
	
	// set up projection matricies
	GLuint modelMatrix = glGetUniformLocationARB(g_program, "model_matrix");
	glUniformMatrix4fv( modelMatrix, 1, GL_FALSE, modelMatrixf );
	GLuint viewMatrix = glGetUniformLocationARB(g_program, "projection_matrix");
	glUniformMatrix4fv( viewMatrix, 1, GL_FALSE, viewMatrixf);

    glFlush(); // force output to graphics hardware

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_DEPTH_TEST);

	glBindBuffer(GL_ARRAY_BUFFER, g_buffers[0]);
	GLuint vPosition = glGetAttribLocation(g_program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(g_program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vec4)* 3));
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindBuffer(GL_ARRAY_BUFFER, g_buffers[1]);
	vPosition = glGetAttribLocation(g_program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	vColor = glGetAttribLocation(g_program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vec4)* 3));
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDisable(GL_DEPTH_TEST);
	// use this call to double buffer
	glutSwapBuffers();
	// you can implement your own buffers with textures
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

void loadSTLFile() {
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
	g_allocatedBufferSize = 1;
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

int main(int argc, char **argv) {
	
	init(argc, argv);
	
	loadSTLFile();

	glClearColor(1, 1, 1, 1);

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return 0;
}

void addToBuffer(vec4 *vertecies, vec4 *colors, int nfaces) {
	cout << g_bufferSize << endl;
	if (g_bufferSize < g_allocatedBufferSize) {
	

		glBindBuffer(GL_ARRAY_BUFFER, g_buffers[g_bufferSize]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec4)* nfaces * 2, NULL, GL_STATIC_DRAW);

		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec4)* nfaces, vertecies);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4)* nfaces, sizeof(vec4)* nfaces, colors);

		g_bufferSize++;
	}
	else {
		GLuint *newBuffers = (GLuint*)malloc(sizeof(GLuint)* g_allocatedBufferSize + 5);
		for (int i = 0; i < g_allocatedBufferSize; i++) {
			newBuffers[i] = g_buffers[i];
		}

		g_buffers = newBuffers;
		g_allocatedBufferSize += 5;
		cout << g_allocatedBufferSize << endl;
		addToBuffer(vertecies, colors, nfaces);
	}
}
