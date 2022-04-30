#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/camera.h>
#include <learnopengl/filesystem.h>
#include <learnopengl/model.h>
#include <learnopengl/shader_m.h>


#include <iostream>
#include <vector>
#include <string>




//screen size
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;


//camera initialization
glm::vec3 cameraPos(0.0f, 2.0f, 5.0f);
Camera camera(cameraPos);

// set the mouse position to be the center of the screen
float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;
bool isCameraFixed = false;
// mouse move
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (!isCameraFixed) {
		if (firstMouse) {
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // coordinate flip to correspond to the coordinate system

		lastX = xpos;
		lastY = ypos;

		camera.ProcessMouseMovement(xoffset, yoffset);
	}
}
//--------------------------------------------------------------------------



//-------------------------------------------OPENGL/WINDOW INITIALIZATIONS----------------------------------


bool init()
{
	// Load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		system("pause");
		return false;
	}

	// Configure the global openGL state
	glEnable(GL_DEPTH_TEST);

	return true;
}

GLFWwindow* windowInit()
{
	//configure opengl
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//create window
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game", NULL, NULL);

	//error checking
	if (window == NULL)
	{
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		return NULL;
	}
	//make window current contex
	glfwMakeContextCurrent(window);
	//TODO add callback functions
	glfwSetCursorPosCallback(window, mouse_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	return window;
}

//---------------------------------------------------------------------------------------------------------

//------------------------------------CAMERA/MOUSE STUFF----------------------------------------


//---------------------------------------SKYBOX FUNCTIONS----------------------------------
//function to load the sky box cube map
unsigned int loadCubemap(std::vector<std::string> faces)
{
	//initialize texture id and bind it
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			//stbi_image_free(data);
		}
		else {
			std::cout << "Could not load texture at path: " << faces[i] << std::endl;
			//stbi_image_free(data);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;

}

//skybox stuff
unsigned int cubemapTexture;
unsigned int skyboxVAO, skyboxVBO;
const float skyboxVertices[] = {
	// positions
	-1.0f, 1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,

	-1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,

	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,

	-1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, 1.0f
};
const std::vector<std::string> faces{
		 FileSystem::getPath("Assets/skybox/right.tga"),
		 FileSystem::getPath("Assets/skybox/left.tga"),
		 FileSystem::getPath("Assets/skybox/top.tga"),
		 FileSystem::getPath("Assets/skybox/bottom.tga"),
		 FileSystem::getPath("Assets/skybox/front.tga"),
		 FileSystem::getPath("Assets/skybox/back.tga")
};

//function to initialize sky box
void skyboxInit() {

	//skybox VAO
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	//load textures
	cubemapTexture = loadCubemap(faces);
}

//function to render the skybox
void renderSkybox(Shader& shader)
{
	//view matrix constructed to remove the movement of the camera
	glm::mat4 viewMatrix = glm::mat4(glm::mat3(camera.GetViewMatrix()));
	// projection
	glm::mat4 projMatrix = camera.GetProjMatrix((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);

	shader.setMat4("view", viewMatrix);
	shader.setMat4("projection", projMatrix);

	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

}






//-----------------------MAIN FUNCTION--------------------------------------------------
int main()
{
	//initializations

	//window init
	GLFWwindow* window = windowInit();
	bool isInit = init();
	if (window == NULL || !isInit) {
		return -1;
	}

	//configure skybox
	skyboxInit();
	//initialize shader for skybox
	Shader skyboxShader("Shaders/skybox.vs", "Shaders/skybox.fs");
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	//main loop
	while (!glfwWindowShouldClose(window))
	{
		//render background
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//render the skybox
		glDepthFunc(GL_LEQUAL);
		skyboxShader.use();
		renderSkybox(skyboxShader);

		// Swap buffers and investigate IO events (key pressed, mouse movement, etc.)
		glfwSwapBuffers(window);


		// poll events
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}