/* render.c */

#include "../include/glad/glad.h"
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <locale.h>
#include <cglm/cglm.h>
#include <GLFW/glfw3.h>
#include "generator.h"
#include "logic.h"
#include "render.h"

const struct block blocks[BLOCK_TYPES_AMOUNT] = {
	{{255, 255, 255, 255}, ORE  },
	{{0  , 0  , 0  , 255}, AIR  },
	{{255, 0  , 255, 255}, '`'  },
	{{255, 255, 0  , 255}, '-'  },
	{{0  , 255, 255, 255}, '@'  },
	{{255,   0, 0  , 255}, STONE},
	{{0  , 255, 0  , 255}, ORE  },
	{{0  , 0  , 255, 255}, '.'  }
};

float vertices[] = {
	/* coordinates                    color               texture */
	 0.5f,  0.5f,  0.5f,        0.0f, 0.0f, 1.0f,        1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,        0.0f, 0.0f, 1.0f,        0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,        0.0f, 0.0f, 1.0f,        1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,        0.0f, 0.0f, 1.0f,        0.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,        0.0f, 1.0f, 0.0f,        1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,        0.0f, 1.0f, 0.0f,        0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,        0.0f, 1.0f, 0.0f,        1.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,        0.0f, 1.0f, 0.0f,        0.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,        1.0f, 0.0f, 1.0f,        1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,        1.0f, 0.0f, 1.0f,        0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,        1.0f, 0.0f, 1.0f,        1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,        1.0f, 0.0f, 1.0f,        0.0f, 0.0f,

	 0.5f,  0.5f, -0.5f,        0.0f, 1.0f, 1.0f,        1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,        0.0f, 1.0f, 1.0f,        0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,        0.0f, 1.0f, 1.0f,        1.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,        0.0f, 1.0f, 1.0f,        0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,        1.0f, 0.0f, 1.0f,        1.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,        1.0f, 0.0f, 1.0f,        0.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,        1.0f, 0.0f, 1.0f,        1.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,        1.0f, 0.0f, 1.0f,        0.0f, 0.0f,

	 0.5f, -0.5f,  0.5f,        1.0f, 1.0f, 0.0f,        1.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,        1.0f, 1.0f, 0.0f,        0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,        1.0f, 1.0f, 0.0f,        1.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,        1.0f, 1.0f, 0.0f,        0.0f, 0.0f
};

unsigned int indices[] = {
	0, 1, 2,
	1, 2, 3,

	4, 5, 6,
	5, 6, 7,

	8, 9, 10,
	9, 10, 11,

	12, 13, 14,
	13, 14, 15,

	16, 17, 18,
	17, 18, 19,

	20, 21, 22,
	21, 22, 23
};

float vertices2[] = {
	// Передняя грань
	 0.55f,  0.55f,  0.55f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
	-0.55f,  0.55f,  0.55f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,
	 0.55f, -0.55f,  0.55f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,
	-0.55f, -0.55f,  0.55f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,

	// Правая грань
	 0.55f,  0.55f,  0.55f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
	 0.55f, -0.55f,  0.55f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
	 0.55f,  0.55f, -0.55f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
	 0.55f, -0.55f, -0.55f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,

	// Верхняя грань
	 0.55f,  0.55f,  0.55f,   1.0f, 0.0f, 1.0f,   1.0f, 1.0f,
	-0.55f,  0.55f,  0.55f,   1.0f, 0.0f, 1.0f,   0.0f, 1.0f,
	 0.55f,  0.55f, -0.55f,   1.0f, 0.0f, 1.0f,   1.0f, 0.0f,
	-0.55f,  0.55f, -0.55f,   1.0f, 0.0f, 1.0f,   0.0f, 0.0f,

	// Задняя грань
	 0.55f,  0.55f, -0.55f,   0.0f, 1.0f, 1.0f,   1.0f, 1.0f,
	-0.55f,  0.55f, -0.55f,   0.0f, 1.0f, 1.0f,   0.0f, 1.0f,
	 0.55f, -0.55f, -0.55f,   0.0f, 1.0f, 1.0f,   1.0f, 0.0f,
	-0.55f, -0.55f, -0.55f,   0.0f, 1.0f, 1.0f,   0.0f, 0.0f,

	// Левая грань
	-0.55f,  0.55f,  0.55f,   1.0f, 0.0f, 1.0f,   1.0f, 1.0f,
	-0.55f, -0.55f,  0.55f,   1.0f, 0.0f, 1.0f,   0.0f, 1.0f,
	-0.55f,  0.55f, -0.55f,   1.0f, 0.0f, 1.0f,   1.0f, 0.0f,
	-0.55f, -0.55f, -0.55f,   1.0f, 0.0f, 1.0f,   0.0f, 0.0f,

	// Нижняя грань
	 0.55f, -0.55f,  0.55f,   1.0f, 1.0f, 0.0f,   1.0f, 1.0f,
	-0.55f, -0.55f,  0.55f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,
	 0.55f, -0.55f, -0.55f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f,
	-0.55f, -0.55f, -0.55f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,
};

unsigned int indices2[] = {
	0,  1,   2,  1,  2,  3,
	4,  5,   6,  5,  6,  7,
	8,  9,  10,  9, 10, 11,
	12, 13, 14, 13, 14, 15,
	16, 17, 18, 17, 18, 19,
	20, 21, 22, 21, 22, 23,
};

void create_window(GLFWwindow **window, int fb_width , int fb_height)
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	*window = glfwCreateWindow(fb_width, fb_height, "Mine", NULL, NULL);

	if (*window == NULL) {
		printf("Failed to create a window\n");
		glfwTerminate();
	}

	glfwMakeContextCurrent(*window);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void prepare_gl_environment(unsigned int *VBO, unsigned int *VAO,
                            unsigned int *EBO, unsigned int *instanceVBO,
			    unsigned int *VBO_highlight,
			    unsigned int *VAO_highlight,
			    unsigned int *EBO_highlight)
{
	glGenVertexArrays(1, VAO);
	glGenBuffers(1, EBO);
	glBindVertexArray(*VAO);

	glGenBuffers(1, instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, *instanceVBO);

	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE,
	                      sizeof(mat4), (void *)0);
	glEnableVertexAttribArray(2);
	glVertexAttribDivisor(2, 1);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE,
	                      sizeof(mat4), (void *)(4 * sizeof(float)));
	glEnableVertexAttribArray(3);
	glVertexAttribDivisor(3, 1);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE,
	                      sizeof(mat4), (void *)(8 * sizeof(float)));
	glEnableVertexAttribArray(4);
	glVertexAttribDivisor(4, 1);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE,
	                      sizeof(mat4), (void *)(12 * sizeof(float)));
	glEnableVertexAttribArray(5);
	glVertexAttribDivisor(5, 1);

	glGenBuffers(1, VBO);
	glBindBuffer(GL_ARRAY_BUFFER, *VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),
	             vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices),
	             indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
	                      8 * sizeof(float), (void *)0);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
	                      8 * sizeof(float), (void *)(3 * sizeof(float)));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE,
	                      8 * sizeof(float),
	                      (void *)(6 * sizeof(float)));

	glEnableVertexAttribArray(6);
	glGenVertexArrays(1, VAO_highlight);

	glGenBuffers(1, VBO_highlight);
	glGenBuffers(1, EBO_highlight);

	glBindVertexArray(*VAO_highlight);
	glBindBuffer(GL_ARRAY_BUFFER, *VBO_highlight);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2),
	             vertices2, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO_highlight);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices2),
	             indices2, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
	                      8 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
	                      8 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE,
	                      8 * sizeof(float), (void *)(6 * sizeof(float)));
	glEnableVertexAttribArray(6);
}

void render_chunks(struct chunk *chunks, unsigned int texture_stone,
                   unsigned int texture_ore, unsigned int VAO)
{
	for (int i = 0; i < CHUNKS_AMOUNT; i++) {
		mat4 *modelMatrices_stone = (mat4 *)calloc(BLOCKS_IN_CHUNK,
		                                           sizeof(mat4));
		mat4 *modelMatrices_ore = (mat4 *)calloc(BLOCKS_IN_CHUNK,
		                                         sizeof(mat4));
		draw_chunk(&(chunks[i]),
		           &modelMatrices_stone,
		           &modelMatrices_ore);

		glBindVertexArray(VAO);
		glBindTexture(GL_TEXTURE_2D, texture_stone);
		glBufferData(GL_ARRAY_BUFFER,
		             BLOCKS_IN_CHUNK * sizeof(mat4),
		             modelMatrices_stone, GL_STATIC_DRAW);

		glDrawElementsInstanced(GL_TRIANGLES,
		                        sizeof(indices) / sizeof(indices[0]),
		                        GL_UNSIGNED_INT, 0, BLOCKS_IN_CHUNK);

		glBindTexture(GL_TEXTURE_2D, texture_ore);
		glBufferData(GL_ARRAY_BUFFER,
		             BLOCKS_IN_CHUNK * sizeof(mat4),
		             modelMatrices_ore, GL_STATIC_DRAW);

		glDrawElementsInstanced(GL_TRIANGLES,
		                        sizeof(indices) / sizeof(indices[0]),
		                        GL_UNSIGNED_INT, 0, BLOCKS_IN_CHUNK);

		free(modelMatrices_ore);
		free(modelMatrices_stone);
	}
}

void draw_chunk(struct chunk *chunk, mat4 **stone, mat4 **ore)
{
	for (int i = 0; i < BLOCKS_IN_CHUNK; i++) {
		mat4 model = GLM_MAT4_IDENTITY_INIT;
		struct position lpos = index2lpos(i, CHUNK_SIZE);
		struct position gpos = lpos2gpos(&lpos, chunk->pos);
		vec3 vec_gpos;

		pos2vec(&gpos, vec_gpos);
		glm_translate(model, vec_gpos);

		switch(chunk->chunk_data[i]) {
			case STONE:
				glm_mat4_copy(model, (*stone)[i]);
				break;
			case ORE:
				glm_mat4_copy(model, (*ore)[i]);
				break;
		}
	}
}

struct notcurses *notcurses_prepare(void)
{
	setlocale(LC_ALL, "");

	struct notcurses* nc = notcurses_init(NULL, stdout);

	return nc;
}

void notcurses_putpixel(struct ncplane *n, unsigned char *pixel_rgba,
                        float depth, int i)
{
	int background_light = (int)(255 * (1 / exp(150 - 150 * depth)));
	int is_same_color[3];
	int x = i % FB_WIDTH;
	int y = i / FB_WIDTH;

	if (background_light < 0)
		background_light = 0;
	if (background_light > 200)
		background_light = 200;

	for (int i = 0; i < BLOCK_TYPES_AMOUNT; i++) {
		for (int j = 0; j < 3; j++) {
			is_same_color[j] = pixel_rgba[j] ==
			                   blocks[i].color[j];
		}

		int is_same = is_same_color[0] &&
		              is_same_color[1] &&
		              is_same_color[2];

		if (is_same) {
			ncplane_set_bg_rgb8(n, background_light,
			                       background_light,
			                       background_light);

			ncplane_putchar_yx(n, FB_HEIGHT - y, x,
			                   blocks[i].ascii);
		}
	}
}

void notcurses_render_ascii(struct notcurses* nc, struct ncplane* n,
                            unsigned char *frame_buffer, float *depth_buffer)
{
	for (int i = 0; i < FB_PIXELS_AMOUNT; i++) {
		float depth = depth_buffer[i];
		unsigned char *pixel_rgba = frame_buffer + 4 * i;

		notcurses_putpixel(n, pixel_rgba, depth, i);
	}
}

void stat_render(struct notcurses* nc, struct ncplane* n, struct time time) {
	struct ncplane_options opts = {
		.y = 0,
		.x = FB_WIDTH - 80,
		.rows = 20,
		.cols = 80,
		.userptr = NULL,
		.name = "child",
		.resizecb = NULL,
		.flags = 0,
	};

	struct ncplane* child_plane = ncplane_create(n, &opts);

	for (int y = 0; y < opts.rows; y++) {
		for (int x = 0; x < opts.cols; x++) {
			ncplane_set_bg_rgb8(child_plane, 0, 0, 0);
			ncplane_putchar_yx(child_plane, y, x, ' ');
		}
	}

//	ncplane_printf_yx(child_plane, 0, 0, "FPS: %d", calculate_fps(&time));

	return;
}
