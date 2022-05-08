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
#include "Kart.h"




//screen size
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

bool isDay = false;

//camera initialization
glm::vec3 cameraPos(60.0f, 20.0f, -140.0f);
Camera camera(cameraPos);

// set the mouse position to be the center of the screen
float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;
bool isCameraFixed = false;

// kart (constructor with the position of the kart
Kart kart(glm::vec3(60.0f, 2.5f, -300.0f));

// world Y axis
glm::vec3 WORLD_UP(0.0f, 1.0f, 0.0f);

// depth map id
unsigned int depthMap;
unsigned int depthMapFBO;

//first person camera
bool fpCamera = false;

// mouse move
// mouse callback
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

//handle input function
void keyInput(GLFWwindow* window)
{
	// esc exit
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (fpCamera == false)
	{// Camera WSAD front, back, left and right Space up, left and Ctrl down
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.ProcessKeyboard(FORWARD, 0.05f);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.ProcessKeyboard(BACKWARD, 0.05f);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.ProcessKeyboard(LEFT, 0.05f);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.ProcessKeyboard(RIGHT, 0.05f);
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			camera.ProcessKeyboard(UP, 0.05f);
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
			camera.ProcessKeyboard(DOWN, 0.05f);

		//kart moving
		// car moving
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
			kart.ProcessKeyboard(K_FORWARD, 0.04f);
		else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			kart.ProcessKeyboard(K_LEFT, 0.04f);
		else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			kart.ProcessKeyboard(K_RIGHT, 0.04f);
		else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
			kart.ProcessKeyboard(K_BACKWARD, 0.04f);
	}
	else if (fpCamera == true)
	{// first person camera
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		{
			kart.ProcessKeyboard(K_FORWARD, 0.04f);
			camera.ProcessKeyboard(FORWARD, 0.04f);
		}
		else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		{
			kart.ProcessKeyboard(K_LEFT, 0.04f);
			camera.ProcessKeyboard(LEFT, 0.04f);
		}
		else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		{
			kart.ProcessKeyboard(K_RIGHT, 0.04f);
			camera.ProcessKeyboard(RIGHT, 0.04f);
		}
		else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		{
			kart.ProcessKeyboard(K_BACKWARD, 0.04f);
			camera.ProcessKeyboard(BACKWARD, 0.04f);
		}
	}
}



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

// light variables
glm::vec3 lightPos(-1.0f, 1.0f, -1.0f);
glm::vec3 lightDirection = glm::normalize(lightPos);
glm::mat4 lightSpaceMatrix;
// Set lighting related properties
void renderLight(Shader& shader)
{
	shader.setVec3("viewPos", camera.Position);
	shader.setVec3("lightDirection", lightDirection);
	shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

	glActiveTexture(GL_TEXTURE15);
	glBindTexture(GL_TEXTURE_2D, depthMap);
}


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
		 FileSystem::getPath("Assets/skybox/day/right.jpg"),
		 FileSystem::getPath("Assets/skybox/day/left.jpg"),
		 FileSystem::getPath("Assets/skybox/day/top.jpg"),
		 FileSystem::getPath("Assets/skybox/day/bottom.jpg"),
		 FileSystem::getPath("Assets/skybox/day/front.jpg"),
		 FileSystem::getPath("Assets/skybox/day/back.jpg")
};
const std::vector<std::string> faces2{
		 FileSystem::getPath("Assets/skybox/night/right.jpg"),
		 FileSystem::getPath("Assets/skybox/night/left.jpg"),
		 FileSystem::getPath("Assets/skybox/night/top.jpg"),
		 FileSystem::getPath("Assets/skybox/night/bottom.jpg"),
		 FileSystem::getPath("Assets/skybox/night/front.jpg"),
		 FileSystem::getPath("Assets/skybox/night/back.jpg")
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

void skyboxReload() {

	//skybox VAO
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	//load textures
	cubemapTexture = loadCubemap(faces2);
}


//----------------------RENDER FUNCTIONS-----------------------------------
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

void renderRaceTrack(Model& model, Shader& shader)
{
	// view transition
	glm::mat4 viewMatrix = camera.GetViewMatrix();
	shader.setMat4("view", viewMatrix);
	// model conversion
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	shader.setMat4("model", modelMatrix);
	// projection transformation
	glm::mat4 projMatrix = camera.GetProjMatrix((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);
	shader.setMat4("projection", projMatrix);

	model.Draw(shader);
}

void renderTrafficLight(Model& model, Shader& shader)
{
	// view transition
	glm::mat4 viewMatrix = camera.GetViewMatrix();
	shader.setMat4("view", viewMatrix);
	// model conversion
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-10.0f, 0.0f, -170.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(-270.0f), WORLD_UP);
	shader.setMat4("model", modelMatrix);
	// projection transformation
	glm::mat4 projMatrix = camera.GetProjMatrix((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);
	shader.setMat4("projection", projMatrix);

	model.Draw(shader);
}
// tree function
void tree(Model& model, Shader& shader, glm::vec3 positions)
{
	// view transition
	glm::mat4 viewMatrix = camera.GetViewMatrix();
	shader.setMat4("view", viewMatrix);
	// model conversion
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, positions);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(-270.0f), WORLD_UP);
	shader.setMat4("model", modelMatrix);
	// projection transformation
	glm::mat4 projMatrix = camera.GetProjMatrix((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);
	shader.setMat4("projection", projMatrix);

	model.Draw(shader);
}

void renderKart(Model& model, glm::mat4 modelMatrix, Shader& shader)
{
	modelMatrix = glm::rotate(modelMatrix, glm::radians(kart.getYaw()), WORLD_UP);
	// Offset the original rotation of the model
	modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f), WORLD_UP);
	// resize the model
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.004f, 0.004f, 0.004f));

	// apply transformation matrix
	shader.setMat4("model", modelMatrix);

	model.Draw(shader);
}

void configKart(Model& kartModel, Shader& shader)
{
	
	// view transition
	glm::mat4 viewMatrix = camera.GetViewMatrix();
	shader.setMat4("view", viewMatrix);
	// projection transformation
	glm::mat4 projMatrix = camera.GetProjMatrix((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);
	shader.setMat4("projection", projMatrix);

	// -------
	// Hierarchical modeling

	// model conversion
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, kart.getMidValPosition());
	modelMatrix = glm::rotate(modelMatrix, glm::radians(kart.getYaw() / 2), WORLD_UP);

	// render the car
	renderKart(kartModel, modelMatrix, shader);

	// Since mat4 is passed by value as a function parameter, there is no need to back up modelMatrix

}
//-----------------------------------------------------------------------------




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

	

	//racetrack stuff
	Shader raceTrackShader("Shaders/racetrack.vs", "Shaders/racetrack.fs");
	Shader raceTrackShaderN("", "");
	Model raceTrackModel(FileSystem::getPath("Assets/race-track/race-track.obj"));
	Model trafficLightModel(FileSystem::getPath("Assets/traffic light/traffic light.obj"));

	//kart stuff
	Model kartModel(FileSystem::getPath("Assets/kart2/kart.obj"));

	//tree
	Model tree1(FileSystem::getPath("Assets/tree/Lowpoly_tree_sample.obj"));

	
	bool isDay=true;

	//main loop
	while (!glfwWindowShouldClose(window))
	{
		//wait for input
		keyInput(window);
		if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		{
			skyboxInit();
			isDay = true;
		}
		if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		{
			skyboxReload();
			isDay = false;
		}
		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		{
			fpCamera = true;
			
		}
		if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		{
			fpCamera = false;
		}
		if (fpCamera == true)
		{
			camera.setPosition(kart.getPosition());
		}
		//render background
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		//render the race track
		raceTrackShader.use(); //activate the racetrack shader
		// Set lighting related properties

		renderRaceTrack(raceTrackModel, raceTrackShader);//create the racetrack

		
		//trees
		tree(tree1, raceTrackShader, glm::vec3(-80.0f, 0.0f, -60.0f));
		tree(tree1, raceTrackShader, glm::vec3(0.0f, 0.0f, 30.0f));
		tree(tree1, raceTrackShader, glm::vec3(0.0f, 0.0f, 30.0f));
		tree(tree1, raceTrackShader, glm::vec3(100.0f, 0.0f, 30.0f));
		tree(tree1, raceTrackShader, glm::vec3(120.0f, 0.0f, -117.0f));


		renderLight(raceTrackShader);

		configKart(kartModel, raceTrackShader);

		renderTrafficLight(trafficLightModel, raceTrackShader);

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