#ifndef RENDER_H
#define RENDER_H

#include "../include/glad/glad.h"
#include <cglm/cglm.h>
#include <GLFW/glfw3.h>
#include "generator.h"

enum render_parameters {
	FB_WIDTH = 2560,
	FB_HEIGHT = 1000,
	BLOCK_TYPES_AMOUNT = 8,
	//FB_WIDTH = 510,
	//FB_HEIGHT = 90,
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

void create_window(GLFWwindow ** window, int fb_width , int fb_height);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void prepare_gl_environment(unsigned int *VBO, unsigned int *VAO, unsigned int *EBO, unsigned int *instanceVBO, unsigned int *VBO_highlight, unsigned int *VAO_highlight, unsigned int *EBO_highlight);
void render_chunks(struct chunk *chunks, unsigned int texture_stone,
                   unsigned int texture_ore, unsigned int VAO);

void draw_chunk(struct chunk *chunk, int *ID, mat4 **stone, mat4 **ore);
void putpixel(unsigned char *pixels, float pixels_depth);

#endif
