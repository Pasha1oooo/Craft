#ifndef RENDER_H
#define RENDER_H

#include "../include/glad/glad.h"
#include <cglm/cglm.h>
#include <GLFW/glfw3.h>
#include "generator.h"
#include <notcurses/notcurses.h>

enum render_parameters {
//	FB_WIDTH = 2560,
//	FB_HEIGHT = 1000,
	BLOCK_TYPES_AMOUNT = 8,
	FB_WIDTH = 600,
	FB_HEIGHT = 120,
	FB_PIXELS_AMOUNT = FB_WIDTH * FB_HEIGHT,
	FPS_COUNT_TIME_INTERVAL = 3,
};

struct block {
	unsigned char color[4];
	char ascii;
};

void create_window(GLFWwindow ** window, int fb_width , int fb_height);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void prepare_gl_environment(unsigned int *VBO, unsigned int *VAO,
                            unsigned int *EBO, unsigned int *instanceVBO,
                            unsigned int *VBO_highlight,
                            unsigned int *VAO_highlight,
                            unsigned int *EBO_highlight);

void render_chunks(struct chunk *chunks, unsigned int texture_stone,
                   unsigned int texture_ore, unsigned int VAO);
void draw_chunk(struct chunk *chunk, mat4 **stone, mat4 **ore);

struct notcurses *notcurses_prepare();
void notcurses_putpixel(struct ncplane *n, unsigned char *pixel_rgba,
                        float depth, int i);
void notcurses_render_ascii(struct notcurses* nc, struct ncplane* n,
                            unsigned char *frame_buffer, float *depth_buffer);

void stat_render(struct notcurses* nc, struct ncplane* n, struct time time);

#endif
