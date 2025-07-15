#include <stdio.h>

#define GLFW_INCLUDE_NONE //this prevents GLFW from including <GL/gl.h> ,glad includes that
#include <GLFW/glfw3.h>
#include "glad/glad.h"
#include "shader.h"

#ifdef _WIN32
	#define GLFW_EXPOSE_NATIVE_WIN32
	#pragma comment(lib,"opengl32.lib")
	#pragma comment(lib,"glfw3.lib")
#else
	#define GLFW_EXPOSE_NATIVE_X11
#endif

void error_callback(int error, const char* description) {
	fprintf(stderr, "Error %d: %s\n", error, description);
}

void check_shader_compile(unsigned int shader, const char* name) {
	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		fprintf(stderr, "ERROR::SHADER::COMPILATION_FAILED (%s)\n%s\n", name, infoLog);
	}
}

void check_program_link(unsigned int program) {
	int success;
	char infoLog[512];
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		fprintf(stderr, "ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
	}
}

int main(void) {

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


	//Game loop
	while (!glfwWindowShouldClose(window)) {
		//Set background color and clear screen
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f); //dark grey
		glClear(GL_COLOR_BUFFER_BIT);


		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3); //draw the triangle

		//swap front and back buffers
		glfwSwapBuffers(window);
		//poll for and process events
		glfwPollEvents();
	}

	//clearup and close
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
