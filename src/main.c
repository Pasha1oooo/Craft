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

struct position player_block_intersect(struct position block_gpos,
                                       vec3 player_vec_pos)
{
	struct position is_side_intersect;
	struct position tmp_is_intersect;
	vec3 block_vec_pos;
	vec3 block_player_distance;
	int is_intesrect;

	int vertical_intersect[2];

	pos2vec(&block_gpos, block_vec_pos);
	glm_vec3_sub(player_vec_pos, block_vec_pos, block_player_distance);

	block_player_distance[0] = fabs(block_player_distance[0]);
	block_player_distance[1] = fabs(block_player_distance[1]);

	vertical_intersect[0] = block_player_distance[2] >= 0 &&
	                        block_player_distance[2] < BLOCK_SIZE / 2.0f +
	                        EYES_HEIGHT;

	vertical_intersect[1] = block_player_distance[2] < 0 &&
	                       -block_player_distance[2] < BLOCK_SIZE / 2.0f +
	                        PLAYER_HEIGHT - EYES_HEIGHT;

	tmp_is_intersect.x = block_player_distance[0] <
	                     (BLOCK_SIZE + PLAYER_WIDTH) / 2.0f;

	tmp_is_intersect.y = block_player_distance[1] <
	                     (BLOCK_SIZE + PLAYER_WIDTH) / 2.0f;

	tmp_is_intersect.z = vertical_intersect[0] + vertical_intersect[1];

	is_intesrect = tmp_is_intersect.x &&
	               tmp_is_intersect.y &&
	               tmp_is_intersect.z;

	if (!is_intesrect) {
		is_side_intersect.x = 0;
		is_side_intersect.y = 0;
		is_side_intersect.z = 0;

		return is_side_intersect;
	}

	is_side_intersect.x = block_player_distance[0] >
	                      block_player_distance[1];

	is_side_intersect.y = block_player_distance[1] >
	                      block_player_distance[0];

	is_side_intersect.z = tmp_is_intersect.z;

	if (!is_side_intersect.x &&
	    !is_side_intersect.y &&
	    !is_side_intersect.z) {

		is_side_intersect.x = 0;
		is_side_intersect.y = 0;
		is_side_intersect.z = 0;
	}

	return is_side_intersect;
}

int calculate_collision_area(struct position *collision_area_side)
{
	int collision_area_volume;

	collision_area_side->x = 2 * floor(PLAYER_WIDTH) + 3;
	collision_area_side->y = 2 * floor(PLAYER_WIDTH) + 3;
	collision_area_side->z = floor(PLAYER_HEIGHT) + 4;

	collision_area_volume = collision_area_side->x *
	                        collision_area_side->y *
	                        collision_area_side->z;

	return collision_area_volume;
}

struct position check_intersect(struct chunk *chunks, vec3 player_vec_pos)
{
	struct position player_gpos = vec2pos(player_vec_pos);
	struct position block_gpos;
	struct position saved_block_gpos;
	struct position collision_area_side;
	struct position is_side_intersect;
	struct position result_intersect = {.x = 0, .y = 0, .z = 0};
	int collision_area_volume = calculate_collision_area(
	                            &collision_area_side);
	int is_solid;

	saved_block_gpos.x = player_gpos.x - collision_area_side.x / 2;
	saved_block_gpos.y = player_gpos.y - collision_area_side.y / 2;
	saved_block_gpos.z = player_gpos.z - abs(EYES_HEIGHT) - 1;

	printf("[%2d %2d %2d]\n", player_gpos.x, player_gpos.y, player_gpos.z);

	for (int i = 0; i < collision_area_volume; i++) {
		block_gpos.x = saved_block_gpos.x + i % collision_area_side.x;
		block_gpos.y = saved_block_gpos.y + (i / collision_area_side.y)
		               % collision_area_side.y;
		block_gpos.z = saved_block_gpos.z + (i / collision_area_side.z)
		               / collision_area_side.y;

		printf("<%2d %2d %2d>\n", block_gpos.x, block_gpos.y, block_gpos.z);
		is_solid = get_world_block(block_gpos, chunks) == STONE ||
		           get_world_block(block_gpos, chunks) == ORE;

		if (!is_solid)
			continue;

		is_side_intersect = player_block_intersect(block_gpos,
		                                           player_vec_pos);

		if (!result_intersect.x)
			result_intersect.x = is_side_intersect.x;

		if (!result_intersect.y)
			result_intersect.y = is_side_intersect.y;

		if (!result_intersect.x)
			result_intersect.z = is_side_intersect.z;
	}

	return result_intersect;
}

const int COLLISION_STEPS = 1;

void process_collisions(struct chunk *chunks, struct player *player)
{
	struct position is_collision;
	vec3 delta_vec_pos;

	glm_vec3_sub(player->position, player->prev_position, delta_vec_pos);
	glm_vec3_scale(delta_vec_pos,
	               1.0f / (float)COLLISION_STEPS,
	               delta_vec_pos);

	glm_vec3_copy(player->prev_position, player->position);

	for (int i = 0; i < COLLISION_STEPS; i++) {

		glm_vec3_add(player->position,
		             delta_vec_pos,
		             player->position);

		is_collision = check_intersect(chunks, player->position);

		if (is_collision.x)
			player->position[0] = player->prev_position[0];
		if (is_collision.y)
			player->position[1] = player->prev_position[1];
		if (is_collision.z)
			player->position[2] = player->prev_position[2];

		player->prev_position[0] = player->position[0];
		player->prev_position[1] = player->position[1];
		player->prev_position[2] = player->position[2];
	}

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
	get_chunks(loaded_chunks, player.head.cameraPos);

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

			struct position player_gpos = vec2pos(
			                              player.head.cameraPos);
			saved_chunk_pos = gpos2chunkpos(&player_gpos);
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
//		notcurses_render(nc);

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
