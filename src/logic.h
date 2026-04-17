#ifndef LOGIC_H
#define LOGIC_H

#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

char get_world_block(struct position block, struct chunk *chunks, int count);
void save_chunk_pos(vec3 *player_pos, struct position *chunk_pos);
int is_chunk_changed(vec3 player_pos, struct position *prev_chunk);
void calculate_fps(struct time *time);
void processInput(GLFWwindow * window, struct player *player, struct chunk **chunks, struct position selected_block);
struct camera create_camera(void);
struct player create_player(void);
int select_block(struct player player, struct chunk *chunks, struct position *selected_block);

#endif
