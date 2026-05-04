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

int is_block_solid(struct chunk *chunks, struct position *gpos)
{
	struct position chunk_gpos = gpos2chunkpos(gpos);
	struct position chunk_lpos;
	struct position lpos = gpos2lpos(gpos);

	int chunk_index;
	int block_index;

	int block_type, is_solid;

	chunk_lpos.x = chunk_gpos.x - chunks[0].pos->x;
	chunk_lpos.y = chunk_gpos.y - chunks[0].pos->y;
	chunk_lpos.z = chunk_gpos.z - chunks[0].pos->z;

	chunk_index = lpos2index(&chunk_lpos, CHUNKS_SIDE);
	block_index = lpos2index(&lpos, CHUNK_SIZE);



	block_type = chunks[chunk_index].chunk_data[block_index];
	is_solid = block_type == STONE ||
	           block_type == ORE;

	return is_solid;
}

void calculate_hitbox_vpos(vec3 player_vpos, vec3 hitbox_vpos)
{
	glm_vec3_copy(player_vpos, hitbox_vpos);
	hitbox_vpos[2] += PLAYER_HEIGHT / 2.0f - EYES_HEIGHT;
}

struct position calculate_blocks_area_size(void)
{
	struct position area_size;

	area_size.x = 2 * floor(PLAYER_WIDTH  / 2.0f) + 3;
	area_size.y = 2 * floor(PLAYER_WIDTH  / 2.0f) + 3;
	area_size.z = 2 * floor(PLAYER_HEIGHT / 2.0f) + 3;

	return area_size;
}

int is_colliding(vec3 dist)
{
	struct position is_side_colliding;
	int is_colliding;

	is_side_colliding.x = dist[0] < (BLOCK_SIZE + PLAYER_WIDTH)  / 2.0f;
	is_side_colliding.y = dist[1] < (BLOCK_SIZE + PLAYER_WIDTH)  / 2.0f;
	is_side_colliding.z = dist[2] < (BLOCK_SIZE + PLAYER_HEIGHT) / 2.0f;

	is_colliding = is_side_colliding.x &&
	               is_side_colliding.y &&
	               is_side_colliding.z;

	if (is_colliding)
		printf("Collision\n");

	return is_colliding;
}

void process_collisions(struct chunk *chunks, struct player *player)
{
	struct position hitbox_gpos;
	struct position blocks_area_size = calculate_blocks_area_size();
	vec3 hitbox_vpos;

	int blocks_amount = blocks_area_size.x *
	                    blocks_area_size.y *
	                    blocks_area_size.z;

	for (int i = 0; i < 3; i++) {
		struct position saved_block_gpos;

		glm_vec3_copy(player->prev_position, hitbox_vpos);
		hitbox_vpos[i] = player->position[i];
		calculate_hitbox_vpos(hitbox_vpos, hitbox_vpos);

		hitbox_gpos = vec2pos(hitbox_vpos);

		saved_block_gpos.x = hitbox_gpos.x - blocks_area_size.x / 2;
		saved_block_gpos.y = hitbox_gpos.y - blocks_area_size.y / 2;
		saved_block_gpos.z = hitbox_gpos.z - blocks_area_size.z / 2;

		for (int j = 0; j < blocks_amount; j++) {
			struct position block_gpos;
			vec3 block_vpos;
			vec3 dist;

			block_gpos.x = saved_block_gpos.x + j % blocks_area_size.x;
			block_gpos.y = saved_block_gpos.y + j / blocks_area_size.x %
			               blocks_area_size.y;
			block_gpos.z = saved_block_gpos.z + j / blocks_area_size.x /
			               blocks_area_size.y;

			if (!is_block_solid(chunks, &block_gpos))
				continue;

			pos2vec(&block_gpos, block_vpos);
			glm_vec3_sub(block_vpos, hitbox_vpos, dist);
			glm_vec3_abs(dist, dist);

			if (is_colliding(dist)) {
				player->position[i] = player->prev_position[i];
				break;
			}
		}
	}
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

	player.position[0] = 0.0f;
	player.position[1] = 0.0f;
	player.position[2] = 2.0f;

	player.prev_position[0] = 0.0f;
	player.prev_position[1] = 0.0f;
	player.prev_position[2] = 2.0f;

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
