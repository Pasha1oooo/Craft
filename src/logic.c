/* logic.h */

#include "../include/glad/glad.h"
#include <cglm/cglm.h>
#include <GLFW/glfw3.h>
#include "generator.h"
#include "render.h"
#include "logic.h"

void save_chunk_pos(vec3 *player_pos, struct position *chunk_pos)
{
	chunk_pos->x = (int)(*player_pos)[0] / CHUNK_SIZE;
	chunk_pos->y = (int)(*player_pos)[1] / CHUNK_SIZE;
	chunk_pos->z = (int)(*player_pos)[2] / CHUNK_SIZE;

	if ((*player_pos)[0] < 0.0f)
		chunk_pos->x--;
	if ((*player_pos)[1] < 0.0f)
		chunk_pos->y--;
	if ((*player_pos)[2] < 0.0f)
		chunk_pos->z--;

	return;
}

int is_chunk_changed(vec3 player_pos, struct position *prev_chunk) //2ptr
{
	int is_chunk_changed[3];
	struct position current_chunk;
	int is_changed = 0;

	current_chunk.x = (int)player_pos[0] / CHUNK_SIZE;
	current_chunk.y = (int)player_pos[1] / CHUNK_SIZE;
	current_chunk.z = (int)player_pos[2] / CHUNK_SIZE;

	if (player_pos[0] > 0) {
		is_chunk_changed[0] = current_chunk.x != prev_chunk->x;
	} else {
		is_chunk_changed[0] = current_chunk.x - 1 != prev_chunk->x;
	}

	if (player_pos[1] > 0) {
		is_chunk_changed[1] = current_chunk.y != prev_chunk->y;
	} else {
		is_chunk_changed[1] = current_chunk.y - 1 != prev_chunk->y;
	}

	if (player_pos[2] > 0) {
		is_chunk_changed[2] = current_chunk.z != prev_chunk->z;
	} else {
		is_chunk_changed[2] = current_chunk.z - 1!= prev_chunk->z;
	}

	is_changed = is_chunk_changed[0] ||
	             is_chunk_changed[1] ||
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
}

void processInput(GLFWwindow * window, struct player *player,
                  struct chunk **chunks, struct position selected_block)
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

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		if (selected_block.x == -1)
			return;

		int chank_x = floor(selected_block.x / 16.0f);
		int chank_y = floor(selected_block.y / 16.0f);
		int chank_z = floor(selected_block.z / 16.0f);

		int local_x = selected_block.x - chank_x * 16;
		int local_y = selected_block.y - chank_y * 16;
		int local_z = selected_block.z - chank_z * 16;

		int j = 256 * local_z + 16 * local_y + local_x;

		for (int i = 0; i < pow(2 * RENDER_DISTANCE - 1, 3); i++) {
			if ((*chunks)[i].pos->x == chank_x &&
			    (*chunks)[i].pos->y == chank_y &&
			    (*chunks)[i].pos->z == chank_z) {

				(*chunks)[i].chunk_data[j] = ' ';
				save_chunk((*chunks)[i].chunk_data,
				           get_chunk_name((*chunks)[i].pos));

				break;
			}
		}
	}

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		player->head.pitch += 5.0f;

		if (player->head.pitch > 89.0f) player->head.pitch = 89.0f;
			update_camera_direction(&player->head);
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		player->head.pitch -= 5.0f;

		if (player->head.pitch < -89.0f) player->head.pitch = -89.0f;
			update_camera_direction(&player->head);
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		player->head.yaw-=5.0f;

		if (player->head.yaw>=360) player->head.yaw = 0;
			update_camera_direction(&player->head);
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		player->head.yaw+=5.0f;

		if(player->head.yaw<=-360) player->head.yaw = 0;
			update_camera_direction(&player->head);
	}
	//it will be roll later
	if (glfwGetKey(window, GLFW_KEY_PERIOD) == GLFW_PRESS){
		player->head.roll+=2;
		update_camera_direction(&player->head);
	}

	if (glfwGetKey(window, GLFW_KEY_COMMA) == GLFW_PRESS){
		player->head.roll-=2;
		update_camera_direction(&player->head);
	}

}

struct camera create_camera(void)
{
	struct camera camera;

	camera.pitch = 0.0f;
	camera.yaw   = 90.0f;
	camera.roll  = 0.0f;

	camera.cameraPos[0] = 0.0f;
	camera.cameraPos[1] = 0.0f;
	camera.cameraPos[2] = 0.0f;

	update_camera_direction(&camera);

	// поле должно называться cameraUp
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
	player.position[0] = 0;
	player.position[1] = 0;
	player.position[2] = 1;
	player.speed = 0.2f;
	player.rotation_speed = 0.2f;

	return player;
}

char get_world_block(struct position w, struct chunk *chunks, int count) {
	int cx = w.x >> 4, cy = w.y >> 4, cz = w.z >> 4;

	for (int i = 0; i < count; i++) {
		if (chunks[i].pos->x == cx &&
		    chunks[i].pos->y == cy &&
		    chunks[i].pos->z == cz) {

			int lx = w.x - (cx << 4);
			int ly = w.y - (cy << 4);
			int lz = w.z - (cz << 4);

			int j = lx + (ly << 4) + (lz << 8);
			return chunks[i].chunk_data[j];
		}
	}

	return 0;
}

int select_block(struct player player, struct chunk *chunks,
                 struct position *selected_block)
{
	int CHUNK_SIDE = pow(2 * RENDER_DISTANCE - 1, 3);
	struct position ray_pos;
	vec3 dir;

	glm_vec3_copy(player.head.cameraDirection, dir);
	glm_vec3_normalize(dir);

	ray_pos.x = (int)floorf(player.head.cameraPos[0]);
	ray_pos.y = (int)floorf(player.head.cameraPos[1]);
	ray_pos.z = (int)floorf(player.head.cameraPos[2]);

	int step_x = (dir[0] > 0) ? 1 : -1;
	int step_y = (dir[1] > 0) ? 1 : -1;
	int step_z = (dir[2] > 0) ? 1 : -1;

	float delta_x = (dir[0] != 0) ? fabsf(1.0f / dir[0]) : 1e10f;
	float delta_y = (dir[1] != 0) ? fabsf(1.0f / dir[1]) : 1e10f;
	float delta_z = (dir[2] != 0) ? fabsf(1.0f / dir[2]) : 1e10f;

	float tMaxX = dir[0] ? ((step_x > 0 ? ray_pos.x + 1 : ray_pos.x) -
	                       player.head.cameraPos[0]) / dir[0] : 1e10f;
	float tMaxY = dir[1] ? ((step_y > 0 ? ray_pos.y + 1 : ray_pos.y) -
	                       player.head.cameraPos[1]) / dir[1] : 1e10f;
	float tMaxZ = dir[2] ? ((step_z > 0 ? ray_pos.z + 1 : ray_pos.z) -
	                       player.head.cameraPos[2]) / dir[2] : 1e10f;

	for (int i = 0; i < 100; i++) {
		int is_block = get_world_block(ray_pos, chunks,
		                               CHUNK_SIDE) == '*' ||
		               get_world_block(ray_pos, chunks,
		                               CHUNK_SIDE) == '#';
		if (is_block) {

			selected_block->x = ray_pos.x;
			selected_block->y = ray_pos.y;
			selected_block->z = ray_pos.z;

			return 1;
		}

		if (tMaxX < tMaxY) {
			if (tMaxX < tMaxZ) {
				ray_pos.x += step_x;
				tMaxX += delta_x;
			} else {
				ray_pos.z += step_z;
				tMaxZ += delta_z;
			}
		} else {
			if (tMaxY < tMaxZ) {
				ray_pos.y += step_y;
				tMaxY += delta_y;
			} else {
				ray_pos.z += step_z;
				tMaxZ += delta_z;
			}
		}
	}

	return -1;
}
