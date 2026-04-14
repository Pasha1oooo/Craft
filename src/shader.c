/* shader.c */

#include "../include/glad/glad.h"
#include <stdio.h>
#include <stdlib.h>

char *get_shader(char *shader_path)
{
	FILE *file = fopen(shader_path, "r");
	char *shader;
	size_t fsize;

	if (!file) {
		printf("File openning error [%s]\n", shader_path);
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	fsize = (size_t)ftell(file);
	fseek(file, 0, SEEK_SET);

	shader = (char *)calloc(fsize + 1, sizeof(char));

	if (!shader) {
		printf("Memory allocation error\n");
		fclose(file);
		return NULL;
	}

	fread(shader, sizeof(char), fsize, file);

	return shader;
}

unsigned int prepare_shaders(void)
{
	unsigned int shaderProgram;

	char vertexShaderPath[] = "src/shaders/vertex_shader.glsl";
	const char *vertexShaderSource = get_shader(vertexShaderPath);
	unsigned int vertexShader;

	char fragmentShaderPath[] = "src/shaders/fragment_shader.glsl";
	const char *fragmentShaderSource = get_shader(fragmentShaderPath);
	unsigned int fragmentShader;

	shaderProgram = glCreateProgram();

	vertexShader =  glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

unsigned int prepare_shaders2(void)
{
	unsigned int shaderProgram;

	char vertexShaderPath[] = "src/shaders/vertex_shader2.glsl";
	const char *vertexShaderSource = get_shader(vertexShaderPath);
	unsigned int vertexShader;

	char fragmentShaderPath[] = "src/shaders/fragment_shader.glsl";
	const char *fragmentShaderSource = get_shader(fragmentShaderPath);
	unsigned int fragmentShader;

	shaderProgram = glCreateProgram();

	vertexShader =  glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}
