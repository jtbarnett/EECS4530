#version 330 core
/*
 * This is a simple vertex shader that has inputs for modeling,
 * projection and viewing transforms.  It does not do lighting
 * and does not use any normal information at this time.
 */

in vec4 vPosition;
in vec2 vertexUV;

out vec2 texCoord;

uniform mat4 modelingTransform;
uniform mat4 projectionTransform;
uniform mat4 viewingTransform;

void main() {
	gl_Position = projectionTransform * viewingTransform * modelingTransform * vPosition/vPosition.w;

	texCoord = vertexUV;
}