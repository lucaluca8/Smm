#include "VAO.h"

VAO::VAO()
{
	glGenVertexArrays(1, &id); //generate this before the VBO
}

void VAO::LinkAtrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset)
{
	VBO.Bind();

	//configure the vertex attribute so OpenGL knows how to read the VBO
	// (position of the vertex atribute, how many values we got for vertex, type of values, something for intgers, the ammount of data between each vertex, offset)
	glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
	//enable vertex attribute so OpenGL knows to use it
	glEnableVertexAttribArray(layout);

	VBO.Unbind();
}

void VAO::Bind()
{
	//making the VAO the current Vertex Array Object by binding it
	glBindVertexArray(id);
}

void VAO::Unbind()
{
	glBindVertexArray(0);
}

void VAO::Delete()
{
	//deleting the object we have created
	glDeleteVertexArrays(1, &id);
}
