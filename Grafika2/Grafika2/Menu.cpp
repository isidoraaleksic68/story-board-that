#include "Menu.h"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <filesystem>
#include <sstream>    // For std::stringstream
#include <iomanip>    // For std::fixed and std::setprecision

Menu::Menu(Shader& shader, Shader& textureShader, Avatar& avatar)
    : shader(shader), textureShader(textureShader), avatar(avatar),
    editOption(-1), mainOption(-1), resizeOption(-1), character(nullptr){
    editOptions = { "Eyes", "Eyebrows" ,"Lips", "Nose", "Hair", "T-shirts", "Pants"};
    mainOptions = { "Resize", "Edit" };
    resizeOptions = {"Height", "Width", "X", "Y"};
    setupMenuBackground();
}

Menu::~Menu() {
    glDeleteVertexArrays(1, &menuVAO);
    glDeleteBuffers(1, &menuVBO);
    glDeleteBuffers(1, &menuEBO);
}


void Menu::setupMenuBackground() {
    float color[3] = { 1.0f, 0.8f, 0.8f };


    float vertices[] = {
        0.6f,  0.8f, 0.0f, color[0], color[1], color[2], 1.0f,  0.0f, 1.0f, // Top-left
        0.6f,  -0.55f, 0.0f, color[0], color[1], color[2], 1.0f, 1.0f, 1.0f, // Bottom-left
         1.0f,  0.8f, 0.0f, color[0], color[1], color[2], 1.0f, 1.0f, 0.0f, // Top-right
         1.0f,  -0.55f, 0.0f,  color[0], color[1], color[2], 1.0f, 0.0f, 0.0f // Bottom-right
    };

    unsigned int indices[] = { 0, 1, 2, 1, 3, 2 };

    glGenVertexArrays(1, &menuVAO);
    glGenBuffers(1, &menuVBO);
    glGenBuffers(1, &menuEBO);

    glBindVertexArray(menuVAO);
    glBindBuffer(GL_ARRAY_BUFFER, menuVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, menuEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void Menu::renderMenuBackground() {
    glBindVertexArray(menuVAO);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f));
    model = glm::scale(model, glm::vec3(1.0f));

    Shader* shaderToUse = nullptr;

    shaderToUse = &shader;
    shaderToUse->use();

    // Force upload model and useTexture uniforms
    shaderToUse->setMat4("model", model);
    shaderToUse->setBool("useTexture", false);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // --- Cleanup ---
    glBindVertexArray(0);
}


void Menu::renderButton(float x, float y, float width, float height,
    std::string selectedOption, std::string menuFolder) {
    GLuint mouthTexture = 0;
    mouthTexture = avatar.loadTexture(( menuFolder + "/" + selectedOption + ".png").c_str());

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    float vertices[] = {
        // Positions         // Texture Coords
        x, y - height,  0.0f, 0.0f, // Bottom-left
        x + width, y - height, 1.0f, 0.0f, // Bottom-right
        x + width, y + height, 1.0f, 1.0f, // Top-right
        x, y + height , 0.0f, 1.0f  // Top-left
    };


    unsigned int indices[] = {
        0, 1, 2, // First triangle
        0, 2, 3  // Second triangle
    };

    unsigned int VAO, VBO, EBO;

    // Generate and bind VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Generate and bind VBO
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Generate and bind EBO
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Define vertex attributes
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float))); // Texture coordinates
    glEnableVertexAttribArray(1);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3( 0.0f));
    model = glm::scale(model, glm::vec3(1.0f));
    Shader* shaderToUse = nullptr;

    shaderToUse = &textureShader;
    shaderToUse->use();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // Always bind texture and upload sampler uniform
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mouthTexture);
    shaderToUse->setInt("texture1", 0);

    // Force upload model and useTexture uniforms
    shaderToUse->setMat4("model", model);
    shaderToUse->setBool("useTexture", true);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Cleanup
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
}

void Menu::handleMouseClick(double mouseX, double mouseY, int windowWidth, int windowHeight) {
    float aspectRatio = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);

    float xNDC = (2.0f * mouseX) / windowWidth - 1.0f;
    float yNDC = 1.0f - (2.0f * mouseY) / windowHeight;

    float buttonX = 0.7f;
    float buttonYStart = 0.7f;
    float buttonWidth = 0.22f;
    float buttonHeight = 0.04f * aspectRatio;
    float spacing = 0.1f * aspectRatio;

    float resizeButtonSpacing = 0.2f * aspectRatio;
    float resizeButtonXMinus = 0.7f;
    float resizeButtonXPlus = 0.7f + buttonWidth / 2;
    float resizeButtonYStart = 0.7f - 0.05f - buttonHeight * 2;
    float resizeButtonWidth = buttonWidth / 2;
    float resizeButtonHeight = buttonHeight;

    if (mainOption == -1) {
        for (int i = 0; i < mainOptions.size(); ++i) {
            float y = buttonYStart - i * spacing;

            if (xNDC >= buttonX && xNDC <= buttonX + buttonWidth &&
                yNDC >= y - buttonHeight && yNDC <= y + buttonHeight) {
                std::cout << "Clicked on " << mainOptions[i] << std::endl;
                mainOption = i;
                break;
            }
        }
    }
    else if (mainOption == 0) {
        const float slideX = 0.0f;
        const float slideY = 0.2f;
        const float slideWidth = 1.1f;
        const float slideHeight = 1.1f;

        for (int i = 0; i < resizeOptions.size(); ++i) {
            float y = resizeButtonYStart - i * resizeButtonSpacing;

            bool clickedMinus = (xNDC >= resizeButtonXMinus && xNDC <= resizeButtonXMinus + resizeButtonWidth &&
                yNDC >= y - resizeButtonHeight && yNDC <= y + resizeButtonHeight);

            bool clickedPlus = (xNDC >= resizeButtonXPlus && xNDC <= resizeButtonXPlus + resizeButtonWidth &&
                yNDC >= y - resizeButtonHeight && yNDC <= y + resizeButtonHeight);

            if (clickedMinus || clickedPlus) {
                resizeOption = i;
                float delta = clickedPlus ? 0.05f : -0.05f;
                float slideLimitWidth = clickedPlus ? slideX + slideWidth / 2 : slideX - slideWidth / 2;
                float slideLimitHeight = clickedPlus ? slideY + slideHeight / 2 : slideY - slideHeight / 2;
                bool maxLimitX = false;
                bool minLimitX = false;

                switch (resizeOption) {
                case 0: { // Height
                    if (character->height + delta >= 0.05f) {
                        float newHalfHeight = (character->height/2 + delta);
                        float bottomEdge = character->y - newHalfHeight;
                        float topEdge = character->y + newHalfHeight;

                        if (bottomEdge >= slideY - slideHeight/2 + 0.27f && topEdge <= slideY + slideHeight/2 - 0.08f) {
                            character->height += delta;
                        }
                    }
                    break;
                }
                case 1: // Width
                    if (character->width + delta >= 0.05f) {
                        float newHalfWidth = (character->width/2 + delta);
                        float leftEdge = character->x - newHalfWidth;
                        float rightEdge = character->x + newHalfWidth;

                        if (leftEdge >= slideX - slideWidth / 2 - 0.08f && rightEdge <= slideX + slideWidth / 2 + 0.08f) {
                            character->width += delta;
                        }
                    }
                    break;
                case 2: // X
                    if (clickedPlus) {
                        if (character->x + character->width / 2 + delta <= slideX + slideWidth / 2 + 0.08f) {
                            character->x += delta;
                        }
                        break;
                    }
                    else {
                        if (character->x - character->width / 2 + delta >= slideX - slideWidth / 2 - 0.08f) {
                            character->x += delta;
                        }
                        break;
                    }
                case 3: // Y
                    if (clickedPlus) {
                        if (character->y + character->height / 2 + delta <= slideY + slideHeight / 2 - 0.08f) {
                            character->y += delta;
                        }
                        break;
                    }
                    else {
                        if (character->y - character->height / 2 + delta >= slideY - slideHeight / 2 + 0.27f) {
                            character->y += delta;
                        }
                        break;
                    }
                }

                std::cout << "Clicked on " << (clickedPlus ? "plus" : "minus") << " " << resizeOptions[i] << std::endl;
                break;
            }
        }


        //for previous button
        //renderPreviousButton(0.64f, 0.75f, 0.08f, 0.1f);

        if (xNDC >= 0.64f - 0.04f && xNDC <= 0.64f + 0.04f &&
            yNDC >= 0.75f - 0.05f && yNDC <= 0.75f + 0.05f) {
            std::cout << "Clicked on previous button" << std::endl;
            mainOption = -1;
        }


    }
    else if (mainOption == 1) {
        for (int i = 0; i < editOptions.size(); ++i) {
            float y = buttonYStart - i * spacing;

            if (xNDC >= buttonX && xNDC <= buttonX + buttonWidth &&
                yNDC >= y - buttonHeight && yNDC <= y + buttonHeight) {
                std::cout << "Clicked on " << editOptions[i] << std::endl;
                editOption = i;
                break;
            }
        }

        //for previous button
        //renderPreviousButton(0.64f, 0.75f, 0.08f, 0.1f);

        if (xNDC >= 0.64f - 0.04f && xNDC <= 0.64f + 0.04f &&
            yNDC >= 0.75f - 0.05f && yNDC <= 0.75f + 0.05f) {
            std::cout << "Clicked on previous button" << std::endl;
            mainOption = -1;
        }
    }
}



std::string Menu::getNextFile(const std::string& folderPath) {
    namespace fs = std::filesystem;
    std::vector<std::string> imagePaths;

    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".png") {
            imagePaths.push_back(entry.path().string());
        }
    }

    if (imagePaths.empty()) {
        return "";
    }

    // Sort files to maintain consistency
    std::sort(imagePaths.begin(), imagePaths.end());

    if (buttonFileIndices.find(folderPath) == buttonFileIndices.end()) {
        buttonFileIndices[folderPath] = 0;
    }

    int& currentIndex = buttonFileIndices[folderPath];
    std::string nextFile = imagePaths[currentIndex];

    // Convert path separators for consistency
    std::replace(nextFile.begin(), nextFile.end(), '\\', '/');

    std::cout << nextFile<< std::endl;
    currentIndex = (currentIndex + 1) % imagePaths.size();
    return nextFile;
}



void Menu::renderPreviousButton(float x, float y, float width, float height) {
    GLuint texture = avatar.loadTexture("arrows/previous.png");
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


void Menu::renderImagesInLipsContainer(const std::string& folderPath) {
    namespace fs = std::filesystem;
    std::vector<std::string> imagePaths;

    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".png") {
            size_t pos = entry.path().string().find('\\');
            if (pos != std::string::npos) {
                entry.path().string().replace(pos, 1, "/");
            }

            std::cout << entry.path().string() << std::endl;

            imagePaths.push_back(entry.path().string());
        }
    }
}

void Menu::renderInputBox(float x, float y, float width, float height, float value) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << value;
    renderButton(x, y, width, height, ss.str(), "input");
}


void Menu::render(float x, float y, float width, float height, int windowWidth, int windowHeight) {
    renderMenuBackground();

    float aspectRatio = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
    float buttonSpacing = 0.1f * aspectRatio;
    float resizeButtonSpacing = 0.2f * aspectRatio;
    float buttonHeight = height * aspectRatio;

    if (mainOption == -1) {
        for (int i = 0; i < mainOptions.size(); ++i) {
            float yPos = y - i * buttonSpacing;
            renderButton(x, yPos, width, buttonHeight, mainOptions[i], "menuButtons");
        }
    }

    if (mainOption == 0) {
        renderPreviousButton(0.64f, 0.75f, 0.08f, 0.1f);
        for (int i = 0; i < resizeOptions.size(); ++i) {
            float yPos = y - 0.05 - i * resizeButtonSpacing;
            renderButton(x, yPos, width, buttonHeight, resizeOptions[i], "resizeButtons");
            renderButton(x, yPos - buttonHeight*2, width/2, buttonHeight, "minus", "resizeButtons");
            renderButton(x + width/2, yPos - buttonHeight*2, width/2, buttonHeight, "plus", "resizeButtons");
        }

    }


    if (mainOption == 1) {
        renderPreviousButton(0.64f, 0.75f, 0.08f, 0.1f);
        for (int i = 0; i < editOptions.size(); ++i) {
            float yPos = y - i * buttonSpacing;
            renderButton(x, yPos, width, buttonHeight, editOptions[i], "editButtons");
        }
    }
}

