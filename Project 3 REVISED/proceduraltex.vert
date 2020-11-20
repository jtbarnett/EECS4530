#version 330 core

in vec4 vPosition;
in vec3 vNormal;

uniform mat4 modelingTransform;
uniform mat4 projectionTransform;
uniform mat4 viewingTransform;
uniform mat3 normalTransform;

uniform vec3 LightPosition;

const float SpecularContribution = 0.3;
const float DiffuseContribution = 1.0 - SpecularContribution;

out float LightIntensity;
out vec3 MCposition;

void main()
{
    vec3 ecPosition = vec3(modelingTransform * vPosition);
    vec3 tnorm = normalize(normalTransform * vNormal);
    vec3 lightVec = normalize(LightPosition - ecPosition);
    vec3 reflectVec = reflect(-lightVec, tnorm);
    vec3 viewVec = normalize(-ecPosition);
    float diffuse = max(dot(lightVec, tnorm), 0.0);
    float spec = 1.5;

    if (diffuse > 0.0)
    {
        spec = max(dot(reflectVec, viewVec), 0.0);
        spec = pow(spec, 16.0);
    }

    LightIntensity = DiffuseContribution * diffuse + SpecularContribution * spec;

    MCposition = vPosition.xyz;
    gl_Position = projectionTransform * viewingTransform * modelingTransform * vPosition/vPosition.w;
}