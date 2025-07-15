#include <stdio.h>
#include <stdio.h>

#define GLFW_INCLUDE_NONE //this prevents GLFW from including <GL/gl.h> ,glad includes that
#include <GLFW/glfw3.h>

#include "glad/glad.h"

#ifdef _WIN32
	#define GLFW_EXPOSE_NATIVE_WIN32
	#pragma comment(lib,"opengl32.lib")
	#pragma comment(lib,"glfw3.lib")
#else
	#define GLFW_EXPOSE_NATIVE_X11
#endif


int main(void) {
	//initialize GLFW
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		fprintf(stderr, "Failed to initialize GLAD\n");
		return -1;
	}



	GLFWwindow* window = glfwCreateWindow(800, 600, "My indie Game", NULL, NULL);
	if (!window) {
		fprintf(stderr, "Failed to create GLFW window\n");
		glfwTerminate();
		return -1;
	}
	//Make the window's OpenGL context current
	glfwMakeContextCurrent(window);

	//Enable VSync
	glfwSwapInterval(1);

	//Game loop
	while (!glfwWindowShouldClose(window)) {
		//Set background color and clear screen
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f); //dark grey
		glClear(GL_COLOR_BUFFER_BIT);

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
