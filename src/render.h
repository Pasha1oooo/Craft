#ifndef RENDER_H
#define RENDER_H

#include <cglm/cglm.h>
#include <GLFW/glfw3.h>

enum render_parameters {
	FB_WIDTH = 2560,
	FB_HEIGHT = 1600,
	FPS_COUNT_TIME_INTERVAL = 3
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

int is_chunk_changed(vec3 player_pos, vec3 prev_chunk);
void calculate_fps(struct time *time);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow * window, struct player *player);
void create_window(GLFWwindow ** window, int fb_width , int fb_height);
void prepare_gl_environment(unsigned int *VBO, unsigned int *VAO,
                            unsigned int *EBO, unsigned int * instanceVBO);
void glDraw(int num); /* TODO REMAKE*/
struct camera create_camera(void);
struct player create_player(void);
void deinit_gl_environment(unsigned int *VAO, unsigned int *instanceVBO,
              unsigned int *VBO, unsigned int *EBO, unsigned int shaderProgram);

#endif
