/* main.c */

#include "../include/glad/glad.h"
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <cglm/cglm.h>
#include <GLFW/glfw3.h>
#include "render.h"
#include "generator.h"
#include "shader.h"
#include "render.h"
#include "texture.h"

#define BLOCKS_TYPES_AMOUNT 8

const struct block blocks[BLOCKS_TYPES_AMOUNT] = {{{255, 255, 255, 255}, '#'},
                                                 {{0  , 0  , 0  , 255}, ' '},
                                                 {{255, 0  , 255, 255}, '`'},
                                                 {{255, 255, 0  , 255}, '-'},
                                                 {{0  , 255, 255, 255}, '@'},
                                                 {{255,   0, 0  , 255}, '+'},
                                                 {{0  , 255, 0  , 255}, '&'},
                                                 {{0  , 0  , 255, 255}, '!'}};

mat4 *create_chunk(struct chunk *chunk, int *num)
{
	int TOTAL_CUBES = pow(CHUNK_SIZE, 3);
	mat4 *modelMatrices = (mat4 *)calloc(TOTAL_CUBES, sizeof(mat4));
	int idx = 0;

	for (int i = 0; i < TOTAL_CUBES; i++, idx++) {
		if (chunk->chunk_data[i] == '*'){
			mat4 model = GLM_MAT4_IDENTITY_INIT;
			vec3 offset;

			offset[0] = (float)(chunk->pos->x * CHUNK_SIZE +
			                                     (i % CHUNK_SIZE));
			offset[1] = (float)(chunk->pos->y * CHUNK_SIZE +
			                        (i / CHUNK_SIZE) % CHUNK_SIZE);
			offset[2] = (float)(chunk->pos->z * CHUNK_SIZE +
			                        (i / CHUNK_SIZE) / CHUNK_SIZE);

			glm_translate(model, offset);
			glm_mat4_copy(model, modelMatrices[idx]);
		}
	}

	*num = idx;

	return modelMatrices;
}

int main(void)
{
	const int CHUNKS_AMOUNT = pow(2 * RENDER_DISTANCE - 1, 3);
	GLFWwindow * window;
	unsigned int modelLoc, viewLoc, projectionLoc;
	unsigned int VBO, VAO, EBO, instanceVBO;
	unsigned int shaderProgram;
	unsigned int texture;
	struct time time;
	struct player player;
	struct chunk *loaded_chunks = init_chunks();
	vec3 player_pos, target;
	mat4 view = GLM_MAT4_IDENTITY_INIT;

	create_window(&window, FB_WIDTH, FB_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("Failed to initialize GLAD\n");
		return 0;
	}

	prepare_gl_environment(&VBO, &VAO, &EBO, &instanceVBO);
	prepare_texture(&texture);
	shaderProgram = prepare_shaders();
	player = create_player();

	modelLoc = glGetUniformLocation(shaderProgram, "model");
	viewLoc = glGetUniformLocation(shaderProgram, "view");
	projectionLoc = glGetUniformLocation(shaderProgram, "projection");

	time.frame_counter = 0;
	time.start = glfwGetTime();

	while(!glfwWindowShouldClose(window)) {

		processInput(window, &player);

		glEnable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.2f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture"),
		                                                            0);
		player_pos[0] = player.x;
		player_pos[1] = player.y;
		player_pos[2] = player.z;
// >>> add ability to rotate camera
		glm_vec3_copy(player_pos, player.head.cameraPos);
		glm_vec3_add(player.head.cameraPos, (vec3){0,1,0}, target);
		glm_lookat(player.head.cameraPos, target, player.head.cameraUp,
		                                                         view);
// <<<

// >>> rewrite
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float*)view);
		mat4 projection;
		glm_perspective(glm_rad(90.0f), (float)FB_WIDTH /
		                  (FB_HEIGHT+150), 0.1f, 10000.0f, projection);
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE,
		                                           (float*)projection);

		get_chunks(loaded_chunks, player.head.cameraPos);
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
// <<<

		for (int i = 0; i < CHUNKS_AMOUNT; i++) {
			int num;
			mat4 *modelMatrices = create_chunk(&(loaded_chunks[i]),
			                                                 &num);

			glBindTexture(GL_TEXTURE_2D, texture);
			glBindVertexArray(VAO);
			glBufferData(GL_ARRAY_BUFFER, num * sizeof(mat4),
		                                modelMatrices, GL_STATIC_DRAW);
			glDraw(num);
			free(modelMatrices);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();

		calculate_fps(&time);
	}

	deinit_chunks(loaded_chunks);
	deinit_gl_environment(&VAO, &instanceVBO, &VBO, &EBO, shaderProgram);

	return 0;
}
