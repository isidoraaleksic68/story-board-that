#ifndef MENU_H
#define MENU_H

#include <vector>
#include <string>
#include "Shader.h"
#include "Avatar.h"
#include <unordered_map>

class Menu {
public:
    Menu(Shader& avatarShader, Shader& textureShader, Avatar& avatar);
    ~Menu();

    void render(float x, float y, float width, float height);
    void handleMouseClick(double mouseX, double mouseY, int windowWidth, int windowHeight);

private:
    Shader& shader;
    Shader& textureShader;
    Avatar& avatar;
    std::vector<std::string> menuOptions;
    int selectedOption;
    std::unordered_map<std::string, int> buttonFileIndices;

    GLuint menuVAO, menuVBO;

    void setupMenuVertices();
    void renderButton(float x, float y, float width, float height, bool isSelected, std::string selectedOption);
    void renderImagesInLipsContainer(const std::string& folderPath);
    std::string getNextFile(const std::string& folderPath);
};

#endif
