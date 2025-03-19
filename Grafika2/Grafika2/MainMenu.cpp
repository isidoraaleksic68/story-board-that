#include "MainMenu.h"
#include <iostream>
#include "stb_image.h"
#include <filesystem>

MainMenu::MainMenu(Shader& shader, Shader& textureShader, Avatar& avatar)
    : shader(shader), textureShader(textureShader), avatar(avatar), selectedOption(-1) {
    menuOptions = { "Scenes", "Characters", "Items", "Speech bubbles" };
}

MainMenu::~MainMenu() {
    glDeleteVertexArrays(1, &menuVAO);
    glDeleteBuffers(1, &menuVBO);
}

void MainMenu::renderButton(float x, float y, float width, float height, bool isSelected, std::string selectedOption) {
    float color[3] = { 0.8f, 0.8f, 0.8f };
    if (isSelected) {
        color[0] = 0.5f;
    }

    GLuint buttonTexture = avatar.loadTexture(("mainMenuButtons/" + selectedOption + ".png").c_str());

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float vertices[] = {
        x, y - height,  0.0f, 0.0f,
        x + width, y - height, 1.0f, 0.0f,
        x + width, y + height, 1.0f, 1.0f,
        x, y + height , 0.0f, 1.0f
    };

    unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    textureShader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, buttonTexture);
    textureShader.setInt("texture1", 0);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void MainMenu::render(float x, float y, float width, float height, int windowWidth, int windowHeight) {
    shader.use();
    glBindVertexArray(menuVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    float aspectRatio = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);

    for (int i = 0; i < menuOptions.size(); ++i) {
        bool isSelected = (i == selectedOption);
        float buttonX = -0.9f + i * 0.31f * aspectRatio; // Adjust button spacing based on aspect ratio
        renderButton(buttonX, y, width * aspectRatio, height, isSelected, menuOptions[i]);
    }
}

void MainMenu::handleMouseClick(double mouseX, double mouseY, int windowWidth, int windowHeight) {
    float xNDC = (2.0f * mouseX) / windowWidth - 1.0f;
    float yNDC = 1.0f - (2.0f * mouseY) / windowHeight;

    float aspectRatio = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);

    float buttonWidth = 0.3f * aspectRatio;
    float buttonHeight = 0.1f;

    for (int i = 0; i < menuOptions.size(); ++i) {
        float x = -0.9f + i * 0.31f * aspectRatio; // Adjust for resizing
        float y = 0.9f;

        if (xNDC >= x && xNDC <= x + buttonWidth &&
            yNDC >= y - buttonHeight && yNDC <= y + buttonHeight) {
            selectedOption = i;
            std::cout << "Clicked on " << menuOptions[i] << std::endl;
        }
    }
}


