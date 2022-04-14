#include "ShaderClass.h"

ShaderClass::ShaderClass(const char* vertexFile, const char* fragmentFile)
{
	std::string vertexCode = get_file_contents(vertexFile);
	std::string fragmentCode = get_file_contents(fragmentFile);

	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

	//creating vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER); //creating the vertex shader; in the parantheses specify what type of shader you create
	glShaderSource(vertexShader, 1, &vertexSource, NULL); //giving the reference to the const vertexShader variable
	glCompileShader(vertexShader); //compiling the vertex shader

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); // creating the fragment shader
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader); //compilig the fragment shader

	//creating the shader program 
	id = glCreateProgram();
	glAttachShader(id, vertexShader);
	glAttachShader(id, fragmentShader);

	//linking all shaders in the shader program
	glLinkProgram(id);

	//deleting the vertex shaders because they are useless now
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void ShaderClass::Activate()
{
	glUseProgram(id);
}

void ShaderClass::Delete()
{
	glDeleteProgram(id);
}

std::string get_file_contents(const char* filename)
{
	std::ifstream in(filename, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}
