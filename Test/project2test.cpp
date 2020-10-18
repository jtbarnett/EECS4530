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
float rotationAngle;
bool elements;
int nbrTriangles, materialToUse = 0;
int startTriangle = 0, endTriangle = 12;
bool rotationOn = false;

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

	GLfloat vertices[] = {
					   -0.5f, -0.5f, -0.5f, 1.0f, -0.5f,  0.5f,  0.5f, 1.0f, -0.5f, -0.5f,  0.5f, 1.0f,
					   -0.5f, -0.5f, -0.5f, 1.0f, -0.5f,  0.5f,  0.5f, 1.0f, -0.5f,  0.5f, -0.5f, 1.0f,
					   -0.5f, -0.5f, -0.5f, 1.0f, -0.5f,  0.5f, -0.5f, 1.0f,  0.5f,  0.5f, -0.5f, 1.0f,
					   -0.5f, -0.5f, -0.5f, 1.0f,  0.5f,  0.5f, -0.5f, 1.0f,  0.5f, -0.5f, -0.5f, 1.0f,
					   -0.5f, -0.5f, -0.5f, 1.0f,  0.5f, -0.5f, -0.5f, 1.0f,  0.5f, -0.5f,  0.5f, 1.0f,
					   -0.5f, -0.5f, -0.5f, 1.0f,  0.5f, -0.5f,  0.5f, 1.0f, -0.5f, -0.5f,  0.5f, 1.0f,
					   -0.5f, -0.5f,  0.5f, 1.0f, -0.5f,  0.5f,  0.5f, 1.0f,  0.5f,  0.5f,  0.5f, 1.0f,
					   -0.5f, -0.5f,  0.5f, 1.0f,  0.5f,  0.5f,  0.5f, 1.0f,  0.5f, -0.5f,  0.5f, 1.0f,
					   -0.5f,  0.5f,  0.5f, 1.0f, -0.5f,  0.5f, -0.5f, 1.0f,  0.5f,  0.5f,  0.5f, 1.0f,
						0.5f,  0.5f,  0.5f, 1.0f,  0.5f,  0.5f, -0.5f, 1.0f, -0.5f,  0.5f, -0.5f, 1.0f,
						0.5f, -0.5f,  0.5f, 1.0f,  0.5f, -0.5f, -0.5f, 1.0f,  0.5f,  0.5f, -0.5f, 1.0f,
						0.5f, -0.5f,  0.5f, 1.0f,  0.5f,  0.5f, -0.5f, 1.0f,  0.5f,  0.5f,  0.5f, 1.0f
	};

	GLfloat colors[] = {
							0.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f, 1.0f,  0.0f, 1.0f, 1.0f, 1.0f,
							0.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f, 1.0f,  0.0f, 1.0f, 0.0f, 1.0f,
							0.0f, 0.0f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f, 1.0f,  1.0f, 1.0f, 0.0f, 1.0f,
							0.0f, 0.0f, 0.0f, 1.0f,  1.0f, 1.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
							0.0f, 0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
							0.0f, 0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,  0.0f, 0.0f, 1.0f, 1.0f,
							0.0f, 0.0f, 1.0f, 1.0f,  0.0f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 1.0f,
							0.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
							0.0f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f, 1.0f,
							1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f, 1.0f,
							1.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f, 0.0f, 1.0f,  1.0f, 1.0f, 0.0f, 1.0f,
							1.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f, 1.0f
	};

	glGenVertexArrays(1, vertexBuffers);
	glBindVertexArray(vertexBuffers[0]);

	// Alternately...
	// GLuint   vaoID;
	// glGenVertexArrays(1, &vaoID);
	// glBindVertexArray(vaoID);
	//

/*
 * Test code for internal object.
 */
	nbrTriangles = 12;
	glGenBuffers(1, &(arrayBuffers[0]));
	glBindBuffer(GL_ARRAY_BUFFER, arrayBuffers[0]);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vertices) + sizeof(colors),
		NULL, GL_STATIC_DRAW);
	//                               offset in bytes   size in bytes     ptr to data    
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colors), colors);
	/*
	 * Set up variables into the shader programs (Note:  We need the
	 * shaders loaded and built into a program before we do this)
	 */
	GLuint vPosition = glGetAttribLocation(programID, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(programID, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vertices)));


	//	objectList.push_back(new PositionInfo(vertexBuffers[0], 
	//		arrayBuffers[0], nbrTriangles * 3, 
	//		locationMap["modelingTransform"], vmath::scale(1.0f), 
	//		vmath::vec4(1.0f, 1.0f, 1.0f, 1.0f)));
	//float offset;
	//for (offset = 1.06065f; offset < 6.0f; offset += 1.06065f) {
	//	objectList.push_back(new PositionInfo(vertexBuffers[0],
	//		arrayBuffers[0], nbrTriangles * 3, locationMap["modelingTransform"],
	//		vmath::translate(offset, 0.0f, -offset),
	//		vmath::vec4(1.0f, 1.0f, 1.0f, 1.0f)));
	//	objectList.push_back(new PositionInfo(vertexBuffers[0],
	//		arrayBuffers[0], nbrTriangles * 3, locationMap["modelingTransform"],
	//		vmath::translate(-offset, 0.0f, -offset),
	//		vmath::vec4(1.0f, 1.0f, 1.0f, 1.0f)));
	//	objectList.push_back(new PositionInfo(vertexBuffers[0],
	//		arrayBuffers[0], nbrTriangles * 3, locationMap["modelingTransform"],
	//		vmath::translate(0.0f, offset, 0.0f),
	//		vmath::vec4(1.0f, 1.0f, 1.0f, 1.0f)));
	//}
	/*
	* Test code for a file to read in...
	*/
	vector<float> filevertices, filenormals, filetextures;
	readfile3("vwTriangles.obj", filevertices, filenormals, filetextures);
	glGenVertexArrays(1, &(vertexBuffers[1]));
	glBindVertexArray(vertexBuffers[1]);
	glGenBuffers(1, &(arrayBuffers[1]));
	glBindBuffer(GL_ARRAY_BUFFER, arrayBuffers[1]);
	glBufferData(GL_ARRAY_BUFFER,
		filevertices.size() * sizeof(float) +
		(filevertices.size() / 3) * 4 * sizeof(float) +
		filenormals.size() * sizeof(float),
		NULL, GL_STATIC_DRAW);
	//                               offset in bytes   size in bytes     ptr to data    
	glBufferSubData(GL_ARRAY_BUFFER, 0, filevertices.size() * sizeof(float),
		filevertices.data());
	// Create a color array since colors don't come in...
	float* filecolors = new float[filevertices.size() / 3 * 4];
	float red = 1.0f, green = 1.0f, blue = 1.0f, alpha = 1.0f;
	for (unsigned int currentVertex = 0; currentVertex < filevertices.size() / 3; currentVertex++) {
		filecolors[currentVertex * 4] = red;
		filecolors[currentVertex * 4 + 1] = green;
		filecolors[currentVertex * 4 + 2] = blue;
		filecolors[currentVertex * 4 + 3] = alpha;
	}
	glBufferSubData(GL_ARRAY_BUFFER,
		filevertices.size() * sizeof(float),
		filevertices.size() / 3 * 4 * sizeof(float), filecolors);
	glBufferSubData(GL_ARRAY_BUFFER, filevertices.size() * sizeof(float) +
		filevertices.size() / 3 * 4 * sizeof(float),
		filenormals.size() * sizeof(float), filenormals.data());

	vPosition = glGetAttribLocation(programID, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	vColor = glGetAttribLocation(programID, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(filevertices.size() * sizeof(float)));

	GLuint vNormal = glGetAttribLocation(programID, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_TRUE, 0,
		BUFFER_OFFSET(filevertices.size() * sizeof(float) +
			filevertices.size() / 3 * 4 * sizeof(float)));

	objectList.push_back(new PositionInfo(vertexBuffers[1], arrayBuffers[1],
		filevertices.size() / 3, locationMap["modelingMatrix"],
		vmath::scale(1.0f, 1.0f, 1.0f), vmath::vec4(1.0f, 1.0f, 1.0f, 1.0f),
		locationMap["normalMatrix"]));

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

	getLocations();

	buildObjects();



}

/*
* Timer routine -- when called it increments the angle by 1 degree,
* tells Glut to redraw the screen, and sets a timer for 1/30 of a
* second later.
*/
void timer(int value) {
	if (rotationOn) {
		rotationAngle += 1.0f;
		if (rotationAngle >= 360.0)
			rotationAngle -= 360.0;
		glutPostRedisplay();
	}
	glutTimerFunc(1000 / 30, timer, 1);
}

/*
 * The display routine is basically unchanged at this point.
 */
void display() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// needed
	vmath::mat4 currentMatrix = vmath::rotate(rotationAngle, 0.0f, 1.0f, 0.0f);
	vmath::mat4 viewingTransform;
	vmath::mat4 projectionTransform;
	vmath::vec3 ambientLight(0.2f, 0.2f, 0.2f);
	vmath::vec3 directionalLightDirection(0.7071f, 0.7071f, 0.0f);
	vmath::vec3 directionalLightColor(1.0f, 1.0f, 1.0f);
	vmath::vec3 viewDirection(0.0f, 0.0f, 1.0f);
	vmath::vec3 halfVector, sum, norm;
	GLuint ambientLocation;

	halfVector = directionalLightDirection + viewDirection;
	halfVector = vmath::normalize(halfVector);

	glUniform3fv(locationMap["halfVector"], 1, halfVector);
	glUniform3fv(locationMap["directionalLightDirection"], 1, directionalLightDirection);
	glUniform3fv(locationMap["directionalLightColor"], 1, directionalLightColor);
	ambientLocation = glGetUniformLocation(programID, "ambientLight");
	glUniform3fv(ambientLocation, 1, ambientLight);
	projectionTransform = vmath::ortho(-6.0f, 6.0f, -6.0f, 6.0f, -20.0f, 20.0f);
	// Use a 12x12x12 cube centered at the origin.
	//projectionTransform = vmath::perspective(45.0, 1.0, 0.1f, 1000.0f);
	glUniformMatrix4fv(locationMap["projectionMatrix"], 1, GL_FALSE,
		projectionTransform);

	/* for Orthographic We don't need a view transformation unless we want to rotate the camera or point it somewhere
	 * other than down the z axis.  We will use the identity matrix here.
	 */
	viewingTransform = vmath::scale(1.0f);
	viewingTransform = vmath::lookat(vmath::vec3(4.0f, 0.0f, 0.0f),
		vmath::vec3(0.0f, 0.0f, 0.0f),
		vmath::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(locationMap["viewingMatrix"], 1, GL_FALSE,
		viewingTransform);

	glUniformMatrix3fv(locationMap["normalMatrix"], 1,
		GL_FALSE, vmath::mat3(vmath::vec3(1.0f, 0.0f, 0.0f),
			vmath::vec3(0.0f, 1.0f, 0.0f),
			vmath::vec3(0.0f, 0.0f, 1.0f)));

	/*
	 *  Draw everything in the objectList.  The actual objects are set up in
	 *  the buildObjects function.
	 *
	 */


	for (Drawable* d : objectList) {
		d->draw();
	}
	glutSwapBuffers();  // if double buffered
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
	case 'r':
	case 'R':
		rotationOn = !rotationOn;
		break;
	case 'x':
	case 'X':
	case 'y':
	case 'Y':
	case 'z':
	case 'Z':
		viewingChoice = key;
		break;
	default:
		// do nothing....
		break;
	}
	glutPostRedisplay();
}

/*
* Main program with calls for many of the helper routines.
*/
int main(int argCount, char* argValues[]) {

	glutStartUp(argCount, argValues, "My Test of New Routines");
	init("directional.vert", "directional.frag");
	glutDisplayFunc(display);
	glutTimerFunc(1000 / 30, timer, 1);
	glutKeyboardFunc(keypress);
	glutMainLoop();
}
