#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Questa riga è FONDAMENTALE per le texture
#define STB_IMAGE_IMPLEMENTATION
#include "Camera.h"
#include "Model.h"

#include <iostream>

// --- SETUP CAMERA ---
Camera camera(glm::vec3(0.0f, 3.0f, 15.0f)); 
float lastX = 1920.0f / 2.0; 
float lastY = 1080.0f / 2.0;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// --- CALLBACKS ---
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// --- VERTEX SHADER ---
const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aNormal;\n" 
    "layout (location = 2) in vec2 aTexCoords;\n"
    "out vec3 Normal;\n"
    "out vec3 FragPos;\n"
    "out vec2 TexCoords;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "   FragPos = vec3(model * vec4(aPos, 1.0));\n"
    "   Normal = mat3(transpose(inverse(model))) * aNormal;\n"
    "   TexCoords = aTexCoords;\n"
    "   gl_Position = projection * view * vec4(FragPos, 1.0);\n"
    "}\0";

// --- FRAGMENT SHADER (SISTEMATO) ---
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec3 Normal;\n"
    "in vec3 FragPos;\n"
    "in vec2 TexCoords;\n"
    
    "uniform sampler2D texture_diffuse1;\n"

    "void main()\n"
    "{\n"
    "   vec4 texColor = texture(texture_diffuse1, TexCoords);\n"
    
    // --- DEBUG TEXTURE ---
    // Mostriamo SOLO il colore dell'immagine caricata.
    // Niente luci, niente ombre, niente calcoli.
    
    "   if(texColor.a < 0.1) discard;\n" // Mantiene le foglie trasparenti
    "   FragColor = texColor;\n"
    "}\n\0";
int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Foresta 3D Finale", primaryMonitor, NULL);
    if (window == NULL) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;
    glEnable(GL_DEPTH_TEST);

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // --- CARICAMENTO MODELLO ---
    // Assicurati che questo percorso sia corretto e che Model.h sia aggiornato
    Model ourModel("C:\\Users\\andre\\Documents\\GitHub\\Computer_Graphics\\assets\\trees\\trees.obj"); 

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        // Sfondo Cielo
        glClearColor(0.5f, 0.7f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, &camera.Position[0]);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)mode->width / (float)mode->height, 0.1f, 200.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);

        glm::mat4 view = camera.GetViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);

        // Draw Model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -5.0f, -10.0f)); 
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f)); 
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
        ourModel.Draw(shaderProgram);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.ProcessKeyboard(RIGHT, deltaTime);
}
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse) { lastX = xpos; lastY = ypos; firstMouse = false; }
    camera.ProcessMouseMovement(xpos - lastX, lastY - ypos);
    lastX = xpos; lastY = ypos;
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) { camera.ProcessMouseScroll(static_cast<float>(yoffset)); }
void framebuffer_size_callback(GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); }