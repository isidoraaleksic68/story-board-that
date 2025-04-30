#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <vector>
#include <string>
#include "Shader.h"
#include "Scenes.h"
#include <unordered_map>
#include "Menu.h"
#include "Slides.h"

class MainMenu {
public:
    MainMenu(Shader& avatarShader, Shader& textureShader, Avatar& avatar, 
        Scenes& scenes, Menu& menu, Slides& slides);
    ~MainMenu();

    void render(float x, float y, float width, float height,int windowWidth, int windowHeight);
    void handleMouseClick(double mouseX, double mouseY, int windowWidth, int windowHeight);

private:
    Shader& shader;
    Shader& textureShader;
    Avatar& avatar;
    Scenes& scenes;
    Slides& slides;
    Menu& menu;

    std::vector<std::string> menuOptions;
    int selectedOption;
    int selectedScene;
    int selectedCharacter;
    std::unordered_map<std::string, int> buttonFileIndices;

    unsigned int menuBackgroundVAO, menuBackgroundVBO, menuBackgroundEBO;
    unsigned int bottomMenuVAO, bottomMenuVBO, bottomMenuEBO;

    void renderButton(float x, float y, 
        float width, float height,
        bool isSelected, std::string selectedOption);

    void setupMainMenuBackground();
    void setupBottomMainMenu();
    void renderMainMenuBackground();
    void renderBottomMainMenu();
    void renderSelectedOptionComponents(int windowWidth, int windowHeight);
    void renderNextButton(float x, float y, float width, float height);
    void renderPreviousButton(float x, float y, float width, float height);

    int totalMenuPage;
    int currentMenuPage;

};

#endif
