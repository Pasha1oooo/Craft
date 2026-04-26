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
	{{255, 255, 255, 255}, '#'},
	{{0  , 0  , 0  , 255}, ' '},
	{{255, 0  , 255, 255}, '`'},
	{{255, 255, 0  , 255}, '-'},
	{{0  , 255, 255, 255}, '@'},
	{{255,   0, 0  , 255}, '*'},
	{{0  , 255, 0  , 255}, '#'},
	{{0  , 0  , 255, 255}, '.'}
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

	return;
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
			8 * sizeof(float), (void *)(6 * sizeof(float)));

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

	return;
}

void render_chunks(struct chunk *chunks, unsigned int texture_stone,
                   unsigned int texture_ore, unsigned int VAO)
{
	const int CHUNKS_AMOUNT = pow(2 * RENDER_DISTANCE - 1, 3);
	const int BLOCKS_AMOUNT = pow(CHUNK_SIZE, 3);
	int block_amount = pow(CHUNK_SIZE, 3);
	mat4 *modelMatrices;
	int ID = 0;

	for (int i = 0; i < CHUNKS_AMOUNT; i++) {
		mat4 *modelMatrices_stone = (mat4 *)calloc(BLOCKS_AMOUNT,
		                                           sizeof(mat4));
		mat4 *modelMatrices_ore = (mat4 *)calloc(BLOCKS_AMOUNT,
		                                         sizeof(mat4));
		draw_chunk(&(chunks[i]), &ID,
		           &modelMatrices_stone,
		           &modelMatrices_ore);

		glBindVertexArray(VAO);
		glBindTexture(GL_TEXTURE_2D, texture_stone);
		glBufferData(GL_ARRAY_BUFFER,
		             block_amount * sizeof(mat4),
		             modelMatrices_stone, GL_STATIC_DRAW);

		glDrawElementsInstanced(GL_TRIANGLES,
		                        sizeof(indices) / sizeof(indices[0]),
		                        GL_UNSIGNED_INT, 0, block_amount);

		glBindTexture(GL_TEXTURE_2D, texture_ore);
		glBufferData(GL_ARRAY_BUFFER,
		             block_amount * sizeof(mat4),
		             modelMatrices_ore, GL_STATIC_DRAW);

		glDrawElementsInstanced(GL_TRIANGLES,
		                        sizeof(indices) / sizeof(indices[0]),
		                        GL_UNSIGNED_INT, 0, block_amount);

		free(modelMatrices_ore);
		free(modelMatrices_stone);
	}

	return;
}

void draw_chunk(struct chunk *chunk, int *ID, mat4 **stone, mat4 **ore)
{
	const int BLOCKS_AMOUNT = pow(CHUNK_SIZE, 3);
	//TODO remove calloc
	//mat4 *modelMatrices_stone = (mat4 *)calloc(BLOCKS_AMOUNT, sizeof(mat4));
	//mat4 *modelMatrices_ore = (mat4 *)calloc(BLOCKS_AMOUNT, sizeof(mat4));

	for (int i = 0; i < BLOCKS_AMOUNT; i++) {
		if (chunk->chunk_data[i] == STONE) {
			mat4 model = GLM_MAT4_IDENTITY_INIT;
			vec3 offset;

			offset[0] = (float)(chunk->pos->x * CHUNK_SIZE +
			            (i % CHUNK_SIZE));
			offset[1] = (float)(chunk->pos->y * CHUNK_SIZE +
			            (i / CHUNK_SIZE) % CHUNK_SIZE);
			offset[2] = (float)(chunk->pos->z * CHUNK_SIZE +
			            i / (CHUNK_SIZE * CHUNK_SIZE));

			glm_translate(model, offset);
			glm_mat4_copy(model, (*stone)[i]);
			*ID = 1;
		}

		if (chunk->chunk_data[i] == ORE) {
			mat4 model = GLM_MAT4_IDENTITY_INIT;
			vec3 offset;

			offset[0] = (float)(chunk->pos->x * CHUNK_SIZE +
		                                     (i % CHUNK_SIZE));
			offset[1] = (float)(chunk->pos->y * CHUNK_SIZE +
			                       (i / CHUNK_SIZE) % CHUNK_SIZE);
			offset[2] = (float)(chunk->pos->z * CHUNK_SIZE +
			                        i / (CHUNK_SIZE * CHUNK_SIZE));

			glm_translate(model, offset);
			glm_mat4_copy(model, (*ore)[i]);
			*ID = 2;
		}

	}

	return;
}

void putpixel(unsigned char *pixels, float pixels_depth)
{
	int background = (int)(255 * (1 / exp(150 - 150 * pixels_depth)));
	int is_same_color[3];

	if (background < 0)
		background = 0;
	if (background > 200)
		background = 200;

	for (int i = 0; i < BLOCK_TYPES_AMOUNT; i++) {
		for (int j = 0; j < 3; j++)
			is_same_color[j] = pixels[j] == blocks[i].color[j];

		if (is_same_color[0] && is_same_color[1] && is_same_color[2]) {
			printf("\033[1;48;2;%d;%d;%dm%c\033[0m", background,
			                                      background,
			                                      background,
			                                      blocks[i].ascii);
		}
	}

	return;
}

struct notcurses* notcurses_prepare() {
	setlocale(LC_ALL, "");

	struct notcurses* nc = notcurses_init(NULL, stdout);

	return nc;
}

void notcurses_render_ascii(struct notcurses* nc, struct ncplane* n,
                            unsigned char *frame_buffer, float *depth_buffer)
{
	for (int y = 0; y < FB_HEIGHT; y++) {
		for (int x = 0; x < FB_WIDTH; x++) {
			//ncplane_cursor_move_yx(n, y, x);

			int idx = (y * FB_WIDTH + x) * 4;
			float depth = depth_buffer[y * FB_WIDTH + x];
			unsigned char *pixels = frame_buffer + idx;

			int background = (int)(255 * (1 /
			                 exp(150 - 150 * depth)));
			int is_same_color[3];

			if (background < 0)
				background = 0;
			if (background > 200)
				background = 200;

			for (int i = 0; i < BLOCK_TYPES_AMOUNT; i++) {
				for (int j = 0; j < 3; j++) {
					is_same_color[j] = pixels[j] ==
					                   blocks[i].color[j];
				}

				int is_same = is_same_color[0] &&
				              is_same_color[1] &&
				              is_same_color[2];
				if (is_same) {
					ncplane_set_bg_rgb8(n, background,
					                       background,
					                       background);
					ncplane_putchar_yx(n, FB_HEIGHT - y, x,
					                   blocks[i].ascii);
				}
			}
		}
	}

	return;
}

void stat_render(struct notcurses* nc, struct ncplane* n, struct time time){
	struct ncplane_options opts = {
		.y = 0,              // Строка 5 (относительно родителя)
		.x = FB_WIDTH - 80,             // Колонка 10
		.rows = 20,          // Высота 10 строк
		.cols = 80,          // Ширина 20 столбцов
		.userptr = NULL,     // Без пользовательских данных
		.name = "child",     // Имя для отладки
		.resizecb = NULL,    // Без колбэка
		.flags = 0,          // Без флагов
	};

	// Создаём дочернюю плоскость
	struct ncplane* child_plane = ncplane_create(n, &opts);

	// Пример вывода текста в дочерней плоскости
	for (int y = 0; y < opts.rows; y++) {
		for (int x = 0; x < opts.cols; x++) {
			ncplane_set_bg_rgb8(child_plane, 0, 0, 0);
			ncplane_putchar_yx(child_plane, y, x, ' ');
		}
	}

//	ncplane_printf_yx(child_plane, 0, 0, "FPS: %d", calculate_fps(&time));

	return;
}
