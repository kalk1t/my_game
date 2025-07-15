#include <stdio.h>
#include <math.h>

#include "define.h"
#define GLFW_INCLUDE_NONE //this prevents GLFW from including <GL/gl.h> ,glad includes that
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include "shader.h"

#ifdef _WIN32
	#define GLFW_EXPOSE_NATIVE_WIN32
	#pragma comment(lib,"opengl32.lib")
	#pragma comment(lib,"glfw3.lib")
#else
	#define GLFW_EXPOSE_NATIVE_X11
#endif

#include "fncts.h"


int main(void) {
	double lastTime = glfwGetTime();
	int frame_count = 0;

	glfwSetErrorCallback(error_callback);


	//initialize GLFW
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}


	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	GLFWwindow* window = glfwCreateWindow(800, 600, "My indie Game", NULL, NULL);
	if (!window) {
		fprintf(stderr, "Failed to create GLFW window\n");
		glfwTerminate();
		return -1;
	}

	//Make the window's OpenGL context current
	glfwMakeContextCurrent(window);


	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		fprintf(stderr, "Failed to initialize GLAD\n");
		return -1;
	}

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	//Vertext shader
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	check_shader_compile(vertexShader, "VERTEX");
	//Fragment shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	check_shader_compile(fragmentShader, "FRAGMENT");
	//Shader program
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	check_program_link(shaderProgram);
	//cleanup shaders
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);



	float vertices[] = {
		-0.5f, -0.5f, 0.0f, // left
		 0.5f, -0.5f, 0.0f, // right
		 0.0f,  0.5f, 0.0f  // top
	};

	unsigned int VAO; //Vertex array Object
	unsigned int VBO; //Vertex buffer Object

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); //unbind the VBO
	glBindVertexArray(0); //unbind the VAO


	//Enable VSync
	glfwSwapInterval(1);

	printf("OpenGL Version: %s\n", glGetString(GL_VERSION));


	struct v2 Offset = { 0 };
	//Game loop
	while (!glfwWindowShouldClose(window)) {
		double currentTime = glfwGetTime();
		double deltaTime = currentTime - lastTime;
		lastTime = currentTime;
		frame_count++;
#if DEBUG_OUTPUT
		if (frame_count % 60 == 0) {
			printf("Frame: %d | Delta Time: %f\n", frame_count, deltaTime);
			printf("FPS: %.2f\n", frame_count / deltaTime);
			frame_count = 0;	
		}
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}
#endif
		//Set background color and clear screen
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f); //dark grey
		glClear(GL_COLOR_BUFFER_BIT);

		float speed = 0.3f;

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			Offset.y += speed * deltaTime; //move up
		}
		if (glfwGetKey(window,	GLFW_KEY_S) == GLFW_PRESS) {
			Offset.y -= speed * deltaTime; //move down
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			Offset.x -= speed * deltaTime; //move right
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			Offset.x += speed * deltaTime; //move left
		}

		// Clamp
		if (Offset.x > 1.0f) Offset.x = 1.0f;
		if (Offset.x < -1.0f) Offset.x = -1.0f;
		if (Offset.y > 1.0f) Offset.y = 1.0f;
		if (Offset.y < -1.0f) Offset.y = -1.0f;
		draw(shaderProgram,VAO,Offset);

		
		//swap front and back buffers
		glfwSwapBuffers(window);
		//poll for and process events
		glfwPollEvents();
	}//main loop

	//clearup and close
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
