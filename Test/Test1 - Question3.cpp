// Code for setting up object in an array buffer using 2-sub buffers

GLfloat vertices[] = { 0.0f,  0.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f, 1.0f };
GLfloat colors[] = { 1.0f, 0.0f, 0.0f, 1.0f,    1.0f, 1.0f, 0.0f, 1.0f,      0.0f, 1.0f, 1.0f, 1.0f };

glGenVertexArrays(1, vertexBuffers);
glBindVertexArray(vertexBuffers[0]);

glGenBuffers(1, &(arrayBuffers[0]));
glBindBuffer(GL_ARRAY_BUFFER, arrayBuffers[0]);
glBufferData(GL_ARRAY_BUFFER,
	sizeof(vertices) + sizeof(colors),
	NULL, GL_STATIC_DRAW);
glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colors), colors);
