#include "EBO.h"

EBO::EBO(GLuint* indices, GLsizeiptr size)
{
	glGenBuffers(1, &id); //generate this before the VBO

	//initializing the buffer; glarraybuffer because that we use for our vertices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);

	//itroduceing the vertices into the vbo
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,size, indices, GL_STATIC_DRAW);
}

void EBO::Bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,id);
}

void EBO::Unbind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
}

void EBO::Delete()
{
	glDeleteBuffers(1, &id);
}
