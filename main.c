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

void print_block(unsigned char * pixels, float * pixels_depth, char * buffer, int * ic){
    char color[4];
    for( int i = 0; i < 4; i++) color[i] = pixels[i];
    for( int i = 0; i < BLOCKS_NUM; i++){
        if (color[0] == blocks[i].color[0] && color[1] == blocks[i].color[1] && color[2] == blocks[i].color[2]){
            buffer[*ic] = blocks[i].ascii;
            (*ic)+=1;
            //printf("%c", blocks[i].ascii);
            //printf("\033[1;48;2;%d;%d;%dm%c\033[0m", 255, 255, 255, '#');
        }
    }
}

/*
void print_block(unsigned char * pixels, float  pixels_depth){
    char color[4];
    int gray = (int)(pixels_depth * 255);
    if (gray < 0) gray = 0;
    if (gray > 255) gray = 255;
    for( int i = 0; i < 4; i++) color[i] = pixels[i];
    for( int i = 0; i < BLOCKS_NUM; i++){
        if (color[0] == blocks[i].color[0] && color[1] == blocks[i].color[1] && color[2] == blocks[i].color[2]){
            //printf("%c", blocks[i].ascii);
            printf("\033[1;48;2;%d;%d;%dm%c\033[0m", 255- gray, 255- gray, 255- gray, blocks[i].ascii);
        }
    }
}
*/
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
    int fb_width = 381, fb_height = 77;
    //int fb_width = 481, fb_height = 87;
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

float vertices[] = {
        //coordinates           //color                 //texture
        1.0f, 1.0f, 1.0f,       0.0f, 0.0f, 1.0f,       1.0f, 1.0f,
        -1.0f,1.0f, 1.0f,       0.0f, 0.0f, 1.0f,       0.0f, 1.0f,
        1.0f, -1.0f, 1.0f,       0.0f, 0.0f, 1.0f,      1.0f, 0.0f,
        -1.0f, -1.0f, 1.0f,       0.0f, 0.0f, 1.0f,     0.0f, 0.0f,

        1.0f, 1.0f, 1.0f,       0.0f, 1.0f, 0.0f,       1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,       0.0f, 1.0f, 0.0f,       0.0f, 1.0f,
        1.0f, 1.0f, -1.0f,       0.0f, 1.0f, 0.0f,      1.0f, 0.0f,
        1.0f, -1.0f, -1.0f,       0.0f, 1.0f, 0.0f,     0.0f, 0.0f,

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

    /*
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
    */
    //configuration VBO
    #define CUBES_PER_AXIS 100
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(6);


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

    unsigned char * pixels = (unsigned char*)calloc(data_size, sizeof(char));
    float * pixels_depth = (float*)calloc(fb_width * fb_height , sizeof(float));

    unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
    unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
    unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    char * buffer = calloc(data_size, sizeof(char));
    float time;

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(1);
    unsigned char *data = stbi_load("cat.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        printf("Failed to load texture\n");
    }
    stbi_image_free(data);
    while(!glfwWindowShouldClose(window))
    {
        global_time1 = glfwGetTime();
        processInput(window);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//////////////////////////////////////////////////////////////////////

        //glBindTexture(GL_TEXTURE_2D, texture);
        glUseProgram(shaderProgram);
        glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture"), 0);
        time = glfwGetTime();

        mat4 view = GLM_MAT4_IDENTITY_INIT;
        //glm_rotate(view, time, (vec3){1.0f, 1.0f, 0.0f});
        glm_translate(view, (vec3){0.0f, 0.0f, time*2});
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float*)view);

        mat4 projection;
        //glm_perspective(glm_rad(60.0f), (float)fb_width / (fb_height+150), 0.1f, 100.0f, projection);
        glm_perspective(glm_rad(60.0f), (float)fb_width / (fb_height+150), 0.1f, 1000.0f, projection);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, (float*)projection);

        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);
        glDrawElementsInstanced(GL_TRIANGLES,  sizeof(indices)/sizeof(indices[0]), GL_UNSIGNED_INT, 0, TOTAL_CUBES);

        glPixelStorei(GL_PACK_ALIGNMENT, 1);//???

        glReadBuffer(GL_BACK);

        glReadPixels(0, 0, fb_width, fb_height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        glReadPixels(0, 0, fb_width, fb_height, GL_DEPTH_COMPONENT, GL_FLOAT, pixels_depth);
        int ic = 0;
        for(int i = 0; i < pixel_count*4; i+=4) {
            print_block(pixels + i, pixels_depth, buffer, &ic);
            //print_block(pixels + i, *(pixels_depth + i/4));
            if (i%(fb_width*4) == 0) {
                buffer[ic] = '\n';
                ic++;
                //printf("\n");
            }
        }
        fwrite(buffer, sizeof(char), pixel_count, stdout);
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
    free(buffer);
    free(modelMatrices);
    glDeleteBuffers(1, &instanceVBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}
