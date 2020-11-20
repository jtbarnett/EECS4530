#version 330 core

uniform vec3 BrickColor, MortarColor;
uniform vec3 BrickSize;
uniform vec3 BrickPct;

in vec3 MCposition;
in float LightIntensity;

out vec4 FragColor;

void main()
{
    vec3 color;
    vec3 position;
    vec3 useBrick;

    position = MCposition / BrickSize;

    if ((fract(position.y * 0.5) > 0.5) && (((position.z * 0.2) == 1.0) || ((position.z * 0.2) == -1.0)))
    {
        position.x += 0.5;
    }

    if ((fract(position.y * 0.5) > 0.5) && (((position.x * 0.2) == -4.0) || ((position.x * 0.2) == -2.0)))
    {
        position.z += 0.5;
    }

    if ((fract(position.z * 0.5) > 0.5) && (((position.y * 0.2) == 1.0) || ((position.y * 0.2) == -1.0)))
    {
        position.x += 0.5;
    }

    position = fract(position);

    useBrick = step(position, BrickPct);

    color = mix(MortarColor, BrickColor, useBrick.x * useBrick.y * useBrick.z);
    color *= LightIntensity;
    FragColor = vec4(color, 1.0);
}
