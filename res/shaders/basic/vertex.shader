#version 330 core
layout(location = 0) in vec4 position;
layout(location = 1) in vec3 colorInput;
layout(location = 2) in vec2 aTexCoord;


out vec3 colorToUse;
out vec2 TexCoord;

uniform mat4 transform;


void main()
{
	gl_Position = transform * position;
	colorToUse = colorInput;
	TexCoord = aTexCoord;
};
