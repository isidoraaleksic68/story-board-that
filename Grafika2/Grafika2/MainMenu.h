#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <vector>
#include <string>
#include "Shader.h"
#include "Avatar.h"
#include <unordered_map>

class MainMenu {
public:
    MainMenu(Shader& avatarShader, Shader& textureShader, Avatar& avatar);
    ~MainMenu();

    void render(float x, float y, float width, float height,int windowWidth, int windowHeight);
    void handleMouseClick(double mouseX, double mouseY, int windowWidth, int windowHeight);
    void renderMainMenuBackground();

private:
    Shader& shader;
    Shader& textureShader;
    Avatar& avatar;
    std::vector<std::string> menuOptions;
    int selectedOption;
    std::unordered_map<std::string, int> buttonFileIndices;

    GLuint menuVAO, menuVBO;

    void renderButton(float x, float y, float width, float height, bool isSelected, std::string selectedOption);
   // void renderImagesInLipsContainer(const std::string& folderPath);
    //std::string getNextFile(const std::string& folderPath);
};

#endif
