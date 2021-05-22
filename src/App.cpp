#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <shader.h>
#include <camera.h>
#include <vao.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
void play(Shader& bgShader, Shader& birdShader, Shader& pipeShader,
    glm::vec3& birdCurPos, glm::vec3& bgCurPos, std::vector<glm::vec3>& pipeCurPos,
    unsigned int& birdTexture, unsigned int& bgTexture, unsigned int& pipeTexture,
    unsigned int& birdVAO, unsigned int& bgVAO, unsigned int& pipeVAO);
void gameOver(Shader& bgShader, Shader& birdShader, 
    unsigned int& birdTexture, unsigned int& bgTexture, unsigned int& birdVAO, unsigned int& bgVAO);
unsigned int getVAO(float positions[], unsigned int  indices[]);
void generatePipes(Shader& pipeShader, std::vector<glm::vec3>& pipeCurPos, unsigned int& pipeTexture, unsigned int& pipeVAO);

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080; 
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

int main(){
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
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
    glfwSetScrollCallback(window, scroll_callback);

    //glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // build and compile our shader program
    Shader birdShader("shaders/bird.vs", "shaders/bird.fs");
    Shader bgShader("shaders/bg.vs", "shaders/bg.fs");
    Shader pipeShader("shaders/pipe.vs", "shaders/pipe.fs");

    // allows key callback function to use the shaderProgram, as we are setting a pointer to it from window
    //glfwSetWindowUserPointer(window, &birdShader);

    // removes mouse cursor
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    stbi_set_flip_vertically_on_load(true);

    // texture setup
    unsigned int birdTexture = loadTexture("images/flappy.png");
    unsigned int bgTexture = loadTexture("images/city-bg_resized2.png");
    unsigned int bg_bwTexture = loadTexture("images/city-bg_bw.png");
    unsigned int bird_koTexture = loadTexture("images/flappy_ko.png");
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
         3.0f,  1.0f, 0.7f,   1.0f, 1.0f,   // top right
         3.0f, -1.0f, 0.7f,   1.0f, 0.0f,   // bottom right
        -1.0f, -1.0f, 0.7f,   0.0f, 0.0f,   // bottom left
        -1.0f,  1.0f, 0.7f,   0.0f, 1.0f    // top left 
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
    
    glm::vec3 birdCurPos = glm::vec3(0.0f);
    glm::vec3 bgCurPos = glm::vec3(0.0f);
    std::vector<glm::vec3> pipeCurPos = { glm::vec3(0.5f, 0.0f, 0.0f), glm::vec3(1.0f), glm::vec3(1.5f, 0.0f, 0.0f), glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(2.5f, 0.0f, 0.0f),
                                          glm::vec3(3.0f), glm::vec3(3.5f, 0.0f, 0.0f), glm::vec3(4.0f, 0.0f, 0.0f) };

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

        if (currentState == 1) {
            play(bgShader, birdShader, pipeShader, birdCurPos, bgCurPos, pipeCurPos, birdTexture, 
                bgTexture, pipeTexture, birdVAO.VAO, bgVAO.VAO, pipeVAO.VAO);
            if (birdCurPos.y <= -0.77f) {
                currentState = 2;
            }
        }
        else if (currentState == 2) {
            gameOver(bgShader, birdShader, bird_koTexture, bg_bwTexture, birdVAO.VAO, bgVAO.VAO);
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &birdVAO.VAO);
    glDeleteVertexArrays(1, &bgVAO.VAO);
    glDeleteVertexArrays(1, &pipeVAO.VAO);

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window, int key, int scancode, int action, int mods){
    float cameraSpeed = 10.0f * deltaTime;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    else if (key == GLFW_KEY_SPACE && action != GLFW_RELEASE) {
        flyUp += 40;
    }
    else if (key == GLFW_KEY_W && action != GLFW_RELEASE)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    else if (key == GLFW_KEY_S && action != GLFW_RELEASE)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    else if (key == GLFW_KEY_A && action != GLFW_RELEASE)
        camera.ProcessKeyboard(LEFT, deltaTime);
    else if (key == GLFW_KEY_D && action != GLFW_RELEASE)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouseMovement) {
        lastX = xpos;
        lastY = ypos;
        firstMouseMovement = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
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

void play(Shader &bgShader, Shader &birdShader, Shader& pipeShader, 
            glm::vec3 &birdCurPos, glm::vec3 &bgCurPos, std::vector<glm::vec3>& pipeCurPos,
            unsigned int &birdTexture, unsigned int &bgTexture, unsigned int& pipeTexture, 
            unsigned int &birdVAO, unsigned int &bgVAO, unsigned int& pipeVAO) {
    if (flyUp == 0) {
        birdCurPos.y = glm::max((float)(birdCurPos.y - 0.005), -0.77f);
    }
    else {
        birdCurPos.y = glm::min((float)(birdCurPos.y + 0.01), 0.9f);
        flyUp--;
        if (birdCurPos.y == 0.9f)
            flyUp = 0;
    }

    if (bgCurPos.x <= -4.0f) {
        bgCurPos.x = 0.0f;
    }

    // BG
    bgShader.use();

    bgCurPos.x -= 0.001;
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, bgCurPos);
    bgShader.setMat4("model", model);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, bgTexture);
    glBindVertexArray(bgVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glm::vec3 bgScrollPos = bgCurPos;
    bgScrollPos.x = bgCurPos.x + 4.0f;
    model = glm::translate(glm::mat4(1.0f), bgScrollPos);
    bgShader.setMat4("model", model);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Bird
    birdShader.use();

    model = glm::mat4(1.0f);
    model = glm::translate(model, birdCurPos);
    birdShader.setMat4("model", model);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, birdTexture);
    glBindVertexArray(birdVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Pipes
    generatePipes(pipeShader, pipeCurPos, pipeTexture, pipeVAO);
}

void gameOver(Shader& bgShader, Shader& birdShader,
    unsigned int& birdTexture, unsigned int& bgTexture, unsigned int &birdVAO, unsigned int& bgVAO) {
    bgShader.use();

    glm::mat4 model = glm::mat4(1.0f);
    bgShader.setMat4("model", model);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, bgTexture);
    glBindVertexArray(bgVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Bird
    birdShader.use();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.77f, 1.0f));;
    birdShader.setMat4("model", model);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, birdTexture);
    glBindVertexArray(birdVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}


void generatePipes(Shader& pipeShader, std::vector<glm::vec3>& pipeCurPos, unsigned int& pipeTexture, unsigned int & pipeVAO) {
    for (auto& curPos : pipeCurPos) {
        if (curPos.x <= -3.0f)
            curPos.x = 1.0f;

        pipeShader.use(); 
        if (curPos.x >= 1.0f) {
            curPos.y = (rand() % 100 + 50) / 100.0f;
        }
        curPos.x -= 0.001;
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.4f + curPos.x, -curPos.y, 0.0f));
        pipeShader.setMat4("model", model);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, pipeTexture);
        glBindVertexArray(pipeVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(1.0f, -1.0f, 1.0f));
        model = glm::translate(model, glm::vec3(0.4f + curPos.x, -1.5f + curPos.y, 0.0f));
        pipeShader.setMat4("model", model);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
        
    
}