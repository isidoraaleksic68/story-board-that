#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "Avatar.h"
#include "Menu.h"
#include <thread>  // Za std::this_thread::sleep_for
#include <chrono>  // Za std::chrono::milliseconds

float scrollOffset = 0.0f; // Initial scroll offset

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    scrollOffset += yoffset * 0.1f;
    if (scrollOffset < 0.1f) scrollOffset = 0.1f;
    if (scrollOffset > 2.0f) scrollOffset = 2.0f;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        int windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);

        Menu* menu = (Menu*)glfwGetWindowUserPointer(window);
        if (menu) {
            menu->handleMouseClick(mouseX, mouseY, windowWidth, windowHeight);
        }
    }
}

int main() {
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(1200, 1000, "Avatar Renderer", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewInit();

    glViewport(0, 0, 1200, 1000);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    Shader avatarShader("vertex.vert", "fragment.frag");
    Shader hairShader("hairVertex.vert", "hairFragment.frag");
    Avatar avatar;  // Ensure this is initialized before usage

    // Instantiate the Menu after avatar is initialized
    Menu menu(avatarShader, hairShader, avatar);
    glfwSetWindowUserPointer(window, &menu);

    const double targetFPS = 60.0;           // Ciljani FPS
    const double frameTime = 1.0 / targetFPS; // Trajanje svakog frame-a u sekundama

    while (!glfwWindowShouldClose(window)) {
        double startTime = glfwGetTime(); // Poèetak iteracije petlje

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        avatarShader.use();
        hairShader.use();

        avatar.draw(avatarShader, hairShader, 1200, 1000, scrollOffset);

        float color[] = { 1.0, 0.1, 0.1 };


        avatar.drawDress(avatarShader, hairShader, color, "ts");
        avatar.drawPants(avatarShader, hairShader, color, "ts");
        avatar.drawTshirt(avatarShader, hairShader, color, "ts");
        avatar.drawFace(hairShader);

        // Render the menu after the avatar has been rendered
        menu.render(-0.95f, 0.8f, 0.4f, 0.05f);

        glfwSwapBuffers(window);
        glfwPollEvents();
        double endTime = glfwGetTime();          // Kraj iteracije petlje
        double elapsedTime = endTime - startTime; // Vreme provedeno na render

        if (elapsedTime < frameTime) {
            std::this_thread::sleep_for(
                std::chrono::duration<double>(frameTime - elapsedTime)
            );
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
