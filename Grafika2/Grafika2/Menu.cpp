#include "menu.h"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <filesystem>

Menu::Menu(Shader& shader, Shader& textureShader, Avatar& avatar)
    : shader(shader), textureShader(textureShader), avatar(avatar), selectedOption(-1) {
    menuOptions = { "Eyes", "Lips", "Nose", "T-shirts", "Pants", "Dresses" };
    setupMenuVertices();
}

Menu::~Menu() {
    glDeleteVertexArrays(1, &menuVAO);
    glDeleteBuffers(1, &menuVBO);
}

void Menu::setupMenuVertices() {
    float vertices[] = {
         0.5f,  1.0f, 0.0f,  0.0f, 0.0f, 0.0f,
         0.5f,  -1.0f, 0.0f,  0.0f, 0.0f, 0.0f,
         1.0f,  -1.0f, 0.0f,  0.0f, 0.0f, 0.0f,
         0.5f,  1.0f, 0.0f,  0.0f, 0.0f, 0.0f,
         1.0f,  -1.0f, 0.0f,  0.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 0.0f
    };

    glGenVertexArrays(1, &menuVAO);
    glGenBuffers(1, &menuVBO);

    glBindVertexArray(menuVAO);

    glBindBuffer(GL_ARRAY_BUFFER, menuVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Menu::renderButton(float x, float y, float width, float height, bool isSelected, std::string selectedOption) {
    float color[3] = { 0.8f, 0.8f, 0.8f };
    if (isSelected) {
        color[0] = 0.5f;
    }

    GLuint mouthTexture = 0;
    mouthTexture = avatar.loadTexture(("Buttons/" + selectedOption + ".png").c_str());

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

    // Use the shader program and set up texture
    textureShader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mouthTexture);
    textureShader.setInt("texture1", 0);

    // Draw the textured quad
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Cleanup
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);


    glDeleteTextures(1, &mouthTexture); // Properly clean up texture
}

void Menu::handleMouseClick(double mouseX, double mouseY, int windowWidth, int windowHeight) {
    float xNDC = (2.0f * mouseX) / windowWidth - 1.0f;
    float yNDC = 1.0f - (2.0f * mouseY) / windowHeight;

    float buttonWidth = 0.4f;
    float buttonHeight = 0.1f;

    for (int i = 0; i < menuOptions.size(); ++i) {
        float x = -0.8f;
        float y = 0.8f - i * 0.2f;

        if (xNDC >= x && xNDC <= x + buttonWidth &&
            yNDC >= y && yNDC <= y + buttonHeight) {
            selectedOption = i;
            std::cout << "Clicked on " << menuOptions[i] << std::endl;

            std::string nextFile = getNextFile(menuOptions[i]);
            if (!nextFile.empty()) {
                GLuint textureID = avatar.loadTexture(nextFile.c_str());
                if (menuOptions[i] == "Lips") {
                    avatar.setMouthTexture(textureID);
                    avatar.drawMouth(shader);
                }
                else if (menuOptions[i] == "Eyes") {
                    avatar.setEyeTexture(textureID);
                }
                else if (menuOptions[i] == "Nose") {
                    avatar.setNoseTexture(textureID);
                }
                else if (menuOptions[i] == "Dresses") {
                    avatar.setTshirtTexture(0);
                    avatar.setPantsTexture(0);
                    avatar.setDressTexture(textureID);
                }
                else if (menuOptions[i] == "T-shirts") {
                    avatar.setDressTexture(0);
                    avatar.setTshirtTexture(textureID);
                }
                else if (menuOptions[i] == "Pants") {
                    avatar.setDressTexture(0);
                    avatar.setPantsTexture(textureID);
                }
            }
            else {
                std::cout << "No more files in folder: " << menuOptions[i] << std::endl;
            }
            break;
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


void Menu::render(float x, float y, float width, float height) {
    shader.use();
    glBindVertexArray(menuVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    for (int i = 0; i < menuOptions.size(); ++i) {
        bool isSelected = (i == selectedOption);
        renderButton(x, y - i * 0.2f, width, height, isSelected, menuOptions[i]);
    }
}
