


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

unsigned int loadTexture(const char* path)	 {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	//set parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int width;
	int height;
	int nrChannels;
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);

	if (data) {
		GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		printf("Failed to load texture: %s\n", path);
		exit(1);
	}

	stbi_image_free(data);
	return textureID;
}

int check_bullet_collision(Bullet bullet, Enemy enemy,int* score) {
	if (bullet.active && enemy.alive) {
		//simple AABB collision detection
		if (bullet.x < enemy.x + 0.1f && bullet.x + 0.05f > enemy.x &&
			bullet.y < enemy.y + 0.1f && bullet.y + 0.05f > enemy.y) {
			enemy.alive = 0; //mark enemy as dead
			bullet.active = 0; //deactivate bullet
			*score += 100;
			return 1; //collision detected
		}
	}
	return 0; //no collision
}

void draw_sprite(unsigned int shaderProgram, unsigned int VAO,unsigned int texture,float x ,float y,float scale) {

	glUseProgram(shaderProgram);
	int offsetLoc = glGetUniformLocation(shaderProgram, "offset");
	glUniform2f(offsetLoc, x, y);

	int scaleLoc = glGetUniformLocation(shaderProgram, "scale");
	glUniform1f(scaleLoc, scale);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

