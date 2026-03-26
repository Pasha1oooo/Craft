#ifndef RENDER_H
#define RENDER_H

#include <cglm/cglm.h>
#include <GLFW/glfw3.h>

typedef struct block{
	unsigned char color[4];
	char ascii;
} block;

typedef struct camera{
	vec3 cameraPos;
	vec3 cameraTarget;
	vec3 cameraDirection;
	vec3 up;
	vec3 cameraRight;
	vec3 cameraUp;
} camera;

typedef struct player{
	camera head;
	float x;
	float y;
	float z;
	float speed;
} player;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow * window);
void create_window(GLFWwindow ** window, int fb_width , int fb_height);
camera create_camera(void);
player create_player(void);

/*
void prepare_gl_environment(unsigned int *VBO, unsigned int *VAO,
                            unsigned int *EBO, unsigned int * instanceVBO);

*/
#endif
