/* main.c */

#include "../include/glad/glad.h"
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <cglm/cglm.h>
#include <GLFW/glfw3.h>
#include "render.h"
#include "generator.h"
#include "shader.h"
#include "render.h"
#include "texture.h"

#define BLOCKS_TYPES_AMOUNT 8

const struct block blocks[BLOCKS_TYPES_AMOUNT] = {{{255, 255, 255, 255}, '#'},
                                                 {{0  , 0  , 0  , 255}, '.'},
                                                 {{255, 0  , 255, 255}, '`'},
                                                 {{255, 255, 0  , 255}, '-'},
                                                 {{0  , 255, 255, 255}, '@'},
                                                 {{255,   0, 0  , 255}, '+'},
                                                 {{0  , 255, 0  , 255}, '&'},
                                                 {{0  , 0  , 255, 255}, '!'}};

void print_block(unsigned char *pixels, float pixels_depth){
	unsigned char color[3];
                           //far smooth  //distance
    int gray = (int)(255 * (2/exp(110 - 200*pixels_depth)));
    if (gray < 0) gray = 0;
    //intensive
    if (gray > 200) gray = 200;
    for( int i = 0; i < 3; i++) color[i] = pixels[i];
    //printf("%d", color[2]);
    for( int i = 0; i < BLOCKS_TYPES_AMOUNT; i++){
        if (color[0] == blocks[i].color[0] && color[1] == blocks[i].color[1] && color[2] == blocks[i].color[2]){
            printf("\033[1;48;2;%d;%d;%dm%c\033[0m", gray, gray, gray, blocks[i].ascii);
	}
    }
}

int main(void)
{
/*
 * Сократить до ~7-9 переменных
 */
	const int CHUNKS_AMOUNT = pow(2 * RENDER_DISTANCE - 1, 3); // (1) переписать в функцию
	GLFWwindow *window;
	unsigned int modelLoc, viewLoc, projectionLoc; // ???
	unsigned int VBO, VAO, EBO, instanceVBO;
	unsigned int shaderProgram, texture;
	struct time time;
	struct player player;
	struct chunk *loaded_chunks = init_chunks();
	vec3 player_pos, target;
	mat4 view = GLM_MAT4_IDENTITY_INIT;
	vec3 saved_chunk_pos = {0.0f, 0.0f, 0.0f};

	create_window(&window, FB_WIDTH, FB_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("Failed to initialize GLAD\n");
		return 0;
	}

	prepare_gl_environment(&VBO, &VAO, &EBO, &instanceVBO);
	prepare_texture(&texture);
	shaderProgram = prepare_shaders();
	player = create_player();

	modelLoc = glGetUniformLocation(shaderProgram, "model");
	viewLoc = glGetUniformLocation(shaderProgram, "view");
	projectionLoc = glGetUniformLocation(shaderProgram, "projection");

	time.frame_counter = 0;
	time.start = glfwGetTime();

	size_t pixel_count = FB_WIDTH * FB_HEIGHT;
    size_t data_size = pixel_count;
    unsigned char * pixels = (unsigned char*)calloc(data_size, sizeof(float));
    float * pixels_depth = (float*)calloc(FB_WIDTH * FB_HEIGHT , sizeof(float));

	while(!glfwWindowShouldClose(window)) {

		processInput(window, &player);

		glEnable(GL_DEPTH_TEST);

		//glClearColor(0.0f, 0.2f, 0.4f, 1.0f);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture"),
		                                                            0);
		player_pos[0] = player.x;
		player_pos[1] = player.y;
		player_pos[2] = player.z;
// >>> add ability to rotate camera
		glm_vec3_copy(player_pos, player.head.cameraPos);
		glm_vec3_add(player.head.cameraPos, (vec3){0,1,0}, target);
		glm_lookat(player.head.cameraPos, target, player.head.cameraUp,
		                                                         view);
// <<<

// >>> rewrite
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float*)view);
		mat4 projection;
		glm_perspective(glm_rad(90.0f), (float)FB_WIDTH /
		                  (FB_HEIGHT+150), 0.1f, 10000.0f, projection);
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE,
		                                           (float*)projection);

		if (is_chunk_changed(player.head.cameraPos, saved_chunk_pos)) {
			get_chunks(loaded_chunks, player.head.cameraPos);

			saved_chunk_pos[0] = player.head.cameraPos[0] > 0 ? (int)player.head.cameraPos[0] / CHUNK_SIZE :
		                                         (int)player.head.cameraPos[0] / CHUNK_SIZE - 1;
			saved_chunk_pos[1] = player.head.cameraPos[1] > 0 ? (int)player.head.cameraPos[1] / CHUNK_SIZE :
		                                         (int)player.head.cameraPos[1] / CHUNK_SIZE - 1;
			saved_chunk_pos[2] = player.head.cameraPos[2] > 0 ? (int)player.head.cameraPos[2] / CHUNK_SIZE :
		                                         (int)player.head.cameraPos[2] / CHUNK_SIZE - 1;
		}

		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
// <<<
// loaded_chunks, texture, VAO -> void
		for (int i = 0; i < CHUNKS_AMOUNT; i++) { // TODO (1)
			int num;
			mat4 *modelMatrices = draw_chunk(&(loaded_chunks[i]),
			                                                 &num);

			glBindTexture(GL_TEXTURE_2D, texture);
			glBindVertexArray(VAO);
			glBufferData(GL_ARRAY_BUFFER, num * sizeof(mat4),
		                                modelMatrices, GL_STATIC_DRAW);
			glDraw(num);
			free(modelMatrices);
		}

		glfwSwapBuffers(window);
		glReadBuffer(GL_BACK);

        glReadPixels(0, 0, FB_WIDTH, FB_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        glReadPixels(0, 0, FB_WIDTH, FB_HEIGHT, GL_DEPTH_COMPONENT, GL_FLOAT, pixels_depth);

	for (int y = FB_HEIGHT - 1; y >= 0; y--) {
            for (int x = 0; x < FB_WIDTH; x++) {
                int idx = (y * FB_WIDTH + x) * 4;
                float depth = pixels_depth[y * FB_WIDTH + x];
                print_block(pixels + idx, depth);
            }
            printf("\n");
        }

		glfwPollEvents();
		printf("\033[H");
		//calculate_fps(&time);
	}

	deinit_chunks(loaded_chunks);
	deinit_gl_environment(&VAO, &instanceVBO, &VBO, &EBO, shaderProgram);

	return 0;
}
