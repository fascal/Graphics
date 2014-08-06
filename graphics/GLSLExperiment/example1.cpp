// Draws colored cube  

#include "Angel.h"


//----------------------------------------------------------------------------
int width = 0;
int height = 0;

void display( void );
void keyboard( unsigned char key, int x, int y );

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

// handle to program
GLuint program;

using namespace std;


//----------------------------------------------------------------------------
// this is where the drawing should happen
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
	GLuint modelMatrix = glGetUniformLocationARB(program, "model_matrix");
	glUniformMatrix4fv( modelMatrix, 1, GL_FALSE, modelMatrixf );
	GLuint viewMatrix = glGetUniformLocationARB(program, "projection_matrix");
	glUniformMatrix4fv( viewMatrix, 1, GL_FALSE, viewMatrixf);

    glFlush(); // force output to graphics hardware

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDisable(GL_DEPTH_TEST);
	// use this call to double buffer
	glutSwapBuffers();
	// you can implement your own buffers with textures
}

//----------------------------------------------------------------------------

// keyboard handler
void keyboard( unsigned char key, int x, int y )
{
    switch ( key ) {
    case 033:
        exit( EXIT_SUCCESS );
        break;
    }
}

//----------------------------------------------------------------------------
// entry point
//int main( int argc, char **argv )
//{
//	// init glut
//    glutInit( &argc, argv );
//    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
//    glutInitWindowSize( 512, 512 );
//	width = 512;
//	height = 512;
//
//	// create window
//	// opengl can be incorperated into other packages like wxwidgets, fltoolkit, etc.
//    glutCreateWindow( "Color Cube" );
//
//	// init glew
//    glewInit();
//
//    generateGeometry();
//
//	// assign handlers
//    glutDisplayFunc( display );
//    glutKeyboardFunc( keyboard );
//	// should add menus
//	// add mouse handler
//	// add resize window functionality (should probably try to preserve aspect ratio)
//
//	// enter the drawing loop
//	// frame rate can be controlled with 
//    glutMainLoop();
//    return 0;
//}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	width = 512;
	height = 512;

	glutCreateWindow("display");

	glewInit();
	int nfaces = 3;
	vec4 *vertecies = (vec4*)malloc(sizeof(vec4)* nfaces);
	vec4 *colors = (vec4*)malloc(sizeof(vec4)* nfaces);

	vertecies[0] = vec4(0, 0, 0, 1);
	vertecies[1] = vec4(0.5, 0.5, 0, 1);
	vertecies[2] = vec4(0.5, -0.5, 0, 1);

	colors[0] = vec4(0, 0, 0, 1);
	colors[1] = vec4(0, 0, 0, 1);
	colors[2] = vec4(0, 0, 0, 1);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4)* nfaces * 2, NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec4)* nfaces, vertecies);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4)* nfaces, sizeof(vec4)* nfaces, colors);

	program = InitShader("vshader1.glsl", "fshader1.glsl");
	glUseProgram(program);

	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vec4)* nfaces));

	glClearColor(1, 1, 1, 1);

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	cout << 3 << endl;
	getchar();
	return 0;
}