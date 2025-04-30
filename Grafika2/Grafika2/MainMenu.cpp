#include "MainMenu.h"
#include <iostream>
#include "stb_image.h"
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

MainMenu::MainMenu(Shader& shader, Shader& textureShader, Avatar& avatar, 
    Scenes& scenes, Menu& menu, Slides& slides)
    : shader(shader), textureShader(textureShader), avatar(avatar), 
    scenes(scenes), menu(menu), slides(slides)
    , selectedOption(-1), selectedScene(-1), selectedCharacter(-1) {
    menuOptions = { "Scenes", "Characters", "Items", "Speech bubbles" };
    setupMainMenuBackground();
    setupBottomMainMenu();
    totalMenuPage = 1;
    currentMenuPage = 1;
}


MainMenu::~MainMenu() {
    glDeleteBuffers(1, &menuBackgroundVAO);
    glDeleteBuffers(1, &menuBackgroundVBO);
    glDeleteVertexArrays(1, &menuBackgroundEBO);
    glDeleteBuffers(1, &bottomMenuVAO);
    glDeleteBuffers(1, &bottomMenuVBO);
    glDeleteVertexArrays(1, &bottomMenuEBO);
}


void MainMenu::setupMainMenuBackground() {
    float color[3] = { 0.949f, 0.788f, 0.769f };

    float vertices[] = {
        -1.0f,  1.0f, color[0], color[1], color[2], 1.0f, // Top-left
        -1.0f,  0.8f, color[0], color[1], color[2], 1.0f, // Bottom-left
         1.0f,  1.0f, color[0], color[1], color[2], 1.0f, // Top-right
         1.0f,  0.8f, color[0], color[1], color[2], 1.0f  // Bottom-right
    };

    unsigned int indices[] = { 0, 1, 2, 1, 3, 2 };

    glGenVertexArrays(1, &menuBackgroundVAO);
    glBindVertexArray(menuBackgroundVAO);

    glGenBuffers(1, &menuBackgroundVBO);
    glBindBuffer(GL_ARRAY_BUFFER, menuBackgroundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &menuBackgroundEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, menuBackgroundEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void MainMenu::setupBottomMainMenu() {
    float color[3] = { 0.9961f, 0.9608f, 0.9098f };


    float vertices[] = {
    -0.7f,  -0.55f, color[0], color[1], color[2], 1.0f, // Top-left
    -0.7f,  -1.0f, color[0], color[1], color[2], 1.0f, // Bottom-left
     1.0f,  -0.55f, color[0], color[1], color[2], 1.0f, // Top-right
     1.0f,  -1.0f, color[0], color[1], color[2], 1.0f  // Bottom-right
    };

    unsigned int indices[] = { 0, 1, 2, 1, 3, 2 };

    glGenVertexArrays(1, &bottomMenuVAO);
    glBindVertexArray(bottomMenuVAO);

    glGenBuffers(1, &bottomMenuVBO);
    glBindBuffer(GL_ARRAY_BUFFER, bottomMenuVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &bottomMenuEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bottomMenuEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void MainMenu::renderMainMenuBackground() {
    shader.use();
    glBindVertexArray(menuBackgroundVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void MainMenu::renderBottomMainMenu() {
    shader.use();
    glBindVertexArray(bottomMenuVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
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
    -0.5f, -0.9f, 0.0f, 0.0f,
     0.5f, -0.9f, 1.0f, 0.0f,
     0.5f,  0.9f, 1.0f, 1.0f,
    -0.5f,  0.9f, 0.0f, 1.0f
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

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x + width / 2.0f, y, 0.0f));
    model = glm::scale(model, glm::vec3(width, height, 1.0f));

    GLuint modelLoc = glGetUniformLocation(textureShader.getID(), "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);

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
    renderMainMenuBackground();
    slides.render(windowWidth, windowHeight);
    float aspectRatio = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);

    if (selectedOption != -1) {
        renderBottomMainMenu();
        renderButton(-0.7f, -0.5f, 0.25f, 0.05f, true, menuOptions[selectedOption]);
        renderSelectedOptionComponents(windowWidth, windowHeight);
    }

    if (selectedScene != -1) {
        scenes.drawScene(selectedScene, 0.0, 0.0, 1.0, 1.0);
    }

    if (selectedCharacter != -1) {
        avatar.drawCharacter(selectedCharacter, 0.0f, 0.2f, 0.5f, 0.5f);
        menu.render(0.7f, 0.7f, 0.22f, 0.04f, windowWidth, windowHeight);
    }

    for (int i = 0; i < menuOptions.size(); ++i) {
        bool isSelected = (i == selectedOption);
        float buttonX = -0.75f + i * 0.31f * aspectRatio; // Adjust button spacing based on aspect ratio
        renderButton(buttonX, y, width * aspectRatio, height, isSelected, menuOptions[i]);
    }
}


void MainMenu::renderNextButton(float x, float y, float width, float height) {
    GLuint texture = avatar.loadTexture("arrows/rightarrow.png");
    if (texture == 0) {
        std::cerr << "Error: Failed to load Add Slide texture!" << std::endl;
        return;
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // Texture coordinates are from bottom-left (0, 0) to top-right (1, 1)
    float vertices[] = {
        // positions        // tex coords
        -0.5f, -0.5f,       0.0f, 0.0f, // bottom left
         0.5f, -0.5f,       1.0f, 0.0f, // bottom right
         0.5f,  0.5f,       1.0f, 1.0f, // top right
        -0.5f,  0.5f,       0.0f, 1.0f  // top left
    };

    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3
    };

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Vertex positions
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);


    textureShader.use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x, y, 0.0f));
    model = glm::scale(model, glm::vec3(width, height, 1.0f));

    // Pass model matrix
    GLuint modelLoc = glGetUniformLocation(textureShader.getID(), "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Texture binding
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    textureShader.setInt("texture1", 0);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
};

void MainMenu::renderPreviousButton(float x, float y, float width, float height) {
    GLuint texture = avatar.loadTexture("arrows/leftarrow.png");
    if (texture == 0) {
        std::cerr << "Error: Failed to load Add Slide texture!" << std::endl;
        return;
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // Texture coordinates are from bottom-left (0, 0) to top-right (1, 1)
    float vertices[] = {
        // positions        // tex coords
        -0.5f, -0.5f,       0.0f, 0.0f, // bottom left
         0.5f, -0.5f,       1.0f, 0.0f, // bottom right
         0.5f,  0.5f,       1.0f, 1.0f, // top right
        -0.5f,  0.5f,       0.0f, 1.0f  // top left
    };

    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3
    };

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Vertex positions
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);


    textureShader.use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x, y, 0.0f));
    model = glm::scale(model, glm::vec3(width, height, 1.0f));

    // Pass model matrix
    GLuint modelLoc = glGetUniformLocation(textureShader.getID(), "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Texture binding
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    textureShader.setInt("texture1", 0);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
};

void MainMenu::renderSelectedOptionComponents(int windowWidth, int windowHeight) {
    std::string selected = menuOptions[selectedOption];
    if (selected == "Scenes") {
        totalMenuPage = scenes.getTotalMenuPages();
        scenes.renderActiveScenes(currentMenuPage, windowWidth, windowHeight);
        if (totalMenuPage > currentMenuPage) {
            renderNextButton(0.9f, -0.75f, 0.1f, 0.2f);
        }
        if (currentMenuPage > 1) {
            renderPreviousButton(0.78f, -0.75f, 0.1f, 0.2f);
        }   
    }
    if (selected == "Characters") {
        totalMenuPage = avatar.getTotalMenuPages();
        avatar.renderActiveCharacters(currentMenuPage, windowWidth, windowHeight);
        if (totalMenuPage > currentMenuPage) {
            renderNextButton(0.9f, -0.75f, 0.1f, 0.2f);
        }
        if (currentMenuPage > 1) {
            renderPreviousButton(0.78f, -0.75f, 0.1f, 0.2f);
        }
    }
};

void MainMenu::handleMouseClick(double mouseX, double mouseY, int windowWidth, int windowHeight) {
    float xNDC = (2.0f * mouseX) / windowWidth - 1.0f;
    float yNDC = 1.0f - (2.0f * mouseY) / windowHeight;

    float aspectRatio = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
    
    float buttonWidth = 0.3f * aspectRatio;
    float buttonHeight = 0.08f;

    for (int i = 0; i < menuOptions.size(); ++i) {
        float x = -0.75f + i * 0.31f * aspectRatio; // Adjust for resizing
        float y = 0.9f;

        if (xNDC >= x && xNDC <= x + buttonWidth &&
            yNDC >= y - buttonHeight && yNDC <= y + buttonHeight) {
            selectedOption = i;
            currentMenuPage = 1;
            std::cout << "Clicked on " << menuOptions[i] << std::endl;
        }
    }

    //from here to recognize clicking on the menu item
    float optionWidth = 0.2f * aspectRatio;
    float optionHeight = 0.2f;

    for (int i = 0; i < 4; i++) {
        float x = -0.45f + i * 0.25f * aspectRatio;
        float y = -0.8f;

        float halfWidth = optionWidth / 2.0f;
        float halfHeight = optionHeight / 2.0f;

        if (xNDC >= x - halfWidth && xNDC <= x + halfWidth &&
            yNDC >= y - halfHeight && yNDC <= y + halfHeight) {
            std::string selected = menuOptions[selectedOption];
            if (selected == "Scenes") {
                std::cout << "Clicked on scene index: " << i << std::endl;
                int realSceneIndex = (currentMenuPage - 1) * 4 + i;
                selectedScene = realSceneIndex;
            }
            if (selected == "Characters") {
                std::cout << "Clicked on character index: " << i << std::endl;
                int realCharacterIndex = (currentMenuPage - 1) * 4 + i;
                selectedCharacter = realCharacterIndex;

            }

        }
    }

    //to here

    // renderNextButton(0.9f, -0.75f, 0.1f, 0.2f);

    float halfWidth = 0.1f / 2.0f;
    float halfHeight = 0.2f / 2.0f;

    if (xNDC >= 0.9 - halfWidth && xNDC <= 0.9 + halfWidth &&
        yNDC >= -0.75 - halfHeight && yNDC <= -0.75 + halfHeight 
        && totalMenuPage > currentMenuPage) {
        std::cout << "Clicked on next button." << std::endl;
        currentMenuPage++;
    }

    //renderPreviousButton(0.78f, -0.75f, 0.1f, 0.2f);

    if (xNDC >= 0.78 - halfWidth && xNDC <= 0.78 + halfWidth &&
        yNDC >= -0.75 - halfHeight && yNDC <= -0.75 + halfHeight 
        && currentMenuPage > 1) {
        std::cout << "Clicked on previous button." << std::endl;
        currentMenuPage--;
    }




}


