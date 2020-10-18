// Fragment shader for global ambient lighting
#version 330 core
uniform vec3 ambientLight;
uniform vec3 directionalLightDirection;
uniform vec3 directionalLightColor;
uniform vec3 halfVector;
uniform float shininess;
uniform float strength;

in vec3 Normal;
in vec4 Color;

out vec4 FragColor;
void main()
{
	float diffuse, specular;
	diffuse = max(0.0, dot(Normal, directionalLightDirection));
	specular = max(0.0, dot(Normal, halfVector));
	if (diffuse == 0.0) {
		specular = 0.0;
	} else {
		specular = pow(specular, shininess);
	}
	vec3 scatteredLight = ambientLight + directionalLightColor * diffuse;
	vec3 reflectedLight = directionalLightColor * specular * strength;
	vec3 rgb = clamp(Color.rgb * scatteredLight + reflectedLight, vec3(0.0), vec3(1.0));

	FragColor = vec4(rgb, Color.a);
}