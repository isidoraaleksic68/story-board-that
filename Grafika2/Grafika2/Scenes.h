
#ifndef SCENES_H
#define SCENES_H

#include "Shader.h"
#include <string>
#include <unordered_map>

class Scenes {
private:
    std::unordered_map<std::string, GLuint> textureCache;

    Shader& shader;
    Shader& textureShader;

public:
    Scenes(Shader& shader, Shader& textureShader);
    ~Scenes();

    struct ScenePart {
        float x, y, width, height;
        GLuint VAO, VBO, EBO;
        GLuint textureId;
        float color[3];
    };

    struct Scene {
        std::vector<ScenePart> sceneParts;
    };
   
    std::vector<Scene> scenes;
    std::vector<Scene> activeScenes;

    GLuint loadTexture(const char* filepath);
    GLuint loadTextureCached(const std::string& filepath);

    void setupBuffers(ScenePart& scenePart);
    void setupBuffersTexture(ScenePart& scenePart);

    void drawScene(int sceneIndex, float x, float y, float width, float height);

    void setupBathroomScene();
    void setupBeachScene();
    void setupBedroomScene();
    void setupCafeScene();
    void setupCinemaScene();
    void setupCityScene();
    void setupFarmScene();
    void setupForestScene();
    void setupForestDayScene();
    void setupKitchenScene();
    void setupLakeScene();
    void setupLivingRoomScene();
    void setupNatureScene();
    void setupParkScene();
    void setupStreetScene();
    void setupRestaurantScene();
    void setupRiverScene();
    void setupSchoolScene();
    void setupSchoolInsideScene();
    void setupVillageScene();
    void setupOfficeScene();
    void setupSalonScene();
    void setupInfirmaryScene();
    void setupPoolScene();
    void setupClubScene();
    void setupBoatScene();
    void setupPlaneScene();
    void setupCafe2Scene();
    void setupRestoScene();
    void setupBistroScene();

    std::vector<Scene> getAllScenes();

    void renderActiveScenes(int currentMenuPage, int windowWidth, int windowHeight);
    int getTotalMenuPages();
};

#endif // SCENES_H