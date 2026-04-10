#ifndef RENDER_H
#define RENDER_H

#include "../include/glad/glad.h"
#include <cglm/cglm.h>
#include <GLFW/glfw3.h>

enum render_parameters {
//	FB_WIDTH = 2560,
//	FB_HEIGHT = 1600,
	BLOCK_TYPES_AMOUNT = 8,
	FB_WIDTH = 600,
	FB_HEIGHT = 120,
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
};

struct player {
	struct camera head;
	float x;
	float y;
	float z;
	float speed;
	float rotation_speed;
};

struct time {
	float start;
	float end;
	int frame_counter;
	int fps;
};

void create_window(GLFWwindow ** window, int fb_width , int fb_height);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void prepare_gl_environment(unsigned int *VBO, unsigned int *VAO,
                            unsigned int *EBO, unsigned int * instanceVBO);

void render_chunks(struct chunk *chunks, unsigned int texture,
                                                             unsigned int VAO);
mat4 *draw_chunk(struct chunk *chunk);
void putpixel(unsigned char *pixels, float pixels_depth);

#endif
