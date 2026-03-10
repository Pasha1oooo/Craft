#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <math.h>

#include "main.h"
#include <cglm/cglm.h>
#include <time.h>
//TODO create funcs for clear
//#define background(R,G,B,str) printf("\033[1;48;2;%d;%d;%dm%c\033[0m", R, G, B, str);
const char *vertexShaderSource = "#version 460 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "out vec3 color;\n"
    "uniform mat4 transform;\n"
    "void main()\n"
    "{\n"
    "gl_Position = transform * vec4(aPos, 1.0f);\n"
    "color = aColor;\n"
    "}\n\0";
// in: VBO -> aPos

const char *fragmentShaderSource = "#version 460 core\n"
    "out vec4 FragColor;\n"
    "in vec3 color;\n"
    "void main()\n"
    "{\n"
    "FragColor = vec4(color, 1.0f);\n"
    "}\n";
// out: shader -> FrameBuffer

typedef struct block{
    char color[4];
    char ascii;
}block;

#define BLOCKS_NUM 5
const block blocks[BLOCKS_NUM] = {{{255,255,255,255}, '#'},
                                  {{0,0,0,255}, ' '},
                                  {{255,0,255,255}, '`'},
                                  {{255,255,0,255}, '.'},
                                  {{0,255,255,255}, '@'},
                                  };
void print_block(unsigned char * pixels){
    char color[4];
    for( int i = 0; i < 4; i++) color[i] = pixels[i];
    for( int i = 0; i < BLOCKS_NUM; i++){
        if (color[0] == blocks[i].color[0] && color[1] == blocks[i].color[1] && color[2] == blocks[i].color[2]){
            printf("%c", blocks[i].ascii);
//          printf("\033[1;48;2;%d;%d;%dm%c\033[0m", 255, 255, 255, '#');
        }
    }
}

int main(int argc, char * argv[]){
    printf("Start %lg\n", 10.0f/((float)(rand()%10)));

    //glxinfo | grep "OpenGL version"
    //Current: OpenGL version string: 4.6 (Compatibility Profile) Mesa 25.3.3-arch1.1
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //??????????
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    //??????????

    //My screen: 2560x1600 1000x1000 is a test
    int fb_width = 300, fb_height =70;
    GLFWwindow * window = glfwCreateWindow(fb_width, fb_height, "Game Window", NULL,NULL);
    if (window == NULL){
        printf("Failed to create a window\n");
        glfwTerminate();
        return 0;
    }
    else{
        printf("Window created\n");
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        printf("Failed to initialize GLAD\n");
        return 0;
    }
    else{
        printf("GLAD initialized\n");
    }
    /*
    int num = rand()%100000;
    float * vertices = calloc(num, sizeof(float));
    for(int i = 0; i < num; i++){
        float a = ((float)rand() + 13)/171717;
        vertices[i] = (a - floor(a))*pow(-1, rand()%10);
    }
    */
    float vertices[] = {
        //coordinates           //color
        0.0f, 1.0f, 0.0f,       1.0f, 1.0f, 0.0f,
        -1.0f,-1.0f, 0.0f,       1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,       1.0f, 1.0f, 0.0f,

        0.5f, -1.0f, 0.0f,       1.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 0.0f,       1.0f, 0.0f, 1.0f,
        0.3f, 1.0f, 0.0f,       1.0f, 0.0f, 1.0f,

        -0.2f, -1.0f, 0.0f,       0.0f, 1.0f, 1.0f,
        -0.9f, -0.4f, 0.0f,       0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f,       0.0f, 1.0f, 1.0f,
    };
    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3,
    };
    //configuration VBO
    unsigned int VBO, VAO, EBO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    //glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    //set pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);


    //Vertex shader
    unsigned int vertexShader;
    vertexShader =  glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    //Fragment shader
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);


    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    float global_time1 = 0 , global_time2 = 0;
    int FPS = 0;
    int i = 0;
    printf("Start drawing\n");
    glfwMakeContextCurrent(window);
    //glfwGetFramebufferSize(window, &fb_width, &fb_height);
    size_t pixel_count = fb_width * fb_height;
    size_t data_size = pixel_count * 4;
    printf("%d %d|||",  fb_width ,fb_height);
    unsigned char* pixels = (unsigned char*)calloc(data_size, sizeof(char));
    while(!glfwWindowShouldClose(window))
    {
        global_time1 = glfwGetTime();
        processInput(window);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
//////////////////////////////////////////////////////////////////////
        glUseProgram(shaderProgram);

        float time = glfwGetTime();

        mat4 TRS_matrix = {{0.5f, 0.0f, 0.0f, 0.0f},
                           {0.0f, 0.5f, 0.0f, 0.0f},
                           {0.0f, 0.0f, 0.5f, 0.0f},
                           {0.0f, 0.0f, 0.0f, 1.0f}};
        vec3 axis = {0,0,1};

        glm_rotate(TRS_matrix, time, axis);
        unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, *TRS_matrix);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 9);
        glPixelStorei(GL_PACK_ALIGNMENT, 1);//???

        glReadBuffer(GL_BACK);//

        glReadPixels(0, 0, fb_width, fb_height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        for(int i = 0; i <= pixel_count*4; i+=4) {
            print_block(pixels + i);
            if (i%(fb_width*4) == 0) {
                printf("\n");
            }
        }
        //printf("\033c");

//////////////////////////////////////////////////////////////////////
        //glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(indices[0]), GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(window);
        glfwPollEvents();
        i++;
        if (global_time1 - global_time2 > 1){
            global_time2 = global_time1;
            //printf("\nFPS: %d\n", i);
            FPS = i;
            i = 0;
        }
        printf("\nFPS: %d  %lg\n", FPS, time);
    }
    free(pixels);
    //free(vertices);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}
