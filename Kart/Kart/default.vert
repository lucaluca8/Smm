#version 330 core

//position/coordonates
layout (location = 0) in vec3 aPos;

//colors
layout (location = 1) in vec3 aColor;

//texture
layout (location = 2) in vec2 aTex;

//outputs the colors for the fragment shader
out vec3 color;

//outputs the texture coordinates to the fragment shader
out vec2 texCoord;

uniform mat4 camMatrix;

void main()
{
   gl_Position = camMatrix * vec4(aPos,1.0);
   color = aColor;
   texCoord=aTex;
};