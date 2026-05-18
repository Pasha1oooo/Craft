/* main.c */

#include "../include/glad/glad.h"
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <cglm/cglm.h>
#include <GLFW/glfw3.h>
#include "generator.h"
#include "logic.h"
#include "render.h"
#include "shader.h"
#include "render.h"
#include "texture.h"

struct entity{
	float x;
	float y;
	float z;
	float angle;
};

const float GRAVITY = 0.05f;
const float MAX_VELOCITY = 1.0f;

int is_on_ground(struct chunk *chunks, struct player player)
{
	player.position[2] -= 0.2f;

	process_collisions(chunks, &player);

	return player.on_ground;
}

void init_model_matrices(struct model_matrices *mm)
{
	mm->stone = (mat4 *)calloc(BLOCKS_IN_CHUNK, sizeof(mat4));
	mm->ore   = (mat4 *)calloc(BLOCKS_IN_CHUNK, sizeof(mat4));
}

void deinit_model_matrices(struct model_matrices *mm)
{
	free(mm->stone);
	free(mm->ore);
}

void process_gravity(struct player *player)
{
	if (!player->on_ground) {
		if (abs(player->velocity[2]) < MAX_VELOCITY)
			player->velocity[2] -= GRAVITY;
	} else {
		player->velocity[2] = 0;
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
	unsigned int texture, texture2, texture3, texture4;
	vec3 target;
	mat4 view = GLM_MAT4_IDENTITY_INIT;
	struct time time;
	struct player player;
	struct chunk *loaded_chunks = init_chunks();
	struct model_matrices model_matrices;

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
	prepare_texture(&texture4, "black.png");
	struct entity entity = {10,10,10,0.3};

	shaderProgram = prepare_shaders();
	shaderProgram2 = prepare_shaders2();

	struct notcurses *nc = notcurses_prepare();

	modelLoc = glGetUniformLocation(shaderProgram, "model");
	viewLoc = glGetUniformLocation(shaderProgram, "view");
	projectionLoc = glGetUniformLocation(shaderProgram, "projection");

	player = create_player();

	get_chunks(loaded_chunks, player.head.cameraPos);
	init_model_matrices(&model_matrices);

	time.start = glfwGetTime();
	struct position selected_block;
	selected_block.x = -1; selected_block.y = -1; selected_block.z = -1;

	while(!glfwWindowShouldClose(window)) {
		mat4 projection;

		process_gravity(&player);
		processInput(window, &player, loaded_chunks, &selected_block);
		process_collisions(loaded_chunks, &player);
		player.on_ground = is_on_ground(loaded_chunks, player);

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


//		time.start = glfwGetTime();

		render_chunks(loaded_chunks, &model_matrices,
		              texture, texture3, VAO);


//calculate_fps(&time);

		glm_vec3_copy(player.position, player.head.cameraPos);
		double lastFrame;
		double currentFrame = glfwGetTime();
		double deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		const float MOB_SPEED = 0.2f;
		vec3 mobPos = {entity.x, entity.y, entity.z};
		vec3 dir;
		glm_vec3_sub(player.position, mobPos, dir);
		float distance = glm_vec3_norm(dir);
		if (distance > 0.001f) {
		    float step = MOB_SPEED * (float)deltaTime;
		    if (step > distance) step = distance;
		    entity.x += dir[0] * step;
		    entity.y += dir[1] * step;
		    entity.z += dir[2] * step;
		}


		int a = 0;
		for(int i = 0; i < pow(2 * RENDER_DISTANCE - 1, 3); i++) {
		    if((int)floor(entity.x / 16.0f) == loaded_chunks[i].pos->x &&
		       (int)floor(entity.y / 16.0f) == loaded_chunks[i].pos->y &&
		       (int)floor(entity.z / 16.0f) == loaded_chunks[i].pos->z)
		        a = 1;
		}
		if (a != 0) {
			glUseProgram(shaderProgram2);
			glUniformMatrix4fv(glGetUniformLocation(shaderProgram2,"view"),1, GL_FALSE,(float *)view);

			glUniformMatrix4fv(glGetUniformLocation(shaderProgram2,"projection"),1, GL_FALSE,(float *)projection);

			mat4 model_highlight = GLM_MAT4_IDENTITY_INIT;

			glm_translate(model_highlight, (vec3){entity.x,
			                                     entity.y,
			                                    entity.z});
			glm_rotate(model_highlight, entity.angle,(vec3){0,0,1});
			glm_scale(model_highlight, (vec3){3,3,3});

			glUniformMatrix4fv(glGetUniformLocation(shaderProgram2,
			                                        "model"),
			                   1, GL_FALSE,
			                   (float *)model_highlight);

			glBindVertexArray(VAO_highlight);
			glBindTexture(GL_TEXTURE_2D, texture4);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
			glUseProgram(shaderProgram);
		}


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

		//!!!!!!!!!!!!!!!!!!!!!!!!!!!
//		time.fps = player.on_ground;

		calculate_fps(&time);

		struct ncplane* n = notcurses_stdplane(nc);

		notcurses_render_ascii(nc, n, frame_buffer, depth_buffer);
		stat_render(nc, n, time, &player);
		notcurses_render(nc);

		glfwPollEvents();

		printf("\033[H");
	}

	free(frame_buffer);
	free(depth_buffer);

	deinit_chunks(loaded_chunks);
	init_model_matrices(&model_matrices);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &instanceVBO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(shaderProgram);

	notcurses_stop(nc);
	glfwTerminate();

	return 0;
}
