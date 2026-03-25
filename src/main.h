#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow * window);

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    //glViewport(0,0,1000,1000); //x,y view: h,w
}
void processInput(GLFWwindow * window){
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
         printf("ESC\n");
         glfwSetWindowShouldClose(window, 1);
    }

}

