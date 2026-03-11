#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include "main.h"
#include <cglm/cglm.h>
#include <time.h>
//TODO create funcs for clear
//#define background(R,G,B,str) printf("\033[1;48;2;%d;%d;%dm%c\033[0m", R, G, B, str);
const char *vertexShaderSource = "#version 460 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "layout (location = 2) in vec4 aInstanceCol0;\n"
    "layout (location = 3) in vec4 aInstanceCol1;\n"
    "layout (location = 4) in vec4 aInstanceCol2;\n"
    "layout (location = 5) in vec4 aInstanceCol3;\n"
    "out vec3 color;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "mat4 model = mat4(aInstanceCol0, aInstanceCol1, aInstanceCol2, aInstanceCol3);\n"
    "gl_Position = projection * view * model * vec4(aPos, 1.0f);\n"
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

#define BLOCKS_NUM 8
const block blocks[BLOCKS_NUM] = {{{255,255,255,255}, '#'},
                                  {{0,0,0,255}, ' '},
                                  {{255,0,255,255}, '`'},
                                  {{255,255,0,255}, '-'},
                                  {{0,255,255,255}, '@'},
                                  {{255,0,0,255}, '+'},
                                  {{0,255,0,255}, '&'},
                                  {{0,0,255}, '!'},
                                  };
void print_block(unsigned char * pixels){
    char color[4];
    for( int i = 0; i < 4; i++) color[i] = pixels[i];
    for( int i = 0; i < BLOCKS_NUM; i++){
        if (color[0] == blocks[i].color[0] && color[1] == blocks[i].color[1] && color[2] == blocks[i].color[2]){
            printf("%c", blocks[i].ascii);
            //printf("\033[1;48;2;%d;%d;%dm%c\033[0m", 255, 255, 255, '#');
        }
    }
}

int main(int argc, char * argv[]){
    printf("Start %lg\n", 10.0f/((float)(rand()%10)));

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
    int fb_width = 381, fb_height =77;
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
    float vertices[] = {
        //coordinates           //color
        1.0f, 1.0f, 1.0f,       0.0f, 1.0f, 1.0f,
        -1.0f,1.0f, 1.0f,       0.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,       0.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,       0.0f, 1.0f, 1.0f,

        1.0f, 1.0f, 1.0f,       1.0f, 1.0f, 0.0f,
        1.0f,-1.0f, 1.0f,       1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, -1.0f,       1.0f, 1.0f, 0.0f,
        1.0f, -1.0f, -1.0f,       1.0f, 1.0f, 0.0f,

        1.0f, 1.0f, 1.0f,       1.0f, 0.0f, 1.0f,
        -1.0f,1.0f, 1.0f,       1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, -1.0f,       1.0f, 0.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,       1.0f, 0.0f, 1.0f,

        1.0f, 1.0f, -1.0f,       1.0f, 0.0f, 0.0f,
        -1.0f,1.0f, -1.0f,       1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, -1.0f,       1.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,       1.0f, 0.0f, 0.0f,

        -1.0f, 1.0f, 1.0f,       0.0f, 1.0f, 0.0f,
        -1.0f,-1.0f, 1.0f,       0.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, -1.0f,       0.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,       0.0f, 1.0f, 0.0f,

        1.0f, -1.0f, 1.0f,       0.0f, 0.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,       0.0f, 0.0f, 1.0f,
        1.0f, -1.0f, -1.0f,       0.0f, 0.0f, 1.0f,
        -1.0f,-1.0f, -1.0f,        0.0,  0.0f, 1.0f,

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
        21, 22, 23,

    };
    */
    float vertices[] = {
        //coordinates           //color
        1.0f, 1.0f, 1.0f,       0.0f, 1.0f, 1.0f,
        -1.0f,1.0f, 1.0f,       0.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,       0.0f, 1.0f, 1.0f,
        -1.0f,1.0f, 1.0f,       0.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,       0.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,       0.0f, 1.0f, 1.0f,

        1.0f, 1.0f, 1.0f,       1.0f, 1.0f, 0.0f,
        1.0f,-1.0f, 1.0f,       1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, -1.0f,       1.0f, 1.0f, 0.0f,
        1.0f,-1.0f, 1.0f,       1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, -1.0f,       1.0f, 1.0f, 0.0f,
        1.0f, -1.0f, -1.0f,       1.0f, 1.0f, 0.0f,

        1.0f, 1.0f, 1.0f,       1.0f, 0.0f, 1.0f,
        -1.0f,1.0f, 1.0f,       1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, -1.0f,       1.0f, 0.0f, 1.0f,
        -1.0f,1.0f, 1.0f,       1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, -1.0f,       1.0f, 0.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,       1.0f, 0.0f, 1.0f,

        1.0f, 1.0f, -1.0f,       1.0f, 0.0f, 0.0f,
        -1.0f,1.0f, -1.0f,       1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, -1.0f,       1.0f, 0.0f, 0.0f,
        -1.0f,1.0f, -1.0f,       1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, -1.0f,       1.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,       1.0f, 0.0f, 0.0f,

        -1.0f, 1.0f, 1.0f,       0.0f, 1.0f, 0.0f,
        -1.0f,-1.0f, 1.0f,       0.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, -1.0f,       0.0f, 1.0f, 0.0f,
        -1.0f,-1.0f, 1.0f,       0.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, -1.0f,       0.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,       0.0f, 1.0f, 0.0f,

        1.0f, -1.0f, 1.0f,       0.0f, 0.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,       0.0f, 0.0f, 1.0f,
        1.0f, -1.0f, -1.0f,       0.0f, 0.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,       0.0f, 0.0f, 1.0f,
        1.0f, -1.0f, -1.0f,       0.0f, 0.0f, 1.0f,
        -1.0f,-1.0f, -1.0f,        0.0,  0.0f, 1.0f,

    };
    //configuration VBO
    #define CUBES_PER_AXIS 41
    #define TOTAL_CUBES (CUBES_PER_AXIS * CUBES_PER_AXIS * CUBES_PER_AXIS)

    mat4 * modelMatrices = malloc(TOTAL_CUBES * sizeof(mat4));
    int idx = 0;
    for (int k = 0; k < CUBES_PER_AXIS; k++) {
        for (int j = 0; j < CUBES_PER_AXIS; j++) {
            for (int i = 0; i < CUBES_PER_AXIS; i++) {
                mat4 model = GLM_MAT4_IDENTITY_INIT;
                if((k==(int)(CUBES_PER_AXIS/2) && k==i && k==j)){
                    continue;
                }
                else{

                    glm_translate(model, (vec3){
                        (i - CUBES_PER_AXIS/2.0f) * 4.0f + (int)(CUBES_PER_AXIS/2),
                        (j - CUBES_PER_AXIS/2.0f) * 4.0f + (int)(CUBES_PER_AXIS/2),
                        (k - CUBES_PER_AXIS/2.0f) * 4.0f + (int)(CUBES_PER_AXIS/2)
                    });
                }

                glm_mat4_copy(model, modelMatrices[idx]);
                idx++;
            }

        }
    }

    unsigned int VBO, VAO, instanceVBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, TOTAL_CUBES * sizeof(mat4), modelMatrices, GL_STATIC_DRAW);


    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(4 * sizeof(float)));
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(4);
    glVertexAttribDivisor(4, 1);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(12 * sizeof(float)));
    glEnableVertexAttribArray(5);
    glVertexAttribDivisor(5, 1);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
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
    size_t pixel_count = fb_width * fb_height;
    size_t data_size = pixel_count * 4;

    unsigned char* pixels = (unsigned char*)calloc(data_size, sizeof(char));
    unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
    unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
    unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");

    float time;
    while(!glfwWindowShouldClose(window))
    {

        global_time1 = glfwGetTime();
        processInput(window);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//////////////////////////////////////////////////////////////////////
        glUseProgram(shaderProgram);

        time = glfwGetTime();

        mat4 view = GLM_MAT4_IDENTITY_INIT;
        glm_rotate(view, time, (vec3){1.0f, 1.0f, 0.0f});
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float*)view);

        mat4 projection;
        glm_perspective(glm_rad(60.0f), (float)fb_width / (fb_height+150), 0.1f, 100.0f, projection);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, (float*)projection);

        glBindVertexArray(VAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, TOTAL_CUBES); // 36 вершин на куб

        glPixelStorei(GL_PACK_ALIGNMENT, 1);//???

        glReadBuffer(GL_BACK);

        glReadPixels(0, 0, fb_width, fb_height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        for(int i = 0; i < pixel_count*4; i+=4) {
            print_block(pixels + i);
            if (i%(fb_width*4) == 0) {
                printf("\n");
            }
        }

        printf("\033[H");

//////////////////////////////////////////////////////////////////////
        glfwSwapBuffers(window);
        glfwPollEvents();
        i++;
        if (global_time1 - global_time2 > 1){
            global_time2 = global_time1;
            FPS = i;
            i = 0;
        }
        printf("\nFPS: %d %lg\n", FPS, time);
    }
    free(pixels);
    free(modelMatrices);
    glDeleteBuffers(1, &instanceVBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}
