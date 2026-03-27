#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include "main.h"
#include "generator.h"
#include <cglm/cglm.h>
#include <time.h>

#define STB_IMAGE_IMPLEMENTATION
#include "external/stb/stb_image.h"

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

//typedef struct chunk{
//    mat4 blocks[4096];
//    int x;
//    int y;
//}chunk;

typedef struct camera{
    vec3 cameraPos;
    vec3 cameraTarget;
    vec3 cameraDirection;
    vec3 up;
    vec3 cameraRight;
    vec3 cameraUp;
}camera;

typedef struct player{
    camera head;
    float x;
    float y;
    float z;
    float speed;
}player;

#define BLOCKS_NUM 8
const block blocks[BLOCKS_NUM] = {{{255,255,255,255}, '#'},
                                  {{0,0,0,255}, ' '},
                                  {{255,0,255,255}, '`'},
                                  {{255,255,0,255}, '-'},
                                  {{0,255,255,255}, '@'},
                                  {{255,0,0,255}, '#'},
                                  {{0,255,0,255}, '&'},
                                  {{0,0,255}, '.'},
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

mat4 * create_chunk(struct chunk chunk, int * num){
    int CUBES_PER_AXIS = 16;
    int TOTAL_CUBES = CUBES_PER_AXIS * CUBES_PER_AXIS * CUBES_PER_AXIS;
    mat4 * modelMatrices = calloc(TOTAL_CUBES, sizeof(mat4));
    int idx = 0;
    for (int i = 0; i < TOTAL_CUBES; i++) {
        if(chunk.chunk_data[i] == '*'){
            mat4 model = GLM_MAT4_IDENTITY_INIT;
            glm_translate(model, (vec3){
                chunk.pos->x * CUBES_PER_AXIS + (i % CUBES_PER_AXIS),
                chunk.pos->y * CUBES_PER_AXIS + (i / CUBES_PER_AXIS) % CUBES_PER_AXIS,
                chunk.pos->z * CUBES_PER_AXIS + (i / CUBES_PER_AXIS) / CUBES_PER_AXIS,
            });
            glm_mat4_copy(model, modelMatrices[idx]);
            idx++;
        }
    }
    *num = idx;
    return modelMatrices;
}

void print_block(unsigned char * pixels, float  pixels_depth){
    char color[4];
                           //far smooth  //distance
    int gray = (int)(255 * (2/exp(110 - 110*pixels_depth)));
    if (gray < 0) gray = 0;
    //intensive
    if (gray > 200) gray = 200;
    for( int i = 0; i < 4; i++) color[i] = pixels[i];
    for( int i = 0; i < BLOCKS_NUM; i++){
        if (color[0] == blocks[i].color[0] && color[1] == blocks[i].color[1] && color[2] == blocks[i].color[2]){
            printf("\033[1;48;2;%d;%d;%dm%c\033[0m", gray, gray, gray, blocks[i].ascii);
        }
    }
}

camera create_camera(){
    camera camera;
    glm_vec3_copy((vec3){0.0f, 10.0f, 10.0f}, camera.cameraPos);
    glm_vec3_copy((vec3){0.0f, -1.0f, 0.0f}, camera.cameraTarget);
    glm_vec3_copy((vec3){0.0f, 0.0f, -1.0f}, camera.cameraDirection);
    glm_vec3_copy((vec3){0.0f, 1.0f, 0.0f}, camera.up);
    glm_vec3_sub(camera.cameraPos, camera.cameraTarget, camera.cameraDirection);
    glm_vec3_normalize(camera.cameraDirection);
    glm_cross(camera.up, camera.cameraDirection, camera.cameraRight);
    glm_normalize(camera.cameraRight);
    glm_cross(camera.cameraDirection, camera.cameraRight, camera.cameraUp);
    return camera;
}
player create_player(){
    player player;
    player.head = create_camera();
    player.x = 0;
    player.y = 0;
    player.z = 0;
    player.speed = 0.1f;
    return player;
}

int main(int argc, char * argv[]){
    printf("Start\n");

    GLFWwindow * window;
    int fb_width = 480, fb_height = 90;
    //int fb_width = 2560, fb_height = 1600;
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
        0.5f, 0.5f, 0.5f,       0.0f, 0.0f, 1.0f,       1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f,       0.0f, 0.0f, 1.0f,       0.0f, 1.0f,
        0.5f, -0.5f, 0.5f,       0.0f, 0.0f, 1.0f,      1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f,       0.0f, 0.0f, 1.0f,     0.0f, 0.0f,

        0.5f, 0.5f, 0.5f,       0.0f, 1.0f, 0.0f,       1.0f, 1.0f,
        0.5f, -0.5f, 0.5f,       0.0f, 1.0f, 0.0f,       0.0f, 1.0f,
        0.5f, 0.5f, -0.5f,      0.0f, 1.0f, 0.0f,      1.0f, 0.0f,
        0.5f, -0.5f, -0.5f,     0.0f, 1.0f, 0.0f,     0.0f, 0.0f,

        0.5f, 0.5f, 0.5f,       1.0f, 0.0f, 1.0f,       1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f,       1.0f, 0.0f, 1.0f,       0.0f, 1.0f,
        0.5f, 0.5f, -0.5f,       1.0f, 0.0f, 1.0f,      1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f,       1.0f, 0.0f, 1.0f,     0.0f, 0.0f,

        0.5f, 0.5f, -0.5f,       0.0f, 1.0f, 1.0f,      1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f,       0.0f, 1.0f, 1.0f,      0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,       0.0f, 1.0f, 1.0f,     1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,       0.0f, 1.0f, 1.0f,    0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f,       1.0f, 0.0f, 1.0f,      1.0f, 1.0f,
        -0.5f, -0.5f, 0.5f,       1.0f, 0.0f, 1.0f,      0.0f, 1.0f,
        -0.5f, 0.5f, -0.5f,       1.0f, 0.0f, 1.0f,     1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,       1.0f, 0.0f, 1.0f,    0.0f, 0.0f,

        0.5f, -0.5f, 0.5f,       1.0f, 1.0f, 0.0f,      1.0f, 1.0f,
        -0.5f, -0.5f, 0.5f,       1.0f, 1.0f, 0.0f,      0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,       1.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,       1.0f, 1.0f, 0.0f,    0.0f, 0.0f,
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
    #define MAX_CHUNKS_PER_AXIS 2
    //chunk *** world = create_chunk_map(MAX_CHUNKS_PER_AXIS);
    //configuration VBO

    unsigned int VBO, VAO, EBO, instanceVBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);



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
    shaderProgram = glCreateProgram();


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

///////////////////////////////////////////texture
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
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

    float frame_start = 0 , frame_end = 0, global_time = 0;
    int FPS = 0, frame_counter = 0;
    unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
    unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
    unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");

    size_t pixel_count = fb_width * fb_height;
    size_t data_size = pixel_count * 4;
    unsigned char * pixels = (unsigned char*)calloc(data_size, sizeof(char));
    float * pixels_depth = (float*)calloc(fb_width * fb_height , sizeof(float));
    char * buffer = calloc(data_size, sizeof(char));

    mat4 view = GLM_MAT4_IDENTITY_INIT;

    player player = create_player();

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


        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) player.y += player.speed;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) player.y -= player.speed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) player.x -= player.speed;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) player.x += player.speed;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) player.z -= player.speed;
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) player.z += player.speed;
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) glm_vec3_add(player.head.cameraDirection, (vec3){0,0.1,0.1}, player.head.cameraDirection);
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) glm_vec3_add(player.head.cameraDirection, (vec3){0,-0.1,-0.1}, player.head.cameraDirection);

        glm_vec3_copy((vec3){player.x, player.y, player.z}, player.head.cameraPos);
        vec3 target;
        glm_vec3_add(player.head.cameraPos, player.head.cameraDirection, target);
        glm_lookat(player.head.cameraPos, target, player.head.cameraUp, view);
        //glm_lookat((vec3){5.0f, 5.0f, 5.0f}, (vec3){0.0f, 0.0f, 0.0f}, player.head.cameraUp, view);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float*)view);
        mat4 projection;
        glm_perspective(glm_rad(90.0f), (float)fb_width / (fb_height+150), 0.1f, 1000.0f, projection);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, (float*)projection);



        struct chunk *loaded_chunks = init_chunks();
        get_chunks(loaded_chunks, player.head.cameraPos);
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        for (int i = 0; i < (int)pow(2 * RENDER_DISTANCE - 1, 3); i++) {
            int num;
            mat4 *modelMatrices = create_chunk(loaded_chunks[i], &num);
            glBindTexture(GL_TEXTURE_2D, texture);
            glBindVertexArray(VAO);
            glBufferData(GL_ARRAY_BUFFER, num * sizeof(mat4), modelMatrices, GL_STATIC_DRAW);
            glDrawElementsInstanced(GL_TRIANGLES, sizeof(indices)/sizeof(indices[0]), GL_UNSIGNED_INT, 0, num);
            free(modelMatrices);
        }
        deinit_chunks(loaded_chunks);
        glfwSwapBuffers(window);
        glReadBuffer(GL_BACK);
        glReadPixels(0, 0, fb_width, fb_height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        glReadPixels(0, 0, fb_width, fb_height, GL_DEPTH_COMPONENT, GL_FLOAT, pixels_depth);
        for (int y = fb_height - 1; y >= 0; y--) {
            for (int x = 0; x < fb_width; x++) {
                int idx = (y * fb_width + x) * 4;
                float depth = pixels_depth[y * fb_width + x];
                print_block(pixels + idx, depth);
            }
            printf("\n");
        }
//////////////////////////////////////////////////////////////////////

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

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &instanceVBO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}
