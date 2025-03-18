#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "Avatar.h"
#include "Menu.h"
#include <thread> 
#include <chrono>  


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

    GLFWwindow* window = glfwCreateWindow(1200, 1000, "Storyboard that", nullptr, nullptr);
    glfwMaximizeWindow(window);

    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewInit();
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    Shader avatarShader("vertex.vert", "fragment.frag");
    Shader hairShader("hairVertex.vert", "hairFragment.frag");
    Avatar avatar;
    Menu menu(avatarShader, hairShader, avatar);
    glfwSetWindowUserPointer(window, &menu);

    const double targetFPS = 60.0;
    const double frameTime = 1.0 / targetFPS;

    while (!glfwWindowShouldClose(window)) {
        double startTime = glfwGetTime();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        avatarShader.use();
        hairShader.use();

        avatar.draw(avatarShader, hairShader, 1200, 1000);

        float color[] = { 1.0, 0.1, 0.1 };


        //avatar.drawDress(avatarShader, hairShader, color, "ts");
        //avatar.drawPants(avatarShader, hairShader, color, "ts");
        //avatar.drawTshirt(avatarShader, hairShader, color, "ts");
        //avatar.drawFace(hairShader);
        //avatar.drawStudent(hairShader);

        //menu.render(-0.95f, 0.8f, 0.4f, 0.05f);

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
