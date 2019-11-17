#include <stdio.h>
#include <stdlib.h>

#include "VerticesStruct.h";
#include "TextureManager.h";
#include "Camera.h";
#include "OneModel.h";
#include "Material.h";
#include "Terrain.h";
#include "Water.h";
#include "Skybox.h";

#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT  1024
#define X_AXIS 1
#define Y_AXIS 2
#define Z_AXIS 3

//Main window
GLFWwindow* window;
//Main camera
Camera camera;

// Time between current frame and last frame
float deltaTime = 0.0f;	
float time_passed = 0.f;
//Buffer size for the window
int frame_buffer_width = 0;
int frame_buffer_height = 0;

//To check where was the mouse last frame, to get its direction
float mouseLastX = WINDOW_WIDTH / 2, mouseLastY = WINDOW_HEIGHT / 2;
bool firstMouse = true;
glm::vec2 last_mouse_position;

//input
bool _n_pressed = false;

//Callback when resizing the window
void frameBufferResizeCallback(GLFWwindow* window, int fb_width, int fb_height)
{
	glViewport(0, 0, fb_width, fb_height);
}

GLuint initVaoVboEbo()
{
	//Generate VAO (Vertex attribut object) and bind
	GLuint VAO;
	glCreateVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//Generate VBO (Vertex buffer object), bind and send data
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

	//Generate EBO (Element buffer object), bind and send data
	GLuint EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//Set vertex attributs pointers and enable (input assembly)
	//Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(GLvoid*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);
	//texCoord
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(GLvoid*)offsetof(Vertex, texcoord));
	glEnableVertexAttribArray(1);
	//Normal 
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(GLvoid*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);

	//unbind
	glBindVertexArray(0);

	return VAO;
}

void inputMoveCamera(GLFWwindow* window, Camera& camera, float speed)
{
	speed *= deltaTime;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.CameraMovement(FORWARD, speed);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.CameraMovement(RIGHT, speed);

	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.CameraMovement(LEFT, speed);

	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.CameraMovement(BACKWARD, speed);
	}

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		camera.CameraMovement(UP, speed);
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		camera.CameraMovement(DOWN, speed);
	}
}

//Callback when moving the mouse
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse) // this bool variable is initially set to true
	{
		mouseLastX = xpos;
		mouseLastY = ypos;
		firstMouse = false;
	}

	float x_offset = xpos - mouseLastX;
	float y_offset = mouseLastY - ypos; // reversed since y-coordinates range from bottom to top
	mouseLastX = xpos;
	mouseLastY = ypos;

	float sensitivity = 0.05f;
	x_offset *= sensitivity;
	y_offset *= sensitivity;

	camera.UpdateMouseRotation(x_offset, y_offset);

	//Updating mouse position
	last_mouse_position = glm::vec2(xpos, ypos);
}

//Callback when scrolling
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.UpdateFov(yoffset);
}

//Create a new window
// return : new glfw window ptr 
GLFWwindow* createWindow()
{
	GLFWwindow* window;

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); Mac OS

	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "tuto", NULL, NULL);

	if (window == NULL)
	{
		cout << "ERROR createWindow : failed to create a window" << endl;
		glfwTerminate();
		return NULL;
	}

	glfwGetFramebufferSize(window, &frame_buffer_width, &frame_buffer_height);
	glViewport(0, 0, frame_buffer_width, frame_buffer_height);
	glfwSetFramebufferSizeCallback(window, frameBufferResizeCallback);


	//important for GLEW
	glfwMakeContextCurrent(window);

	//Init mouse input, hide mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	return window;
}

//Init gl context
void init()
{
	//init glfw
	glfwInit();
	
	window = createWindow();

	//Glew
	//So we can use new function of glew
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		std::cout << "ERROR::MAIN.CPP::GlewInit() failed" << "\n";
		glfwTerminate();
	}

	//OpenGl options
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glEnable(GL_STENCIL_TEST);
	glStencilMask(0x06);

	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


}

void bindAll()
{

}

void input(Terrain &terrain, Water &water, Shader* water_shader)
{
	//Get input
	glfwPollEvents();

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

	//Input terrain generation
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
	{
		if (!_n_pressed)
		{
			terrain.generateRandomTerrain();
			water.generateWater(water_shader, terrain._perlin_noise);
		}
		_n_pressed = true;
	}
	else
	{
		_n_pressed = false;
	}
}
void mainUpdate()
{
	//----------DRAW
	//Clear	
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	glfwGetFramebufferSize(window, &frame_buffer_width, &frame_buffer_height);

	glfwSwapBuffers(window);
	glFlush();
}

void unbindAll()
{
	glBindVertexArray(0);
	glUseProgram(0);
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

int main()
{
	init();
	ShaderManager shader_manager;
	Shader* ground_shader = shader_manager.CreateShader("vertex_core.glsl", "fragment_terrain_ground.glsl");
	Shader* depth_shader = shader_manager.CreateShader("depth_shader.glsl", "fragment_empty.glsl");
	Shader* main_water_shader = shader_manager.CreateShader("vertex_water.glsl", "fragment_water.glsl");
	Shader* grass_shader = shader_manager.CreateShader("vertex_core.glsl", "fragment_grass.glsl");
	Shader* skybox_shader = shader_manager.CreateShader("vertex_skybox.glsl", "fragment_skybox.glsl");
	//Shader skybox_shader("vertex_skybox.glsl", "fragment_skybox.glsl");
	//Skybox Uniform
	skybox_shader->use();
	glUniform1i(glGetUniformLocation(skybox_shader->getProgram(), "skybox"), 0);

	//Terrain uniform
	ground_shader->use();
	glUniform1i(glGetUniformLocation(ground_shader->getProgram(), "herb_text"), 0);
	glUniform1i(glGetUniformLocation(ground_shader->getProgram(), "rock01_text"), 1);
	glUniform1i(glGetUniformLocation(ground_shader->getProgram(), "rock02_text"), 2);
	glUniform1i(glGetUniformLocation(ground_shader->getProgram(), "shadowMap"), 3);
	glUniform1i(glGetUniformLocation(ground_shader->getProgram(), "cubemap"), 4);

	//Water uniform
	main_water_shader->use();
	glUniform1i(glGetUniformLocation(main_water_shader->getProgram(), "main_water_texture"), 0);
	glUniform1i(glGetUniformLocation(main_water_shader->getProgram(), "foam_texture"), 1);
	glUniform1i(glGetUniformLocation(main_water_shader->getProgram(), "perlin_noise"), 2);
	glUniform1i(glGetUniformLocation(main_water_shader->getProgram(), "terrain_perlin_noise"), 3);
	glUniform1i(glGetUniformLocation(main_water_shader->getProgram(), "skybox"), 4);

	GLuint VAO = initVaoVboEbo();
	GLuint texture0, texture1, texture2, water_texture, foam_texture;
	const char* img_path0 = "Images/herb01.jpg";
	const char* img_path1 = "Images/rock02.jpg";
	const char* img_path2 = "Images/rock03.jpeg";
	const char* water_img_path = "Images/water01.png";
	const char* foam_img_path = "Images/foam01.jpg";

	shared_ptr<Material> mat_terrain(new Material());
	shared_ptr<Material> mat_grass(new Material());

	if (TextureManager::loadTexture(texture0, img_path0) < 0)
	{
		cout << "ERROR : texture0 loading failed" << endl;
		return -1;
	}

	if (TextureManager::loadTexture(texture1, img_path1) < 0)
	{
		cout << "ERROR : texture1 loading failed" << endl;
		return -1;
	}

	if (TextureManager::loadTexture(texture2, img_path2) < 0)
	{
		cout << "ERROR : texture1 loading failed" << endl;
		return -1;
	}

	if (TextureManager::loadTexture(water_texture, water_img_path) < 0)
	{
		cout << "ERROR : water_texture loading failed" << endl;
		return -1;
	}

	if (TextureManager::loadTexture(foam_texture, foam_img_path) < 0)
	{
		cout << "ERROR : foam_texture loading failed" << endl;
		return -1;
	}

	int dimension = 512;

	//Init terrain
	mat_terrain->shininess = 1;
	Terrain terrain(dimension, 1.f, mat_terrain);
	terrain._rock_text2 = texture2;
	terrain._rock_text = texture1;
	terrain._herb_text = texture0;
	terrain._height_multiplier = 30.f;
	terrain._display_grass = true;
	terrain._mesh_precision = 1.f;
	glm::vec3 pos = camera.GetCameraPosition();
	GLuint ground_program = ground_shader->getProgram();
	camera.UpdateCamera(shader_manager, frame_buffer_width, frame_buffer_height);
	
	//Init grass
	const char* grass_path = "3DModels/grass05/grass01.obj";
	terrain._grass_multiplier = 4;
	terrain.setGrass(grass_shader, grass_path, mat_grass);

	//Generate terrain
	terrain.generateRandomTerrain();

	//Init water
	shared_ptr<Material> water_material(new Material());
	water_material->shininess = 2.f;
	Water water(dimension, water_material, main_water_shader, 5.f, terrain._perlin_noise);
	water._main_water_text = water_texture;
	water._foam_text = foam_texture;
	water._waves_amplitude = 0.1f;
	water._waves_speed = 2.f;
	water._mesh_precision = 3.f;
	
	//Init time variable
	float last_frame_time = 0.f;
	float time_passed = 0;

	camera.SetCameraPosition(glm::vec3(0.f, .0f, .0f));
	camera.SetSpeed(20.f);

	//Light
	glm::vec3 diffuse_light_position(50.f, 2000.f, 50.f);
	const char* model_path = "3DModels/block01/block01.obj";
	OneModel cube_model(model_path, mat_terrain	);
	cube_model.translateModel(diffuse_light_position);

	ground_shader->setVec3("DiffuseLightPosition", diffuse_light_position);

	//Shadow map
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);

	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	float near_plane_sm = 1.0f, far_plane_sm = 7.5f;
	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane_sm, far_plane_sm);
	glm::mat4 lightView = glm::lookAt(diffuse_light_position,
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightSpaceMatrix = lightProjection * lightView;
	ground_shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

	terrain._shadow_map = depthMap;

	//skybox
	Skybox skybox;
	water._skybox = skybox.cubemapID;

	//MAIN LOOP
	int fps_counter = 0;
	float fps_timer = 1.f;
	while (!glfwWindowShouldClose(window))
	{
		float current_frame_time = glfwGetTime();
		deltaTime = current_frame_time - last_frame_time;

		bindAll();
		input(terrain, water, main_water_shader);
		//----------DRAW
		//Clear	
		glClearColor(0.2f, 0.2f, 0.8f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		//Render shadown map
		depth_shader->setMat4("ModelMatrix", terrain._model_matrix);
		depth_shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		terrain.Draw(depth_shader);
		cube_model.Draw(depth_shader);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, frame_buffer_width, frame_buffer_height);
		
		//Camera input
		inputMoveCamera(window, camera, camera.GetSpeed());
		glfwGetFramebufferSize(window, &frame_buffer_width, &frame_buffer_height);
		ground_shader->setVec3("CameraPos", camera.GetCameraPosition());
		main_water_shader->setVec3("CameraPos", camera.GetCameraPosition());

		camera.UpdateCamera(shader_manager, frame_buffer_width, frame_buffer_height);
		
		//Time update 
		time_passed += deltaTime;
		last_frame_time = current_frame_time;

		//Skybox update
		skybox.Draw(skybox_shader, camera);

		//terrain update
		ground_shader->setFloat("TimePassed", time_passed);
		terrain.updateInput(window);
		terrain.Draw(ground_shader);

		//Cube
		cube_model.Draw(ground_shader);
		
		//Water update
		main_water_shader->setFloat("TimePassed", time_passed);
		main_water_shader->setFloat("DeltaTime", deltaTime);
		water.updateInput(window);
		water.Draw(main_water_shader);

		//Display fps
		fps_counter++;
		fps_timer += deltaTime;
		if (fps_timer >= 1.f)
		{
			cout << fps_counter << endl;
			fps_timer = 0.f;
			fps_counter = 0;
		}

		glfwSwapBuffers(window);
		glFlush();
		unbindAll();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	glDeleteProgram(depth_shader->getProgram());
	glDeleteProgram(ground_shader->getProgram());

	return 0;
}