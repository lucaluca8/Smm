#ifndef VAO_CLASS_H
#define VAO_CLASS_H

#include <glad/glad.h>
#include "VBO.h"

class VAO
{
public:
	//id reference for the Vertex Array Object
	GLuint id;
	//constructor that generates a Vertex Array Object id
	VAO();

	//link the VBO to the VAO
	void LinkAtrib(VBO& VBO, GLuint layout,GLuint numComponents,GLenum type,GLsizeiptr stride,void* offset);
	void Bind();
	void Unbind();
	void Delete();
};

#endif
