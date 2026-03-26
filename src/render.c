/* render.c */

#include "../include/glad/glad.h"
#include <stdio.h>
#include <GLFW/glfw3.h>
#include "render.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow * window)
{
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
		printf("ESC\n");
		glfwSetWindowShouldClose(window, 1);
	}

	return;
}

void create_window(GLFWwindow ** window, int fb_width , int fb_height)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	*window = glfwCreateWindow(fb_width, fb_height, "Game Window",
	                                                           NULL, NULL);
	if (*window == NULL) {
		printf("Failed to create a window\n");
		glfwTerminate();
	} else {
		printf("Window created\n");
	}

	glfwMakeContextCurrent(*window);
}

camera create_camera(void)
{
	camera camera;

	glm_vec3_copy((vec3){0.0f, 10.0f, 10.0f}, camera.cameraPos);
	glm_vec3_copy((vec3){0.0f, -1.0f, 0.0f}, camera.cameraTarget);
	glm_vec3_copy((vec3){0.0f, 0.0f, 0.0f}, camera.cameraDirection);
	glm_vec3_copy((vec3){0.0f, 1.0f, 0.0f}, camera.up);
	glm_vec3_sub(camera.cameraPos, camera.cameraTarget,
	                                               camera.cameraDirection);
	glm_vec3_normalize(camera.cameraDirection);
	glm_cross(camera.up, camera.cameraDirection, camera.cameraRight);
	glm_normalize(camera.cameraRight);
	glm_cross(camera.cameraDirection, camera.cameraRight, camera.cameraUp);

	return camera;
}

player create_player(void)
{
	player player;

	player.head = create_camera();
	player.x = 0;
	player.y = 0;
	player.z = 0;
	player.speed = 0.5f;

	return player;
}

/*
void prepare_gl_environment(unsigned int *VBO, unsigned int *VAO,
                            unsigned int *EBO, unsigned int * instanceVBO)
{
	glGenVertexArrays(1, VAO);
	glGenBuffers(1, EBO);
	glBindVertexArray(*VAO);

	glGenBuffers(1, instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, *instanceVBO);

	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(mat4),
	                                                             (void*)0);
	glEnableVertexAttribArray(2);
	glVertexAttribDivisor(2, 1);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(mat4),
	                                           (void*)(4 * sizeof(float)));
	glEnableVertexAttribArray(3);
	glVertexAttribDivisor(3, 1);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(mat4),
	                                           (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(4);
	glVertexAttribDivisor(4, 1);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(mat4),
	                                          (void*)(12 * sizeof(float)));
	glEnableVertexAttribArray(5);
	glVertexAttribDivisor(5, 1);

	glGenBuffers(1, VBO);
	glBindBuffer(GL_ARRAY_BUFFER, *VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
	                                                       GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
	                                                       GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float),
	                                                             (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float),
	                                             (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float),
	                                           (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(6);

}

*/
