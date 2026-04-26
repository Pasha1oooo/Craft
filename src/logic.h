#ifndef LOGIC_H
#define LOGIC_H

#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

struct camera {
	vec3 cameraPos;
	vec3 cameraTarget;
	vec3 cameraDirection;
	vec3 up;
	vec3 cameraRight;
	vec3 cameraUp;
	float yaw;
	float pitch;
	float roll;
	float FOV;
};

struct time {
	float start;
	float end;
	float fps;
};

struct player {
	struct camera head;
	struct time time;
	vec3 position;
	vec3 prev_position;
	float speed;
	float rotation_speed;
};

struct camera create_camera(void);
struct player create_player(void);

void processInput(GLFWwindow * window, struct player *player,
                  struct chunk *chunks, struct position *selected_block);
void update_camera_direction(struct camera *cam);

void save_chunk_pos(vec3 *player_pos, struct position *chunk_pos);
int is_chunk_changed(vec3 player_pos, struct position *prev_chunk);

int select_block(struct player player, struct chunk *chunks,
                 struct position *selected_block);
char get_world_block(struct position block, struct chunk *chunks, int count);

void calculate_fps(struct time *time);

#endif
