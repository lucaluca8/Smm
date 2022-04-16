#include "Texture.h"

Texture::Texture(const char* image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType)
{
	//assign the type of texture to the texture object
	type = texType;

	//flip the image so it appears the right side up
	stbi_set_flip_vertically_on_load(true);

	//store the details of the image
	int widthImg, heightImg, numColChannels;

	//read the image
	unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColChannels, 0);

	//generate an OpenGL texture object
	glGenTextures(1, &id);
	//assigns the texture to a texture unit 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, id);

	//configure the algorithm we use to make the image bigger or smaller
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//confirgure if the texture repeats
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//error checking to see if the texture loaded
	if(!bytes)
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	else
	{
		//assign the texture to OpenGL object
		glTexImage2D(texType, 0, format, widthImg, heightImg, 0, format, pixelType, bytes);
		//generate MipMaps (smaller versions of the image)
		glGenerateMipmap(GL_TEXTURE_2D);
		//delete the image because it is already an OpenGL texture object
		stbi_image_free(bytes);
		//unbind the OpenGL texture object so that it cant be modified
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	
}

void Texture::texUnit(ShaderClass& shader, const char* unifor, GLuint unit)
{
	//get the location of the uniform
	GLuint tex0Uni = glGetUniformLocation(shader.id, "tex0");
	//shader needs to be activated before changing the value of the uniform
	shader.Activate();
	//set the value of the uniform
	glUniform1i(tex0Uni, 0);
}

void Texture::Bind()
{
	glBindTexture(type, id);
}

void Texture::Unbind()
{
	glBindTexture(type, 0);
}

void Texture::Delete()
{
	glDeleteTextures(1, &id);
}
