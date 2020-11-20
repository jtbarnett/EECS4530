#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>
#include <iostream>
#include <cstdlib>
#include "LoadShaders.h"
#include "vmath.h"
#include "vgl.h"
#include <map>
#include "stb_image.h"

using namespace std;

#define BUFFER_OFFSET(x)  ((const void*) (x))

/*
* GLobal ID variables
* programID1 -- this is the Image Mapping program (right cube on screen)
* programID2 -- this is the procedural texturing program (left cube on screen)
*/
GLuint programID1;
GLuint programID2;
GLuint textureID;
GLuint texID;

/*
* Arrays to store the indices/names of the Vertex Array Objects and
* Buffers.  Rather than using the books enum approach I've just
* gone out and made a bunch of them and will use them as needed.
*/
GLuint vertexBuffers[10], arrayBuffers[10];

/*
* Global variables
*/
float rotationAngle;
int numberQuads = 6;
map<string, GLuint> locationMap1;
map<string, GLuint> locationMap2;
vmath::vec3 xAxis = vmath::vec3(1.0f, 0.0f, 0.0f);
vmath::vec3 yAxis = vmath::vec3(0.0f, 1.0f, 0.0f);
vmath::vec3 zAxis = vmath::vec3(0.0f, 0.0f, 1.0f);

/*
* Prototypes
*/
GLuint buildProgram(string vertexShaderName, string fragmentShaderName);
GLuint LoadTexture(const string& path);
void BindTexture(unsigned int slot = 0);
int glutStartUp(int& argCount, char* argValues[],
	string windowTitle = "No Title", int width = 500, int height = 500);
void setAttributes(float lineWidth = 1.0, GLenum face = GL_FRONT_AND_BACK,
	GLenum fill = GL_FILL);
void buildObjects();
void getLocations();
void init(string vertexShader1, string fragmentShader1, string vertexShader2, string fragmentShader2);

/*
 * Routine to encapsulate some of the startup routines for GLUT.  It returns the window ID of the
 * single window that is created.
 */
int glutStartUp(int& argCount, char* argValues[], string title, int width, int height) {
	int windowID;
	GLenum glewErrCode;

	glutInit(&argCount, argValues);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
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
	/*
	*  Use the Books code to load in the shaders.
	*/
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
	/*
	* I'm using wide lines so that they are easier to see on the screen.
	* In addition, this version fills in the polygons rather than leaving it
	* as lines.
	*/
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glLineWidth(lineWidth);
	glPolygonMode(face, fill);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
}

/*
 * read and/or build the objects to be displayed.  Also sets up attributes that are
 * vertex related.
 */
void buildObjects() {
	GLfloat vertices1[] = {
		 // TOP
		 4.0,  1.0, -1.0, 1.0,
		 2.0,  1.0, -1.0, 1.0,
		 2.0,  1.0,  1.0, 1.0,
		 4.0,  1.0,  1.0, 1.0,

		 // BOTTOM
		 4.0, -1.0,  1.0, 1.0,
		 2.0, -1.0,  1.0, 1.0,
		 2.0, -1.0, -1.0, 1.0,
		 4.0, -1.0, -1.0, 1.0,

		 // FRONT
		 4.0,  1.0,  1.0, 1.0,
		 2.0,  1.0,  1.0, 1.0,
		 2.0, -1.0,  1.0, 1.0,
		 4.0, -1.0,  1.0, 1.0,

		 // BACK
		 4.0, -1.0, -1.0, 1.0,
		 2.0, -1.0, -1.0, 1.0,
		 2.0,  1.0, -1.0, 1.0,
		 4.0,  1.0, -1.0, 1.0,

		 // RIGHT
		 2.0,  1.0,  1.0, 1.0,
		 2.0,  1.0, -1.0, 1.0,
		 2.0, -1.0, -1.0, 1.0,
		 2.0, -1.0,  1.0, 1.0,

		 // LEFT
		 4.0,  1.0, -1.0, 1.0,
		 4.0,  1.0,  1.0, 1.0,
		 4.0, -1.0,  1.0, 1.0,
		 4.0, -1.0, -1.0, 1.0,
	};

	GLfloat texCoords[] = {
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
		0.0, 1.0,
		0.0, 0.0,
		1.0, 0.0,
	};

	glUseProgram(programID1);

	glGenVertexArrays(1, &(vertexBuffers[0]));
	glBindVertexArray(vertexBuffers[0]);
	glGenBuffers(1, &(arrayBuffers[0]));
	glBindBuffer(GL_ARRAY_BUFFER, arrayBuffers[0]);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1) + sizeof(texCoords), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices1), vertices1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices1), sizeof(texCoords), texCoords);

	GLuint vPosition1 = glGetAttribLocation(programID1, "vPosition");
	glEnableVertexAttribArray(vPosition1);
	glVertexAttribPointer(vPosition1, 4, GL_FLOAT, GL_FALSE, 0, 0);

	GLuint texCoordID = glGetAttribLocation(programID1, "vertexUV");
	glEnableVertexAttribArray(texCoordID);
	glVertexAttribPointer(texCoordID, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vertices1)));

	
	GLfloat vertices2[] = {
		 // TOP
		 -4.0,  1.0, -1.0, 1.0,
		 -2.0,  1.0, -1.0, 1.0,
		 -2.0,  1.0,  1.0, 1.0,
		 -4.0,  1.0,  1.0, 1.0,

		 // BOTTOM
		 -4.0, -1.0,  1.0, 1.0,
		 -2.0, -1.0,  1.0, 1.0,
		 -2.0, -1.0, -1.0, 1.0,
		 -4.0, -1.0, -1.0, 1.0,

		 // FRONT
		 -4.0,  1.0,  1.0, 1.0,
		 -2.0,  1.0,  1.0, 1.0,
		 -2.0, -1.0,  1.0, 1.0,
		 -4.0, -1.0,  1.0, 1.0,

		 // BACK
		 -4.0, -1.0, -1.0, 1.0,
		 -2.0, -1.0, -1.0, 1.0,
		 -2.0,  1.0, -1.0, 1.0,
		 -4.0,  1.0, -1.0, 1.0,

		 // RIGHT
		 -2.0,  1.0,  1.0, 1.0,
		 -2.0,  1.0, -1.0, 1.0,
		 -2.0, -1.0, -1.0, 1.0,
		 -2.0, -1.0,  1.0, 1.0,

		 // LEFT
		 -4.0,  1.0, -1.0, 1.0,
		 -4.0,  1.0,  1.0, 1.0,
		 -4.0, -1.0,  1.0, 1.0,
		 -4.0, -1.0, -1.0, 1.0,
	};

	GLfloat normals[] = {
		0.00, 1.00, 0.00,
		0.00, 1.00, 0.00,
		0.00, 1.00, 0.00,
		0.00, 1.00, 0.00,

		0.00, 1.00, 0.00,
		0.00, 1.00, 0.00,
		0.00, 1.00, 0.00,
		0.00, 1.00, 0.00,

		0.00, 0.00, 1.00,
		0.00, 0.00, 1.00,
		0.00, 0.00, 1.00,
		0.00, 0.00, 1.00,

		0.00, 0.00, 1.00,
		0.00, 0.00, 1.00,
		0.00, 0.00, 1.00,
		0.00, 0.00, 1.00,

		1.00, 0.00, 0.00,
		1.00, 0.00, 0.00,
		1.00, 0.00, 0.00,
		1.00, 0.00, 0.00,

		1.00, 0.00, 0.00,
		1.00, 0.00, 0.00,
		1.00, 0.00, 0.00,
		1.00, 0.00, 0.00,
	};

	glUseProgram(programID2);

	glGenVertexArrays(1, &(vertexBuffers[1]));
	glBindVertexArray(vertexBuffers[1]);
	glGenBuffers(1, &(arrayBuffers[1]));
	glBindBuffer(GL_ARRAY_BUFFER, arrayBuffers[1]);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2) + sizeof(normals), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices2), vertices2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices2), sizeof(normals), normals);

	GLuint vPosition2 = glGetAttribLocation(programID2, "vPosition");
	glEnableVertexAttribArray(vPosition2);
	glVertexAttribPointer(vPosition2, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint vNormal = glGetAttribLocation(programID2, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vertices2)));
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
	glGetProgramiv(programID2, GL_ACTIVE_UNIFORMS, &numberBlocks);
	for (int blockIndex = 0; blockIndex < numberBlocks; blockIndex++) {
		glGetActiveUniform(programID2, blockIndex, 1024, &nameLength, &size, &type, uniformName);
		cout << uniformName << endl;
		locationMap2[string(uniformName)] = blockIndex;
	}
}

void init(string vertexShader1, string fragmentShader1, string vertexShader2, string fragmentShader2) {
	setAttributes(1.0f, GL_FRONT_AND_BACK, GL_FILL);

	// Create the two separate programs
	programID1 = buildProgram(vertexShader1, fragmentShader1);
	programID2 = buildProgram(vertexShader2, fragmentShader2);

	buildObjects();
	getLocations();

	// Load in the PNG image and bind the textures for it
	glUseProgram(programID1);
	textureID = LoadTexture("bird.png");
	texID = glGetUniformLocation(programID1, "tex");
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(texID, 0);
	BindTexture();
}

/*
* Loads the PNG image in and sets up the textures to be mapped to programID1 cube
*/
GLuint LoadTexture(const string& path) {
	GLuint texture = 0;
	string filePath = path;
	unsigned char* localBuffer;
	int width = 0, height = 0, bitsPerPixel = 0;

	stbi_set_flip_vertically_on_load(1);
	localBuffer = stbi_load(path.c_str(), &width, &height, &bitsPerPixel, 4);

	if (!localBuffer) {
		printf("Image could not be opened\n");
		return 0;
	}

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (localBuffer) {
		stbi_image_free(localBuffer);
	}

	return texture;
}

/*
* Bind the textures for image mapping
*/
void BindTexture(unsigned int slot) {
	glBindTexture(GL_TEXTURE_2D, textureID);
}

/*
* Increase the rotation angle for the two cubes so they rotate on screen
*/
void timer(int value) {
	rotationAngle += 1.0f;
	if (rotationAngle >= 360.0)
		rotationAngle -= 360.0;
	glutPostRedisplay();
	glutTimerFunc(1000 / 30, timer, 1);
}

/*
 * The display routine is basically unchanged at this point.
 */
void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	vmath::mat4 currentMatrix;
	vmath::mat4 projectionTransform;
	vmath::mat4 viewingTransform;

	// Set up the matricies for the shaders
	currentMatrix = vmath::rotate(rotationAngle, 1.0f, 0.25f, 0.25f);
	projectionTransform = vmath::ortho(-6.0f, 6.0f, -6.0f, 6.0f, -100.0f, 100.0f);
	viewingTransform = vmath::lookat(vmath::vec3(0.0f, 0.0f, 12.0f), vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec3(0.0f, 1.0f, 0.0f));

	// Set up for the Image Mapping cube
	glUseProgram(programID1);

	BindTexture();
	glUniformMatrix4fv(locationMap1["modelingTransform"], 1, GL_TRUE, currentMatrix);
	glUniformMatrix4fv(locationMap1["projectionTransform"], 1, GL_FALSE, projectionTransform);
	glUniformMatrix4fv(locationMap1["viewingTransform"], 1, GL_FALSE, viewingTransform);
	glBindVertexArray(vertexBuffers[0]);
	glBindBuffer(GL_ARRAY_BUFFER, arrayBuffers[0]);
	glDrawArrays(GL_QUADS, 0, numberQuads * 4);
	glutSwapBuffers();
	glFlush();

	// Set up for the Procedural Texturing cube
	vmath::vec3 LightPosition(1.0f, 1.0f, 1.0f);
	vmath::vec3 BrickColor(0.7960f, 0.2549f, 0.3294f);
	vmath::vec3 MortarColor(0.3372f, 0.3137f, 0.3176f);
	vmath::vec3 BrickSize(0.2f, 0.2f, 0.2f);
	vmath::vec3 BrickPct(0.85f, 0.85f, 0.85f);

	glUseProgram(programID2);

	glUniform3fv(locationMap2["LightPosition"], 1, LightPosition);
	glUniform3fv(locationMap2["BrickColor"], 1, BrickColor);
	glUniform3fv(locationMap2["MortarColor"], 1, MortarColor);
	glUniform3fv(locationMap2["BrickSize"], 1, BrickSize);
	glUniform3fv(locationMap2["BrickPct"], 1, BrickPct);
	glUniformMatrix4fv(locationMap2["modelingTransform"], 1, GL_TRUE, currentMatrix);
	glUniformMatrix4fv(locationMap2["projectionTransform"], 1, GL_FALSE, projectionTransform);
	glUniformMatrix4fv(locationMap2["viewingTransform"], 1, GL_FALSE, viewingTransform);
	glUniformMatrix3fv(locationMap2["normalTransform"], 1, GL_FALSE, vmath::mat3(vmath::vec3(1.0f, 0.0f, 0.0f), vmath::vec3(0.0f, 1.0f, 0.0f), vmath::vec3(0.0f, 0.0f, 1.0f)));
	glBindVertexArray(vertexBuffers[1]);
	glBindBuffer(GL_ARRAY_BUFFER, arrayBuffers[1]);
	glDrawArrays(GL_QUADS, 0, numberQuads * 4);
	glutSwapBuffers();
	glFlush();
}

/*
* Main program with calls for many of the helper routines.
*/
int main(int argCount, char* argValues[]) {
	glutStartUp(argCount, argValues, "My Test of New Routines");
	// The first two shader files are for Image Mapping, the second
	// two are for Procedural Texturing
	init("texmap.vert", "texmap.frag", "proceduraltex.vert", "proceduraltex.frag");
	glutDisplayFunc(display);
	glutTimerFunc(1000 / 30, timer, 1);
	glutMainLoop();
}
