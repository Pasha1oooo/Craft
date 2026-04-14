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
#include "logic.h"

vec3 cameraFront = {0,0,-1};

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	static bool firstMouse = true;
	static float lastX = 0.0f, lastY = 0.0f;
	static float yaw = -90.0f;    // начальное направление (смотрит вдоль -Z)
	static float pitch = 0.0f;
	extern vec3 cameraFront;
    (void)window; // подавляем предупреждение о неиспользуемом параметре

    if (firstMouse) {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
        return;
    }

    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos; // reversed since y-coordinates go from bottom to top
    lastX = (float)xpos;
    lastY = (float)ypos;

    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    // Ограничение угла обзора (чтобы не перевернуться)
    if (pitch > 89.0f)  pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    // Вычисляем вектор направления камеры (cameraFront)
    float yaw_rad   = glm_rad(yaw);
    float pitch_rad = glm_rad(pitch);

    vec3 direction = {
        cosf(yaw_rad) * cosf(pitch_rad),
        sinf(pitch_rad),
        sinf(yaw_rad) * cosf(pitch_rad)
    };
    // Нормализуем и сохраняем в cameraFront
    glm_vec3_copy(direction, cameraFront);
    glm_vec3_normalize(cameraFront);
}

int main(void)
{
	unsigned char *frame_buffer = (unsigned char*)calloc(FB_WIDTH * FB_HEIGHT,
	                                                        sizeof(float));
	float *depth_buffer = (float*)calloc(FB_WIDTH * FB_HEIGHT,
	                                                        sizeof(float));
	GLFWwindow *window;
	struct position saved_chunk_pos = {.x = 0, .y = 0, .z = 0};
	unsigned int modelLoc, viewLoc, projectionLoc; // modelLoc?
	unsigned int VBO, VAO, EBO, instanceVBO;
	unsigned int VBO_highlight, VAO_highlight, EBO_highlight;
	unsigned int shaderProgram, shaderProgram2, texture, texture2;
	vec3 player_pos, target;
	mat4 view = GLM_MAT4_IDENTITY_INIT;
	struct time time;
	struct player player;
	struct chunk *loaded_chunks = init_chunks();
	glm_vec3_copy(cameraFront, player.head.cameraDirection);
	create_window(&window, FB_WIDTH, FB_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("Failed to initialize GLAD\n");
		return 0;
	}

	prepare_gl_environment(&VBO, &VAO, &EBO, &instanceVBO, &VBO_highlight, &VAO_highlight, &EBO_highlight);

	prepare_texture(&texture, "a.png");
	prepare_texture(&texture2, "black.png");
	shaderProgram = prepare_shaders();
	shaderProgram2 = prepare_shaders2();

	modelLoc = glGetUniformLocation(shaderProgram, "model");
	viewLoc = glGetUniformLocation(shaderProgram, "view");
	projectionLoc = glGetUniformLocation(shaderProgram, "projection");

	player = create_player();

	time.frame_counter = 0;
	time.start = glfwGetTime();
	struct position selected_block;
	selected_block.x = -1; selected_block.y = -1; selected_block.z = -1;

	while(!glfwWindowShouldClose(window)) {



		processInput(window, &player, &loaded_chunks, selected_block);

		glEnable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glUniform1i(glGetUniformLocation(shaderProgram, "Texture"), 0);

		player_pos[0] = player.x;
		player_pos[1] = player.y;
		player_pos[2] = player.z;

		glm_vec3_copy(player_pos, player.head.cameraPos);
		glm_vec3_copy(cameraFront, player.head.cameraDirection);
		glm_vec3_add(player.head.cameraPos, player.head.cameraDirection, target);
		glm_lookat(player.head.cameraPos, target, player.head.cameraUp,
		                                                         view);


		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float*)view);
		mat4 projection;
		glm_perspective(glm_rad(90.0f), (float)FB_WIDTH /
		                  (FB_HEIGHT+150), 0.1f, 10000.0f, projection);
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE,
		                                           (float*)projection);

		if (is_chunk_changed(player.head.cameraPos, &saved_chunk_pos)) {
			get_chunks(loaded_chunks, player.head.cameraPos);
			save_chunk_pos(&player.head.cameraPos, &saved_chunk_pos);
		}

		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);

		render_chunks(loaded_chunks, texture, VAO);
		glm_vec3_copy((vec3){player.x, player.y, player.z},player.head.cameraPos);


        if (select_block(player, loaded_chunks, &selected_block) == 1) {
            glUseProgram(shaderProgram2);
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram2, "view"), 1, GL_FALSE, (float*)view);
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram2, "projection"), 1, GL_FALSE, (float*)projection);
            mat4 model_highlight = GLM_MAT4_IDENTITY_INIT;
            glm_translate(model_highlight, (vec3){selected_block.x , selected_block.y, selected_block.z});
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram2, "model"), 1, GL_FALSE, (float*)model_highlight);
            glBindVertexArray(VAO_highlight);
            glBindTexture(GL_TEXTURE_2D, texture2);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            glUseProgram(shaderProgram); // возвращаемся к основному шейдеру для следующего кадра
        }

		glfwSwapBuffers(window);
		glReadBuffer(GL_BACK);

		glReadPixels(0, 0, FB_WIDTH, FB_HEIGHT, GL_RGBA,
		                                     GL_UNSIGNED_BYTE, frame_buffer);
		glReadPixels(0, 0, FB_WIDTH, FB_HEIGHT,
		                   GL_DEPTH_COMPONENT, GL_FLOAT, depth_buffer);

		for (int y = FB_HEIGHT - 1; y >= 0; y--) {
			for (int x = 0; x < FB_WIDTH; x++) {
				int idx = (y * FB_WIDTH + x) * 4;
				float depth = depth_buffer[y * FB_WIDTH + x];

				putpixel(frame_buffer + idx, depth);
			}

			printf("\n");
		}

		glfwPollEvents();
		printf("\033[H");
		//calculate_fps(&time);
	}

	free(frame_buffer);
	free(depth_buffer);

	deinit_chunks(loaded_chunks);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &instanceVBO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(shaderProgram);

	glfwTerminate();

	return 0;
}
