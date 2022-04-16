#version 330 core

//position/coordonates
layout (location = 0) in vec3 aPos;

//colors
layout (location = 1) in vec3 aColor;

//texture
layout (location = 2) in vec2 aTex;

//outputs the colors for the fragment shader
out vec3 color;

out vec2 texCoord;

uniform float scale;

void main()
{
   gl_Position = vec4(aPos.x + aPos.x*scale, aPos.y + aPos.y*scale, aPos.z+aPos.z*scale, 1.0);
   color = aColor;
   texCoord=aTex;
};