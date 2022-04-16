#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include <glad/glad.h>
#include <stb_image.h>

#include "ShaderClass.h"

class Texture
{
public:
	GLuint id;
	GLenum type;

	Texture(const char* image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType);

	//assign a unit to a texture
	void texUnit(ShaderClass& shader, const char* unifor, GLuint unit);

	//bind a texture
	void Bind();

	//unbind a texture
	void Unbind();

	//delete a texture
	void Delete();
};
#endif


