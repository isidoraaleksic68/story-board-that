#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "Avatar.h"
#include "Menu.h"
#include "MainMenu.h"
#include <thread> 
#include <chrono>
#include "stb_image.h"
#include <iostream>

// Initial window size (used for coordinate normalization)
const int initialWindowWidth = 1200;
const int initialWindowHeight = 1000;
int windowWidth = initialWindowWidth;
int windowHeight = initialWindowHeight;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    windowWidth = width;
    windowHeight = height;
}

// Universal Mouse Click Callback
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        // Convert mouse coordinates to normalized initial window space
        double normalizedX = (mouseX / windowWidth) * initialWindowWidth;
        double normalizedY = (mouseY / windowHeight) * initialWindowHeight;

        MainMenu* mainMenu = (MainMenu*)glfwGetWindowUserPointer(window);
        if (mainMenu) {
            mainMenu->handleMouseClick(normalizedX, normalizedY, initialWindowWidth, initialWindowHeight);
        }
    }
}

// Function to set the window icon
void setWindowIcon(GLFWwindow* window, const char* iconPath) {
    int width, height, channels;
    unsigned char* image = stbi_load(iconPath, &width, &height, &channels, 4);

    if (image) {
        GLFWimage icon;
        icon.width = width;
        icon.height = height;
        icon.pixels = image;

        glfwSetWindowIcon(window, 1, &icon);
        stbi_image_free(image);
    }
    else {
        printf("Failed to load icon: %s\n", iconPath);
    }
}

int main() {
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Storyboard that", nullptr, nullptr);

    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewInit();

    // Set window icon
    setWindowIcon(window, "logo/icon.png");

    // Set callback for resizing window
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    Shader avatarShader("vertex.vert", "fragment.frag");
    Shader textureShader("textureVertex.vert", "textureFragment.frag");

    Avatar avatar;
    MainMenu mainMenu(avatarShader, textureShader, avatar);

    glfwSetWindowUserPointer(window, &mainMenu);

    glfwShowWindow(window);

    const double targetFPS = 60.0;
    const double frameTime = 1.0 / targetFPS;

    while (!glfwWindowShouldClose(window)) {
        double startTime = glfwGetTime();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, windowWidth, windowHeight);

        avatarShader.use();
        textureShader.use();

        // Pass the fixed initial window dimensions to maintain button visibility and size
        mainMenu.render(-1.0f, 0.9f, 0.3f, 0.08f, initialWindowWidth, initialWindowHeight);
        avatar.draw(avatarShader, textureShader, initialWindowWidth, initialWindowHeight);

        glfwSwapBuffers(window);
        glfwPollEvents();

        double elapsedTime = glfwGetTime() - startTime;
        if (elapsedTime < frameTime) {
            std::this_thread::sleep_for(std::chrono::duration<double>(frameTime - elapsedTime));
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
