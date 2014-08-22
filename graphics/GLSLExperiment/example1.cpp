// Draws colored cube  

#include "Angel.h"
#include "matrix_calculator.h"
#include "ray_trace.h"

using namespace std;

#pragma region declare variable

void display( void );
void keyboard( unsigned char key, int x, int y );
void init(int argc, char **argv);
void addToBuffer(vec4 *vertecies, vec4 *colors, int nfaces);
void enableBuffer(int index, int size);
vec4* allocRandomColor(int nfaces);
void loadSTLfile(string _filename, vec4** _facebuffer, int* _nfacebuffer);
void loadSTLfile(string _filename, vec4** _facebuffer, int* _nfacebuffer);
void loadVertecies();
void genTestVertecies();
vec4 get_normal(vec4 a, vec4 b, vec4 c);

GLuint g_program;

int width = 0;
int height = 0;

GLuint *g_buffers;
int g_bufferSize, g_nfaces, g_allocatedBufferSize;
mat4 g_nomMat, g_centerMat;
mat4 g_scaleMat, g_transMat, g_rotate;
mat4 g_perspectiveMat, g_mvmat;
vec2 g_prevMousePos = NULL;
int g_mouseState, g_mouseButton, g_mouseFlow = 0;
bool g_r_toggle = false;

vec4 *g_vertecies, *g_colors, *g_norms;
vec4 g_v1, g_v2;
bool g_is_line_ready;// = false;
//mouse flow == 0 when no button pressed
//mouse flow == 1 when the left button first pressed
//mouse flow returns to 0 when left button released
#pragma endregion
void drawLine(vec4 v1, vec4 v2) {
	GLuint buffer;
	glGenBuffers(1, &buffer);

	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4)* 2 * 2, NULL, GL_STATIC_DRAW);
	vec4 *vertecies = (vec4*)malloc(sizeof(vec4)* 2);
	vertecies[0] = v1;
	vertecies[1] = v2;
	vec4 *colors = (vec4*)malloc(sizeof(vec4)* 2);
	colors[0] = vec4(1, 0, 0, 1);
	colors[1] = vec4(1, 0, 0, 1);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec4)* 2, vertecies);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4)* 2, sizeof(vec4)* 2, colors);
	GLuint vPosition = glGetAttribLocation(g_program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(g_program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vec4)* 2));

	glDrawArrays(GL_LINE_STRIP, 0, 2);

}
void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Angel::mat4 perspectiveMat = Angel::Perspective((GLfloat)45.0, (GLfloat)width / (GLfloat)height, (GLfloat)0.1, (GLfloat) 100.0);
	//Angel::mat4 perspectiveMat = Angel::Ortho(-1, 1, -1, 1, -1, 1);
	//mat4 perspectiveMat = Angel::LookAt(vec4(0, 0, 0, 1), vec4(0, 0, -1, 1), vec4(0, 1, 0, 1));
	Angel::mat4 perspectiveMat = Angel::Ortho(-1, 1, -1, 1, 1, -1);
	float viewMatrixf[16];
	allocateMatrix(viewMatrixf, perspectiveMat);
	Angel::mat4 modelMat = Angel::identity();

	mat4 revRotate;
	gluInvertMatrix(g_rotate, revRotate);
	modelMat = modelMat * g_nomMat * g_scaleMat * g_rotate * g_centerMat * revRotate * g_transMat * g_rotate;
	float modelMatrixf[16];
	allocateMatrix(modelMatrixf, modelMat);

	GLuint modelMatrix = glGetUniformLocationARB(g_program, "model_matrix");
	glUniformMatrix4fv( modelMatrix, 1, GL_FALSE, modelMatrixf );
	GLuint viewMatrix = glGetUniformLocationARB(g_program, "projection_matrix");
	glUniformMatrix4fv( viewMatrix, 1, GL_FALSE, viewMatrixf);

	glFlush();


	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	if (g_is_line_ready) {
		drawLine(g_v1, g_v2);
	}
	enableBuffer(0, g_nfaces);
	glDrawArrays(GL_TRIANGLES, 0, g_nfaces * 3);

	glDisable(GL_DEPTH_TEST);

	glutSwapBuffers();

	g_perspectiveMat = perspectiveMat;
	g_mvmat = modelMat;
}


#pragma region user control
void keyboard( unsigned char key, int x, int y )
{ 
    switch ( key ) {
    case 033:
        exit( EXIT_SUCCESS );
        break;
	case 'R':
		g_transMat = Angel::identity();
		g_rotate = Angel::identity();
		g_scaleMat = Angel::identity();
		break;
	case 'r':
		g_r_toggle = !(g_r_toggle);
		break;
	case 'Z': 
		g_transMat *= Angel::Translate(0, 0, 1);
		break;
	case 'z':
		g_transMat *= Angel::Translate(0, 0, -1);
		break;
    }
}

void mouseFunc(int button, int state, int x, int y) {
	if (g_r_toggle) {
		GLfloat xx = (2.0f * x) / width - 1;
		GLfloat yy = 1 - (2.0f * y) / height;
		vec4 v1 = vec4(xx, yy, 0, 1);
		vec4 v2 = vec4(xx, yy, -1, 1);

		v1 = unproject_vertex(g_perspectiveMat * g_mvmat, v1);
		v2 = unproject_vertex(g_perspectiveMat * g_mvmat, v2);

		vec4 ray_vec4 = v2 - v1;
		vec3 ray = vec3(ray_vec4.x, ray_vec4.y, ray_vec4.z);
		GLfloat d = sqrt(pow(ray.x, 2) + pow(ray.y, 2) + pow(ray.z, 2));
		ray /= d;
		vec3 point = vec3(v1.x, v1.y, v1.z) + ray * 300;
		g_v1 = v1;
		g_v2 = vec4(point.x, point.y, point.z, 1);


		g_is_line_ready = true;

		int index = ray_intersection_for_index(g_vertecies, g_norms, g_nfaces, vec3(ray.x, ray.y, ray.z), v1);
		if (index > 0 && index < g_nfaces) {
			GLfloat posx = g_vertecies[index * 3].x,
				posy = g_vertecies[index * 3].y,
				posz = g_vertecies[index * 3].z;
			

			g_colors[index * 3] = vec4(1, 0, 0, 1);
			g_colors[index * 3 + 1] = vec4(1, 0, 0, 1);
			g_colors[index * 3 + 2] = vec4(1, 0, 0, 1);
			for (int j = 0; j < g_nfaces * 3; j ++) {
				//g_colors[j] = vec4(1, 0, 0, 1);
				if (sqrt(pow((g_vertecies[j].x - posx), 2) + pow((g_vertecies[j].y - posy), 2) + pow((g_vertecies[j].z - posz), 2)) < 20) {
					g_colors[j] = vec4(1, 0, 0, 1);
				}
			}
			glBindBuffer(GL_ARRAY_BUFFER, g_buffers[0]);
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4)* g_nfaces * 3, sizeof(vec4)* g_nfaces * 3, g_colors);
			//display();

		}
		else {
			//cout << "index out of bound: " << index << endl;
			cout << "no intersection." << endl;
		}
		return;
	}
	g_mouseButton = button;
	g_mouseState = state;
	if (state == GLUT_UP) {
		g_mouseFlow = 0;
	}

	if (button == 3 || button == 4) {
		if (state == GLUT_UP) {
			return;
		}
		else {
			if (button == 3) {
				g_scaleMat *= Angel::Scale(1.2f, 1.2f, 1.2f);
			}
			else {
				g_scaleMat *= Angel::Scale(0.83f, 0.83f, 0.83f);
			}
			display(); 
		}

	}
}

void motionMouseFunc(int x, int y) {
	if (g_r_toggle) {
		return;
	}
	if (g_mouseFlow == 0) {
		g_prevMousePos = vec2((GLfloat)x, (GLfloat)y);
		g_mouseFlow = 1;
	}
	else if (g_mouseState == GLUT_DOWN) {
		if (g_mouseButton == GLUT_LEFT_BUTTON) {
			GLfloat transX = x - g_prevMousePos[0];
			GLfloat transY = y - g_prevMousePos[1];
			g_transMat *= Angel::Translate(vec4(transX, -transY, 0, 1));
			g_prevMousePos = vec2((GLfloat)x, (GLfloat)y);
		}

	}
	if (g_mouseButton == GLUT_RIGHT_BUTTON) {
		GLfloat rotX = x - g_prevMousePos[0];
		GLfloat rotY = y - g_prevMousePos[1];
		g_rotate *= Angel::RotateY(rotX) * Angel::RotateX(-rotY);
		g_prevMousePos = vec2((GLfloat)x, (GLfloat)y);

	}

}

#pragma endregion

void init(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512, 512);

	width = 512;
	height = 512;

	glutCreateWindow("display");

	glewInit();

	g_is_line_ready = false;
	g_transMat = Angel::Translate(0, 0, -2);
}

int main(int argc, char **argv) {
	srand(1);
	init(argc, argv);
	//genTestVertecies();
	loadVertecies();
	glClearColor(1, 1, 1, 1);

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouseFunc);
	glutMotionFunc(motionMouseFunc);
	glutIdleFunc(display);
	glutMainLoop();

	return 0;
}

#pragma region ray tracing


GLfloat pointDistance(vec4 p1, vec4 p2) {
	return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2) + pow(p1.z - p2.z, 2));
}

#pragma endregion

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
	g_rotate = Angel::identity();
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

	initBuffer(1);

	g_program = InitShader("vshader1.glsl", "fshader1.glsl");
	glUseProgram(g_program);

	addToBuffer(vertecies, colors, nfaces);

	g_nomMat = getScalingMatrix(vertecies, nfaces * 3);
	g_centerMat = getCenteringMatrix(vertecies, nfaces * 3);

}

void genTestVertecies() {
	int nfaces = 1;
	vec4 *vertecies = (vec4*)malloc(sizeof(vec4) * 3);
	vertecies[0] = vec4(0, 0, 0, 1);
	vertecies[1] = vec4(1, 1, 1, 1);
	vertecies[2] = vec4(1, -1, 1, 1);
	g_nfaces = nfaces;
	vec4 *colors = allocRandomColor(nfaces);

	initBuffer(2);

	g_program = InitShader("vshader1.glsl", "fshader1.glsl");
	glUseProgram(g_program);

	addToBuffer(vertecies, colors, nfaces);

	g_nomMat = getScalingMatrix(vertecies, nfaces * 3);
	g_centerMat = getCenteringMatrix(vertecies, nfaces * 3);

	g_norms = (vec4*)malloc(sizeof(vec4));
	g_norms[0] = vec4(0, 0, 1, 1);
	g_vertecies = vertecies;
	g_colors = colors;
}

vec4* allocRandomColor(int nfaces) {
	vec4 *colors = (vec4*)malloc(sizeof(vec4)* nfaces * 3);
	for (int i = 0; i < nfaces * 3; i++) {
		float r1 = rand() / (float)RAND_MAX, r2 = rand() / (float)RAND_MAX, r3 = rand() / (float)RAND_MAX;
		float z = (i / (float)nfaces * 3.0f) * 2 - 1.5;
		//colors[i] = vec4(z, z, z, 1);
		//colors[i] = vec4(r1, r2, r3, 1);
		colors[i] = vec4(0, 0, 1, 1);
	}
	g_colors = colors;
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
	vec4* norms = (vec4*)malloc(sizeof(vec4)* TriangleNum);
	for (int i = 0; i < TriangleNum; i++) {
		vec4 points[3];
		norms[i] = vec4(Triangle[0][i], Triangle[1][i], Triangle[1][i], 1);
		for (int j = 0; j < 3; j++) {
			points[j] = vec4(Triangle[j * 3 + 3][i] / factor, Triangle[j * 3 + 4][i] / factor, Triangle[j * 3 + 5][i] / factor, 1);
		}
		facebuffer[i * 3] = points[0];
		facebuffer[i * 3 + 1] = points[1];
		facebuffer[i * 3 + 2] = points[2];
	}

	*_facebuffer = facebuffer;
	*_nfacebuffer = TriangleNum;

	g_vertecies = facebuffer;

	for (int i = 0; i < TriangleNum; i++) {
		norms[i] = get_normal(facebuffer[i * 3],
			facebuffer[i * 3 + 1], facebuffer[i * 3 + 2]);
	}
	g_norms = norms;
}

#pragma endregion

vec4 get_normal(vec4 a, vec4 b, vec4 c) {
	vec3 ab = vec3(b.x - a.x, b.y - a.y, b.z - a.z);
	vec3 ac = vec3(c.x - a.x, c.y - a.y, c.z - a.z);
	vec3 n = Angel::cross(ab, ac);
	GLfloat d =  sqrt(pow(n.x, 2) + pow(n.y, 2) + pow(n.z, 2));
	n /= d;
	return vec4(n.x, n.y, n.z, 1);
}