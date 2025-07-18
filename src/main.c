#include <stdio.h>
#include <math.h>

#include "define.h"
#define GLFW_INCLUDE_NONE //this prevents GLFW from including <GL/gl.h> ,glad includes that
#include <GLFW/glfw3.h>
#include <glad.h>
#include "shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#ifdef _WIN32
	#include <windows.h>
	#define GLFW_EXPOSE_NATIVE_WIN32
	#pragma comment(lib,"opengl32.lib")
	#pragma comment(lib,"glfw3.lib")
#else
	#define GLFW_EXPOSE_NATIVE_X11
	#include <unistd.h>

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

	//Load textures
	playerTexture = loadTexture("assets/images/player.png");
	bulletTexture = loadTexture("assets/images/bullet.png");
	enemyTexture = loadTexture("assets/images/enemy.png");
	backgroundTexture = loadTexture("assets/images/background.png");


	//initialize sound engine
	ma_engine engine;

	if (ma_engine_init(NULL, &engine)) {
		fprintf(stderr, "Failed to initialize sound engine\n");
	}

	ma_sound shootSound;
	ma_sound scoreSound;
	ma_sound hitSound;

	ma_sound_init_from_file(&engine, "assets/sounds/hit.wav", 0, NULL, NULL, &hitSound);
	ma_sound_init_from_file(&engine, "assets/sounds/shoot.wav", 0, NULL,NULL, &shootSound);
	ma_sound_init_from_file(&engine, "assets/sounds/score.wav", 0, NULL, NULL, &scoreSound);

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
	//cleanup shaders
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	check_program_link(shaderProgram);

	glUseProgram(shaderProgram);
	int texLoc = glGetUniformLocation(shaderProgram, "spriteTexture");
	if (texLoc == -1) {
		fprintf(stderr, "Failed to find uniform location for spriteTexture\n");
		return -1;
	}
	glUniform1i(texLoc, 0); //set texture unit 0 for spriteTexture




	float vertices[] = {
		//positions		//tex coords
		-0.5f, -0.5f,	0.0f, 0.0f,
		 0.5f, -0.5f,	1.0f, 0.0f,
		 0.5f,  0.5f,	1.0f, 1.0f,
		-0.5f,  0.5f,	0.0f, 1.0f,
	};

	unsigned int indices[] = { 0,1,2,2,3,0 };

	unsigned int VAO; //Vertex array Object
	unsigned int VBO; //Vertex buffer Object
	unsigned int EBO; //Element Buffer Object

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//position attribute
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//texCoord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);


	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //enable alpha blending

	int space_was_pressed = 0;

	Player player = { 0.0f, -0.8f, 1.0f,3 }; //Init player position,speed is 1.0f unit persec
	Bullet bullets[MAX_BULLETS] = { 0 };
	Enemy enemies[MAX_ENEMIES] = { 0 };
	int game_score = 0;
	int check_score = 0;
	
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

		//draw background
		draw_sprite(shaderProgram, VAO, backgroundTexture, 0.0f, 0.0f, 2.0f);


		float velocity = player.speed * deltaTime;
		//movement
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			player.y += velocity;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			player.y -= velocity;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			player.x -= velocity;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			player.x += velocity;
		}

		// Clamp
		if (player.x > 1.0f) player.x = 1.0f;
		if (player.x < -1.0f) player.x = -1.0f;
		if (player.y > 1.0f) player.y = 1.0f;
		if (player.y < -1.0f) player.y = -1.0f;

		int space_is_pressed = glfwGetKey(window, GLFW_KEY_SPACE);
		//fire bullet
		if (space_is_pressed==GLFW_PRESS && space_was_pressed==GLFW_RELEASE) {
			int bullet_fired = 0;
			for (int i = 0; i < MAX_BULLETS; i++) {
				if (bullets[i].active == 0) {
					bullets[i].x = player.x;
					bullets[i].y = player.y;
					bullets[i].speed = 1.0f; //bullet speed is double the player speed
					bullets[i].active = 1; //set bullet as active
					bullet_fired = 1;
					break;
				}
			}
			if (bullet_fired) {
				ma_sound_seek_to_pcm_frame(&shootSound, 0); //reset sound to start
				ma_sound_start(&shootSound); // Play it
			}
		}
		space_was_pressed = space_is_pressed; //update space_was_pressed for next frame
		//update bullets
		for (int i = 0; i < MAX_BULLETS; i++) {
			if (bullets[i].active) {
				bullets[i].y += bullets[i].speed * deltaTime; //move bullet up
				if (bullets[i].y > 1.0f) { //if bullet goes off screen
					bullets[i].active = 0; //deactivate bullet
				}
			}
		}

		//spawn enemies
		if (rand() % 100 < 1) { //1% chance to spawn an enemy each frame
			for (int i = 0; i < MAX_ENEMIES; i++) {
				if (enemies[i].alive == 0) { //find inactive enemy
					enemies[i].x = (rand() % 200-100) / 100.0f; //random x position between -1.0 and 1.0
					enemies[i].y = 1.2f; //start at top of screen
					enemies[i].speed = 0.3f + ((float)(rand() % 30) / 100.0f); //random speed between 0.3 and 0.6
					enemies[i].alive = 1; //set enemy as active
					break; //exit loop after finding an inactive enemy
				}
			}
		}

		check_score = game_score;
		//check if bullet touchs enemy
		for (int i = 0; i < MAX_BULLETS; i++) {
			for (int j = 0; j < MAX_ENEMIES; j++) {
				if (check_bullet_collision(bullets[i], enemies[j],&game_score)) {
					//bullet hit enemy, deactivate bullet and mark enemy as dead
					bullets[i].active = 0; //deactivate bullet
					enemies[j].alive = 0; //mark enemy as dead
					printf("Hit! Score: %d\n", game_score);
				}
				if (check_enemy_player_collision(player, enemies[j])) {
					//enemy hit player
					player.health--;

					ma_sound_seek_to_pcm_frame(&hitSound, 0); //reset sound to start
					ma_sound_start(&hitSound); // Play it

					enemies[j].alive = 0; //mark enemy as dead
					printf("Player hit! Health: %d\n", player.health);
					if (player.health <= 0) {
						printf("Game Over! Final Score: %d\n", game_score);
						glfwSetWindowShouldClose(window, GLFW_TRUE); //close window if player is dead
					}
				}
			}
		}
		if (game_score > check_score) {
			//play score sound

			ma_sound_seek_to_pcm_frame(&scoreSound, 0); //reset sound to start
			ma_sound_start(&scoreSound); // Play it

		}

		//update enemies
		for (int i = 0; i < MAX_ENEMIES; i++) {
			if (enemies[i].alive) {
				enemies[i].y -= enemies[i].speed * deltaTime; //move enemy down
			}
			//if enemy goes off screen or is dead, deactivate it
			if (enemies[i].y < -1.0f || !enemies[i].alive) enemies[i].alive = 0;
			
		
		}

		//draw enemies
		for (int i = 0; i < MAX_ENEMIES; i++) {
			if (enemies[i].alive) {
				draw_sprite(shaderProgram, VAO, enemyTexture, enemies[i].x, enemies[i].y, 0.2f);
			}
		}

		//draw player
		draw_sprite(shaderProgram,VAO,playerTexture,player.x,player.y,0.2f);
		for (int i = 0; i < player.health; i++) {
			draw_sprite(shaderProgram, VAO, playerTexture, -0.9f + i * 0.1f, -0.9f, 0.05f); //draw health icons
		}
		//draw bullets
		for (int i = 0; i < MAX_BULLETS; i++) {
			if (bullets[i].active) {
				draw_sprite(shaderProgram, VAO,bulletTexture, bullets[i].x, bullets[i].y,0.05f);
			}
		}

		
		//swap front and back buffers
		glfwSwapBuffers(window);
		//poll for and process events
		glfwPollEvents();
	}//main loop

	//cleanup sound engine
	ma_sound_uninit(&shootSound);
	ma_sound_uninit(&scoreSound);
	ma_sound_uninit(&hitSound);

	ma_engine_uninit(&engine);

	//clearup and close
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
