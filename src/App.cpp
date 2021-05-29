#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <textRenderer.h>
#include <shader.h>
#include <camera.h>
#include <vao.h>
#include <game.h>

#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <streambuf>
#include <string>
#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(const char* fp);

float current_opacity = 0.0;
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 2.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouseMovement = true;

int flyUp = 0;
int currentState = 1; //0 - Start menu, 1 - Playing, 2 - Game Over
float GAME_SPEED = 0.002;

glm::vec3 birdCurPos = glm::vec3(0.0f);
glm::vec3 bgCurPos = glm::vec3(0.0f);
std::vector<glm::vec3> pipeCurPos = { glm::vec3(1.5f, 0.0f, 0.0f), glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(2.5f, 0.0f, 0.0f), glm::vec3(3.0f),
                                glm::vec3(3.5f, 0.0f, 0.0f), glm::vec3(4.0f, 0.0f, 0.0f), glm::vec3(4.5f, 0.0f, 0.0f), glm::vec3(5.0f, 0.0f, 0.0f) };

int main(){
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", glfwGetPrimaryMonitor(), NULL);
    if (window == NULL){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetKeyCallback(window, processInput);
    //glfwSetCursorPosCallback(window, mouse_callback);
    //glfwSetScrollCallback(window, scroll_callback);

    //glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // build and compile our shader program
    Shader birdShader("shaders/bird.vs", "shaders/bird.fs");
    Shader bgShader("shaders/bg.vs", "shaders/bg.fs");
    Shader pipeShader("shaders/pipe.vs", "shaders/pipe.fs");

    //removes mouse cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    stbi_set_flip_vertically_on_load(true);

    // texture setup
    unsigned int birdTexture = loadTexture("images/flappy.png");
    unsigned int bgTexture = loadTexture("images/city-bg-long.png");
    unsigned int bg_koTexture = loadTexture("images/city-bg_bw.png");
    unsigned int menuBgTexture = loadTexture("images/menu-bg.jpg");
    unsigned int bird_koTexture = loadTexture("images/flappy_ko.png");
    unsigned int bird_45Texture = loadTexture("images/flappy_45.png");
    unsigned int bird_45DownTexture = loadTexture("images/flappy_45-.png");
    unsigned int bird_DownTexture = loadTexture("images/flappy_down.png");
    unsigned int pipeTexture = loadTexture("images/pipe.png");

    // set uniform values
    birdShader.use();
    glUniform1i(glGetUniformLocation(birdShader.ID, "birdTexture"), 0);
    bgShader.use();
    glUniform1i(glGetUniformLocation(bgShader.ID, "bgTexture"), 1);
    pipeShader.use();
    glUniform1i(glGetUniformLocation(pipeShader.ID, "pipeTexture"), 2);

    float birdVertices[] = {
        // positions          // texture coords
         0.06f,  0.10f, 0.0f,   1.0f, 1.0f,   // top right
         0.06f, -0.10f, 0.0f,   1.0f, 0.0f,   // bottom right
        -0.06f, -0.10f, 0.0f,   0.0f, 0.0f,   // bottom left
        -0.06f,  0.10f, 0.0f,   0.0f, 1.0f    // top left 
    };
    
    float bgVertices[] = {
        // positions          // texture coords
         3.0f,  1.0f, -1.0f,   1.0f, 1.0f,   // top right
         3.0f, -1.0f, -1.0f,   1.0f, 0.0f,   // bottom right
        -1.0f, -1.0f, -1.0f,   0.0f, 0.0f,   // bottom left
        -1.0f,  1.0f, -1.0f,   0.0f, 1.0f    // top left 
    };

    float pipeVertices[] = {
        // positions          // texture coords
         0.1f,  0.5f, 0.7f,   1.0f, 1.0f,   // top right
         0.1f, -0.5f, 0.7f,   1.0f, 0.0f,   // bottom right
        -0.1f, -0.5f, 0.7f,   0.0f, 0.0f,   // bottom left
        -0.1f,  0.5f, 0.7f,   0.0f, 1.0f    // top left 
    };

    unsigned int quadIndices[] = {
        0, 1, 3,
        1, 2, 3
    };

    Vao birdVAO(birdVertices, quadIndices, sizeof(birdVertices), sizeof(quadIndices));
    Vao bgVAO(bgVertices, quadIndices, sizeof(bgVertices), sizeof(quadIndices));
    Vao pipeVAO(pipeVertices, quadIndices, sizeof(pipeVertices), sizeof(quadIndices));
    
    Game game(birdShader.ID, bgShader.ID, pipeShader.ID, 
                birdTexture, bird_koTexture, bird_45DownTexture, bird_DownTexture, 
                bgTexture, bg_koTexture, menuBgTexture, pipeTexture, 
                birdVAO.VAO, bgVAO.VAO, pipeVAO.VAO);
    game.init();

    glfwSetWindowUserPointer(window, &game);
    TextRenderer textRenderer("fonts/blocks.ttf", "shaders/text.vs", "shaders/text.fs", 0, 48);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)){
        // render
        // ------
        glClearColor(0.2f, 0.3f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        game.run(deltaTime);

        textRenderer.RenderText("Score: " + std::to_string(game.getScore()), 25.0f, 1000.0f, 1.0f, glm::vec3(0.8, 0.2f, 0.4f));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &birdVAO.VAO);
    glDeleteVertexArrays(1, &bgVAO.VAO);
    glDeleteVertexArrays(1, &pipeVAO.VAO);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window, int key, int scancode, int action, int mods){
    Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    else if (key == GLFW_KEY_SPACE && action != GLFW_RELEASE) {
        game->fallPoint = game->birdCurPos.y;
        game->flyUpCount += 25;
    }
    else if (key == GLFW_KEY_RIGHT && action != GLFW_RELEASE) {
        if (game->curGameState == MENU) {
            game->curOption = std::min(3, (int)(game->curOption + 1));
        }
    }
    else if (key == GLFW_KEY_LEFT && action != GLFW_RELEASE) {
        if (game->curGameState == MENU) {
            game->curOption = std::max(1, (int)(game->curOption - 1));
        }
    }
    else if (key == GLFW_KEY_ENTER && action != GLFW_RELEASE) {
        if (game->curGameState == MENU) {
            if (game->enterPressed == true and game->curOption == 2) {
                game->enterPressed = false;
                game->curOption = 2;
                return;
            }
            game->enterPressed = true;
            if (game->curOption == 3) {
                glfwSetWindowShouldClose(window, true);
            }
        }
        else if(game->curGameState == GAME_OVER) {
            game->init();
        }
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

unsigned int loadTexture(char const* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}