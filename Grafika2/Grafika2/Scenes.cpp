#include "Scenes.h"
#include "stb_image.h"
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



Scenes::Scenes(Shader& shader, Shader& textureShader)
    : shader(shader), textureShader(textureShader){

    setupBathroomScene();
    setupBeachScene();
    setupBedroomScene();
    setupCafeScene();
    setupCinemaScene();
    setupCityScene();
    setupFarmScene();
    setupForestScene();
    setupForestDayScene();
    setupKitchenScene();
    setupLakeScene();
    setupLivingRoomScene();
    setupNatureScene();
    setupParkScene();
    setupStreetScene();
    setupRestaurantScene();
    setupRiverScene();
    setupSchoolScene();
    setupSchoolInsideScene();
    setupVillageScene();
    setupOfficeScene();
    setupSalonScene();
    setupInfirmaryScene();
};


Scenes::~Scenes() {
    for (auto& scene : scenes) {
        for (auto& part : scene.sceneParts) {
            glDeleteBuffers(1, &part.VBO);
            glDeleteBuffers(1, &part.EBO);
            glDeleteVertexArrays(1, &part.VAO);
        }
    }
}

GLuint Scenes::loadTexture(const char* filepath) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filepath, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else {
        std::cerr << "Failed to load texture: " << filepath << std::endl;
    }
    stbi_image_free(data);
    return textureID;
}


GLuint Scenes::loadTextureCached(const std::string& filepath) {
    if (textureCache.find(filepath) != textureCache.end()) {
        return textureCache[filepath];
    }
    GLuint textureID = loadTexture(filepath.c_str());
    textureCache[filepath] = textureID;
    return textureID;
}

void Scenes::drawScene(int sceneIndex, float x, float y, float width, float height) {
    if (sceneIndex < 0 || sceneIndex >= static_cast<int>(scenes.size())) {
        return;
    }

    Scene& scene = scenes[sceneIndex];

    for (auto& part : scene.sceneParts) {
        glBindVertexArray(part.VAO);

        // Compute model matrix
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x, y, 0.0f));
        model = glm::scale(model, glm::vec3(width, height, 1.0f));

        Shader* shaderToUse = nullptr;

        // Case 1: Texture is used
        if (part.textureId != 0) {
            shaderToUse = &textureShader;
            shaderToUse->use();
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


            // Always bind texture and upload sampler uniform
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, part.textureId);
            shaderToUse->setInt("texture1", 0);

            // Force upload model and useTexture uniforms
            shaderToUse->setMat4("model", model);
            shaderToUse->setBool("useTexture", true);
        }
        // Case 2: No texture
        else {
            shaderToUse = &shader;
            shaderToUse->use();

            // Force upload model and useTexture uniforms
            shaderToUse->setMat4("model", model);
            shaderToUse->setBool("useTexture", false);
        }

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Cleanup
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
    }
}

void Scenes::setupBuffers(ScenePart& scenePart) {

    GLfloat vertices[] = {
        // Positions       // Colors          // Texture Coords
        scenePart.x - scenePart.width / 2, scenePart.y - scenePart.height / 2, 0.0f,  scenePart.color[0], scenePart.color[1], scenePart.color[2], 1.0f,  0.0f, 1.0f, // Top-left
        scenePart.x + scenePart.width / 2, scenePart.y - scenePart.height / 2, 0.0f,  scenePart.color[0], scenePart.color[1], scenePart.color[2], 1.0f,  1.0f, 1.0f, // Top-right
        scenePart.x + scenePart.width / 2, scenePart.y + scenePart.height / 2, 0.0f,  scenePart.color[0], scenePart.color[1], scenePart.color[2], 1.0f,  1.0f, 0.0f, // Bottom-right
        scenePart.x - scenePart.width / 2, scenePart.y + scenePart.height / 2, 0.0f,  scenePart.color[0], scenePart.color[1], scenePart.color[2], 1.0f,  0.0f, 0.0f  // Bottom-left
    };

    GLuint indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    glGenVertexArrays(1, &scenePart.VAO);
    glGenBuffers(1, &scenePart.VBO);
    glGenBuffers(1, &scenePart.EBO);

    glBindVertexArray(scenePart.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, scenePart.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, scenePart.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
};


void Scenes::setupBuffersTexture(ScenePart& scenePart) {
    float vertices[] = {
        // Positions        // Texture Coords
        scenePart.x - scenePart.width / 2, scenePart.y - scenePart.height / 2, 0.0f,  0.0f, 0.0f, // Bottom-left
        scenePart.x + scenePart.width / 2, scenePart.y - scenePart.height / 2, 0.0f,  1.0f, 0.0f, // Bottom-right
        scenePart.x + scenePart.width / 2, scenePart.y + scenePart.height / 2, 0.0f,  1.0f, 1.0f, // Top-right
        scenePart.x - scenePart.width / 2, scenePart.y + scenePart.height / 2, 0.0f,  0.0f, 1.0f  // Top-left
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    glGenVertexArrays(1, &scenePart.VAO);
    glGenBuffers(1, &scenePart.VBO);
    glGenBuffers(1, &scenePart.EBO);

    glBindVertexArray(scenePart.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, scenePart.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, scenePart.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute (x, y, z)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coord attribute (u, v)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}



void Scenes::setupParkScene() {
    Scene scene;

    ScenePart park;
    GLuint texture = loadTexture("Scenes/park (2).png");
    if (texture == 0) {
        std::cerr << "Error: Failed to load texture!" << std::endl;
        return;
    }
    park.textureId = texture;
    park.height = 1.1f;
    park.width = 1.1f;
    park.x = 0.0f;
    park.y = 0.2f;
    setupBuffersTexture(park);
    
    scene.sceneParts.push_back(park);

    scenes.push_back(scene);
}

void Scenes::setupBathroomScene() {
    Scene scene;

    ScenePart bathroom;
    GLuint texture = loadTexture("scenes/bathroom (2).png");
    if (texture == 0) {
        std::cerr << "Error: Failed to load texture!" << std::endl;
        return;
    }
    bathroom.textureId = texture;
    bathroom.height = 1.1f;
    bathroom.width = 1.1f;
    bathroom.x = 0.0f;
    bathroom.y = 0.2f;
    setupBuffersTexture(bathroom);
    scene.sceneParts.push_back(bathroom);
    scenes.push_back(scene);
};

void Scenes::setupBeachScene() {
    Scene scene;

    ScenePart beach;
    GLuint texture = loadTexture("scenes/beach.png");
    if (texture == 0) {
        std::cerr << "Error: Failed to load texture!" << std::endl;
        return;
    }
    beach.textureId = texture;
    beach.height = 1.22f;
    beach.width = 1.135f;
    beach.x = 0.0f;
    beach.y = 0.3f;
    setupBuffersTexture(beach);

    ScenePart floor;
    floor.textureId = 0;
    floor.height = 0.27f;
    floor.width = 1.1f;
    floor.x = 0.0f;
    floor.y = -0.25f;
    floor.color[0] = 0.9961f;
    floor.color[1] = 1.0f;
    floor.color[2] = 0.9294f;

    setupBuffers(floor);

    scene.sceneParts.push_back(beach);
    scene.sceneParts.push_back(floor);

    scenes.push_back(scene);
};

void Scenes::setupBedroomScene() {
    Scene scene;

    ScenePart bedroom;
    GLuint texture = loadTexture("scenes/bedroom (2).png");
    if (texture == 0) {
        std::cerr << "Error: Failed to load texture!" << std::endl;
        return;
    }
    bedroom.textureId = texture;
    bedroom.height = 1.1f;
    bedroom.width = 1.1f;
    bedroom.x = 0.0f;
    bedroom.y = 0.2f;
    setupBuffersTexture(bedroom);
    scene.sceneParts.push_back(bedroom);
    scenes.push_back(scene);
    
};

void Scenes::setupCafeScene() {
    Scene scene;

    ScenePart cafe;
    GLuint texture = loadTexture("scenes/cafe.png");
    if (texture == 0) {
        std::cerr << "Error: Failed to load texture!" << std::endl;
        return;
    }
    cafe.textureId = texture;
    cafe.height = 1.0f;
    cafe.width = 1.1f;
    cafe.x = 0.0f;
    cafe.y = 0.25f;
    setupBuffersTexture(cafe);

    ScenePart floor;
    floor.textureId = 0;
    floor.height = 0.17f;
    floor.width = 1.1f;
    floor.x = 0.0f;
    floor.y = -0.27f;
    
    //float color[3] = { 0.6510f, 0.6510f, 0.6510f };

    floor.color[0] = 0.6510f;
    floor.color[1] = 0.6510f;
    floor.color[2] = 0.6510f;

    setupBuffers(floor);

    scene.sceneParts.push_back(cafe);
    scene.sceneParts.push_back(floor);

    scenes.push_back(scene);
};

void Scenes::setupCinemaScene() {
    Scene scene;

    ScenePart cinema;
    GLuint texture = loadTexture("scenes/cinema.png");
    if (texture == 0) {
        std::cerr << "Error: Failed to load texture!" << std::endl;
        return;
    }
    cinema.textureId = texture;
    cinema.height = 1.2f;
    cinema.width = 1.1f;
    cinema.x = 0.0f;
    cinema.y = 0.2f;
    setupBuffersTexture(cinema);

    ScenePart background;
    background.textureId = 0;
    background.height = 1.1f;
    background.width = 1.1f;
    background.x = 0.0f;
    background.y = 0.2f;
    
    //float color[3] = { 0.8706f, 0.8118f, 0.8118f };

    background.color[0] = 0.8706f;
    background.color[1] = 0.8118f;
    background.color[2] = 0.8118f;

    setupBuffers(background);

    scene.sceneParts.push_back(background);
    scene.sceneParts.push_back(cinema);

    scenes.push_back(scene);
};

void Scenes::setupCityScene() {
    Scene scene;
    ScenePart city;
    GLuint texture = loadTexture("scenes/city (2).png");
    if (texture == 0) {
        std::cerr << "Error: Failed to load texture!" << std::endl;
        return;
    }
    city.textureId = texture;
    city.height = 1.1f;
    city.width = 1.1f;
    city.x = 0.0f;
    city.y = 0.2f;
    setupBuffersTexture(city);
    scene.sceneParts.push_back(city);
    scenes.push_back(scene);
};

void Scenes::setupFarmScene() {
    Scene scene;
    ScenePart farm;
    GLuint texture = loadTexture("scenes/farm (2).png");
    if (texture == 0) {
        std::cerr << "Error: Failed to load texture!" << std::endl;
        return;
    }
    farm.textureId = texture;
    farm.height = 1.1f;
    farm.width = 1.1f;
    farm.x = 0.0f;
    farm.y = 0.2f;
    setupBuffersTexture(farm);
    scene.sceneParts.push_back(farm);
    scenes.push_back(scene);
};

void Scenes::setupForestScene() {
    Scene scene;

    ScenePart forest;
    GLuint texture = loadTexture("scenes/forest (2).png");
    if (texture == 0) {
        std::cerr << "Error: Failed to load texture!" << std::endl;
        return;
    }
    forest.textureId = texture;
    forest.height = 1.1f;
    forest.width = 1.1f;
    forest.x = 0.0f;
    forest.y = 0.2f;
    setupBuffersTexture(forest);
    scene.sceneParts.push_back(forest);
    scenes.push_back(scene);
};

void Scenes::setupForestDayScene() {
    Scene scene;

    ScenePart forest;
    GLuint texture = loadTexture("scenes/forestday.png");
    if (texture == 0) {
        std::cerr << "Error: Failed to load texture!" << std::endl;
        return;
    }
    forest.textureId = texture;
    forest.height = 1.1f;
    forest.width = 1.1f;
    forest.x = 0.0f;
    forest.y = 0.2f;
    setupBuffersTexture(forest);

    ScenePart sky;
    sky.textureId = 0;
    sky.height = 1.1f;
    sky.width = 1.1f;
    sky.x = 0.0f;
    sky.y = 0.2f;
    sky.color[0] = 0.4118f;
    sky.color[1] = 0.8275f;
    sky.color[2] = 0.9412f;

    setupBuffers(sky);

    scene.sceneParts.push_back(sky);
    scene.sceneParts.push_back(forest);

    scenes.push_back(scene);
}

void Scenes::setupKitchenScene() {
    Scene scene;

    ScenePart kitchen;
    GLuint texture = loadTexture("scenes/kitchen.png");
    if (texture == 0) {
        std::cerr << "Error: Failed to load texture!" << std::endl;
        return;
    }
    kitchen.textureId = texture;
    kitchen.height = 1.1f;
    kitchen.width = 1.1f;
    kitchen.x = 0.0f;
    kitchen.y = 0.2f;
    setupBuffersTexture(kitchen);

    ScenePart background;
    background.textureId = 0;
    background.height = 1.1f;
    background.width = 1.1f;
    background.x = 0.0f;
    background.y = 0.2f;

    background.color[0] = 0.8706f;
    background.color[1] = 0.8118f;
    background.color[2] = 0.8118f;

    setupBuffers(background);

    scene.sceneParts.push_back(background);
    scene.sceneParts.push_back(kitchen);

    scenes.push_back(scene);
};

void Scenes::setupLakeScene() {
    Scene scene;
    ScenePart lake;
    GLuint texture = loadTexture("scenes/lake (2).png");
    if (texture == 0) {
        std::cerr << "Error: Failed to load texture!" << std::endl;
        return;
    }
    lake.textureId = texture;
    lake.height = 1.1f;
    lake.width = 1.1f;
    lake.x = 0.0f;
    lake.y = 0.2f;
    setupBuffersTexture(lake);
    scene.sceneParts.push_back(lake);
    scenes.push_back(scene);
};

void Scenes::setupLivingRoomScene() {
    Scene scene;

    ScenePart livingRoom;
    GLuint texture = loadTexture("scenes/living room.png");
    if (texture == 0) {
        std::cerr << "Error: Failed to load texture!" << std::endl;
        return;
    }
    livingRoom.textureId = texture;
    livingRoom.height = 1.1f;
    livingRoom.width = 1.1f;
    livingRoom.x = 0.0f;
    livingRoom.y = 0.2f;
    setupBuffersTexture(livingRoom);

    ScenePart wall;
    wall.textureId = 0;
    wall.height = 0.8f;
    wall.width = 1.1f;
    wall.x = 0.0f;
    wall.y = 0.35f;
    float color[3] = { 0.7216f, 0.4667f, 0.5490f };
    wall.color[0] = color[0];
    wall.color[1] = color[1];
    wall.color[2] = color[2];
    setupBuffers(wall);

    ScenePart floor;
    floor.textureId = 0;
    floor.height = 0.6f;
    floor.width = 1.1f;
    floor.x = 0.0f;
    floor.y = -0.05f;
    float color1[3] = { 0.6314f, 0.6118f, 0.5176f };
    floor.color[0] = color1[0];
    floor.color[1] = color1[1];
    floor.color[2] = color1[2];
    setupBuffers(floor);

    scene.sceneParts.push_back(floor);
    scene.sceneParts.push_back(wall);
    scene.sceneParts.push_back(livingRoom);

    scenes.push_back(scene);
};

void Scenes::setupNatureScene() {
    Scene scene;

    ScenePart forest;
    GLuint texture = loadTexture("scenes/nature.png");
    if (texture == 0) {
        std::cerr << "Error: Failed to load texture!" << std::endl;
        return;
    }
    forest.textureId = texture;
    forest.height = 1.1f;
    forest.width = 1.1f;
    forest.x = 0.0f;
    forest.y = 0.2f;
    setupBuffersTexture(forest);

    ScenePart sky;
    sky.textureId = 0;
    sky.height = 1.1f;
    sky.width = 1.1f;
    sky.x = 0.0f;
    sky.y = 0.2f;
    sky.color[0] = 0.4118f;
    sky.color[1] = 0.8275f;
    sky.color[2] = 0.9412f;

    setupBuffers(sky);

    scene.sceneParts.push_back(sky);
    scene.sceneParts.push_back(forest);

    scenes.push_back(scene);
};

void Scenes::setupStreetScene() {
    Scene scene;
    ScenePart city;
    GLuint texture = loadTexture("scenes/street (2).png");
    if (texture == 0) {
        std::cerr << "Error: Failed to load texture!" << std::endl;
        return;
    }
    city.textureId = texture;
    city.height = 1.1f;
    city.width = 1.1f;
    city.x = 0.0f;
    city.y = 0.2f;
    setupBuffersTexture(city);
    scene.sceneParts.push_back(city);
    scenes.push_back(scene);
};

void Scenes::setupRestaurantScene() {
    Scene scene;

    ScenePart restaurant;
    GLuint texture = loadTexture("scenes/restaurant.png");
    if (texture == 0) {
        std::cerr << "Error: Failed to load texture!" << std::endl;
        return;
    }
    restaurant.textureId = texture;
    restaurant.height = 0.9f;
    restaurant.width = 1.1f;
    restaurant.x = 0.0f;
    restaurant.y = 0.3f;
    setupBuffersTexture(restaurant);

    ScenePart sky;
    sky.textureId = 0;
    sky.height = 0.8f;
    sky.width = 1.1f;
    sky.x = 0.0f;
    sky.y = 0.35f;
    sky.color[0] = 0.4118f;
    sky.color[1] = 0.8275f;
    sky.color[2] = 0.9412f;
    setupBuffers(sky);

    ScenePart floor;
    floor.textureId = 0;
    floor.height = 0.5f;
    floor.width = 1.1f;
    floor.x = 0.0f;
    floor.y = -0.1f;
    float color1[3] = { 0.6314f, 0.6118f, 0.5176f };
    floor.color[0] = color1[0];
    floor.color[1] = color1[1];
    floor.color[2] = color1[2];
    setupBuffers(floor);


    scene.sceneParts.push_back(sky);
    scene.sceneParts.push_back(floor);
    scene.sceneParts.push_back(restaurant);

    scenes.push_back(scene);
};

void Scenes::setupRiverScene() {
    Scene scene;

    ScenePart river;
    GLuint texture = loadTexture("scenes/river.png");
    if (texture == 0) {
        std::cerr << "Error: Failed to load texture!" << std::endl;
        return;
    }
    river.textureId = texture;
    river.height = 0.9f;
    river.width = 1.1f;
    river.x = 0.0f;
    river.y = 0.3f;
    setupBuffersTexture(river);

    ScenePart sky;
    sky.textureId = 0;
    sky.height = 0.5f;
    sky.width = 1.1f;
    sky.x = 0.0f;
    sky.y = 0.5f;
    sky.color[0] = 0.4118f;
    sky.color[1] = 0.8275f;
    sky.color[2] = 0.9412f;
    setupBuffers(sky);

    ScenePart floor;
    floor.textureId = 0;
    floor.height = 0.6f;
    floor.width = 1.1f;
    floor.x = 0.0f;
    floor.y = -0.05f;

    float color[3] = { 0.4745f, 0.7412f, 0.2275f };

    floor.color[0] = color[0];
    floor.color[1] = color[1];
    floor.color[2] = color[2];
    setupBuffers(floor);

    scene.sceneParts.push_back(sky);
    scene.sceneParts.push_back(floor);
    scene.sceneParts.push_back(river);

    scenes.push_back(scene);
};

void Scenes::setupSchoolScene() {
    Scene scene;
    ScenePart school;
    GLuint texture = loadTexture("scenes/school (2).png");
    if (texture == 0) {
        std::cerr << "Error: Failed to load texture!" << std::endl;
        return;
    }
    school.textureId = texture;
    school.height = 1.1f;
    school.width = 1.1f;
    school.x = 0.0f;
    school.y = 0.2f;
    setupBuffersTexture(school);
    scene.sceneParts.push_back(school);
    scenes.push_back(scene);
};


void Scenes::setupSchoolInsideScene() {
    Scene scene;

    ScenePart school;
    GLuint texture = loadTexture("scenes/school inside.png");
    if (texture == 0) {
        std::cerr << "Error: Failed to load texture!" << std::endl;
        return;
    }
    school.textureId = texture;
    school.height = 1.1f;
    school.width = 1.1f;
    school.x = 0.0f;
    school.y = 0.2f;
    setupBuffersTexture(school);
    scene.sceneParts.push_back(school);
    scenes.push_back(scene);
};

void Scenes::setupVillageScene() {
    Scene scene;

    ScenePart village;
    GLuint texture = loadTexture("scenes/village.png");
    if (texture == 0) {
        std::cerr << "Error: Failed to load texture!" << std::endl;
        return;
    }
    village.textureId = texture;
    village.height = 1.1f;
    village.width = 1.1f;
    village.x = 0.0f;
    village.y = 0.2f;
    setupBuffersTexture(village);

    ScenePart sky;
    sky.textureId = 0;
    sky.height = 1.1f;
    sky.width = 1.1f;
    sky.x = 0.0f;
    sky.y = 0.2f;
    sky.color[0] = 0.4118f;
    sky.color[1] = 0.8275f;
    sky.color[2] = 0.9412f;

    setupBuffers(sky);

    scene.sceneParts.push_back(sky);
    scene.sceneParts.push_back(village);

    scenes.push_back(scene);
};

void Scenes::setupOfficeScene() {
    Scene scene;

    ScenePart office;
    GLuint texture = loadTexture("scenes/office.png");
    if (texture == 0) {
        std::cerr << "Error: Failed to load texture!" << std::endl;
        return;
    }
    office.textureId = texture;
    office.height = 0.7f;
    office.width = 0.6f;
    office.x = 0.0f;
    office.y = 0.3f;
    setupBuffersTexture(office);

    ScenePart wall;
    wall.textureId = 0;
    wall.height = 0.7f;
    wall.width = 1.1f;
    wall.x = 0.0f;
    wall.y = 0.4f;
    float color[3] = { 0.9294f, 0.8745f, 0.6039f };
    wall.color[0] = color[0];
    wall.color[1] = color[1];
    wall.color[2] = color[2];
    setupBuffers(wall);

    ScenePart floor;
    floor.textureId = 0;
    floor.height = 0.6f;
    floor.width = 1.1f;
    floor.x = 0.0f;
    floor.y = -0.05f;
    float color1[3] = { 0.6314f, 0.6118f, 0.5176f };
    floor.color[0] = color1[0];
    floor.color[1] = color1[1];
    floor.color[2] = color1[2];
    setupBuffers(floor);

    scene.sceneParts.push_back(floor);
    scene.sceneParts.push_back(wall);
    scene.sceneParts.push_back(office);

    scenes.push_back(scene);
}

void Scenes::setupSalonScene() {
    Scene scene;

    ScenePart salon;
    GLuint texture = loadTexture("scenes/salon (2).png");
    if (texture == 0) {
        std::cerr << "Error: Failed to load texture!" << std::endl;
        return;
    }
    salon.textureId = texture;
    salon.height = 0.9f;
    salon.width = 1.1f;
    salon.x = 0.0f;
    salon.y = 0.3f;
    setupBuffersTexture(salon);

    ScenePart floor;
    floor.textureId = 0;
    floor.height = 0.6f;
    floor.width = 1.1f;
    floor.x = 0.0f;
    floor.y = -0.05f; 
    float color1[3] = { 0.9f, 0.4f, 0.2f };
    floor.color[0] = color1[0];
    floor.color[1] = color1[1];
    floor.color[2] = color1[2];
    setupBuffers(floor);

    scene.sceneParts.push_back(floor);
    scene.sceneParts.push_back(salon);

    scenes.push_back(scene);
}

void Scenes::setupInfirmaryScene() {
    Scene scene;

    ScenePart infirmary;
    GLuint texture = loadTexture("scenes/infirmary.png");
    if (texture == 0) {
        std::cerr << "Error: Failed to load texture!" << std::endl;
        return;
    }
    infirmary.textureId = texture;
    infirmary.height = 0.9f;
    infirmary.width = 1.1f;
    infirmary.x = 0.0f;
    infirmary.y = 0.3f;
    setupBuffersTexture(infirmary);

    ScenePart floor;
    floor.textureId = 0;
    floor.height = 0.6f;
    floor.width = 1.1f;
    floor.x = 0.0f;
    floor.y = -0.05f; 
    float color1[3] = { 1.0f, 0.79f, 0.52f };
    floor.color[0] = color1[0];
    floor.color[1] = color1[1];
    floor.color[2] = color1[2];
    setupBuffers(floor);

    scene.sceneParts.push_back(floor);
    scene.sceneParts.push_back(infirmary);

    scenes.push_back(scene);
}


void setupPoolScene() {};

void setupClubScene() {};

void setupBoatScene() {};

void setupPlaneScene() {};


std::vector<Scenes::Scene> Scenes::getAllScenes() {
    return scenes;
}

int Scenes::getTotalMenuPages() {
    if (scenes.size() % 4 == 0) {
        return scenes.size() / 4;
    }
    return scenes.size() / 4 + 1;
}

void Scenes::renderActiveScenes(int currentMenuPage, int windowWidth, int windowHeight) {

    float aspectRatio = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);

    int startIndex = (currentMenuPage - 1) * 4; // Calculate the starting index
    int endIndex = startIndex + 4; // Get up to 5 slides per page

    //std::cout << "Slides count: " << slides.size() << std::endl;

    if (endIndex > scenes.size()) {
        endIndex = scenes.size();
    }

    float moving = 0.25f;

    //activeScenes.clear();
    for (int i = startIndex; i < endIndex; i++) {
        //activeScenes.push_back(scenes[i]);
        drawScene(i, -0.45f + (i%4) * moving * aspectRatio, -0.8f, 0.2f * aspectRatio, 0.2f);
    }

    /*for (int i = 0; i < 4; ++i) {
        drawScene(i, -0.45f + i * moving * aspectRatio, -0.8f, 0.2f * aspectRatio, 0.2f);
    }*/
};