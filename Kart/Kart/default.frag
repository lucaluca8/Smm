#version 330 core

//outputs colors in RGBA
out vec4 FragColor;

// inputs color from the vertex shader
in vec3 color;

in vec2 texCoord;

uniform sampler2D tex0;

void main()
{
   FragColor = texture(tex0,texCoord);
};