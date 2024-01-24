#version 330 core

in vec4 channelCol;
out vec4 outCol;

uniform vec4 uColor;

void main()
{
	outCol = vec4(uColor);
}