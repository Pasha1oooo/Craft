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
		chunk_pos->x--;
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

void processInput(GLFWwindow * window, struct player *player)
{
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, 1);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
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

int select_block(struct player player, struct chunk chunk, int *selected_block_x, int *selected_block_y, int *selected_block_z) {
    vec3 dir;
    glm_vec3_copy(player.head.cameraDirection, dir);
    glm_vec3_normalize(dir);

    vec3 step;
    glm_vec3_scale(dir, 0.1f, step);

    vec3 current;
    glm_vec3_copy(player.head.cameraPos, current);

    int local_x,local_y,local_z;
    for(float i = 0; i < 1; i+=1){
		glm_vec3_add(current, step, current);


        local_x = current[0] - chunk.pos->x * 16 ;  // x = chunk_x + local_x  x = dir_x + player_x
        local_y = current[1] - chunk.pos->y * 16 ;
        local_z = current[2] - chunk.pos->z * 16 ;
        if(chunk.chunk_data[((int)local_x) + ((int)local_y)*16 + ((int)local_z)*16*16] == '*'){
            *selected_block_x = chunk.pos->x * 16 + local_x;
            *selected_block_y = chunk.pos->y * 16 + local_y;
            *selected_block_z = chunk.pos->z * 16 + local_z;
            return 1;
        }
    }
    return -1;
}


