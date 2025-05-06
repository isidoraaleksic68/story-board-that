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

    void render(float x, float y, float width, float height, int windowWidth, int windowHeight);
    void handleMouseClick(double mouseX, double mouseY, int windowWidth, int windowHeight);
    Avatar::Character* character;

private:
    Shader& shader;
    Shader& textureShader;
    Avatar& avatar;
    
    std::vector<std::string> editOptions;
    std::vector<std::string> mainOptions;
    std::vector<std::string> resizeOptions;

    int editOption;
    int mainOption;
    int resizeOption;

    std::unordered_map<std::string, int> buttonFileIndices;

    GLuint menuVAO, menuVBO, menuEBO;

    void renderButton(float x, float y, float width, float height, 
        std::string selectedOption, std::string menuFolder);
    void renderImagesInLipsContainer(const std::string& folderPath);
    std::string getNextFile(const std::string& folderPath);

    void setupMenuBackground(); 
    void renderMenuBackground();

    void renderPreviousButton(float x, float y, float width, float height);

    void renderLabel(float x, float y, const std::string& text);
    void renderInputBox(float x, float y, float width, float height, float value);
};

#endif
