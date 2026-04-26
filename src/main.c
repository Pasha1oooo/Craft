/* main.c */

#include "../include/glad/glad.h"
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <cglm/cglm.h>
#include <GLFW/glfw3.h>
#include "generator.h"
#include "render.h"
#include "shader.h"
#include "render.h"
#include "texture.h"
#include "logic.h"

const float PLAYER_WIDTH = 0.6f;
const float PLAYER_HEIGHT = 1.8f;
const float EYES_HEIGHT = 1.6f;
const float BLOCK_SIZE = 1.0f;

// optimize
struct position box_box_intersect(struct chunk *chunks, struct player *player)
{
	const int CHUNKS_AMOUNT = pow(2 * RENDER_DISTANCE - 1, 3);
	const float MIN_DIFF = 0.1f;
	struct position is_intersect = {.x = 0, .y = 0, .z = 0};
	struct position tmp_is_intersect;;
	vec3 block_pos;
	vec3 chunk_offset;
	vec3 block_player_distance;
	int intersection;

	for (int i = 0; i < CHUNKS_AMOUNT; i++) {
		chunk_offset[0] = CHUNK_SIZE * (float)chunks[i].pos->x;
		chunk_offset[1] = CHUNK_SIZE * (float)chunks[i].pos->y;
		chunk_offset[2] = CHUNK_SIZE * (float)chunks[i].pos->z;

		for (int j = 0; j < BLOCKS_IN_CHUNK; j++) {
			if (chunks[i].chunk_data[j] == ' ')
				continue;

			block_pos[0] = chunk_offset[0] + j % CHUNK_SIZE;
			block_pos[1] = chunk_offset[1] + (j / CHUNK_SIZE)
			               % CHUNK_SIZE;
			block_pos[2] = chunk_offset[2] + j / (CHUNK_SIZE *
			                                      CHUNK_SIZE);

			block_player_distance[0] = fabs(block_pos[0] -
			                                player->position[0]);

			block_player_distance[1] = fabs(block_pos[1] -
			                                player->position[1]);

			block_player_distance[2] = block_pos[2] -
			                           player->position[2];

			tmp_is_intersect.x = block_player_distance[0] <
			                    (BLOCK_SIZE + PLAYER_WIDTH) / 2.0f;

			tmp_is_intersect.y = block_player_distance[1] <
			                    (BLOCK_SIZE + PLAYER_WIDTH) / 2.0f;

			tmp_is_intersect.z = block_player_distance[2] <
			                     BLOCK_SIZE / 2.0f +
			                    (PLAYER_HEIGHT - EYES_HEIGHT) &&

			                    -block_player_distance[2] <
			                     BLOCK_SIZE / 2.0f + EYES_HEIGHT;

			intersection = tmp_is_intersect.x &&
			               tmp_is_intersect.y &&
			               tmp_is_intersect.z;

			if (!intersection)
				continue;

			if (!is_intersect.x) {
				is_intersect.x = block_player_distance[0] >
				                 block_player_distance[1] +
				                 MIN_DIFF;
			}

			if (!is_intersect.y) {
				is_intersect.y = block_player_distance[1] >
				                 block_player_distance[0] +
				                 MIN_DIFF;
			}

			if (!is_intersect.z) {
			}
		}
	}

	return is_intersect;
}

const int MOVE_STEPS = 100;

void process_collisions(struct chunk *chunks, struct player *player)
{
	struct position collision = box_box_intersect(chunks, player);

	if (collision.x)
		player->position[0] = player->prev_position[0];
	if (collision.y)
		player->position[1] = player->prev_position[1];
	if (collision.z)
		player->position[2] = player->prev_position[2];

	player->prev_position[0] = player->position[0];
	player->prev_position[1] = player->position[1];
	player->prev_position[2] = player->position[2];

	return;
}

int main(void)
{
	unsigned char *frame_buffer = (unsigned char*)calloc(FB_WIDTH *
	                                                     FB_HEIGHT,
	                                                     sizeof(float));
	float *depth_buffer = (float*)calloc(FB_WIDTH * FB_HEIGHT,
	                                     sizeof(float));
	GLFWwindow *window;
	struct position saved_chunk_pos = {.x = 0, .y = 0, .z = 0};
	unsigned int modelLoc, viewLoc, projectionLoc;
	unsigned int VBO, VAO, EBO, instanceVBO;
	unsigned int VBO_highlight, VAO_highlight, EBO_highlight;
	unsigned int shaderProgram, shaderProgram2;
	unsigned int texture, texture2, texture3;
	vec3 target;
	mat4 view = GLM_MAT4_IDENTITY_INIT;
	struct time time;
	struct player player;
	struct chunk *loaded_chunks = init_chunks();

	create_window(&window, FB_WIDTH, FB_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("Failed to initialize GLAD\n");
		return 0;
	}

	prepare_gl_environment(&VBO, &VAO, &EBO,
	                       &instanceVBO,
	                       &VBO_highlight,
	                       &VAO_highlight,
	                       &EBO_highlight);

	prepare_texture(&texture, "a.png");
	prepare_texture(&texture2, "black.png");
	prepare_texture(&texture3, "c.png");

	shaderProgram = prepare_shaders();
	shaderProgram2 = prepare_shaders2();

	struct notcurses *nc = notcurses_prepare();

	modelLoc = glGetUniformLocation(shaderProgram, "model");
	viewLoc = glGetUniformLocation(shaderProgram, "view");
	projectionLoc = glGetUniformLocation(shaderProgram, "projection");

	player = create_player();

	time.start = glfwGetTime();
	struct position selected_block;
	selected_block.x = -1; selected_block.y = -1; selected_block.z = -1;

	while(!glfwWindowShouldClose(window)) {
		mat4 projection;

		processInput(window, &player, loaded_chunks, &selected_block);
		process_collisions(loaded_chunks, &player);

		glEnable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glUniform1i(glGetUniformLocation(shaderProgram, "Texture"), 0);

		glm_vec3_copy(player.position, player.head.cameraPos);
		glm_vec3_add(player.head.cameraPos,
		             player.head.cameraDirection,
		             target);

		glm_lookat(player.head.cameraPos,
		           target,
		           player.head.cameraUp,
		           view);


		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float *)view);

		glm_perspective(glm_rad(player.head.FOV), (float)FB_WIDTH /
		                                          (FB_HEIGHT + 150),
		                0.1f, 10000.0f, projection);

		glUniformMatrix4fv(projectionLoc,
		                   1, GL_FALSE,
		                   (float *)projection);

		if (is_chunk_changed(player.head.cameraPos,
		                     &saved_chunk_pos)) {

			get_chunks(loaded_chunks, player.head.cameraPos);
			save_chunk_pos(&player.head.cameraPos,
			               &saved_chunk_pos);
		}

		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);

		render_chunks(loaded_chunks, texture, texture3, VAO);
		glm_vec3_copy(player.position, player.head.cameraPos);

		int is_block_selected = select_block(player,
		                                     loaded_chunks,
		                                     &selected_block);

		if (is_block_selected) {
			glUseProgram(shaderProgram2);
			glUniformMatrix4fv(glGetUniformLocation(shaderProgram2,
			                                        "view"),
			                   1, GL_FALSE,
			                   (float *)view);

			glUniformMatrix4fv(glGetUniformLocation(shaderProgram2,
			                                        "projection"),
			                   1, GL_FALSE,
			                   (float *)projection);

			mat4 model_highlight = GLM_MAT4_IDENTITY_INIT;

			glm_translate(model_highlight, (vec3){selected_block.x,
			                                     selected_block.y,
			                                    selected_block.z});

			glUniformMatrix4fv(glGetUniformLocation(shaderProgram2,
			                                        "model"),
			                   1, GL_FALSE,
			                   (float *)model_highlight);

			glBindVertexArray(VAO_highlight);
			glBindTexture(GL_TEXTURE_2D, texture2);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
			glUseProgram(shaderProgram);
		}

		glfwSwapBuffers(window);

		glReadBuffer(GL_BACK);
		glReadPixels(0, 0, FB_WIDTH, FB_HEIGHT,
		             GL_RGBA, GL_UNSIGNED_BYTE,
		             frame_buffer);

		glReadPixels(0, 0, FB_WIDTH, FB_HEIGHT,
		             GL_DEPTH_COMPONENT, GL_FLOAT,
		             depth_buffer);

		player.prev_position[0] = player.position[0];
		player.prev_position[1] = player.position[1];
		player.prev_position[2] = player.position[2];

		struct ncplane* n = notcurses_stdplane(nc);

		notcurses_render_ascii(nc, n, frame_buffer, depth_buffer);
		//stat_render(nc, n, time);
		notcurses_render(nc);

		glfwPollEvents();

		printf("\033[H");
	}

	free(frame_buffer);
	free(depth_buffer);

	deinit_chunks(loaded_chunks);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &instanceVBO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(shaderProgram);

	notcurses_stop(nc);
	glfwTerminate();

	return 0;
}
