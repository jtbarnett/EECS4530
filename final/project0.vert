#version 330 core
/*
 * This is a simple vertex shader that has inputs for modeling,
 * projection and viewing transforms.  It does not do lighting
 * and does not use any normal information at this time.
 */

uniform mat4 modelingTransform = {{1.0, 0.0, 0.0, 0.0},
                                  {0.0, 1.0, 0.0, 0.0},
								  {0.0, 0.0, 1.0, 0.0},
								  {0.0, 0.0, 0.0, 1.0}};
uniform mat4 projectionTransform = {{1.0, 0.0, 0.0, 0.0}, 
                                    {0.0, 1.0, 0.0, 0.0}, 
									{0.0, 0.0, 600.0, 0.0}, 
									{0.0, 0.0, 0.0, 1.0}};

in vec4 vPosition;		// vertex position (passed in)
in vec4 vColor;			// vertex color (passed in)
out vec4 passedColor;	// the color being passed out to the fragment shader

void main() {
	passedColor = vColor;

	// do the projection thing now...
	gl_Position = projectionTransform * modelingTransform * vPosition/vPosition.w;
}