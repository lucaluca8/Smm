#ifndef EBO_CLASS_H
#define EBO_CLASS_H

#include <glad/glad.h>

class EBO
{
public:
	//id reference the Element Buffer Object
	GLuint id;

	//constructor that generate a Element Buffer Object and links it to indices
	EBO(GLuint* indices, GLsizeiptr size);

	void Bind();
	void Unbind();
	void Delete();

};
#endif

