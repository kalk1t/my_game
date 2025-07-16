


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

void draw(unsigned int shaderProgram, unsigned int VAO,float x ,float y) {
	
	unsigned int offsetLocation = glGetUniformLocation(shaderProgram, "offset");
	glUseProgram(shaderProgram);
	glUniform3f(offsetLocation,x,y,0.0f);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

