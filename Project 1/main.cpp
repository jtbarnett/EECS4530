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

GLuint programID;

/*
* Arrays to store the indices/names of the Vertex Array Objects and
* Buffers.  Rather than using the books enum approach I've just
* gone out and made a bunch of them and will use them as needed.
*
* Not the best choice I'm sure.
*/
GLuint vertexBuffers[10], arrayBuffers[10], elementBuffers[10];

/*
* Global variables
*   The location for the transformation and the current rotation
*   angle are set up as globals since multiple methods need to
*   access them.
*/
bool elements;
int nbrTriangles, materialToUse = 0;
int startTriangle = 0, endTriangle = 12;

map<string, GLuint> locationMap;
vector<Drawable*> objectList;
string projectionChoice = "Orthographic";
char viewingChoice = 'Z';

// Prototypes
GLuint buildProgram(string vertexShaderName, string fragmentShaderName);
int glutStartUp(int& argCount, char* argValues[],
	string windowTitle = "No Title", int width = 500, int height = 500);
void setAttributes(float lineWidth = 1.0, GLenum face = GL_FRONT_AND_BACK,
	GLenum fill = GL_FILL);
void buildObjects();
void getLocations();
void init(string vertexShader, string fragmentShader);
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

}

/*
 * read and/or build the objects to be displayed.  Also sets up attributes that are
 * vertex related.
 */
void buildObjects() {
	GLuint vPosition = glGetAttribLocation(programID, "vPosition");
	GLuint vColor = glGetAttribLocation(programID, "vColor");

	// Data for elephant
	vector<float> fileVerticesElephant, fileNormalsElephant, fileTexturesElephant;
	readfile3("elephantTriangles.obj", fileVerticesElephant, fileNormalsElephant, fileTexturesElephant);
	glGenVertexArrays(1, &(vertexBuffers[0]));
	glBindVertexArray(vertexBuffers[0]);
	glGenBuffers(1, &(arrayBuffers[0]));
	glBindBuffer(GL_ARRAY_BUFFER, arrayBuffers[0]);
	glBufferData(GL_ARRAY_BUFFER,
		fileVerticesElephant.size() * sizeof(float) + (fileVerticesElephant.size() / 3) * 4 * sizeof(float),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, fileVerticesElephant.size() * sizeof(float), fileVerticesElephant.data());
	// Create a color array since colors don't come in
	float* fileColorsElephant = new float[fileVerticesElephant.size() / 3 * 4];
	float redElephant = 0.0f, greenElephant = 1.0f, blueElephant = 0.0f, alphaElephant = 1.0f;
	for (unsigned int currentVertex = 0; currentVertex < fileVerticesElephant.size() / 3; currentVertex++) {
		fileColorsElephant[currentVertex * 4] = redElephant;
		fileColorsElephant[currentVertex * 4 + 1] = greenElephant;
		fileColorsElephant[currentVertex * 4 + 2] = blueElephant;
		fileColorsElephant[currentVertex * 4 + 3] = alphaElephant;
	}
	glBufferSubData(GL_ARRAY_BUFFER, fileVerticesElephant.size() * sizeof(float), fileVerticesElephant.size() / 3 * 4 * sizeof(float), fileColorsElephant);

	vPosition = glGetAttribLocation(programID, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	vColor = glGetAttribLocation(programID, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(fileVerticesElephant.size() * sizeof(float)));

	PositionInfo* elephant = new PositionInfo(vertexBuffers[0], arrayBuffers[0],
		fileVerticesElephant.size() / 3, locationMap["modelingTransform"],
		vmath::scale(1.8f, 1.8f, 1.8f), vmath::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	objectList.push_back(elephant);

	// Data for fiat
	vector<float> fileVerticesFiat, fileNormalsFiat, fileTexturesFiat;
	readfile3("fiat.obj", fileVerticesFiat, fileNormalsFiat, fileTexturesFiat);
	glGenVertexArrays(1, &(vertexBuffers[1]));
	glBindVertexArray(vertexBuffers[1]);
	glGenBuffers(1, &(arrayBuffers[1]));
	glBindBuffer(GL_ARRAY_BUFFER, arrayBuffers[1]);
	glBufferData(GL_ARRAY_BUFFER,
		fileVerticesFiat.size() * sizeof(float) + (fileVerticesFiat.size() / 3) * 4 * sizeof(float),
		NULL, GL_STATIC_DRAW); 
	glBufferSubData(GL_ARRAY_BUFFER, 0, fileVerticesFiat.size() * sizeof(float), fileVerticesFiat.data());
	// Create a color array since colors don't come in
	float* fileColorsFiat = new float[fileVerticesFiat.size() / 3 * 4];
	float redFiat = 1.0f, greenFiat = 0.0f, blueFiat = 0.0f, alphaFiat = 1.0f;
	for (unsigned int currentVertex = 0; currentVertex < fileVerticesFiat.size() / 3; currentVertex++) {
		fileColorsFiat[currentVertex * 4] = redFiat;
		fileColorsFiat[currentVertex * 4 + 1] = greenFiat;
		fileColorsFiat[currentVertex * 4 + 2] = blueFiat;
		fileColorsFiat[currentVertex * 4 + 3] = alphaFiat;
	}
	glBufferSubData(GL_ARRAY_BUFFER, fileVerticesFiat.size() * sizeof(float), fileVerticesFiat.size() / 3 * 4 * sizeof(float), fileColorsFiat);

	vPosition = glGetAttribLocation(programID, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	vColor = glGetAttribLocation(programID, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(fileVerticesFiat.size() * sizeof(float)));

	PositionInfo *fiat = new PositionInfo(vertexBuffers[1], arrayBuffers[1],
		fileVerticesFiat.size() / 3, locationMap["modelingTransform"],
		vmath::scale(1.0f, 1.0f, 1.0f), vmath::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	fiat->addTranslation(-3.0f, -1.5f, -2.5f);
	objectList.push_back(fiat);

	// Data for trout
	vector<float> fileVerticesTrout, fileNormalsTrout, fileTexturesTrout;
	readfile3("trout.obj", fileVerticesTrout, fileNormalsTrout, fileTexturesTrout);
	glGenVertexArrays(1, &(vertexBuffers[2]));
	glBindVertexArray(vertexBuffers[2]);
	glGenBuffers(1, &(arrayBuffers[2]));
	glBindBuffer(GL_ARRAY_BUFFER, arrayBuffers[2]);
	glBufferData(GL_ARRAY_BUFFER,
		fileVerticesTrout.size() * sizeof(float) + (fileVerticesTrout.size() / 3) * 4 * sizeof(float),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, fileVerticesTrout.size() * sizeof(float), fileVerticesTrout.data());
	// Create a color array since colors don't come in
	float* fileColorsTrout = new float[fileVerticesTrout.size() / 3 * 4];
	float redTrout = 0.0f, greenTrout = 0.0f, blueTrout = 1.0f, alphaTrout = 1.0f;
	for (unsigned int currentVertex = 0; currentVertex < fileVerticesTrout.size() / 3; currentVertex++) {
		fileColorsTrout[currentVertex * 4] = redTrout;
		fileColorsTrout[currentVertex * 4 + 1] = greenTrout;
		fileColorsTrout[currentVertex * 4 + 2] = blueTrout;
		fileColorsTrout[currentVertex * 4 + 3] = alphaTrout;
	}
	glBufferSubData(GL_ARRAY_BUFFER, fileVerticesTrout.size() * sizeof(float), fileVerticesTrout.size() / 3 * 4 * sizeof(float), fileColorsTrout);

	vPosition = glGetAttribLocation(programID, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	vColor = glGetAttribLocation(programID, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(fileVerticesTrout.size() * sizeof(float)));

	PositionInfo* trout = new PositionInfo(vertexBuffers[2], arrayBuffers[2],
		fileVerticesTrout.size() / 3, locationMap["modelingTransform"],
		vmath::scale(0.6f, 0.6f, 0.6f), vmath::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	vmath::vec3 yAxis = vmath::vec3(0.0f, 1.0f, 0.0f);
	trout->addRotation(140.0f, yAxis);
	trout->addTranslation(1.8f, -0.6f, -2.85f);
	objectList.push_back(trout);
}

/*
 * This fills in the locations of most of the uniform variables for the program.
 * there are better ways of handling this but this is good in going directly from
 * what we had.
 *
 * Revised to get the locations and names of the uniforms from OpenGL.  These
 * are then stored in a map so that we can look up a uniform by name when we
 * need to use it.  The map is still global but it is a little neater than the
 * version that used all the locations.  The locations are still there right now
 * in case that is more useful for you.
 *
 */
void getLocations() {
	/*
	 * Find out how many uniforms there are and go out there and get them from the
	 * shader program.  The locations for each uniform are stored in a global -- locationMap --
	 * for later retrieval.
	 */
	GLint numberBlocks;
	char uniformName[1024];
	int nameLength;
	GLint size;
	GLenum type;
	glGetProgramiv(programID, GL_ACTIVE_UNIFORMS, &numberBlocks);
	for (int blockIndex = 0; blockIndex < numberBlocks; blockIndex++) {
		glGetActiveUniform(programID, blockIndex, 1024, &nameLength, &size, &type, uniformName);
		cout << uniformName << endl;
		locationMap[string(uniformName)] = blockIndex;
	}
}

void init(string vertexShader, string fragmentShader) {
	setAttributes(1.0f, GL_FRONT_AND_BACK, GL_FILL);
	programID = buildProgram(vertexShader, fragmentShader);
	buildObjects();
	getLocations();
}

/*
 * The display routine is basically unchanged at this point.
 */
void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	vmath::mat4 translateMatrix;
	vmath::mat4 viewingTransform;
	vmath::mat4 projectionTransform;

	if (projectionChoice == "Orthographic") {
		projectionTransform = vmath::ortho(-6.0f, 6.0f, -6.0f, 6.0f, -100.0f, 100.0f);
	}
	else if (projectionChoice == "Perspective") {
		projectionTransform = vmath::perspective(60.0f, 1.0f, 1.0f, 100.0f);
	}
	else {
		projectionTransform = vmath::ortho(-6.0f, 6.0f, -6.0f, 6.0f, -20.0f, 20.0f);
	}
	glUniformMatrix4fv(locationMap["projectionTransform"], 1, GL_FALSE, projectionTransform);

	switch (viewingChoice) {
	case 'x':
	case 'X':
		viewingTransform = vmath::lookat(vmath::vec3(8.0f, 0.0f, 0.0f), vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec3(0.0f, 1.0f, 0.0f));
		break;
	case 'y':
	case 'Y':
		viewingTransform = vmath::lookat(vmath::vec3(0.0f, 8.0f, 0.0f), vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec3(0.0f, 0.0f, 1.0f));
		break;
	case 'z':
	case 'Z':
		viewingTransform = vmath::lookat(vmath::vec3(0.0f, 0.0f, 8.0f), vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec3(0.0f, 1.0f, 0.0f));
		break;
	default:
		viewingTransform = vmath::lookat(vmath::vec3(0.0f, 0.0f, 8.0f), vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec3(0.0f, 1.0f, 0.0f));
		break;
	}
	glUniformMatrix4fv(locationMap["viewingTransform"], 1, GL_FALSE,
		viewingTransform);

	/*
	 *  Draw everything in the objectList.  The actual objects are set up in
	 *  the buildObjects function.
	 */
	for (Drawable* d : objectList) {
		d->draw();
	}
	glutSwapBuffers();
}

/*
* Handle keypresses -- only one recognized in this version is q for quit.
*/
void keypress(unsigned char key, int x, int y) {
	switch (key) {
	case 'q':
	case 'Q':
		exit(0);
		break;
	case 'p':
	case 'P':
		projectionChoice = "Perspective";
		break;
	case 'o':
	case 'O':
		projectionChoice = "Orthographic";
		break;
	case 'x':
	case 'X':
		viewingChoice = key;
		break;
	case 'y':
	case 'Y':
		viewingChoice = key;
		break;
	case 'z':
	case 'Z':
		viewingChoice = key;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

/*
* Main program with calls for many of the helper routines.
*/
int main(int argCount, char* argValues[]) {
	glutStartUp(argCount, argValues, "My Test of New Routines");
	init("project0a.vert", "project0.frag");
	glutDisplayFunc(display);
	glutKeyboardFunc(keypress);
	glutMainLoop();
}
