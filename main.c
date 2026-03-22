#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include "main.h"
#include <cglm/cglm.h>
#include <time.h>

#define STB_IMAGE_IMPLEMENTATION
#include "external/stb/stb_image.h"
//TODO create funcs for clear
//#define background(R,G,B,str) printf("\033[1;48;2;%d;%d;%dm%c\033[0m", R, G, B, str);
const char *vertexShaderSource = "#version 460 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "layout (location = 2) in vec4 aInstanceCol0;\n"
    "layout (location = 3) in vec4 aInstanceCol1;\n"
    "layout (location = 4) in vec4 aInstanceCol2;\n"
    "layout (location = 5) in vec4 aInstanceCol3;\n"
    "layout (location = 6) in vec2 aTexCoord;\n"
    "out vec3 color;\n"
    "out vec2 TexCoord;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "mat4 model = mat4(aInstanceCol0, aInstanceCol1, aInstanceCol2, aInstanceCol3);\n"
    "gl_Position = projection * view * model * vec4(aPos, 1.0f);\n"
    "color = aColor;\n"
    "TexCoord = aTexCoord;\n"
    "}\n\0";
// in: VBO -> aPos

const char *fragmentShaderSource = "#version 460 core\n"
    "out vec4 FragColor;\n"
    "in vec3 color;\n"
    "in vec2 TexCoord;\n"
    "uniform sampler2D ourTexture;\n"
    "void main()\n"
    "{\n"
    "FragColor = texture(ourTexture, TexCoord);\n"
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
void create_window(GLFWwindow ** window, int fb_width , int fb_height){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //??????????
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    //??????????

    *window = glfwCreateWindow(fb_width, fb_height, "Game Window", NULL,NULL);
    if (*window == NULL){
        printf("Failed to create a window\n");
        glfwTerminate();
    }
    else{
        printf("Window created\n");
    }
    glfwMakeContextCurrent(*window);
}
void create_shader_program(unsigned int * shaderProgram){

    *shaderProgram = glCreateProgram();
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

    glAttachShader(*shaderProgram, vertexShader);
    glAttachShader(*shaderProgram, fragmentShader);
    glLinkProgram(*shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    *shaderProgram = glCreateProgram();
}

int main(int argc, char * argv[]){
    printf("Start\n");
    GLFWwindow * window;
    int fb_width = 2560, fb_height = 1600;
    create_window(&window, fb_width , fb_height);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        printf("Failed to initialize GLAD\n");
        return 0;
    }
    else{
        printf("GLAD initialized\n");
    }

    float vertices[] = {
        //coordinates           //color                 //texture
        1.0f, 1.0f, 1.0f,       0.0f, 0.0f, 1.0f,       1.0f, 1.0f,
        -1.0f,1.0f, 1.0f,       0.0f, 0.0f, 1.0f,       0.0f, 1.0f,
        1.0f, -1.0f, 1.0f,       0.0f, 0.0f, 1.0f,      1.0f, 0.0f,
        -1.0f, -1.0f, 1.0f,       0.0f, 0.0f, 1.0f,     0.0f, 0.0f,

        1.0f, 1.0f, 1.0f,       0.0f, 1.0f, 0.0f,       1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,       0.0f, 1.0f, 0.0f,       0.0f, 1.0f,
        1.0f, 1.0f, -1.0f,      0.0f, 1.0f, 0.0f,      1.0f, 0.0f,
        1.0f, -1.0f, -1.0f,     0.0f, 1.0f, 0.0f,     0.0f, 0.0f,

        1.0f, 1.0f, 1.0f,       1.0f, 0.0f, 1.0f,       1.0f, 1.0f,
        -1.0f,1.0f, 1.0f,       1.0f, 0.0f, 1.0f,       0.0f, 1.0f,
        1.0f, 1.0f, -1.0f,       1.0f, 0.0f, 1.0f,      1.0f, 0.0f,
        -1.0f, 1.0f, -1.0f,       1.0f, 0.0f, 1.0f,     0.0f, 0.0f,

        1.0f, 1.0f, -1.0f,       0.0f, 1.0f, 1.0f,      1.0f, 1.0f,
        -1.0f,1.0f, -1.0f,       0.0f, 1.0f, 1.0f,      0.0f, 1.0f,
        1.0f, -1.0f, -1.0f,       0.0f, 1.0f, 1.0f,     1.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,       0.0f, 1.0f, 1.0f,    0.0f, 0.0f,

        -1.0f, 1.0f, 1.0f,       1.0f, 0.0f, 1.0f,      1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,       1.0f, 0.0f, 1.0f,      0.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,       1.0f, 0.0f, 1.0f,     1.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,       1.0f, 0.0f, 1.0f,    0.0f, 0.0f,

        1.0f, -1.0f, 1.0f,       1.0f, 1.0f, 0.0f,      1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,       1.0f, 1.0f, 0.0f,      0.0f, 1.0f,
        1.0f, -1.0f, -1.0f,       1.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        -1.0f,-1.0f, -1.0f,        1.0,  1.0f, 0.0f,    0.0f, 0.0f,

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

    //configuration VBO
    #define CUBES_PER_AXIS 100
    #define TOTAL_CUBES (CUBES_PER_AXIS * CUBES_PER_AXIS * CUBES_PER_AXIS)

    mat4 * modelMatrices = malloc(TOTAL_CUBES * sizeof(mat4));
    int idx = 0;
    for (int i = 0; i < CUBES_PER_AXIS; i++) {
        for (int j = 0; j < CUBES_PER_AXIS; j++) {
            for (int k = 0; k < 10; k++) {
                mat4 model = GLM_MAT4_IDENTITY_INIT;
                glm_translate(model, (vec3){
                    i*2,
                    -2+(int)(4*sin(i/10))+(int)(2*cos(j)) - 2*k + (int)(2*sin(i)),
                    j*2,
                });
                glm_mat4_copy(model, modelMatrices[idx]);
                idx++;
            }
        }
    }

    unsigned int VBO, VAO, EBO, instanceVBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &EBO);
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
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    //set pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(6);


    unsigned int shaderProgram;
    create_shader_program(&shaderProgram);


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

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    float frame_start = 0 , frame_end = 0, global_time = 0;
    int FPS = 0, frame_counter = 0;
    unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
    unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
    unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");

///////////////////////////////////////////texture
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // load and generate the texture
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(1);
    unsigned char *data = stbi_load("a.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        printf("Failed to load texture\n");
    }
    stbi_image_free(data);
///////////////////////////////////////////texture
    vec3 cameraPos = {0.0f, 0.0f, 3.0f};
    vec3 cameraTarget = {0.0f, 0.0f, 0.0f};
    vec3 cameraDirection;
    glm_vec3_sub(cameraPos, cameraTarget, cameraDirection);
    glm_vec3_normalize(cameraDirection);
    vec3 up = {0.0f, 1.0f, 0.0f};
    vec3 cameraRight;
    glm_cross(up, cameraDirection, cameraRight);
    glm_normalize(cameraRight);
    vec3 cameraUp;
    glm_cross(cameraDirection, cameraRight, cameraUp);
    mat4 view = GLM_MAT4_IDENTITY_INIT;
    const float radius = 10.0f;
    float camX = sin(glfwGetTime()) * radius;
    float camZ = cos(glfwGetTime()) * radius;


    printf("Start render\n");
    while(!glfwWindowShouldClose(window))
    {
        global_time = glfwGetTime();
        frame_start = glfwGetTime();
        processInput(window);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//////////////////////////////////////////////////////////////////////

        //glBindTexture(GL_TEXTURE_2D, texture);
        glUseProgram(shaderProgram);
        glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture"), 0);


        if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
            glm_vec3_sub(cameraPos, (vec3){1,0,0}, cameraPos);
        }
        if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
            glm_vec3_sub(cameraPos, (vec3){-1,0,0}, cameraPos);
        }
        if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
            glm_vec3_sub(cameraPos, (vec3){0,1,0}, cameraPos);
        }
        if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
            glm_vec3_sub(cameraPos, (vec3){0,-1,0}, cameraPos);
        }
        if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
            glm_vec3_sub(cameraPos, (vec3){0,0,1}, cameraPos);
        }
        if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
            glm_vec3_sub(cameraPos, (vec3){0,0,-1}, cameraPos);
        }
        glm_lookat(cameraPos, (vec3){0.0f, 0.0f, 2.0f}, cameraUp, view);

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float*)view);

        mat4 projection;
        glm_perspective(glm_rad(90.0f), (float)fb_width / (fb_height+150), 0.1f, 10000.0f, projection);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, (float*)projection);

        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);
        glDrawElementsInstanced(GL_TRIANGLES,  sizeof(indices)/sizeof(indices[0]), GL_UNSIGNED_INT, 0, TOTAL_CUBES);

//////////////////////////////////////////////////////////////////////
        glfwSwapBuffers(window);
        glfwPollEvents();
        frame_counter++;
        if (frame_start - frame_end > 1){
            frame_end = frame_start;
            FPS = frame_counter;
            frame_counter = 0;
        }
        printf("\033[H");
        printf("\nFPS: %d %lg\n", FPS, global_time);
    }

    free(modelMatrices);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &instanceVBO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}
