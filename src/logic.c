/* logic.h */

#include "../include/glad/glad.h"
#include <cglm/cglm.h>
#include <GLFW/glfw3.h>
#include "generator.h"
#include "logic.h"

int is_chunk_changed(vec3 player_vec_pos, struct position *prev_chunk)
{
	struct position gpos = vec2pos(player_vec_pos);
	struct position current_chunk = gpos2chunkpos(&gpos);
	int is_chunk_changed[3];
	int is_changed = 0;

	is_chunk_changed[0] = current_chunk.x != prev_chunk->x;
	is_chunk_changed[1] = current_chunk.y != prev_chunk->y;
	is_chunk_changed[2] = current_chunk.z != prev_chunk->z;

	is_changed = is_chunk_changed[0] ||
	             is_chunk_changed[1] ||
	             is_chunk_changed[2];

	return is_changed;
}

void break_block(struct position *block_gpos, struct chunk *chunks)
{
	struct position chunk_pos = gpos2chunkpos(block_gpos);
	struct position block_lpos = gpos2lpos(block_gpos);
	int block_index = lpos2index(&block_lpos, CHUNK_SIZE);

	for (int i = 0; i < CHUNKS_AMOUNT; i++) {
		int is_chunk_selected = chunks[i].pos->x == chunk_pos.x &&
		                        chunks[i].pos->y == chunk_pos.y &&
		                        chunks[i].pos->z == chunk_pos.z;

		if (is_chunk_selected) {
			chunks[i].chunk_data[block_index] = AIR;
			save_chunk(chunks[i].chunk_data,
			           get_chunk_name(chunks[i].pos));

			break;
		}
	}
}

void processInput(GLFWwindow * window, struct player *player,
                  struct chunk *chunks, struct position *selected_block)
{
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, 1);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		player->position[0] += player->speed *
		                       player->head.cameraDirection[0];
		player->position[1] += player->speed *
		                       player->head.cameraDirection[1];
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		player->position[0] -= player->speed *
		                       player->head.cameraDirection[0];
		player->position[1] -= player->speed *
		                       player->head.cameraDirection[1];
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		player->position[0] -= player->speed *
		                       player->head.cameraDirection[1];
		player->position[1] += player->speed *
		                       player->head.cameraDirection[0];
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		player->position[0] += player->speed *
		                       player->head.cameraDirection[1];
		player->position[1] -= player->speed *
		                       player->head.cameraDirection[0];
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		player->position[2] += player->speed;

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		player->position[2] -= player->speed;

	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
		player->head.FOV = 10.0f;
	} else {
		player->head.FOV = 90.0f;
	}

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		break_block(selected_block, chunks);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		player->head.pitch += 5.0f;

		if (player->head.pitch > 89.0f)
			player->head.pitch = 89.0f;

		update_camera_direction(&player->head);
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		player->head.pitch -= 5.0f;

		if (player->head.pitch < -89.0f)
			player->head.pitch = -89.0f;

		update_camera_direction(&player->head);
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
		player->head.yaw -= 5.0f;

		if(player->head.yaw >= 360)
			player->head.yaw = 0;

		update_camera_direction(&player->head);
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
		player->head.yaw += 5.0f;

		if(player->head.yaw <= -360)
			player->head.yaw = 0;

		update_camera_direction(&player->head);
	}

	if (glfwGetKey(window, GLFW_KEY_PERIOD) == GLFW_PRESS){
		player->head.roll+=2;
		update_camera_direction(&player->head);
	}

	if (glfwGetKey(window, GLFW_KEY_COMMA) == GLFW_PRESS){
		player->head.roll-=2;
		update_camera_direction(&player->head);
	}
}

void update_camera_direction(struct camera *cam)
{
	vec3 world_up = {0.0f, 0.0f, 1.0f};

	cam->cameraDirection[0] = cos(glm_rad(cam->pitch)) *
	                          cos(glm_rad(cam->yaw));
	cam->cameraDirection[1] = cos(glm_rad(cam->pitch)) *
	                          sin(glm_rad(cam->yaw));
	cam->cameraDirection[2] = sin(glm_rad(cam->pitch));

	glm_vec3_rotate(world_up, glm_rad(cam->roll), cam->cameraDirection);
	glm_vec3_copy(world_up, cam->cameraUp);

	glm_normalize(cam->cameraDirection);
	glm_normalize(cam->cameraUp);
	glm_normalize(cam->cameraRight);

	glm_cross(cam->cameraDirection, cam->cameraUp, cam->cameraRight);

	return;
}

struct camera create_camera(void)
{
	struct camera camera;

	camera.pitch = 0.0f;
	camera.yaw   = 90.0f;
	camera.roll  = 0.0f;
	camera.FOV   = 90.0f;

	camera.cameraPos[0] = 0.0f;
	camera.cameraPos[1] = 0.0f;
	camera.cameraPos[2] = 0.5f;

	update_camera_direction(&camera);

	glm_vec3_copy((vec3){0.0f, 0.0f, 1.0f}, camera.cameraUp);
	glm_vec3_cross(camera.cameraUp,
	               camera.cameraDirection,
	               camera.cameraRight);

	return camera;
}

struct player create_player(void)
{
	struct player player;

	player.head = create_camera();
	player.position[0] = 0.0f;
	player.position[1] = 0.0f;
	player.position[2] = 3.0f;
	player.speed = 0.2f;
	player.rotation_speed = 0.2f;

	return player;
}

char get_world_block(struct position gpos, struct chunk *chunks)
{
	struct position chunk_pos = gpos2chunkpos(&gpos);
	struct position lpos = gpos2lpos(&gpos);
	int index = lpos2index(&lpos, CHUNK_SIZE);
	int is_equal_pos;

	for (int i = 0; i < CHUNKS_AMOUNT; i++) {
		is_equal_pos = chunks[i].pos->x == chunk_pos.x &&
		               chunks[i].pos->y == chunk_pos.y &&
		               chunks[i].pos->z == chunk_pos.z;

		if (is_equal_pos)
			return chunks[i].chunk_data[index];
	}

	return 0;
}

int select_block(struct player player, struct chunk *chunks,
                 struct position *selected_block)
{
	struct position ray_pos;
	struct position step;
	vec3 dir;
	vec3 delta;
	vec3 t_max;

	glm_vec3_copy(player.head.cameraDirection, dir);
	glm_vec3_normalize(dir);

	ray_pos = vec2pos(player.head.cameraPos);

	step.x = (dir[0] > 0.0f) ? 1 : -1;
	step.y = (dir[1] > 0.0f) ? 1 : -1;
	step.z = (dir[2] > 0.0f) ? 1 : -1;

	if (dir[0] != 0.0f) {
		delta[0] = fabsf(1.0f / dir[0]);
		t_max[0] = ray_pos.x;
		t_max[0] += step.x > 0 ? 1.0f : 0.0f;
		t_max[0] = (t_max[0] - player.head.cameraPos[0]) / dir[0];
	} else {
		delta[0] = 1e10f;
		t_max[0] = 1e10f;
	}

	if (dir[1] != 0.0f) {
		delta[1] = fabsf(1.0f / dir[1]);
		t_max[1] = ray_pos.y;
		t_max[1] += step.y > 0 ? 1.0f : 0.0f;
		t_max[1] = (t_max[1] - player.head.cameraPos[1]) / dir[1];
	} else {
		delta[1] = 1e10f;
		t_max[1] = 1e10f;
	}

	if (dir[2] != 0.0f) {
		delta[2] = fabsf(1.0f / dir[2]);
		t_max[2] = ray_pos.z;
		t_max[2] += step.z > 0 ? 1.0f : 0.0f;
		t_max[2] = (t_max[2] - player.head.cameraPos[2]) / dir[2];
	} else {
		delta[2] = 1e10f;
		t_max[2] = 1e10f;
	}

	for (int i = 0; i < 100; i++) {
		int is_block = get_world_block(ray_pos, chunks) == STONE ||
		               get_world_block(ray_pos, chunks) == ORE;
		if (is_block) {
			selected_block->x = ray_pos.x;
			selected_block->y = ray_pos.y;
			selected_block->z = ray_pos.z;

			return 1;
		}

		if (t_max[0] < t_max[1]) {
			if (t_max[0] < t_max[2]) {
				ray_pos.x += step.x;
				t_max[0] += delta[0];
			} else {
				ray_pos.z += step.z;
				t_max[2] += delta[2]; }
		} else {
			if (t_max[1] < t_max[2]) {
				ray_pos.y += step.y;
				t_max[1] += delta[1];
			} else {
				ray_pos.z += step.z;
				t_max[2] += delta[2];
			}
		}
	}

	return 0;
}

void calculate_fps(struct time *time)
{
	float delta_time;

	time->end = glfwGetTime();

	delta_time = time->end - time->start;
	time->fps = 1.0f / delta_time;

	time->start = glfwGetTime();
}
