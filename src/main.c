/* main.c */

#include "../include/glad/glad.h"
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <cglm/cglm.h>
#include <GLFW/glfw3.h>
#include "render.h"
#include "generator.h"
#include "shader.h"
#include "render.h"
#include "texture.h"

/* ??? */
#define BLOCKS_NUM 8

const block blocks[BLOCKS_NUM] = {{{255, 255, 255, 255}, '#'},
                                  {{0  , 0  , 0  , 255}, ' '},
                                  {{255, 0  , 255, 255}, '`'},
                                  {{255, 255, 0  , 255}, '-'},
                                  {{0  , 255, 255, 255}, '@'},
                                  {{255,   0, 0  , 255}, '+'},
                                  {{0  , 255, 0  , 255}, '&'},
                                  {{0  , 0  , 255, 255}, '!'}};

mat4 *create_chunk(struct chunk chunk, int * num)
{
	int CUBES_PER_AXIS = 16;
	int TOTAL_CUBES = CUBES_PER_AXIS * CUBES_PER_AXIS * CUBES_PER_AXIS;
	mat4 *modelMatrices = (mat4 *)calloc(TOTAL_CUBES, sizeof(mat4));
	int idx = 0;

	for (int i = 0; i < TOTAL_CUBES; i++) {
		if(chunk.chunk_data[i] == '*'){
			mat4 model = GLM_MAT4_IDENTITY_INIT;
			vec3 offset;

			offset[0] = (float)(chunk.pos->x * CUBES_PER_AXIS +
			                                 (i % CUBES_PER_AXIS));
			offset[1] = (float)(chunk.pos->y * CUBES_PER_AXIS +
			                (i / CUBES_PER_AXIS) % CUBES_PER_AXIS);
			offset[2] = (float)(chunk.pos->z * CUBES_PER_AXIS +
			                (i / CUBES_PER_AXIS) / CUBES_PER_AXIS);

			glm_translate(model, offset);
			glm_mat4_copy(model, modelMatrices[idx]);
			idx++;
		}
	}

	*num = idx;

	return modelMatrices;
}

int main(void)
{
	GLFWwindow * window;
	int fb_width = 2560, fb_height = 1600;

	create_window(&window, fb_width , fb_height);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		printf("Failed to initialize GLAD\n");
		return 0;
	}

	#define MAX_CHUNKS_PER_AXIS 2

	unsigned int VBO, VAO, EBO, instanceVBO;

	prepare_gl_environment(&VBO, &VAO, &EBO, &instanceVBO);

	unsigned int shaderProgram = prepare_shaders();

	unsigned int texture;
	prepare_texture(&texture);

	float frame_start = 0 , frame_end = 0, global_time = 0;
	int FPS = 0, frame_counter = 0;
	unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
	unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
	unsigned int projectionLoc = glGetUniformLocation(shaderProgram,
	                                                         "projection");
	mat4 view = GLM_MAT4_IDENTITY_INIT;

	player player = create_player();

	printf("Start render\n");

	while(!glfwWindowShouldClose(window)) {
		global_time = glfwGetTime();
		frame_start = glfwGetTime();
		processInput(window);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.2f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture"),
		                                                            0);

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			player.y += player.speed;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			player.y -= player.speed;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			player.x += player.speed;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			player.x -= player.speed;
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			player.z -= player.speed;
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			player.z += player.speed;

		glm_vec3_copy((vec3){player.x, player.y, player.z},
		                                        player.head.cameraPos);
		vec3 target;

		glm_vec3_add(player.head.cameraPos, (vec3){0,0,1}, target);
		glm_lookat(player.head.cameraPos, target, player.head.cameraUp,
		                                                         view);
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float*)view);
		mat4 projection;
		glm_perspective(glm_rad(90.0f), (float)fb_width /
		                  (fb_height+150), 0.1f, 10000.0f, projection);
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE,
		                                           (float*)projection);

		struct chunk *loaded_chunks = init_chunks();

		get_chunks(loaded_chunks, player.head.cameraPos);
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);

		for (int i = 0; i < pow(2 * RENDER_DISTANCE - 1, 3); i++) {
			int num;
			mat4 *modelMatrices = create_chunk(loaded_chunks[i],
			                                                 &num);

			glBindTexture(GL_TEXTURE_2D, texture);
			glBindVertexArray(VAO);
			glBufferData(GL_ARRAY_BUFFER, num * sizeof(mat4),
		                                modelMatrices, GL_STATIC_DRAW);
			glDraw(num);
			free(modelMatrices);
		}

	deinit_chunks(loaded_chunks);
	glfwSwapBuffers(window);

	glfwPollEvents();
	frame_counter++;

	if (frame_start - frame_end > 1) {
		frame_end = frame_start;
		FPS = frame_counter;
		frame_counter = 0;
	}

	printf("\033[H");
		printf("\nFPS: %d %lg\n", FPS, global_time);
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &instanceVBO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(shaderProgram);
	glfwTerminate();

	return 0;
}
