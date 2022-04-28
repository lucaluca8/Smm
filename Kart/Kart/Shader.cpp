#include "Shader.h"



Shader::Shader(const char* vertexShder, const char* fragmentShader)
{
	//retrive vertex/fragment source code from file
	// 
	//initializations
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vertexFile;
	std::ifstream fragmentFile;
	try {
		//open files
		vertexFile.open(vertexShder);
		fragmentFile.open(fragmentShader);

		//read from file buffer into streams
		std::stringstream vertexShaderStream, fragmentShaderStream;
		vertexShaderStream << vertexFile.rdbuf();
		fragmentShaderStream << fragmentFile.rdbuf();

		//close file handlers
		vertexFile.close();
		fragmentFile.close();

		//convert stream into string
		vertexCode = vertexShaderStream.str();
		fragmentCode = fragmentShaderStream.str();
	}
	catch (std::ifstream::failure e) {
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	//convert string to char pointers
	const char* vertexPointer = vertexCode.c_str();
	const char* fragmentPointer = fragmentCode.c_str();

	//2. compile shader
	unsigned int vertexID, fragmentID;
	//vertex
	vertexID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexID, 1, &vertexPointer, NULL);
	glCompileShader(vertexID);

	//fragment
	fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentID, 1, &fragmentPointer, NULL);
	glCompileShader(fragmentID);

	//shader program
	ID = glCreateProgram();
	glAttachShader(ID, vertexID);
	glAttachShader(ID, fragmentID);
	glLinkProgram(ID);

	//delete shaders because they are linked to shader program so we no longer need them
	glDeleteShader(vertexID);
	glDeleteShader(fragmentID);


}

//activate shader
void Shader::use()
{
	glUseProgram(ID);
}
//set uniform int value
void Shader::setInt(const std::string& name, int value)
{
	glUniform1i(glGetUniformLocation(ID,name.c_str()),value);
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat)
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
