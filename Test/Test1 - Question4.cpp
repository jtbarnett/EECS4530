// Code for displaying object

GLuint baoID, vaoID;
int numVertices = 30;

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(vaoID);
	glBindBuffer(GL_ARRAY_BUFFER, baoID);
	glDrawArrays(GL_TRIANGLES, 0, numVertices);
	glFlush();
}
