#version 330 core

layout(location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;

uniform mat4 uM; 
uniform mat4 uV; 
uniform mat4 uP;

out vec3 FragPos;
out vec3 Normal;

// Za Guroovo
//uniform vec3 lightPos;
//uniform vec3 viewPos;
//uniform vec3 lightColor;
//
//uniform bool isGouraud;
//
//out vec3 LightingColor;
//out float Gouraud;

void main()
{
    FragPos = vec3(uM * vec4(inPos, 1.0));
	Normal = mat3(transpose(inverse(uM))) * inNormal; 
	gl_Position = uP * uV * uM * vec4(inPos, 1.0); 
}