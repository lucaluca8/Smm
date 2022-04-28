#pragma once
#include <glad/glad.h>
#include <glm.hpp>

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
class Shader
{
public:
	unsigned int ID;

	//shader constructor
	Shader(const char* vertexShder, const char* fragmentShader);

	//activate shader
	void use();
	//set uniform int value
	void setInt(const std::string& name, int value);
	//mat4 setter
	void setMat4(const std::string& name, const glm::mat4& mat);
};

