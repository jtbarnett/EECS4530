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
GLuint vertexBuffers[11], arrayBuffers[11], elementBuffers[11];

/*
* Global variables
*   The location for the transformation and the current rotation
*   angle are set up as globals since multiple methods need to
*   access them.
*/
float rotationAngle = 1.0f;
bool fullscreen = false;
bool elements;
int nbrTriangles, materialToUse = 0;
int startTriangle = 0, endTriangle = 12;
map<string, GLuint> locationMap;
vector<Drawable*> objectList;
float z = 25.0f;
float x = 0.0f;
float zDirection = -1.0f;
float xDirection = 0.0f;
float angle = 0.0f;
vmath::vec3 xAxis = vmath::vec3(1.0f, 0.0f, 0.0f);
vmath::vec3 yAxis = vmath::vec3(0.0f, 1.0f, 0.0f);
vmath::vec3 zAxis = vmath::vec3(0.0f, 0.0f, 1.0f);

// List of objects
PositionInfo* carousel;
PositionInfo* tree1;
PositionInfo* tree2;
PositionInfo* tree3;
PositionInfo* tree4;
PositionInfo* tree5;
PositionInfo* tree6;
PositionInfo* bench1;
PositionInfo* bench2;
PositionInfo* bench3;
PositionInfo* bench4;

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
	GLuint vNormal = glGetAttribLocation(programID, "vNormal");

	float redTree1 = 0.133f, greenTree1 = 0.545f, blueTree1 = 0.133f, alphaTree = 1.0f;		// forrest green
	float redTree2 = 0.0f, greenTree2 = 0.392f, blueTree2 = 0.0f;							// dark green
	float redTree3 = 0.604f, greenTree3 = 0.804f, blueTree3 = 0.196f;						// yellow/green
	float redBench = 0.396f, greenBench = 0.263f, blueBench = 0.129f, alphaBench = 1.0f;	// brown

	// Carousel
	vector<float> fileVerticesCarousel, fileNormalsCarousel, fileTexturesCarousel;
	readfile3("carousel.obj", fileVerticesCarousel, fileNormalsCarousel, fileTexturesCarousel);
	glGenVertexArrays(1, &(vertexBuffers[0]));
	glBindVertexArray(vertexBuffers[0]);
	glGenBuffers(1, &(arrayBuffers[0]));
	glBindBuffer(GL_ARRAY_BUFFER, arrayBuffers[0]);
	glBufferData(GL_ARRAY_BUFFER,
		fileVerticesCarousel.size() * sizeof(float) + (fileVerticesCarousel.size() / 3) * 4 * sizeof(float),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, fileVerticesCarousel.size() * sizeof(float), fileVerticesCarousel.data());
	float* fileColorsCarousel = new float[fileVerticesCarousel.size() / 3 * 4];
	float redCarousel = 0.635f, greenCarousel = 0.486f, blueCarousel = 0.635f, alphaCarousel = 1.0f;
	for (unsigned int currentVertex = 0; currentVertex < fileVerticesCarousel.size() / 3; currentVertex++) {
		fileColorsCarousel[currentVertex * 4] = redCarousel;
		fileColorsCarousel[currentVertex * 4 + 1] = greenCarousel;
		fileColorsCarousel[currentVertex * 4 + 2] = blueCarousel;
		fileColorsCarousel[currentVertex * 4 + 3] = alphaCarousel;
	}
	glBufferSubData(GL_ARRAY_BUFFER, fileVerticesCarousel.size() * sizeof(float), fileVerticesCarousel.size() / 3 * 4 * sizeof(float), fileColorsCarousel);
	glBufferSubData(GL_ARRAY_BUFFER, fileVerticesCarousel.size() * sizeof(float) +
		fileVerticesCarousel.size() / 3 * 4 * sizeof(float),
		fileNormalsCarousel.size() * sizeof(float), fileNormalsCarousel.data());

	vPosition = glGetAttribLocation(programID, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	vColor = glGetAttribLocation(programID, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(fileVerticesCarousel.size() * sizeof(float)));

	vNormal = glGetAttribLocation(programID, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_TRUE, 0,
		BUFFER_OFFSET(fileVerticesCarousel.size() * sizeof(float) +
			fileVerticesCarousel.size() / 3 * 4 * sizeof(float)));

	carousel = new PositionInfo(vertexBuffers[0], arrayBuffers[0],
		fileVerticesCarousel.size() / 3, locationMap["modelingMatrix"],
		vmath::scale(0.4f, 0.4f, 0.4f), vmath::vec4(1.0f, 1.0f, 1.0f, 1.0f), locationMap["normalMatrix"]);

	carousel->addRotation(-90.0f, xAxis);
	carousel->addTranslation(0.0f, -2.0f, 0.0f);
	objectList.push_back(carousel);


	// Tree 1
	vector<float> fileVertices, fileNormals, fileTextures;
	readfile3("tree.obj", fileVertices, fileNormals, fileTextures);
	glGenVertexArrays(1, &(vertexBuffers[1]));
	glBindVertexArray(vertexBuffers[1]);
	glGenBuffers(1, &(arrayBuffers[1]));
	glBindBuffer(GL_ARRAY_BUFFER, arrayBuffers[1]);
	glBufferData(GL_ARRAY_BUFFER,
		fileVertices.size() * sizeof(float) + (fileVertices.size() / 3) * 4 * sizeof(float),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, fileVertices.size() * sizeof(float), fileVertices.data());
	float* fileColors = new float[fileVertices.size() / 3 * 4];
	for (unsigned int currentVertex = 0; currentVertex < fileVertices.size() / 3; currentVertex++) {
		fileColors[currentVertex * 4] = redTree1;
		fileColors[currentVertex * 4 + 1] = greenTree1;
		fileColors[currentVertex * 4 + 2] = blueTree1;
		fileColors[currentVertex * 4 + 3] = alphaTree;
	}
	glBufferSubData(GL_ARRAY_BUFFER, fileVertices.size() * sizeof(float), fileVertices.size() / 3 * 4 * sizeof(float), fileColors);
	glBufferSubData(GL_ARRAY_BUFFER, fileVertices.size() * sizeof(float) +
		fileVertices.size() / 3 * 4 * sizeof(float),
		fileNormals.size() * sizeof(float), fileNormals.data());

	vPosition = glGetAttribLocation(programID, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	vColor = glGetAttribLocation(programID, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(fileVertices.size() * sizeof(float)));

	vNormal = glGetAttribLocation(programID, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_TRUE, 0,
		BUFFER_OFFSET(fileVertices.size() * sizeof(float) +
			fileVertices.size() / 3 * 4 * sizeof(float)));

	tree1 = new PositionInfo(vertexBuffers[1], arrayBuffers[1],
		fileVertices.size() / 3, locationMap["modelingMatrix"],
		vmath::scale(0.8f, 0.8f, 0.8f), vmath::vec4(1.0f, 1.0f, 1.0f, 1.0f), locationMap["normalMatrix"]);

	tree1->addTranslation(-3.5f, -2.0f, 15.0f);
	objectList.push_back(tree1);


	// Tree 2
	readfile3("tree.obj", fileVertices, fileNormals, fileTextures);
	glGenVertexArrays(1, &(vertexBuffers[2]));
	glBindVertexArray(vertexBuffers[2]);
	glGenBuffers(1, &(arrayBuffers[2]));
	glBindBuffer(GL_ARRAY_BUFFER, arrayBuffers[2]);
	glBufferData(GL_ARRAY_BUFFER,
		fileVertices.size() * sizeof(float) + (fileVertices.size() / 3) * 4 * sizeof(float),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, fileVertices.size() * sizeof(float), fileVertices.data());
	fileColors = new float[fileVertices.size() / 3 * 4];
	for (unsigned int currentVertex = 0; currentVertex < fileVertices.size() / 3; currentVertex++) {
		fileColors[currentVertex * 4] = redTree2;
		fileColors[currentVertex * 4 + 1] = greenTree2;
		fileColors[currentVertex * 4 + 2] = blueTree2;
		fileColors[currentVertex * 4 + 3] = alphaTree;
	}
	glBufferSubData(GL_ARRAY_BUFFER, fileVertices.size() * sizeof(float), fileVertices.size() / 3 * 4 * sizeof(float), fileColors);
	glBufferSubData(GL_ARRAY_BUFFER, fileVertices.size() * sizeof(float) +
		fileVertices.size() / 3 * 4 * sizeof(float),
		fileNormals.size() * sizeof(float), fileNormals.data());

	vPosition = glGetAttribLocation(programID, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	vColor = glGetAttribLocation(programID, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(fileVertices.size() * sizeof(float)));

	vNormal = glGetAttribLocation(programID, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_TRUE, 0,
		BUFFER_OFFSET(fileVertices.size() * sizeof(float) +
			fileVertices.size() / 3 * 4 * sizeof(float)));

	tree2 = new PositionInfo(vertexBuffers[2], arrayBuffers[2],
		fileVertices.size() / 3, locationMap["modelingMatrix"],
		vmath::scale(0.8f, 0.8f, 0.8f), vmath::vec4(1.0f, 1.0f, 1.0f, 1.0f), locationMap["normalMatrix"]);

	tree2->addTranslation(3.5f, -2.0f, 15.0f);
	objectList.push_back(tree2);


	// Tree 3
	readfile3("tree.obj", fileVertices, fileNormals, fileTextures);
	glGenVertexArrays(1, &(vertexBuffers[3]));
	glBindVertexArray(vertexBuffers[3]);
	glGenBuffers(1, &(arrayBuffers[3]));
	glBindBuffer(GL_ARRAY_BUFFER, arrayBuffers[3]);
	glBufferData(GL_ARRAY_BUFFER,
		fileVertices.size() * sizeof(float) + (fileVertices.size() / 3) * 4 * sizeof(float),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, fileVertices.size() * sizeof(float), fileVertices.data());
	fileColors = new float[fileVertices.size() / 3 * 4];
	for (unsigned int currentVertex = 0; currentVertex < fileVertices.size() / 3; currentVertex++) {
		fileColors[currentVertex * 4] = redTree3;
		fileColors[currentVertex * 4 + 1] = greenTree3;
		fileColors[currentVertex * 4 + 2] = blueTree3;
		fileColors[currentVertex * 4 + 3] = alphaTree;
	}
	glBufferSubData(GL_ARRAY_BUFFER, fileVertices.size() * sizeof(float), fileVertices.size() / 3 * 4 * sizeof(float), fileColors);
	glBufferSubData(GL_ARRAY_BUFFER, fileVertices.size() * sizeof(float) +
		fileVertices.size() / 3 * 4 * sizeof(float),
		fileNormals.size() * sizeof(float), fileNormals.data());

	vPosition = glGetAttribLocation(programID, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	vColor = glGetAttribLocation(programID, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(fileVertices.size() * sizeof(float)));

	vNormal = glGetAttribLocation(programID, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_TRUE, 0,
		BUFFER_OFFSET(fileVertices.size() * sizeof(float) +
			fileVertices.size() / 3 * 4 * sizeof(float)));

	tree3 = new PositionInfo(vertexBuffers[3], arrayBuffers[3],
		fileVertices.size() / 3, locationMap["modelingMatrix"],
		vmath::scale(0.8f, 0.8f, 0.8f), vmath::vec4(1.0f, 1.0f, 1.0f, 1.0f), locationMap["normalMatrix"]);

	tree3->addRotation(45.0f, yAxis);
	tree3->addTranslation(-3.5f, -2.0f, 10.0f);
	objectList.push_back(tree3);


	// Tree 4
	readfile3("tree.obj", fileVertices, fileNormals, fileTextures);
	glGenVertexArrays(1, &(vertexBuffers[4]));
	glBindVertexArray(vertexBuffers[4]);
	glGenBuffers(1, &(arrayBuffers[4]));
	glBindBuffer(GL_ARRAY_BUFFER, arrayBuffers[4]);
	glBufferData(GL_ARRAY_BUFFER,
		fileVertices.size() * sizeof(float) + (fileVertices.size() / 3) * 4 * sizeof(float),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, fileVertices.size() * sizeof(float), fileVertices.data());
	fileColors = new float[fileVertices.size() / 3 * 4];
	for (unsigned int currentVertex = 0; currentVertex < fileVertices.size() / 3; currentVertex++) {
		fileColors[currentVertex * 4] = redTree1;
		fileColors[currentVertex * 4 + 1] = greenTree1;
		fileColors[currentVertex * 4 + 2] = blueTree1;
		fileColors[currentVertex * 4 + 3] = alphaTree;
	}
	glBufferSubData(GL_ARRAY_BUFFER, fileVertices.size() * sizeof(float), fileVertices.size() / 3 * 4 * sizeof(float), fileColors);
	glBufferSubData(GL_ARRAY_BUFFER, fileVertices.size() * sizeof(float) +
		fileVertices.size() / 3 * 4 * sizeof(float),
		fileNormals.size() * sizeof(float), fileNormals.data());

	vPosition = glGetAttribLocation(programID, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	vColor = glGetAttribLocation(programID, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(fileVertices.size() * sizeof(float)));

	vNormal = glGetAttribLocation(programID, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_TRUE, 0,
		BUFFER_OFFSET(fileVertices.size() * sizeof(float) +
			fileVertices.size() / 3 * 4 * sizeof(float)));

	tree4 = new PositionInfo(vertexBuffers[4], arrayBuffers[4],
		fileVertices.size() / 3, locationMap["modelingMatrix"],
		vmath::scale(0.8f, 0.8f, 0.8f), vmath::vec4(1.0f, 1.0f, 1.0f, 1.0f), locationMap["normalMatrix"]);

	tree4->addRotation(45.0f, yAxis);
	tree4->addTranslation(3.5f, -2.0f, 10.0f);
	objectList.push_back(tree4);


	// Tree 5
	readfile3("tree.obj", fileVertices, fileNormals, fileTextures);
	glGenVertexArrays(1, &(vertexBuffers[5]));
	glBindVertexArray(vertexBuffers[5]);
	glGenBuffers(1, &(arrayBuffers[5]));
	glBindBuffer(GL_ARRAY_BUFFER, arrayBuffers[5]);
	glBufferData(GL_ARRAY_BUFFER,
		fileVertices.size() * sizeof(float) + (fileVertices.size() / 3) * 4 * sizeof(float),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, fileVertices.size() * sizeof(float), fileVertices.data());
	fileColors = new float[fileVertices.size() / 3 * 4];
	for (unsigned int currentVertex = 0; currentVertex < fileVertices.size() / 3; currentVertex++) {
		fileColors[currentVertex * 4] = redTree2;
		fileColors[currentVertex * 4 + 1] = greenTree2;
		fileColors[currentVertex * 4 + 2] = blueTree2;
		fileColors[currentVertex * 4 + 3] = alphaTree;
	}
	glBufferSubData(GL_ARRAY_BUFFER, fileVertices.size() * sizeof(float), fileVertices.size() / 3 * 4 * sizeof(float), fileColors);
	glBufferSubData(GL_ARRAY_BUFFER, fileVertices.size() * sizeof(float) +
		fileVertices.size() / 3 * 4 * sizeof(float),
		fileNormals.size() * sizeof(float), fileNormals.data());

	vPosition = glGetAttribLocation(programID, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	vColor = glGetAttribLocation(programID, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(fileVertices.size() * sizeof(float)));

	vNormal = glGetAttribLocation(programID, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_TRUE, 0,
		BUFFER_OFFSET(fileVertices.size() * sizeof(float) +
			fileVertices.size() / 3 * 4 * sizeof(float)));

	tree5 = new PositionInfo(vertexBuffers[5], arrayBuffers[5],
		fileVertices.size() / 3, locationMap["modelingMatrix"],
		vmath::scale(0.8f, 0.8f, 0.8f), vmath::vec4(1.0f, 1.0f, 1.0f, 1.0f), locationMap["normalMatrix"]);

	tree5->addRotation(90.0f, yAxis);
	tree5->addTranslation(-3.5f, -2.0f, 5.0f);
	objectList.push_back(tree5);

	// Tree 6
	readfile3("tree.obj", fileVertices, fileNormals, fileTextures);
	glGenVertexArrays(1, &(vertexBuffers[6]));
	glBindVertexArray(vertexBuffers[6]);
	glGenBuffers(1, &(arrayBuffers[6]));
	glBindBuffer(GL_ARRAY_BUFFER, arrayBuffers[6]);
	glBufferData(GL_ARRAY_BUFFER,
		fileVertices.size() * sizeof(float) + (fileVertices.size() / 3) * 4 * sizeof(float),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, fileVertices.size() * sizeof(float), fileVertices.data());
	fileColors = new float[fileVertices.size() / 3 * 4];
	for (unsigned int currentVertex = 0; currentVertex < fileVertices.size() / 3; currentVertex++) {
		fileColors[currentVertex * 4] = redTree3;
		fileColors[currentVertex * 4 + 1] = greenTree3;
		fileColors[currentVertex * 4 + 2] = blueTree3;
		fileColors[currentVertex * 4 + 3] = alphaTree;
	}
	glBufferSubData(GL_ARRAY_BUFFER, fileVertices.size() * sizeof(float), fileVertices.size() / 3 * 4 * sizeof(float), fileColors);
	glBufferSubData(GL_ARRAY_BUFFER, fileVertices.size() * sizeof(float) +
		fileVertices.size() / 3 * 4 * sizeof(float),
		fileNormals.size() * sizeof(float), fileNormals.data());

	vPosition = glGetAttribLocation(programID, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	vColor = glGetAttribLocation(programID, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(fileVertices.size() * sizeof(float)));

	vNormal = glGetAttribLocation(programID, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_TRUE, 0,
		BUFFER_OFFSET(fileVertices.size() * sizeof(float) +
			fileVertices.size() / 3 * 4 * sizeof(float)));

	tree6 = new PositionInfo(vertexBuffers[6], arrayBuffers[6],
		fileVertices.size() / 3, locationMap["modelingMatrix"],
		vmath::scale(0.8f, 0.8f, 0.8f), vmath::vec4(1.0f, 1.0f, 1.0f, 1.0f), locationMap["normalMatrix"]);

	tree6->addRotation(90.0f, yAxis);
	tree6->addTranslation(3.5f, -2.0f, 5.0f);
	objectList.push_back(tree6);


	// Bench 1
	readfile3("bench.obj", fileVertices, fileNormals, fileTextures);
	glGenVertexArrays(1, &(vertexBuffers[7]));
	glBindVertexArray(vertexBuffers[7]);
	glGenBuffers(1, &(arrayBuffers[7]));
	glBindBuffer(GL_ARRAY_BUFFER, arrayBuffers[7]);
	glBufferData(GL_ARRAY_BUFFER,
		fileVertices.size() * sizeof(float) + (fileVertices.size() / 3) * 4 * sizeof(float),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, fileVertices.size() * sizeof(float), fileVertices.data());
	fileColors = new float[fileVertices.size() / 3 * 4];
	for (unsigned int currentVertex = 0; currentVertex < fileVertices.size() / 3; currentVertex++) {
		fileColors[currentVertex * 4] = redBench;
		fileColors[currentVertex * 4 + 1] = greenBench;
		fileColors[currentVertex * 4 + 2] = blueBench;
		fileColors[currentVertex * 4 + 3] = alphaBench;
	}
	glBufferSubData(GL_ARRAY_BUFFER, fileVertices.size() * sizeof(float), fileVertices.size() / 3 * 4 * sizeof(float), fileColors);
	glBufferSubData(GL_ARRAY_BUFFER, fileVertices.size() * sizeof(float) +
		fileVertices.size() / 3 * 4 * sizeof(float),
		fileNormals.size() * sizeof(float), fileNormals.data());

	vPosition = glGetAttribLocation(programID, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	vColor = glGetAttribLocation(programID, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(fileVertices.size() * sizeof(float)));

	vNormal = glGetAttribLocation(programID, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_TRUE, 0,
		BUFFER_OFFSET(fileVertices.size() * sizeof(float) +
			fileVertices.size() / 3 * 4 * sizeof(float)));

	bench1 = new PositionInfo(vertexBuffers[7], arrayBuffers[7],
		fileVertices.size() / 3, locationMap["modelingMatrix"],
		vmath::scale(0.006f, 0.006f, 0.006f), vmath::vec4(1.0f, 1.0f, 1.0f, 1.0f), locationMap["normalMatrix"]);

	bench1->addRotation(90.0f, yAxis);
	bench1->addTranslation(-3.5f, -2.0f, 12.5f);
	objectList.push_back(bench1);


	// Bench 2
	readfile3("bench.obj", fileVertices, fileNormals, fileTextures);
	glGenVertexArrays(1, &(vertexBuffers[8]));
	glBindVertexArray(vertexBuffers[8]);
	glGenBuffers(1, &(arrayBuffers[8]));
	glBindBuffer(GL_ARRAY_BUFFER, arrayBuffers[8]);
	glBufferData(GL_ARRAY_BUFFER,
		fileVertices.size() * sizeof(float) + (fileVertices.size() / 3) * 4 * sizeof(float),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, fileVertices.size() * sizeof(float), fileVertices.data());
	fileColors = new float[fileVertices.size() / 3 * 4];
	for (unsigned int currentVertex = 0; currentVertex < fileVertices.size() / 3; currentVertex++) {
		fileColors[currentVertex * 4] = redBench;
		fileColors[currentVertex * 4 + 1] = greenBench;
		fileColors[currentVertex * 4 + 2] = blueBench;
		fileColors[currentVertex * 4 + 3] = alphaBench;
	}
	glBufferSubData(GL_ARRAY_BUFFER, fileVertices.size() * sizeof(float), fileVertices.size() / 3 * 4 * sizeof(float), fileColors);
	glBufferSubData(GL_ARRAY_BUFFER, fileVertices.size() * sizeof(float) +
		fileVertices.size() / 3 * 4 * sizeof(float),
		fileNormals.size() * sizeof(float), fileNormals.data());

	vPosition = glGetAttribLocation(programID, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	vColor = glGetAttribLocation(programID, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(fileVertices.size() * sizeof(float)));

	vNormal = glGetAttribLocation(programID, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_TRUE, 0,
		BUFFER_OFFSET(fileVertices.size() * sizeof(float) +
			fileVertices.size() / 3 * 4 * sizeof(float)));

	bench2 = new PositionInfo(vertexBuffers[8], arrayBuffers[8],
		fileVertices.size() / 3, locationMap["modelingMatrix"],
		vmath::scale(0.006f, 0.006f, 0.006f), vmath::vec4(1.0f, 1.0f, 1.0f, 1.0f), locationMap["normalMatrix"]);

	bench2->addRotation(90.0f, yAxis);
	bench2->addTranslation(3.5f, -2.0f, 12.5f);
	objectList.push_back(bench2);


	// Bench 3
	readfile3("bench.obj", fileVertices, fileNormals, fileTextures);
	glGenVertexArrays(1, &(vertexBuffers[9]));
	glBindVertexArray(vertexBuffers[9]);
	glGenBuffers(1, &(arrayBuffers[9]));
	glBindBuffer(GL_ARRAY_BUFFER, arrayBuffers[9]);
	glBufferData(GL_ARRAY_BUFFER,
		fileVertices.size() * sizeof(float) + (fileVertices.size() / 3) * 4 * sizeof(float),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, fileVertices.size() * sizeof(float), fileVertices.data());
	fileColors = new float[fileVertices.size() / 3 * 4];
	for (unsigned int currentVertex = 0; currentVertex < fileVertices.size() / 3; currentVertex++) {
		fileColors[currentVertex * 4] = redBench;
		fileColors[currentVertex * 4 + 1] = greenBench;
		fileColors[currentVertex * 4 + 2] = blueBench;
		fileColors[currentVertex * 4 + 3] = alphaBench;
	}
	glBufferSubData(GL_ARRAY_BUFFER, fileVertices.size() * sizeof(float), fileVertices.size() / 3 * 4 * sizeof(float), fileColors);
	glBufferSubData(GL_ARRAY_BUFFER, fileVertices.size() * sizeof(float) +
		fileVertices.size() / 3 * 4 * sizeof(float),
		fileNormals.size() * sizeof(float), fileNormals.data());

	vPosition = glGetAttribLocation(programID, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	vColor = glGetAttribLocation(programID, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(fileVertices.size() * sizeof(float)));

	vNormal = glGetAttribLocation(programID, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_TRUE, 0,
		BUFFER_OFFSET(fileVertices.size() * sizeof(float) +
			fileVertices.size() / 3 * 4 * sizeof(float)));

	bench3 = new PositionInfo(vertexBuffers[9], arrayBuffers[9],
		fileVertices.size() / 3, locationMap["modelingMatrix"],
		vmath::scale(0.006f, 0.006f, 0.006f), vmath::vec4(1.0f, 1.0f, 1.0f, 1.0f), locationMap["normalMatrix"]);

	bench3->addRotation(90.0f, yAxis);
	bench3->addTranslation(-3.5f, -2.0f, 7.5f);
	objectList.push_back(bench3);


	// Bench 4
	readfile3("bench.obj", fileVertices, fileNormals, fileTextures);
	glGenVertexArrays(1, &(vertexBuffers[10]));
	glBindVertexArray(vertexBuffers[10]);
	glGenBuffers(1, &(arrayBuffers[10]));
	glBindBuffer(GL_ARRAY_BUFFER, arrayBuffers[10]);
	glBufferData(GL_ARRAY_BUFFER,
		fileVertices.size() * sizeof(float) + (fileVertices.size() / 3) * 4 * sizeof(float),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, fileVertices.size() * sizeof(float), fileVertices.data());
	fileColors = new float[fileVertices.size() / 3 * 4];
	for (unsigned int currentVertex = 0; currentVertex < fileVertices.size() / 3; currentVertex++) {
		fileColors[currentVertex * 4] = redBench;
		fileColors[currentVertex * 4 + 1] = greenBench;
		fileColors[currentVertex * 4 + 2] = blueBench;
		fileColors[currentVertex * 4 + 3] = alphaBench;
	}
	glBufferSubData(GL_ARRAY_BUFFER, fileVertices.size() * sizeof(float), fileVertices.size() / 3 * 4 * sizeof(float), fileColors);
	glBufferSubData(GL_ARRAY_BUFFER, fileVertices.size() * sizeof(float) +
		fileVertices.size() / 3 * 4 * sizeof(float),
		fileNormals.size() * sizeof(float), fileNormals.data());

	vPosition = glGetAttribLocation(programID, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	vColor = glGetAttribLocation(programID, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(fileVertices.size() * sizeof(float)));

	vNormal = glGetAttribLocation(programID, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_TRUE, 0,
		BUFFER_OFFSET(fileVertices.size() * sizeof(float) +
			fileVertices.size() / 3 * 4 * sizeof(float)));

	bench4 = new PositionInfo(vertexBuffers[10], arrayBuffers[10],
		fileVertices.size() / 3, locationMap["modelingMatrix"],
		vmath::scale(0.006f, 0.006f, 0.006f), vmath::vec4(1.0f, 1.0f, 1.0f, 1.0f), locationMap["normalMatrix"]);

	bench4->addRotation(90.0f, yAxis);
	bench4->addTranslation(3.5f, -2.0f, 7.5f);
	objectList.push_back(bench4);
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
* Function for rotating the carousel object
*/
void timer(int value) {
	carousel->addRotation(rotationAngle, yAxis);
	glutPostRedisplay();
	glutTimerFunc(1000 / 30, timer, 1);
}

/*
 * The display routine is basically unchanged at this point.
 */
void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	vmath::mat4 viewingTransform;
	vmath::mat4 projectionTransform;
	vmath::vec3 ambientLight(0.8f, 0.8f, 0.8f);
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

	projectionTransform = vmath::perspective(60.0f, 1.0f, 1.0f, 100.0f);
	glUniformMatrix4fv(locationMap["projectionMatrix"], 1, GL_FALSE, projectionTransform);

	viewingTransform = vmath::scale(1.0f);
	viewingTransform = vmath::lookat(vmath::vec3(x, 0.0f, z), vmath::vec3(x + xDirection, 0.0f, z + zDirection), vmath::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(locationMap["viewingMatrix"], 1, GL_FALSE, viewingTransform);

	glUniformMatrix3fv(locationMap["normalMatrix"], 1, GL_FALSE,
		vmath::mat3(vmath::vec3(1.0f, 0.0f, 0.0f),
			vmath::vec3(0.0f, 1.0f, 0.0f),
			vmath::vec3(0.0f, 0.0f, 1.0f)));

	for (Drawable* d : objectList) {
		d->draw();
	}
	glutSwapBuffers();
}

void keypress(unsigned char key, int x, int y) {
	switch (key) {
	case 'q':
	case 'Q':
		exit(0);
		break;
	case 'f':
	case 'F':
		if (!fullscreen) {
			glutFullScreen();
			fullscreen = true;
		}
		break;
	case 'w':
	case 'W':
		if (fullscreen) {
			glutPositionWindow(0, 0);
			glutReshapeWindow(500, 500);
			fullscreen = false;
		}
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void SpecialInput(int key, int x, int y) {

	float fraction = 0.5f;

	switch (key) {
	case GLUT_KEY_UP:
		x += xDirection * fraction;
		z += zDirection * fraction;
		break;
	case GLUT_KEY_DOWN:
		x -= xDirection * fraction;
		z -= zDirection * fraction;
		break;
	case GLUT_KEY_LEFT:
		angle -= 0.01f;
		xDirection = sin(angle);
		zDirection = -cos(angle);
		break;
	case GLUT_KEY_RIGHT:
		angle += 0.01f;
		xDirection = sin(angle);
		zDirection = -cos(angle);
		break;
	}
	glutPostRedisplay();
}

int main(int argCount, char* argValues[]) {
	glutStartUp(argCount, argValues, "My Test of New Routines");
	init("directional.vert", "directional.frag");
	glutDisplayFunc(display);
	glutTimerFunc(1000 / 30, timer, 1);
	glutKeyboardFunc(keypress);
	glutSpecialFunc(SpecialInput);
	glutMainLoop();
}
