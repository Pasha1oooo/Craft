#ifndef RENDER_H
#define RENDER_H

#include "../include/glad/glad.h"
#include <cglm/cglm.h>
#include <GLFW/glfw3.h>
#include "generator.h"
#include <notcurses/notcurses.h>
#include <locale.h>
#include <wchar.h>

enum render_parameters {
	//FB_WIDTH = 2560,
	//FB_HEIGHT = 1000,
	BLOCK_TYPES_AMOUNT = 8,
	FB_WIDTH = 510,
	FB_HEIGHT = 90,
	FPS_COUNT_TIME_INTERVAL = 3,
};

struct block {
	unsigned char color[4];
	char ascii;
};

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

struct player {
	struct camera head;
	vec3 position;
	float speed;
	float rotation_speed;
};

struct time {
	float start;
	float end;
	int frame_counter;
	int fps;
};

struct block_edges {
    mat4 *up_edge;
    mat4 *down_edge;
    mat4 *left_edge;
    mat4 *right_edge;
    mat4 *front_edge;
    mat4 *back_edge;
};

struct count_edges {
	int count_up;
    int count_down;
    int count_left;
    int count_right;
    int count_front;
    int count_back;
};

void create_window(GLFWwindow ** window, int fb_width , int fb_height);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void prepare_gl_environment(unsigned int *VBO, unsigned int *VAO, unsigned int *EBO, unsigned int *instanceVBO, unsigned int *VBO_highlight, unsigned int *VAO_highlight, unsigned int *EBO_highlight);
void render_chunks(struct chunk *chunks, unsigned int texture_stone, unsigned int texture_ore, unsigned int VAO);

void draw_chunk(struct chunk *chunk,  struct block_edges *stone, struct block_edges *ore);
void putpixel(unsigned char *pixels, float pixels_depth);
struct notcurses* notcurses_prepare();
void notcurses_render_ascii(struct notcurses* nc, struct ncplane* n, unsigned char *frame_buffer, float *depth_buffer);
void stat_render(struct notcurses* nc, struct ncplane* n, struct time time);
struct count_edges check_edges(struct chunk *chunk, int i);

#endif
