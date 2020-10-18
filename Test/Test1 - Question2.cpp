// Code to display window

int windowWidth = 512, windowHeight = 512;
string windowTitle = "Graphics Test";

glutInit(&argCount, argValues);
glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
glutInitWindowSize(windowWidth, windowHeight);
glutInitContextVersion(4, 3);
glutInitContextProfile(GLUT_CORE_PROFILE);
glutCreateWindow(windowTitle);

init();
glutDisplayFunc(display);
glutMainLoop();
