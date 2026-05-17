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
	vec3 velocity;
	float speed;
	float jump_force;
	float rotation_speed;
	int on_ground;
};

struct camera create_camera(void);
struct player create_player(void);

int is_chunk_changed(vec3 player_vec_pos, struct position *prev_chunk);

void processInput(GLFWwindow * window, struct player *player,
                  struct chunk *chunks, struct position *selected_block);
void update_camera_direction(struct camera *cam);

int select_block(struct player player, struct chunk *chunks,
                 struct position *selected_block);
char get_world_block(struct position block, struct chunk *chunks);

void calculate_fps(struct time *time);

struct position calculate_blocks_area_size(void);
void process_collisions(struct chunk *chunks, struct player *player);
void calculate_hitbox_vpos(vec3 player_vpos, vec3 hitbox_vpos);
int is_colliding(vec3 dist);
int is_block_solid(struct chunk *chunks, struct position *gpos);

#endif
