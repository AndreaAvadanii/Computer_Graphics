#include <glad/glad.h>  // ⚠️ GLAD va SEMPRE incluso PRIMA di GLFW
#include <GLFW/glfw3.h>
#include <iostream>

// Questa funzione viene chiamata quando ridimensioni la finestra col mouse
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Gestione input tastiera
void processInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main() {
    // --- 1. CONFIGURAZIONE INIZIALE ---
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // --- 2. CREAZIONE FINESTRA ---
    GLFWwindow* window = glfwCreateWindow(800, 600, "Motore 3D - Setup Completo", NULL, NULL);
    if (window == NULL) {
        std::cout << "Errore creazione finestra GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // --- 3. CARICAMENTO GLAD (Cruciale!) ---
    // Qui carichiamo i puntatori alle funzioni OpenGL reali
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Errore inizializzazione GLAD" << std::endl;
        return -1;
    }

    // --- 4. LOOP DI RENDERING ---
    while (!glfwWindowShouldClose(window)) {
        // Input
        processInput(window);

        // Rendering (Sfondo colorato)
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Colore: R, G, B, Alpha
        glClear(GL_COLOR_BUFFER_BIT);

        // Swap buffers e gestione eventi
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}