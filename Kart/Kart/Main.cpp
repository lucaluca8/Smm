#include <iostream>


#define GLFW_INCLUDE_NONE
#include <glfw3.h>
#include <glad/glad.h>



int main()
{
	//initializations
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//making the coordonates for a triangle (x,y,z)
	GLfloat vertices[] = {
		-0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,
		0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,
		0.0f, 0.5f * float(sqrt(3))*2 / 3, 0.0f,
		-0.5f / 2, 0.5f * float(sqrt(3)) / 6, 0.0f,
		0.5f/2, 0.5f * float(sqrt(3)) / 6, 0.0f,
		0.0f, -0.5f * float(sqrt(3)) / 3, 0.0f
	};
	//tells the OpenGL in what order to render the triangle
	GLuint indices[] = {
		0,3,5,
		3,2,4,
		5,4,1
	};
	//creating a window
	GLFWwindow* window = glfwCreateWindow(800, 600, "Window", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}

	//starting the window
	glfwMakeContextCurrent(window);

	gladLoadGL(); // initialize openGL
 	glViewport(0, 0, 800, 600); //configure where the openGL will work

	


	//creating references containers to Vertex Array Object and Vertex Buffer Object
	GLuint VAO,VBO,EBO;

	glGenVertexArrays(1, &VAO); //generate this befor the VBO
	glGenBuffers(1, &VBO); //generating buffer; the 1 is because we only have one object
	glGenBuffers(1, &EBO);

	//making the VAO the current Vertex Array Object by binding it
	glBindVertexArray(VAO);

	//initializing the buffer; glarraybuffer because that we use for our vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	

	//itroduceing the vertices into the vbo
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//configure the vertex attribute so OpenGL knows how to read the VBO
	// (position of the vertex atribute, how many values we got for vertex, type of values, something for intgers, the ammount of data between each vertex, offset)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); 
	//enable vertex attribute so OpenGL knows to use it
	glEnableVertexAttribArray(0);

	//Bind both VBO and VAO to 0 so that we dont accidentally modify the VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	//bind the array of indices after binding the VAO so you tell the VAO to use the indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); 

	glClearColor(0.07f, 0.13f, 0.17f, 1.0f); //preparing the OPENGL to clear the buffer and give it another color
	glClear(GL_COLOR_BUFFER_BIT); //use the command before on the color buffer
	glfwSwapBuffers(window); //swap the buffers

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		//tell OpenGL which shader program to use
		glUseProgram(shaderProgram);
		//bind the VAO so OpenGL knows to use it
		glBindVertexArray(VAO);
		//draw the triangle using the GL_TRIANGLES PRIMITIVE
		glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//deleting the object we have created
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(shaderProgram);

	glfwTerminate();
	return 0;
}