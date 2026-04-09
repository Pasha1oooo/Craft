/* render.c */
/*
#include "../include/glad/glad.h"
#include "render.h"
#include <stdio.h>
#include <cglm/cglm.h>
#include <GLFW/glfw3.h>
#include "generator.h"
*/



#include "../include/glad/glad.h"
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <cglm/cglm.h>
#include <GLFW/glfw3.h>
#include "generator.h"
#include "render.h"
#include "render.h"

// Separate it into parts to optimize render
float vertices[] = {
/* coordinates                    color               texture */
 0.5f,  0.5f,  0.5f,        0.0f, 0.0f, 1.0f,        1.0f, 1.0f,
-0.5f,  0.5f,  0.5f,        0.0f, 0.0f, 1.0f,        0.0f, 1.0f,
 0.5f, -0.5f,  0.5f,        0.0f, 0.0f, 1.0f,        1.0f, 0.0f,
-0.5f, -0.5f,  0.5f,        0.0f, 0.0f, 1.0f,        0.0f, 0.0f,

 0.5f,  0.5f,  0.5f,        0.0f, 1.0f, 0.0f,        1.0f, 1.0f,
 0.5f, -0.5f,  0.5f,        0.0f, 1.0f, 0.0f,        0.0f, 1.0f,
 0.5f,  0.5f, -0.5f,        0.0f, 1.0f, 0.0f,        1.0f, 0.0f,
 0.5f, -0.5f, -0.5f,        0.0f, 1.0f, 0.0f,        0.0f, 0.0f,

 0.5f,  0.5f,  0.5f,        1.0f, 0.0f, 1.0f,        1.0f, 1.0f,
-0.5f,  0.5f,  0.5f,        1.0f, 0.0f, 1.0f,        0.0f, 1.0f,
 0.5f,  0.5f, -0.5f,        1.0f, 0.0f, 1.0f,        1.0f, 0.0f,
-0.5f,  0.5f, -0.5f,        1.0f, 0.0f, 1.0f,        0.0f, 0.0f,

 0.5f,  0.5f, -0.5f,        0.0f, 1.0f, 1.0f,        1.0f, 1.0f,
-0.5f,  0.5f, -0.5f,        0.0f, 1.0f, 1.0f,        0.0f, 1.0f,
 0.5f, -0.5f, -0.5f,        0.0f, 1.0f, 1.0f,        1.0f, 0.0f,
-0.5f, -0.5f, -0.5f,        0.0f, 1.0f, 1.0f,        0.0f, 0.0f,

-0.5f,  0.5f,  0.5f,        1.0f, 0.0f, 1.0f,        1.0f, 1.0f,
-0.5f, -0.5f,  0.5f,        1.0f, 0.0f, 1.0f,        0.0f, 1.0f,
-0.5f,  0.5f, -0.5f,        1.0f, 0.0f, 1.0f,        1.0f, 0.0f,
-0.5f, -0.5f, -0.5f,        1.0f, 0.0f, 1.0f,        0.0f, 0.0f,

 0.5f, -0.5f,  0.5f,        1.0f, 1.0f, 0.0f,        1.0f, 1.0f,
-0.5f, -0.5f,  0.5f,        1.0f, 1.0f, 0.0f,        0.0f, 1.0f,
 0.5f, -0.5f, -0.5f,        1.0f, 1.0f, 0.0f,        1.0f, 0.0f,
-0.5f, -0.5f, -0.5f,        1.0f, 1.0f, 0.0f,        0.0f, 0.0f};

// figure out what to do with it
unsigned int indices[] = {
	0, 1, 2,
	1, 2, 3,

	4, 5, 6,
	5, 6, 7,

	8, 9, 10,
	9, 10, 11,

	12, 13, 14,
	13, 14, 15,

	16, 17, 18,
	17, 18, 19,

	20, 21, 22,
	21, 22, 23};

mat4 *draw_chunk(struct chunk *chunk, int *num)
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

int is_chunk_changed(vec3 player_pos, vec3 prev_chunk) //2ptr
{
	int is_chunk_changed[3];
	int current_chunk[3];
	int is_changed = 0;

	current_chunk[0] = (int)player_pos[0] / CHUNK_SIZE;
	current_chunk[1] = (int)player_pos[1] / CHUNK_SIZE;
	current_chunk[2] = (int)player_pos[2] / CHUNK_SIZE;

	if (player_pos[0] > 0) {
		is_chunk_changed[0] = current_chunk[0] != prev_chunk[0];
	} else {
		is_chunk_changed[0] = current_chunk[0] - 1 != prev_chunk[0];
	}

	if (player_pos[1] > 0) {
		is_chunk_changed[1] = current_chunk[1] != prev_chunk[1];
	} else {
		is_chunk_changed[1] = current_chunk[1] - 1 != prev_chunk[1];
	}

	if (player_pos[2] > 0) {
		is_chunk_changed[2] = current_chunk[2] != prev_chunk[2];
	} else {
		is_chunk_changed[2] = current_chunk[2] - 1!= prev_chunk[2];
	}

	is_changed = is_chunk_changed[0] || is_chunk_changed[1] ||
	                                                   is_chunk_changed[2];
	return is_changed;
}

void calculate_fps(struct time *time)
{
	float delta_time;

	time->end = glfwGetTime();
	delta_time = time->end - time->start;
	time->frame_counter += 1;

	if (delta_time > FPS_COUNT_TIME_INTERVAL) {
		time->fps = (int)((float)time->frame_counter / delta_time);
		time->start = glfwGetTime();
		time->frame_counter = 0;

		printf("\033[H");
		printf("\nFPS: %d \n", time->fps);
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow * window, struct player *player) // add other hotkeys here
{
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, 1);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // into move_player
		player->y += player->speed;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		player->y -= player->speed;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		player->x += player->speed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		player->x -= player->speed;

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		player->z -= player->speed;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		player->z += player->speed;

	return;
}

void create_window(GLFWwindow ** window, int fb_width , int fb_height)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	*window = glfwCreateWindow(fb_width, fb_height, "Game Window",
	                                                           NULL, NULL);
	if (*window == NULL) {
		printf("Failed to create a window\n");
		glfwTerminate();
	}

	glfwMakeContextCurrent(*window);

	return;
}

struct camera create_camera(void) // TODO
{
	struct camera camera;

	glm_vec3_copy((vec3){0.0f, 10.0f, 10.0f}, camera.cameraPos);
	glm_vec3_copy((vec3){0.0f, -1.0f, 0.0f}, camera.cameraTarget);
	glm_vec3_copy((vec3){0.0f, 0.0f, 0.0f}, camera.cameraDirection);
	glm_vec3_copy((vec3){0.0f, 1.0f, 0.0f}, camera.up);
	glm_vec3_sub(camera.cameraPos, camera.cameraTarget,
	                                               camera.cameraDirection);
	glm_vec3_normalize(camera.cameraDirection);
	glm_cross(camera.up, camera.cameraDirection, camera.cameraRight);
	glm_normalize(camera.cameraRight);
	glm_cross(camera.cameraDirection, camera.cameraRight, camera.cameraUp);

	return camera;
}

struct player create_player(void)
{
	struct player player;

	player.head = create_camera();
	player.x = 0;
	player.y = 0;
	player.z = 0;
	player.speed = 0.2f;
	player.rotation_speed = 0.2f;

	return player;
}

void prepare_gl_environment(unsigned int *VBO, unsigned int *VAO,
                            unsigned int *EBO, unsigned int * instanceVBO) // TODO
{
	glGenVertexArrays(1, VAO);
	glGenBuffers(1, EBO);
	glBindVertexArray(*VAO);

	glGenBuffers(1, instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, *instanceVBO);

	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(mat4),
	                                                             (void*)0);
	glEnableVertexAttribArray(2);
	glVertexAttribDivisor(2, 1);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(mat4),
	                                           (void*)(4 * sizeof(float)));
	glEnableVertexAttribArray(3);
	glVertexAttribDivisor(3, 1);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(mat4),
	                                           (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(4);
	glVertexAttribDivisor(4, 1);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(mat4),
	                                          (void*)(12 * sizeof(float)));
	glEnableVertexAttribArray(5);
	glVertexAttribDivisor(5, 1);

	glGenBuffers(1, VBO);
	glBindBuffer(GL_ARRAY_BUFFER, *VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
	                                                       GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
	                                                       GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float),
	                                                             (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float),
	                                             (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float),
	                                           (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(6);

}

void glDraw(int num) // TODO
{
	glDrawElementsInstanced(GL_TRIANGLES, sizeof(indices)/
	                          sizeof(indices[0]), GL_UNSIGNED_INT, 0, num);
}

void deinit_gl_environment(unsigned int *VAO, unsigned int *instanceVBO,
              unsigned int *VBO, unsigned int *EBO, unsigned int shaderProgram)
{
	glDeleteVertexArrays(1, VAO);
	glDeleteBuffers(1, instanceVBO);
	glDeleteBuffers(1, VBO);
	glDeleteBuffers(1, EBO);
	glDeleteProgram(shaderProgram);

	glfwTerminate();
}
