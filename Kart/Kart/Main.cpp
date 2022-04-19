#include <iostream>

#define GLFW_INCLUDE_NONE
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <stb_image.h>

#include "ShaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Texture.h"
#include "Camera.h"

//making the coordonates for a triangle (x,y,z)
GLfloat vertices[] = {  
	//COORDONATES                 COLORS             TEXTURE COORD
   -0.9f, 0.5f, 0.5f,           1.0f, 0.0f, 0.0f,    0.0f, 0.0f,    
   -0.1f, 0.0f, 0.5f,		    0.0f, 1.0f, 0.0f,    0.0f, 5.0f,
	0.3f, 0.0f,-0.5f,			0.0f, 0.0f, 1.0f,    5.0f, 5.0f,
    0.2f,-0.5f,-0.5f,			1.0f, 1.0f, 0.0f,    5.0f, 0.0f,
	0.0f, 0.7f,-0.0f,			1.0f, 1.0f, 0.0f,    5.0f, 5.0f
};

//tells the OpenGL in what order to render the vertices
GLuint indices[] = {
	0,1,2,
	0,2,3,
	0,1,4,
	1,2,4,
	2,3,4,
	3,0,4
};

const int width = 800;
const int height = 800;


int main()
{
	//initializations
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	
	//creating a window
	GLFWwindow* window = glfwCreateWindow(width, height, "Window", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}

	//starting the window
	glfwMakeContextCurrent(window);

	gladLoadGL(); // initialize openGL
 	glViewport(0, 0, width, height); //configure where the openGL will work

	// Generates Shader object using shaders defualt.vert and default.frag
	ShaderClass shaderProgram("default.vert", "default.frag");

	//generate the Vertex Array Object and bind it
	VAO VAO1;
	VAO1.Bind();

	//generate the Vertex Buffer Object and link it to vertices
	VBO VBO1(vertices, sizeof(vertices));
	//generate Element Buffer Object and link it to indices
	EBO EBO1(indices, sizeof(indices));

	//link VBO attributes (coordonates,colors) to VAO
	VAO1.LinkAtrib(VBO1, 0, 3, GL_FLOAT,  8 * sizeof(float), (void*)0);
	VAO1.LinkAtrib(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	VAO1.LinkAtrib(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	//unbind all to prvenet accidentally modifying them
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();


	//Texture
	Texture texture("texture1.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	texture.texUnit(shaderProgram, "tex0", 0);
	

	//depth buffer
	glEnable(GL_DEPTH_TEST);


	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

	//main while window
	while (!glfwWindowShouldClose(window))
	{
		//specify the background color
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		//clear the back buffer and assign a new color to it
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//tell OpenGL which shader program to use
		shaderProgram.Activate();

		camera.Inputs(window);
		camera.Matrix(45.0f, 0.1f, 100.0f, shaderProgram, "camMatrix");
		
		texture.Bind();
		//bind the VAO so OpenGL knows to use it
		VAO1.Bind();
		//draw the triangle using the GL_TRIANGLES PRIMITIVE
		glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(int), GL_UNSIGNED_INT, 0);
		//swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		//take care of all GLFW events
		glfwPollEvents();
	}

	//deletes all the objects we've created
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	texture.Delete();
	shaderProgram.Delete();

	//delete the window before ending the program
	glfwDestroyWindow(window);

	//terminate glfw before ending the program
	glfwTerminate();

	return 0;
}