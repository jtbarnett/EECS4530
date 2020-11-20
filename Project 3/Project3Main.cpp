#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>
#include <iostream>
#include <cstdlib>
#include "LoadShaders.h"
#include "vmath.h"
#include "vgl.h"
#include <map>
#include <vector>
#include "PositionInfo.h"

using namespace std;

#define BUFFER_OFFSET(x)  ((const void*) (x))

GLuint programID1;
GLuint programID2;
GLuint crateTexture;

/*
* Arrays to store the indices/names of the Vertex Array Objects and
* Buffers.  Rather than using the books enum approach I've just
* gone out and made a bunch of them and will use them as needed.
*
* Not the best choice I'm sure.
*/
GLuint vertexBuffers[11], arrayBuffers[11], elementBuffers[11];

/*
* Global variables
*   The location for the transformation and the current rotation
*   angle are set up as globals since multiple methods need to
*   access them.
*/
float rotationAngle = 1.0f;
bool elements;
int nbrTriangles, materialToUse = 0;
int numberQuads = 6;
int startTriangle = 0, endTriangle = 12;
map<string, GLuint> locationMap1;
map<string, GLuint> locationMap2;
vector<Drawable*> objectList;
vmath::vec3 xAxis = vmath::vec3(1.0f, 0.0f, 0.0f);
vmath::vec3 yAxis = vmath::vec3(0.0f, 1.0f, 0.0f);
vmath::vec3 zAxis = vmath::vec3(0.0f, 0.0f, 1.0f);

// List of objects
PositionInfo* crate1;
PositionInfo* crate2;

// Prototypes
GLuint buildProgram(string vertexShaderName, string fragmentShaderName);
GLuint LoadTextureRAW(const char* filename);
int glutStartUp(int& argCount, char* argValues[],
	string windowTitle = "No Title", int width = 500, int height = 500);
void setAttributes(float lineWidth = 1.0, GLenum face = GL_FRONT_AND_BACK,
	GLenum fill = GL_FILL);
void buildObjects();
void getLocations();
void init(string vertexShader1, string fragmentShader1);
void readfile(string filename, vector<float>& vertices, vector<unsigned int>& indices);
void readfile2(string filename, vector<float>& vertices, vector<float>& normals);
void readfile3(string filename, vector<float>& vertices, vector<float>& normals, vector<float>& texture);

/*
 * Routine to encapsulate some of the startup routines for GLUT.  It returns the window ID of the
 * single window that is created.
 */
int glutStartUp(int& argCount, char* argValues[], string title, int width, int height) {
	int windowID;
	GLenum glewErrCode;

	glutInit(&argCount, argValues);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(width, height);
	windowID = glutCreateWindow(title.c_str());

	glutInitContextVersion(3, 1);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glewErrCode = glewInit();
	if (glewErrCode != GLEW_OK) {
		cerr << "Glew init failed " << glewErrCode << endl;
		exit(EXIT_FAILURE);
	}
	return windowID;
}

/*
 * Use the author's routines to build the program and return the program ID.
 */
GLuint buildProgram(string vertexShaderName, string fragmentShaderName) {
	ShaderInfo shaders[] = {
		{ GL_VERTEX_SHADER, vertexShaderName.c_str() },
		{ GL_FRAGMENT_SHADER, fragmentShaderName.c_str() },
		{ GL_NONE, NULL }
	};
	GLuint program = LoadShaders(shaders);
	if (program == 0) {
		cerr << "GLSL Program didn't load.  Error \n" << endl
			<< "Vertex Shader = " << vertexShaderName << endl
			<< "Fragment Shader = " << fragmentShaderName << endl;
	}
	glUseProgram(program);
	return program;
}

/*
 * Set up the clear color, lineWidth, and the fill type for the display.
 */
void setAttributes(float lineWidth, GLenum face, GLenum fill) {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glLineWidth(lineWidth);
	glPolygonMode(face, fill);
	glEnable(GL_DEPTH_TEST);
}

/*
 * read and/or build the objects to be displayed.  Also sets up attributes that are
 * vertex related.
 */
void buildObjects() {
	/*GLuint vPosition1 = glGetAttribLocation(programID1, "vPosition");
	GLuint vColor1 = glGetAttribLocation(programID1, "vColor");

	GLuint vPosition2 = glGetAttribLocation(programID2, "vPosition");
	GLuint vColor2 = glGetAttribLocation(programID2, "vColor");

	vector<float> fileVertices, fileNormals, fileTextures;

	// Crate 1
	readfile3("Crate.obj", fileVertices, fileNormals, fileTextures);
	glGenVertexArrays(1, &(vertexBuffers[0]));
	glBindVertexArray(vertexBuffers[0]);
	glGenBuffers(1, &(arrayBuffers[0]));
	glBindBuffer(GL_ARRAY_BUFFER, arrayBuffers[0]);
	glBufferData(GL_ARRAY_BUFFER,
		fileVertices.size() * sizeof(float) + (fileVertices.size() / 3) * 4 * sizeof(float),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, fileVertices.size() * sizeof(float), fileVertices.data());
	float* fileColors = new float[fileVertices.size() / 3 * 4];
	for (unsigned int currentVertex = 0; currentVertex < fileVertices.size() / 3; currentVertex++) {
		fileColors[currentVertex * 4] = 0.0f;
		fileColors[currentVertex * 4 + 1] = 0.0f;
		fileColors[currentVertex * 4 + 2] = 1.0f;
		fileColors[currentVertex * 4 + 3] = 1.0f;
	}
	glBufferSubData(GL_ARRAY_BUFFER, fileVertices.size() * sizeof(float), fileVertices.size() / 3 * 4 * sizeof(float), fileColors);
	glBufferSubData(GL_ARRAY_BUFFER, fileVertices.size() * sizeof(float) +
		fileVertices.size() / 3 * 4 * sizeof(float),
		fileNormals.size() * sizeof(float), fileNormals.data());

	vPosition1 = glGetAttribLocation(programID1, "vPosition");
	glEnableVertexAttribArray(vPosition1);
	glVertexAttribPointer(vPosition1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	vColor1 = glGetAttribLocation(programID1, "vColor");
	glEnableVertexAttribArray(vColor1);
	glVertexAttribPointer(vColor1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(fileVertices.size() * sizeof(float)));

	crate1 = new PositionInfo(vertexBuffers[0], arrayBuffers[0],
		fileVertices.size() / 3, locationMap1["modelingTransform"],
		vmath::scale(0.5f, 0.5f, 0.5f), vmath::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	crate1->addTranslation(0.0f, -3.5f, 0.0f);
	objectList.push_back(crate1);

	// Crate 2
	readfile3("Crate.obj", fileVertices, fileNormals, fileTextures);
	glGenVertexArrays(1, &(vertexBuffers[1]));
	glBindVertexArray(vertexBuffers[1]);
	glGenBuffers(1, &(arrayBuffers[1]));
	glBindBuffer(GL_ARRAY_BUFFER, arrayBuffers[1]);
	glBufferData(GL_ARRAY_BUFFER,
		fileVertices.size() * sizeof(float) + (fileVertices.size() / 3) * 4 * sizeof(float),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, fileVertices.size() * sizeof(float), fileVertices.data());
	fileColors = new float[fileVertices.size() / 3 * 4];
	for (unsigned int currentVertex = 0; currentVertex < fileVertices.size() / 3; currentVertex++) {
		fileColors[currentVertex * 4] = 1.0f;
		fileColors[currentVertex * 4 + 1] = 1.0f;
		fileColors[currentVertex * 4 + 2] = 1.0f;
		fileColors[currentVertex * 4 + 3] = 1.0f;
	}
	glBufferSubData(GL_ARRAY_BUFFER, fileVertices.size() * sizeof(float), fileVertices.size() / 3 * 4 * sizeof(float), fileColors);
	glBufferSubData(GL_ARRAY_BUFFER, fileVertices.size() * sizeof(float) +
		fileVertices.size() / 3 * 4 * sizeof(float),
		fileNormals.size() * sizeof(float), fileNormals.data());

	vPosition2 = glGetAttribLocation(programID2, "vPosition");
	glEnableVertexAttribArray(vPosition2);
	glVertexAttribPointer(vPosition2, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	vColor2 = glGetAttribLocation(programID2, "vColor");
	glEnableVertexAttribArray(vColor2);
	glVertexAttribPointer(vColor2, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(fileVertices.size() * sizeof(float)));

	crate2 = new PositionInfo(vertexBuffers[1], arrayBuffers[1],
		fileVertices.size() / 3, locationMap2["modelingTransform"],
		vmath::scale(0.5f, 0.5f, 0.5f), vmath::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	crate2->addTranslation(0.0f, 1.5f, 0.0f);
	objectList.push_back(crate2);*/


	GLfloat cube1[] = {
		// Front
		 /*1.0,  1.0, -1.0, 1.0,
		-1.0,  1.0, -1.0, 1.0,
		-1.0,  1.0,  1.0, 1.0,
		 1.0,  1.0,  1.0, 1.0,

		// Back
		 1.0, -1.0,  1.0, 1.0,
		-1.0, -1.0,  1.0, 1.0,
		-1.0, -1.0, -1.0, 1.0,
		 1.0, -1.0, -1.0, 1.0,

		// Right
		 1.0,  1.0,  1.0, 1.0,
		-1.0,  1.0,  1.0, 1.0,
		-1.0, -1.0,  1.0, 1.0,
		 1.0, -1.0,  1.0, 1.0,

		// Left
		 1.0, -1.0, -1.0, 1.0,
		-1.0, -1.0, -1.0, 1.0,
		-1.0,  1.0, -1.0, 1.0,
		 1.0,  1.0, -1.0, 1.0,

		// Top
		-1.0,  1.0,  1.0, 1.0,
		-1.0,  1.0, -1.0, 1.0,
		-1.0, -1.0, -1.0, 1.0,
		-1.0, -1.0,  1.0, 1.0,

		// Bottom
		 1.0,  1.0, -1.0, 1.0,
		 1.0,  1.0,  1.0, 1.0,
		 1.0, -1.0,  1.0, 1.0,
		 1.0, -1.0, -1.0, 1.0,*/


		-0.3,  0.3, -0.3, 1.0,
		-0.3, -0.3, -0.3, 1.0,
		 0.3, -0.3, -0.3, 1.0,  // Triangle 1 -- White
		-0.3,  0.3, -0.3, 1.0,
		 0.3, -0.3, -0.3, 1.0,
		 0.3,  0.3, -0.3, 1.0,  // Triangle 2 -- Blue

		// Back
		-0.3,  0.3,  0.3, 1.0,
		-0.3, -0.3,  0.3, 1.0,
		 0.3, -0.3,  0.3, 1.0,  // Triangle 3 -- Cyan
		-0.3,  0.3,  0.3, 1.0,
		 0.3, -0.3,  0.3, 1.0,
		 0.3,  0.3,  0.3, 1.0,  // Triangle 4 -- Yellow

		// Right
		 0.3,  0.3, -0.3, 1.0,
		 0.3, -0.3, -0.3, 1.0,
		 0.3, -0.3,  0.3, 1.0,  // Triangle 5 -- Pink
		 0.3,  0.3, -0.3, 1.0,
		 0.3, -0.3,  0.3, 1.0,
		 0.3,  0.3,  0.3, 1.0,  // Triangle 6 -- Grey

		// Left
		-0.3,  0.3, -0.3, 1.0,
		-0.3, -0.3, -0.3, 1.0,
		-0.3, -0.3,  0.3, 1.0,  // Triangle 7 -- Red
		-0.3,  0.3, -0.3, 1.0,
		-0.3, -0.3,  0.3, 1.0,
		-0.3,  0.3,  0.3, 1.0,  // Triangle 8 -- Green

		// Top
		-0.3,  0.3,  0.3, 1.0,
		-0.3,  0.3, -0.3, 1.0,
		 0.3,  0.3, -0.3, 1.0,  // Triangle 9 -- Magenta
		-0.3,  0.3,  0.3, 1.0,
		 0.3,  0.3, -0.3, 1.0,
		 0.3,  0.3,  0.3, 1.0,  // Triangle 10 -- Orange

		// Bottom
		-0.3, -0.3,  0.3, 1.0,
		-0.3, -0.3, -0.3, 1.0,
		 0.3, -0.3, -0.3, 1.0,  // Triangle 11 -- Indigo
		-0.3, -0.3,  0.3, 1.0,
		 0.3, -0.3, -0.3, 1.0,
		 0.3, -0.3,  0.3, 1.0   // Triangle 12 -- Gold
	};

	GLfloat cubeUV1[] = {
		/*0.0, 0.0,
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0,

		0.0, 0.0,
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0,

		0.0, 0.0,
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0,

		0.0, 0.0,
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0,

		0.0, 0.0,
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0,

		0.0, 0.0,
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0,*/

		1.0,  1.0,  1.0, 1.0,
		1.0,  1.0,  1.0, 1.0,
		1.0,  1.0,  1.0, 1.0,
		1.0,  1.0,  1.0, 1.0,
		1.0,  1.0,  1.0, 1.0,
		1.0,  1.0,  1.0, 1.0,
		1.0,  1.0,  1.0, 1.0,
		1.0,  1.0,  1.0, 1.0,
		1.0,  1.0,  1.0, 1.0,
		1.0,  1.0,  1.0, 1.0,
		1.0,  1.0,  1.0, 1.0,
		1.0,  1.0,  1.0, 1.0,
		1.0,  1.0,  1.0, 1.0,
		1.0,  1.0,  1.0, 1.0,
		1.0,  1.0,  1.0, 1.0,
		1.0,  1.0,  1.0, 1.0,
		1.0,  1.0,  1.0, 1.0,
		1.0,  1.0,  1.0, 1.0,
		1.0,  1.0,  1.0, 1.0,
		1.0,  1.0,  1.0, 1.0,
		1.0,  1.0,  1.0, 1.0,
		1.0,  1.0,  1.0, 1.0,
		1.0,  1.0,  1.0, 1.0,
		1.0,  1.0,  1.0, 1.0,
		1.0,  1.0,  1.0, 1.0,
		1.0,  1.0,  1.0, 1.0,
		1.0,  1.0,  1.0, 1.0,
		1.0,  1.0,  1.0, 1.0,
		1.0,  1.0,  1.0, 1.0,
		1.0,  1.0,  1.0, 1.0,
		1.0,  1.0,  1.0, 1.0,
		1.0,  1.0,  1.0, 1.0,
		1.0,  1.0,  1.0, 1.0,
		1.0,  1.0,  1.0, 1.0,
		1.0,  1.0,  1.0, 1.0,
		1.0,  1.0,  1.0, 1.0
	};

	glGenVertexArrays(1, &(vertexBuffers[0]));
	glBindVertexArray(vertexBuffers[0]);

	glGenBuffers(1, &(arrayBuffers[0]));
	glBindBuffer(GL_ARRAY_BUFFER, arrayBuffers[0]);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(cube1) + sizeof(cubeUV1),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cube1), cube1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(cube1), sizeof(cubeUV1), cubeUV1);

	GLuint vPosition1 = glGetAttribLocation(programID1, "vPosition");
	glEnableVertexAttribArray(vPosition1);
	glVertexAttribPointer(vPosition1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint vColor1 = glGetAttribLocation(programID1, "vColor");
	glEnableVertexAttribArray(vColor1);

	glVertexAttribPointer(vColor1, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(cube1)));
}

void getLocations() {
	GLint numberBlocks;
	char uniformName[1024];
	int nameLength;
	GLint size;
	GLenum type;

	// Program 1
	glGetProgramiv(programID1, GL_ACTIVE_UNIFORMS, &numberBlocks);
	for (int blockIndex = 0; blockIndex < numberBlocks; blockIndex++) {
		glGetActiveUniform(programID1, blockIndex, 1024, &nameLength, &size, &type, uniformName);
		cout << uniformName << endl;
		locationMap1[string(uniformName)] = blockIndex;
	}

	// Program 2
	/*glGetProgramiv(programID2, GL_ACTIVE_UNIFORMS, &numberBlocks);
	for (int blockIndex = 0; blockIndex < numberBlocks; blockIndex++) {
		glGetActiveUniform(programID2, blockIndex, 1024, &nameLength, &size, &type, uniformName);
		cout << uniformName << endl;
		locationMap2[string(uniformName)] = blockIndex;
	}*/
}

void init(string vertexShader1, string fragmentShader1) {
	setAttributes(1.0f, GL_FRONT_AND_BACK, GL_FILL);
	//crateTexture = LoadTextureRAW("wood.bmp");
	programID1 = buildProgram(vertexShader1, fragmentShader1);
	//programID2 = buildProgram(vertexShader2, fragmentShader2);
	getLocations();
	buildObjects();
}

/*
* Function for rotating the carousel object
*/
void timer(int value) {
	crate1->addRotation(rotationAngle, yAxis);
	crate2->addRotation(rotationAngle, yAxis);
	glutPostRedisplay();
	glutTimerFunc(1000 / 30, timer, 1);
}


GLuint LoadTextureRAW(const char* filename) {
	FILE* stream;
	errno_t err;
	GLuint texture;
	int width, height;
	unsigned char* data;
	err = fopen_s(&stream, filename, "rb");

	width = 256;
	height = 256;

	data = (unsigned char*)malloc(width * height * 3);

	fread(data, width * height * 3, 1, stream);

	fclose(stream);

	for (int i = 0; i < width * height; ++i) {
		int index = i * 3;
		unsigned char B, R;
		B = data[index];
		R = data[index + 2];
		data[index] = R;
		data[index + 2] = B;
	}

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);
	free(data);

	return texture;
}


/*
 * The display routine is basically unchanged at this point.
 */
void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	vmath::mat4 viewingTransform;
	vmath::mat4 projectionTransform;

	projectionTransform = vmath::ortho(-6.0f, 6.0f, -6.0f, 6.0f, -100.0f, 100.0f);
	//projectionTransform = vmath::perspective(60.0f, 1.0f, 1.0f, 100.0f);
	glUniformMatrix4fv(locationMap1["projectionTransform"], 1, GL_FALSE, projectionTransform);
	//glUniformMatrix4fv(locationMap2["projectionTransform"], 1, GL_FALSE, projectionTransform);

	viewingTransform = vmath::lookat(vmath::vec3(0.0f, 0.0f, 15.0f), vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(locationMap1["viewingTransform"], 1, GL_FALSE, viewingTransform);
	//glUniformMatrix4fv(locationMap2["viewingTransform"], 1, GL_FALSE, viewingTransform);

	/*for (Drawable* d : objectList) {
		d->draw();
	}
	glutSwapBuffers();*/

	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, crateTexture);*/

	glBindVertexArray(vertexBuffers[0]);
	glBindBuffer(GL_ARRAY_BUFFER, arrayBuffers[0]);
	glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
	glFlush();
}

int main(int argCount, char* argValues[]) {
	glutStartUp(argCount, argValues, "My Test of New Routines");
	init("project0a.vert", "project0.frag");
	glutDisplayFunc(display);
	//glutTimerFunc(1000 / 30, timer, 1);
	glutMainLoop();
}
