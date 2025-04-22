#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "Avatar.h"
#include "Menu.h"
#include "MainMenu.h"
#include "Scenes.h"
#include "Slides.h"
#include <thread> 
#include <chrono>
#include "stb_image.h"
#include <iostream>

// Initial window size
const int initialWindowWidth = 1200;
const int initialWindowHeight = 900;
int windowWidth = initialWindowWidth;
int windowHeight = initialWindowHeight;

// Struct to store MainMenu and Slides pointers
struct AppState {
    MainMenu* mainMenu;
    Slides* slides;
};

// Callback for window resizing
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    windowWidth = width;
    windowHeight = height;
}

// Mouse click callback
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        // Convert mouse coordinates to normalized initial window space
        double normalizedX = (mouseX / windowWidth) * initialWindowWidth;
        double normalizedY = (mouseY / windowHeight) * initialWindowHeight;

        AppState* state = (AppState*)glfwGetWindowUserPointer(window);
        if (!state) return;

        if (state->mainMenu) {
            state->mainMenu->handleMouseClick(normalizedX, normalizedY, initialWindowWidth, initialWindowHeight);
        }

        if (state->slides) {
            state->slides->handleMouseClick(normalizedX, normalizedY, initialWindowWidth, initialWindowHeight);
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

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Storyboard That", nullptr, nullptr);
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
    Shader slidesShader("slides.vert", "fragment.frag");

    Avatar avatar;
    Scenes scenes(slidesShader, textureShader);
    MainMenu mainMenu(avatarShader, textureShader, avatar, scenes);
    Slides slides(slidesShader, avatarShader, textureShader);

    // Store both objects in AppState
    AppState appState = { &mainMenu, &slides };
    glfwSetWindowUserPointer(window, &appState);

    glfwShowWindow(window);

    const double targetFPS = 60.0;
    const double frameTime = 1.0 / targetFPS;

    while (!glfwWindowShouldClose(window)) {
        double startTime = glfwGetTime();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, windowWidth, windowHeight);

        avatarShader.use();
        textureShader.use();


        slides.renderSlidesBackground();
        slides.render(windowWidth, windowHeight);

        // Render menu buttons
        mainMenu.render(-0.75f, 0.9f, 0.3f, 0.08f, initialWindowWidth, initialWindowHeight);

        // Render slides system
        //scenes.drawScene(1, 0.0, 0.0, 1.0, 1.0);

        //10,13,14
        // 17 je skola ali bi bilo bolje da je iz unutra
        // 19 je office mozes bolje
        // fali bazen, plaza da bude normalna, klub, igraonica
        //avatar.draw(avatarShader, textureShader, windowWidth, windowHeight);

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
