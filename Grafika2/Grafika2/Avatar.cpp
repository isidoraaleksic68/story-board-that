#include "Avatar.h"
#include <GL/glew.h>
#include <cmath>
#include <iostream>
#include "stb_image.h"
#include <nlohmann/json.hpp>
#include <fstream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Avatar::Avatar(Shader& shader, Shader& textureShader)
    : shader(shader), textureShader(textureShader) {
    skinColor[0] = 1.0f;
    skinColor[1] = 0.8f;
    skinColor[2] = 0.6f;
    faceColor[0] = 1.0f;
    faceColor[1] = 0.85f;
    faceColor[2] = 0.7f;
    mouthTexture = 0;
    noseTexture = 0;
    eyeTexture = 0;
    dressTexture = 0;
    tshirtTexture = 0;
    pantsTexture = 0;
    clothesTexture = 0;
    rightHandTexture = 0;
    leftHandTexture = 0;
    rightFootTexture = 0;
    leftFootTexture = 0;
    studentTexture = loadTexture("student.png");
    setupNewCharacter();
}

Avatar::~Avatar() {
    for (auto& character : characters) {
        for (auto& bodyPart : character.bodyParts) {
            glDeleteBuffers(1, &bodyPart.VBO);
            glDeleteBuffers(1, &bodyPart.EBO);
            glDeleteVertexArrays(1, &bodyPart.VAO);
        }
    }
}

void Avatar::setupArms(Character& character) {
    float shoulderY = 0.22f;
    float upperArmLength = 0.29f;
    float lowerArmLength = 0.16f;
    float upperArmWidth = 0.065f;
    float lowerArmWidth = 0.065f;

    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    // ---------------- LEFT ARM ----------------
    BodyPart leftUpperArm;
    float leftShoulderX = -0.18f;
    float leftElbowX = leftShoulderX - upperArmLength * cos(M_PI / 2.4);
    float leftElbowY = shoulderY - upperArmLength * sin(M_PI / 2.4);
    float leftHandY = leftElbowY - lowerArmLength;

    leftUpperArm.vertices = {
        // Top-left
        leftShoulderX, shoulderY, 0.0f, skinColor[0], skinColor[1], skinColor[2], 1.0f, 0.0f, 1.0f,
        // Top-right
        leftShoulderX + upperArmWidth, shoulderY, 0.0f, skinColor[0], skinColor[1], skinColor[2], 1.0f, 1.0f, 1.0f,
        // Bottom-right
        leftElbowX + upperArmWidth, leftElbowY, 0.0f, skinColor[0], skinColor[1], skinColor[2], 1.0f, 1.0f, 0.0f,
        // Bottom-left
        leftElbowX, leftElbowY, 0.0f, skinColor[0], skinColor[1], skinColor[2], 1.0f, 0.0f, 0.0f
    };
    leftUpperArm.indices = std::vector<unsigned int>(indices, indices + 6);

    BodyPart leftLowerArm;
    leftLowerArm.vertices = {
        leftElbowX, leftElbowY, 0.0f, skinColor[0], skinColor[1], skinColor[2], 1.0f, 0.0f, 1.0f,
        leftElbowX + lowerArmWidth, leftElbowY, 0.0f, skinColor[0], skinColor[1], skinColor[2], 1.0f, 1.0f, 1.0f,
        leftElbowX + lowerArmWidth - 0.01f, leftHandY, 0.0f, skinColor[0], skinColor[1], skinColor[2], 1.0f, 1.0f, 0.0f,
        leftElbowX + 0.01f, leftHandY, 0.0f, skinColor[0], skinColor[1], skinColor[2], 1.0f, 0.0f, 0.0f
    };
    leftLowerArm.indices = std::vector<unsigned int>(indices, indices + 6);

    // ---------------- RIGHT ARM ----------------
    float rightShoulderX = 0.18f;
    float rightElbowX = rightShoulderX + upperArmLength * cos(M_PI / 2.4);
    float rightElbowY = shoulderY - upperArmLength * sin(M_PI / 2.4);
    float rightHandY = rightElbowY - lowerArmLength;

    BodyPart rightUpperArm;
    rightUpperArm.vertices = {
        rightShoulderX, shoulderY, 0.0f, skinColor[0], skinColor[1], skinColor[2], 1.0f, 0.0f, 1.0f,
        rightShoulderX - upperArmWidth, shoulderY, 0.0f, skinColor[0], skinColor[1], skinColor[2], 1.0f, 1.0f, 1.0f,
        rightElbowX - upperArmWidth, rightElbowY, 0.0f, skinColor[0], skinColor[1], skinColor[2], 1.0f, 1.0f, 0.0f,
        rightElbowX, rightElbowY, 0.0f, skinColor[0], skinColor[1], skinColor[2], 1.0f, 0.0f, 0.0f
    };
    rightUpperArm.indices = std::vector<unsigned int>(indices, indices + 6);

    BodyPart rightLowerArm;
    rightLowerArm.vertices = {
        rightElbowX, rightElbowY, 0.0f, skinColor[0], skinColor[1], skinColor[2], 1.0f, 0.0f, 1.0f,
        rightElbowX - lowerArmWidth, rightElbowY, 0.0f, skinColor[0], skinColor[1], skinColor[2], 1.0f, 1.0f, 1.0f,
        rightElbowX - lowerArmWidth + 0.01f, rightHandY, 0.0f, skinColor[0], skinColor[1], skinColor[2], 1.0f, 1.0f, 0.0f,
        rightElbowX - 0.01f, rightHandY, 0.0f, skinColor[0], skinColor[1], skinColor[2], 1.0f, 0.0f, 0.0f
    };
    rightLowerArm.indices = std::vector<unsigned int>(indices, indices + 6);

    // Setup buffers
    setupBuffers(leftUpperArm);
    setupBuffers(leftLowerArm);
    setupBuffers(rightUpperArm);
    setupBuffers(rightLowerArm);

    leftUpperArm.textureId = 0;
    leftUpperArm.width = 0;
    leftUpperArm.height = 0;
    leftUpperArm.x = 0;
    leftUpperArm.y = 0;


    leftLowerArm.textureId = 0;
    leftLowerArm.width = 0;
    leftLowerArm.height = 0;
    leftLowerArm.x = 0;
    leftLowerArm.y = 0;

    rightUpperArm.textureId = 0;
    rightUpperArm.width = 0;
    rightUpperArm.height = 0;
    rightUpperArm.x = 0;
    rightUpperArm.y = 0;

    rightLowerArm.textureId = 0;
    rightLowerArm.width = 0;
    rightLowerArm.height = 0;
    rightLowerArm.x = 0;
    rightLowerArm.y = 0;

    character.bodyParts.push_back(leftUpperArm);
    character.bodyParts.push_back(leftLowerArm);
    character.bodyParts.push_back(rightUpperArm);
    character.bodyParts.push_back(rightLowerArm);

    BodyPart leftHand;

    leftHand.textureId = loadTexture("hands/left.png");

    float handWidth = 0.13f;
    float handHeight = 0.15f;

    float centerX = leftElbowX + 0.055f;
    float centerY = leftHandY - 0.06f;

    leftHand.x = centerX;
    leftHand.y = centerY;
    leftHand.width = handWidth;
    leftHand.height = handHeight;
    leftHand.indices = {};
    leftHand.vertices = {};

    setupBuffersTexture(leftHand);
    character.bodyParts.push_back(leftHand);

    BodyPart rightHand;

    rightHand.textureId = loadTexture("hands/right.png");

    centerX = rightElbowX - 0.055f;
    centerY = rightHandY - 0.06f;

    rightHand.x = centerX;
    rightHand.y = centerY;
    rightHand.width = handWidth;
    rightHand.height = handHeight;
    rightHand.indices = {};
    rightHand.vertices = {};

    setupBuffersTexture(rightHand);
    character.bodyParts.push_back(rightHand);
}

void Avatar::setupLegs(Character& character) {
    float torsoBottomY = -0.3f;
    float upperLegLength = 0.23f;
    float lowerLegLength = 0.27f;
    float legWidthTop = 0.135f;
    float legWidthBottom = 0.08f;
    float legWidthFoot = 0.05f;
    float legGap = 0.01f;

    float leftLegCenterX = -legGap / 2.0f - legWidthTop / 2.0f;
    float rightLegCenterX = legGap / 2.0f + legWidthTop / 2.0f;

    float upperLegBottomY = torsoBottomY - upperLegLength;
    float lowerLegBottomY = upperLegBottomY - lowerLegLength;

    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    // -------- Left Upper Leg --------
    BodyPart leftUpperLeg;
    leftUpperLeg.vertices = {
        // x, y, z, r, g, b, a, u, v
        leftLegCenterX - legWidthTop / 2, torsoBottomY, 0.0f,         skinColor[0], skinColor[1], skinColor[2], 1.0f, 0.0f, 1.0f,
        leftLegCenterX + legWidthTop / 2, torsoBottomY, 0.0f,         skinColor[0], skinColor[1], skinColor[2], 1.0f, 1.0f, 1.0f,
        leftLegCenterX + legWidthBottom / 2, upperLegBottomY, 0.0f,   skinColor[0], skinColor[1], skinColor[2], 1.0f, 1.0f, 0.0f,
        leftLegCenterX - legWidthBottom / 2, upperLegBottomY, 0.0f,   skinColor[0], skinColor[1], skinColor[2], 1.0f, 0.0f, 0.0f
    };
    leftUpperLeg.indices = std::vector<unsigned int>(indices, indices + 6);

    leftUpperLeg.textureId = 0;
    leftUpperLeg.width = 0;
    leftUpperLeg.height = 0;
    leftUpperLeg.x = 0;
    leftUpperLeg.y = 0;

    setupBuffers(leftUpperLeg);
    character.bodyParts.push_back(leftUpperLeg);

    // -------- Left Lower Leg --------
    BodyPart leftLowerLeg;
    leftLowerLeg.vertices = {
        leftLegCenterX - legWidthBottom / 2, upperLegBottomY, 0.0f,   skinColor[0], skinColor[1], skinColor[2], 1.0f, 0.0f, 1.0f,
        leftLegCenterX + legWidthBottom / 2, upperLegBottomY, 0.0f,   skinColor[0], skinColor[1], skinColor[2], 1.0f, 1.0f, 1.0f,
        leftLegCenterX + legWidthFoot / 2, lowerLegBottomY, 0.0f,     skinColor[0], skinColor[1], skinColor[2], 1.0f, 1.0f, 0.0f,
        leftLegCenterX - legWidthFoot / 2, lowerLegBottomY, 0.0f,     skinColor[0], skinColor[1], skinColor[2], 1.0f, 0.0f, 0.0f
    };
    leftLowerLeg.indices = std::vector<unsigned int>(indices, indices + 6);

    leftLowerLeg.textureId = 0;
    leftLowerLeg.width = 0;
    leftLowerLeg.height = 0;
    leftLowerLeg.x = 0;
    leftLowerLeg.y = 0;

    setupBuffers(leftLowerLeg);
    character.bodyParts.push_back(leftLowerLeg);

    // -------- Right Upper Leg --------
    BodyPart rightUpperLeg;
    rightUpperLeg.vertices = {
        rightLegCenterX - legWidthTop / 2, torsoBottomY, 0.0f,         skinColor[0], skinColor[1], skinColor[2], 1.0f, 0.0f, 1.0f,
        rightLegCenterX + legWidthTop / 2, torsoBottomY, 0.0f,         skinColor[0], skinColor[1], skinColor[2], 1.0f, 1.0f, 1.0f,
        rightLegCenterX + legWidthBottom / 2, upperLegBottomY, 0.0f,   skinColor[0], skinColor[1], skinColor[2], 1.0f, 1.0f, 0.0f,
        rightLegCenterX - legWidthBottom / 2, upperLegBottomY, 0.0f,   skinColor[0], skinColor[1], skinColor[2], 1.0f, 0.0f, 0.0f
    };
    rightUpperLeg.indices = std::vector<unsigned int>(indices, indices + 6);


    rightUpperLeg.textureId = 0;
    rightUpperLeg.width = 0;
    rightUpperLeg.height = 0;
    rightUpperLeg.x = 0;
    rightUpperLeg.y = 0;

    setupBuffers(rightUpperLeg);
    character.bodyParts.push_back(rightUpperLeg);

    // -------- Right Lower Leg --------
    BodyPart rightLowerLeg;
    rightLowerLeg.vertices = {
        rightLegCenterX - legWidthBottom / 2, upperLegBottomY, 0.0f,   skinColor[0], skinColor[1], skinColor[2], 1.0f, 0.0f, 1.0f,
        rightLegCenterX + legWidthBottom / 2, upperLegBottomY, 0.0f,   skinColor[0], skinColor[1], skinColor[2], 1.0f, 1.0f, 1.0f,
        rightLegCenterX + legWidthFoot / 2, lowerLegBottomY, 0.0f,     skinColor[0], skinColor[1], skinColor[2], 1.0f, 1.0f, 0.0f,
        rightLegCenterX - legWidthFoot / 2, lowerLegBottomY, 0.0f,     skinColor[0], skinColor[1], skinColor[2], 1.0f, 0.0f, 0.0f
    };
    rightLowerLeg.indices = std::vector<unsigned int>(indices, indices + 6);

    rightLowerLeg.textureId = 0;
    rightLowerLeg.width = 0;
    rightLowerLeg.height = 0;
    rightLowerLeg.x = 0;
    rightLowerLeg.y = 0;

    setupBuffers(rightLowerLeg);
    character.bodyParts.push_back(rightLowerLeg);

    BodyPart rightFoot;

    // Load texture if not loaded already
    rightFoot.textureId = loadTexture("feet/right1.png");

    // Foot dimensions
    float footWidth = 0.12f;
    float footHeight = 0.15f;

    float rightLegX = rightLegCenterX + legWidthFoot / 2.0f;
    float rightLegY = lowerLegBottomY;
    float leftLegX = leftLegCenterX - legWidthFoot / 2.0f;
    float leftLegY = lowerLegBottomY;

    float centerX = rightLegX - 0.012f;
    float centerY = rightLegY- 0.07f;

    rightFoot.x = centerX;
    rightFoot.y = centerY;
    rightFoot.width = footWidth;
    rightFoot.height = footHeight;
    rightFoot.indices = {};
    rightFoot.vertices = {};


    setupBuffersTexture(rightFoot);
    character.bodyParts.push_back(rightFoot);

    BodyPart leftFoot;

    // Load texture if not loaded already
    leftFoot.textureId = loadTexture("feet/left1.png");

    // Define the center position based on the provided coordinates
    centerX = leftLegX + 0.02f;
    centerY = leftLegY - 0.07f;

    leftFoot.x = centerX;
    leftFoot.y = centerY;
    leftFoot.width = footWidth;
    leftFoot.height = footHeight;
    leftFoot.indices = {};
    leftFoot.vertices = {};

    setupBuffersTexture(leftFoot);
    character.bodyParts.push_back(leftFoot);
};

void Avatar::setupHead(Character& character) {
    BodyPart head;

    float centerX = 0.0f;
    float centerY = 0.48f;
    float centerZ = 0.0f;

    const float a = 0.11f;
    const float b = 0.16f;
    const int numVertices = 50;

    // Center vertex (0) - head center
    head.vertices.insert(head.vertices.end(), {
        centerX, centerY, centerZ,
        skinColor[0], skinColor[1], skinColor[2], 1.0f,
        0.0f, 0.0f
        });

    for (int i = 0; i <= numVertices; ++i) {
        float angle = 2.0f * M_PI * i / numVertices;
        float x = a * cos(angle);
        float y = b * sin(angle);

        head.vertices.insert(head.vertices.end(), {
            x, y + centerY, centerZ,
            skinColor[0], skinColor[1], skinColor[2], 1.0f,
            0.0f, 0.0f  // UVs set to zero
            });

        if (i < numVertices) {
            head.indices.push_back(0);
            head.indices.push_back(i + 1);
            head.indices.push_back(i + 2);
        }
    }
    head.textureId = 0;
    setupBuffers(head);
    character.bodyParts.push_back(head);
}



void Avatar::setupNeck(Character& character) {
    BodyPart neck;
    const float width = 0.09f;
    const float height = 0.16f;
    float neckBaseY = 0.2f;

    neck.vertices = {
        // x, y, z, r, g, b, a, texU, texV
        -width / 2.0f, neckBaseY, 0.0f,  skinColor[0], skinColor[1], skinColor[2], 1.0f,  0.0f, 1.0f,
         width / 2.0f, neckBaseY, 0.0f,  skinColor[0], skinColor[1], skinColor[2], 1.0f,  1.0f, 1.0f,
         width / 2.0f, neckBaseY + height, 0.0f,  skinColor[0], skinColor[1], skinColor[2], 1.0f,  1.0f, 0.0f,
        -width / 2.0f, neckBaseY + height, 0.0f,  skinColor[0], skinColor[1], skinColor[2], 1.0f,  0.0f, 0.0f
    };

    neck.indices = {
        0, 1, 2,
        2, 3, 0
    };
    neck.textureId = 0;
    neck.width = 0;
    neck.height = 0;
    neck.x = 0;
    neck.y = 0;

    setupBuffers(neck);
    
    character.bodyParts.push_back(neck);
}

void Avatar::setupTorso(Character& character) {
    BodyPart torso;

    // Use skinColor
    float r = skinColor[0];
    float g = skinColor[1];
    float b = skinColor[2];

    std::cout << "Skin color: " << r << ", " << g << ", " << b << std::endl;

    std::vector<float> vertices = {
        // Left shoulder curve
        -0.18f,  0.22f, 0.0f,   r, g, b, 1.0f, 0.0f, 1.0f,
        -0.14f,  0.26f, 0.0f,   r, g, b, 1.0f, 1.0f, 0.0f,
        -0.08f,  0.28f, 0.0f,   r, g, b, 1.0f, 1.0f, 1.0f,
        -0.01f,  0.29f, 0.0f,   r, g, b, 1.0f, 0.0f, 1.0f,

        // Right shoulder curve
        0.01f,  0.29f, 0.0f,   r, g, b, 1.0f,   0.0f, 1.0f,
        0.08f,  0.28f, 0.0f,   r, g, b, 1.0f,   1.0f, 0.0f,
        0.14f,  0.26f, 0.0f,   r, g, b, 1.0f,   1.0f, 1.0f,
        0.18f,  0.22f, 0.0f,   r, g, b, 1.0f,   0.0f, 1.0f,

        // Upper torso
        -0.14f,  0.18f, 0.0f,   r, g, b, 1.0f,   0.0f, 1.0f,
        0.14f,  0.18f, 0.0f,   r, g, b, 1.0f,   1.0f, 0.0f,

        // Middle torso
        -0.1f, -0.08f, 0.0f,   r, g, b, 1.0f,   1.0f, 1.0f,
        0.1f, -0.08f, 0.0f,   r, g, b, 1.0f,   0.0f, 1.0f,

        // Waist
        -0.12f, -0.16f, 0.0f,   r, g, b, 1.0f,   1.0f, 0.0f,
        0.12f, -0.16f, 0.0f,   r, g, b, 1.0f,   0.0f, 1.0f,

        // Bottom torso
        -0.14f, -0.3f, 0.0f,   r, g, b, 1.0f,   1.0f, 1.0f,
        0.14f, -0.3f, 0.0f,   r, g, b, 1.0f,   0.0f, 1.0f,
    };

    // Define the indices for the triangles that make up the torso
    std::vector<unsigned int> indices = {
        // Left shoulder
        0, 1, 2,
        2, 3, 8,
        0, 2, 8,

        // Right shoulder
        4, 5, 6,
        6, 7, 9,
        4, 6, 9,

        // Connecting the shoulders
        3, 4, 8,
        8, 4, 9,

        // Filling the shoulder gaps
        1, 2, 8,
        5, 6, 9,

        // Upper torso to middle torso
        8, 9, 10,
        9, 11, 10,

        // Middle torso to waist
        10, 11, 12,
        11, 13, 12,

        // Waist to bottom torso
        12, 13, 14,
        13, 15, 14,
    };

    // Set the vertices and indices for the torso
    torso.vertices = vertices;
    torso.indices = indices;
    torso.textureId = 0;
    // Call the setupBuffers function to bind data
    setupBuffers(torso);

    // Add the torso to the character
    character.bodyParts.push_back(torso);
}




void Avatar::setupNewCharacter() {
    Character character;
    setupNeck(character);
    setupHead(character);
    setupTorso(character);
    setupArms(character);
    setupLegs(character);
    character.name = "Isidora";
    characters.push_back(character);
    writeCharactersToJson(characters, "characters.json");
}


void Avatar::drawCharacter(int characterIndex, float x, float y, float width, float height) {
    if (characterIndex < 0 || characterIndex >= static_cast<int>(characters.size())) {
        return;
    }

    Character& character = characters[characterIndex];

    for (auto& part : character.bodyParts) {
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
        if (part.indices.size() == 0) {
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        else {
            glDrawElements(GL_TRIANGLES, part.indices.size(), GL_UNSIGNED_INT, 0);
        }
        // Cleanup
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
    }
}




// Function to write characters to a JSON file
void Avatar::writeCharactersToJson(const std::vector<Avatar::Character>& characters, const std::string& filePath) {
    nlohmann::json jsonData;

    for (const auto& character : characters) {
        nlohmann::json characterJson;
        characterJson["name"].push_back(character.name);

        // Iterate through each body part and add to character's JSON
        for (const auto& bodyPart : character.bodyParts) {
            nlohmann::json bodyPartJson;
            bodyPartJson["x"] = bodyPart.x;
            bodyPartJson["y"] = bodyPart.y;
            bodyPartJson["width"] = bodyPart.width;
            bodyPartJson["height"] = bodyPart.height;
            bodyPartJson["textureId"] = bodyPart.textureId;
            bodyPartJson["vertices"] = bodyPart.vertices;
            bodyPartJson["indices"] = bodyPart.indices;

            characterJson["bodyParts"].push_back(bodyPartJson);
        }

        jsonData.push_back(characterJson);
    }

    // Write to file
    std::ofstream outFile(filePath);
    outFile << jsonData.dump(4);  // Pretty print with 4 spaces
}

// Function to read characters from a JSON file
std::vector<Avatar::Character> Avatar::readCharactersFromJson(const std::string& filePath) {
    std::vector<Character> characters;

    // Read the JSON file
    std::ifstream inFile(filePath);
    if (inFile.is_open()) {
        nlohmann::json jsonData;
        inFile >> jsonData;

        for (const auto& characterJson : jsonData) {
            Character character;
            character.name = characterJson["name"];
            // Read each body part for the character
            for (const auto& bodyPartJson : characterJson["bodyParts"]) {
                BodyPart bodyPart;
                bodyPart.x = bodyPartJson["x"];
                bodyPart.y = bodyPartJson["y"];
                bodyPart.width = bodyPartJson["width"];
                bodyPart.height = bodyPartJson["height"];
                bodyPart.textureId = bodyPartJson["textureId"];
                bodyPart.vertices = bodyPartJson["vertices"].get<std::vector<float>>();
                bodyPart.indices = bodyPartJson["indices"].get<std::vector<unsigned int>>();

                character.bodyParts.push_back(bodyPart);
            }

            characters.push_back(character);
        }
    }
    else {
        std::cerr << "Failed to open the file for reading.\n";
    }

    return characters;
}
void Avatar::setupBuffers(BodyPart& part) {
    // Generate the VAO, VBO, and EBO
    glGenVertexArrays(1, &part.VAO);
    glGenBuffers(1, &part.VBO);
    glGenBuffers(1, &part.EBO);

    glBindVertexArray(part.VAO);

    // Bind the VBO and load vertex data into the buffer
    glBindBuffer(GL_ARRAY_BUFFER, part.VBO);
    glBufferData(GL_ARRAY_BUFFER, part.vertices.size() * sizeof(float), part.vertices.data(), GL_STATIC_DRAW);

    // Bind the EBO and load index data into the buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, part.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, part.indices.size() * sizeof(unsigned int), part.indices.data(), GL_STATIC_DRAW);

    // Set the vertex position attribute (3 floats per vertex)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Set the vertex color attribute (4 floats per vertex for RGBA)
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Set the texture coordinates attribute (2 floats per vertex)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Unbind the VAO to avoid accidental modification
    glBindVertexArray(0);
}



void Avatar::setupBuffersTexture(BodyPart& bodyPart) {
    float vertices[] = {
        // Positions        // Texture Coords
        bodyPart.x - bodyPart.width / 2, bodyPart.y - bodyPart.height / 2, 0.0f,  0.0f, 0.0f, // Bottom-left
        bodyPart.x + bodyPart.width / 2, bodyPart.y - bodyPart.height / 2, 0.0f,  1.0f, 0.0f, // Bottom-right
        bodyPart.x + bodyPart.width / 2, bodyPart.y + bodyPart.height / 2, 0.0f,  1.0f, 1.0f, // Top-right
        bodyPart.x - bodyPart.width / 2, bodyPart.y + bodyPart.height / 2, 0.0f,  0.0f, 1.0f  // Top-left
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    glGenVertexArrays(1, &bodyPart.VAO);
    glGenBuffers(1, &bodyPart.VBO);
    glGenBuffers(1, &bodyPart.EBO);

    glBindVertexArray(bodyPart.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, bodyPart.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bodyPart.EBO);
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

/*
void Avatar::draw(Shader& shader, Shader& textureShader, float windowWidth, float windowHeight) {
    drawNeck(shader, skinColor);
    drawHead(shader, faceColor);
    drawTorso(shader, skinColor);
    drawArms(shader, textureShader, skinColor);
    drawLegs(shader, textureShader, skinColor);
}*/


GLuint Avatar::loadTextureCached(const std::string& filepath) {
    if (textureCache.find(filepath) != textureCache.end()) {
        return textureCache[filepath];
    }
    GLuint textureID = loadTexture(filepath.c_str());
    textureCache[filepath] = textureID;
    return textureID;
}


GLuint Avatar::loadTexture(const char* filepath) {
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

/*
void Avatar::drawHead(Shader& shader, float color[]) {
    const float a = 0.11f;  // head width
    const float b = 0.16f;  // head height
    const int numVertices = 500; // number of vertices for head

    float* vertices = new float[numVertices * 6]; // 2D positions + RGBA colors

    for (int i = 0; i < numVertices; ++i) {
        float angle = 2.0f * M_PI * i / numVertices;
        float x = a * cos(angle);
        float y = b * sin(angle);

        // Center the head at (0, 0)
        vertices[i * 6] = x; // X = 0.5f
        vertices[i * 6 + 1] = y + 0.48f; // Centered on Y = 0.48 (adjust for more space if needed)
        vertices[i * 6 + 2] = color[0]; // R (Red)
        vertices[i * 6 + 3] = color[1]; // G (Green)
        vertices[i * 6 + 4] = color[2]; // B (Blue)
        vertices[i * 6 + 5] = 1.0f; // A (Alpha)
    }

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, numVertices * 6 * sizeof(float), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glUseProgram(shader.getID());
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, numVertices);
    glBindVertexArray(0);
    glUseProgram(0);

    delete[] vertices;
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}


void Avatar::drawNeck(Shader& shader, float color[]) {
    const float width = 0.09f;   // neck width
    const float height = 0.16f;  // neck height

    // y position for neck
    float neckBaseY = 0.2f;

    float vertices[] = {
        // X, Y, R, G, B, A
        -width / 2.0f , neckBaseY, color[0], color[1], color[2], 2.0f, // left bottom
         width / 2.0f , neckBaseY, color[0], color[1], color[2], 2.0f, // right bottom
         width / 2.0f , neckBaseY + height, color[0], color[1], color[2], 2.0f, // right top
        -width / 2.0f , neckBaseY + height, color[0], color[1], color[2], 2.0f  // left top
    };

    unsigned int indices[] = {
        0, 1, 2, // first triangle
        2, 3, 0  // second triangle
    };

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glUseProgram(shader.getID());
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}


void Avatar::drawTorso(Shader& shader, float color[]) {
    float vertices[] = {
        // Position (x, y)    | Color (r, g, b, a)
        // left shoulder curve
        -0.18f ,  0.22f,  color[0], color[1], color[2], 1.0f,
        -0.14f ,  0.26f,  color[0], color[1], color[2], 1.0f,
        -0.08f ,  0.28f,  color[0], color[1], color[2], 1.0f,
        -0.01f ,  0.29f,  color[0], color[1], color[2], 1.0f,

        // right shoulder curve
         0.01f ,  0.29f,  color[0], color[1], color[2], 1.0f,
         0.08f ,  0.28f,  color[0], color[1], color[2], 1.0f,
         0.14f ,  0.26f,  color[0], color[1], color[2], 1.0f,
         0.18f ,  0.22f,  color[0], color[1], color[2], 1.0f,

         // upper torso
         -0.14f ,  0.18f,  color[0], color[1], color[2], 1.0f,
          0.14f ,  0.18f,  color[0], color[1], color[2], 1.0f,

          // middle torso
          -0.1f , -0.08f,  color[0], color[1], color[2], 1.0f,
           0.1f , -0.08f,  color[0], color[1], color[2], 1.0f,

           // waist
           -0.12f , -0.16f,  color[0], color[1], color[2], 1.0f,
            0.12f , -0.16f,  color[0], color[1], color[2], 1.0f,

            // bottom torso
            -0.14f , -0.3f,   color[0], color[1], color[2], 1.0f,
             0.14f , -0.3f,   color[0], color[1], color[2], 1.0f,
    };

    unsigned int indices[] = {
        // left shoulder curve
        0, 1, 2,
        2, 3, 8,
        0, 2, 8,

        // right shoulder curve
        4, 5, 6,
        6, 7, 9,
        4, 6, 9,

        // connecting left and right shoulder to upper torso
        3, 4, 8,
        8, 4, 9,

        // filling the missing parts in the shoulder and upper torso
        1, 2, 8,
        5, 6, 9,

        // upper torso to middle torso
        8, 9, 10,
        9, 11, 10,

        // middle torso to waist
        10, 11, 12,
        11, 13, 12,

        // waist to bottom torso
        12, 13, 14,
        13, 15, 14,
    };

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glUseProgram(shader.getID());
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}


void drawRectangle(Shader& shader, float* vertices, unsigned int* indices, int numIndices) {
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 4 * 6 * sizeof(float), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glUseProgram(shader.getID());
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}


void Avatar::drawLeftFoot(Shader& shader, float leftFootEndX, float leftFootEndY) {

    if (leftFootTexture == 0) {
        leftFootTexture = loadTexture("feet/left1.png");
    }

    // enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float footWidth = 0.12f;  // left foot width
    float footHeight = 0.15f; // left foot height

    float vertices[] = {
        leftFootEndX + 0.02 - footWidth / 2, leftFootEndY -0.07 - footHeight / 2, 0.0f, 0.0f, // bottom-left
        leftFootEndX + 0.02 + footWidth / 2, leftFootEndY -0.07 - footHeight / 2, 1.0f, 0.0f, // bottom-right
        leftFootEndX + 0.02 + footWidth / 2, leftFootEndY -0.07 + footHeight / 2, 1.0f, 1.0f, // top-right
        leftFootEndX + 0.02 - footWidth / 2, leftFootEndY -0.07 + footHeight / 2, 0.0f, 1.0f  // top-left
    };

    unsigned int indices[] = {
        0, 1, 2, // first triangle
        0, 2, 3  // second triangle
    };

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

    shader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, leftFootTexture);
    shader.setInt("texture1", 0);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
};


void Avatar::drawRightFoot(Shader& shader, float rightFootEndX, float rightFootEndY) {
    if (rightFootTexture == 0) {
        rightFootTexture = loadTexture("feet/right1.png");
    }

    // enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float footWidth = 0.12f;  // foot width
    float footHeight = 0.15f; // foot height

    float vertices[] = {
        rightFootEndX - 0.012 - footWidth / 2, rightFootEndY - 0.07 - footHeight / 2, 0.0f, 0.0f, // bottom-left
        rightFootEndX - 0.012 + footWidth / 2, rightFootEndY - 0.07 - footHeight / 2, 1.0f, 0.0f, // bottom-right
        rightFootEndX - 0.012 + footWidth / 2, rightFootEndY - 0.07 + footHeight / 2, 1.0f, 1.0f, // top-right
        rightFootEndX - 0.012 - footWidth / 2, rightFootEndY - 0.07 + footHeight / 2, 0.0f, 1.0f  // top-left
    };

    unsigned int indices[] = {
        0, 1, 2, // first triangle
        0, 2, 3  // second triangle
    };

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

    shader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, rightFootTexture);
    shader.setInt("texture1", 0);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);

};


void Avatar::drawLeftHand(Shader& shader, float leftArmEndX, float leftArmEndY) {

    if (leftHandTexture == 0) {
        leftHandTexture = loadTexture("hands/left.png");
    }

    // enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float handWidth = 0.13f;  // hand width
    float handHeight = 0.15f; // hand height

    float vertices[] = {
        leftArmEndX + 0.055 - handWidth / 2, leftArmEndY - 0.06 - handHeight / 2, 0.0f, 0.0f, // bottom-left
        leftArmEndX + 0.055 + handWidth / 2, leftArmEndY - 0.06 - handHeight / 2, 1.0f, 0.0f, // bottom-right
        leftArmEndX + 0.055 + handWidth / 2, leftArmEndY - 0.06 + handHeight / 2, 1.0f, 1.0f, // top-right
        leftArmEndX + 0.055 - handWidth / 2, leftArmEndY - 0.06 + handHeight / 2, 0.0f, 1.0f  // top-left
    };

    unsigned int indices[] = {
        0, 1, 2, // First triangle
        0, 2, 3  // Second triangle
    };

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

    shader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, leftHandTexture);
    shader.setInt("texture1", 0);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
};


void Avatar::drawRightHand(Shader& shader, float rightArmEndX, float rightArmEndY) {

    if (rightHandTexture == 0) {
        rightHandTexture = loadTexture("hands/right.png");
    }

    // enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float handWidth = 0.13f;  // hand width
    float handHeight = 0.15f; // hand height

    float vertices[] = {
        rightArmEndX - 0.055 - handWidth / 2, rightArmEndY - 0.06 - handHeight / 2, 0.0f, 0.0f, // bottom-left
        rightArmEndX - 0.055 + handWidth / 2, rightArmEndY - 0.06 - handHeight / 2, 1.0f, 0.0f, // bottom-right
        rightArmEndX - 0.055 + handWidth / 2, rightArmEndY - 0.06 + handHeight / 2, 1.0f, 1.0f, // top-right
        rightArmEndX - 0.055 - handWidth / 2, rightArmEndY - 0.09 + handHeight / 2, 0.0f, 1.0f  // top-left
    };

    unsigned int indices[] = {
        0, 1, 2, // first triangle
        0, 2, 3  // second triangle
    };

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

    shader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, rightHandTexture);
    shader.setInt("texture1", 0);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);

};


void Avatar::drawArms(Shader& shader, Shader& textureShader, float color[]) {
    
    float shoulderY = 0.22f; // shoulder y-coordinate
    float upperArmLength = 0.29f; // upper arm length
    float lowerArmLength = 0.16f; // lower arm length
    float upperArmWidth = 0.065f;  // width of the arm
    float lowerArmWidth = 0.065f;   

    // left arm
    float leftShoulderX = -0.18f; // x-coordinate for left shoulder
    float leftElbowX = leftShoulderX - upperArmLength * cos(M_PI / 2.4);
    float leftElbowY = shoulderY - upperArmLength * sin(M_PI / 2.4);

    float leftHandX = leftElbowX; // keep lower arm straight down
    float leftHandY = leftElbowY - lowerArmLength;

    float leftUpperArmVertices[] = {
        leftShoulderX, shoulderY, color[0], color[1], color[2], 1.0f, // shoulder
        leftShoulderX + upperArmWidth, shoulderY, color[0], color[1], color[2], 1.0f, // shoulder bottom
        leftElbowX + upperArmWidth, leftElbowY, color[0], color[1], color[2], 1.0f, // elbow top
        leftElbowX, leftElbowY, color[0], color[1], color[2], 1.0f  // elbow bottom
    };

    float leftLowerArmVertices[] = {
        leftElbowX, leftElbowY, color[0], color[1], color[2], 1.0f, // elbow top
        leftElbowX + lowerArmWidth, leftElbowY, color[0], color[1], color[2], 1.0f, // elbow bottom
        leftHandX + lowerArmWidth - 0.01f, leftHandY, color[0], color[1], color[2], 1.0f, // hand top
        leftHandX + 0.01f, leftHandY, color[0], color[1], color[2], 1.0f  // hand bottom
    };

    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    drawRectangle(shader, leftUpperArmVertices, indices, 6);
    drawRectangle(shader, leftLowerArmVertices, indices, 6);

    // right arm (mirrored along Y-axis)
    float rightShoulderX = 0.18f;
    float rightElbowX = rightShoulderX + upperArmLength * cos(M_PI / 2.4);
    float rightElbowY = shoulderY - upperArmLength * sin(M_PI / 2.4);

    float rightHandX = rightElbowX;
    float rightHandY = rightElbowY - lowerArmLength;

    float rightUpperArmVertices[] = {
        rightShoulderX, shoulderY, color[0], color[1], color[2], 1.0f,
        rightShoulderX - upperArmWidth, shoulderY, color[0], color[1], color[2], 1.0f,
        rightElbowX - upperArmWidth, rightElbowY, color[0], color[1], color[2], 1.0f,
        rightElbowX, rightElbowY, color[0], color[1], color[2], 1.0f
    };

    float rightLowerArmVertices[] = {
        rightElbowX, rightElbowY, color[0], color[1], color[2], 1.0f,
        rightElbowX - lowerArmWidth, rightElbowY, color[0], color[1], color[2], 1.0f,
        rightHandX - lowerArmWidth + 0.01f, rightHandY, color[0], color[1], color[2], 1.0f,
        rightHandX - 0.01f, rightHandY, color[0], color[1], color[2], 1.0f
    };

    drawRectangle(shader, rightUpperArmVertices, indices, 6);
    drawRectangle(shader, rightLowerArmVertices, indices, 6);

    drawLeftHand(textureShader, leftHandX, leftHandY);
    drawRightHand(textureShader, rightHandX, rightHandY);
}
 

void drawEllipse(Shader& shader, float centerX, float centerY, float radiusX, float radiusY, float color[]) {
    const int numVertices = 100;
    float vertices[numVertices * 6];

    for (int i = 0; i < numVertices; i++) {
        float angle = 2.0f * M_PI * i / numVertices;
        float x = radiusX * cos(angle) + centerX;
        float y = radiusY * sin(angle) + centerY;

        vertices[i * 6] = x;
        vertices[i * 6 + 1] = y;
        vertices[i * 6 + 2] = color[0];
        vertices[i * 6 + 3] = color[1];
        vertices[i * 6 + 4] = color[2];
        vertices[i * 6 + 5] = 1.0f;
    }

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    shader.use();
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, numVertices);
    glBindVertexArray(0);

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}


void Avatar::drawLegs(Shader& shader, Shader& textureShader, float color[]) {

    float torsoBottomY = -0.3f; // bottom of the torso
    float upperLegLength = 0.23f;
    float lowerLegLength = 0.27f;
    float legWidthTop = 0.135f;   // upper leg width
    float legWidthBottom = 0.08f; // width at knee
    float legWidthFoot = 0.05f;   // width at ankle (smaller for more tapered legs)
    float legGap = 0.01f;         // gap between the legs
    float kneeRadiusX = 0.045f;    // ellipse X radius for knee
    float kneeRadiusY = 0.07f;    // ellipse Y radius for knee

    float leftLegCenterX = -legGap / 2.0f - legWidthTop / 2.0f;
    float rightLegCenterX = legGap / 2.0f + legWidthTop / 2.0f;

    float upperLegBottomY = torsoBottomY - upperLegLength;
    float lowerLegBottomY = upperLegBottomY - lowerLegLength;

    // left upper leg (trapezoid)
    float leftUpperLegVertices[] = {
        leftLegCenterX - legWidthTop / 2.0f , torsoBottomY, color[0], color[1], color[2], 1.0f, // top left
        leftLegCenterX + legWidthTop / 2.0f , torsoBottomY, color[0], color[1], color[2], 1.0f, // top right
        leftLegCenterX + legWidthBottom / 2.0f , upperLegBottomY, color[0], color[1], color[2], 1.0f, // bottom right
        leftLegCenterX - legWidthBottom / 2.0f , upperLegBottomY, color[0], color[1], color[2], 1.0f  // bottom left
    };

    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    drawRectangle(shader, leftUpperLegVertices, indices, 6);

    // left knee (ellipse)
    drawEllipse(shader, leftLegCenterX , upperLegBottomY, kneeRadiusX, kneeRadiusY, color);

    // left lower leg (tapered trapezoid)
    float leftLowerLegVertices[] = {
        leftLegCenterX - legWidthBottom / 2.0f, upperLegBottomY, color[0], color[1], color[2], 1.0f, // top left
        leftLegCenterX + legWidthBottom / 2.0f, upperLegBottomY, color[0], color[1], color[2], 1.0f, // top right
        leftLegCenterX + legWidthFoot / 2.0f, lowerLegBottomY, color[0], color[1], color[2], 1.0f, // bottom right (narrower)
        leftLegCenterX - legWidthFoot / 2.0f, lowerLegBottomY, color[0], color[1], color[2], 1.0f  // bottom left (narrower)
    };

    drawRectangle(shader, leftLowerLegVertices, indices, 6);

    // right upper leg (trapezoid)
    float rightUpperLegVertices[] = {
        rightLegCenterX - legWidthTop / 2.0f, torsoBottomY, color[0], color[1], color[2], 1.0f,
        rightLegCenterX + legWidthTop / 2.0f, torsoBottomY, color[0], color[1], color[2], 1.0f,
        rightLegCenterX + legWidthBottom / 2.0f, upperLegBottomY, color[0], color[1], color[2], 1.0f,
        rightLegCenterX - legWidthBottom / 2.0f, upperLegBottomY, color[0], color[1], color[2], 1.0f
    };

    drawRectangle(shader, rightUpperLegVertices, indices, 6);

    // right knee (ellipse)
    drawEllipse(shader, rightLegCenterX, upperLegBottomY, kneeRadiusX, kneeRadiusY, color);

    // right lower leg (tapered trapezoid)
    float rightLowerLegVertices[] = {
        rightLegCenterX - legWidthBottom / 2.0f , upperLegBottomY, color[0], color[1], color[2], 1.0f,
        rightLegCenterX + legWidthBottom / 2.0f , upperLegBottomY, color[0], color[1], color[2], 1.0f,
        rightLegCenterX + legWidthFoot / 2.0f , lowerLegBottomY, color[0], color[1], color[2], 1.0f,
        rightLegCenterX - legWidthFoot / 2.0f, lowerLegBottomY, color[0], color[1], color[2], 1.0f
    };

    drawRectangle(shader, rightLowerLegVertices, indices, 6);

    float rightLegX = rightLegCenterX + legWidthFoot / 2.0f ;
    float rightLegY = lowerLegBottomY;
    float leftLegX = leftLegCenterX - legWidthFoot / 2.0f ;
    float leftLegY = lowerLegBottomY;

    drawRightFoot(textureShader, rightLegX, rightLegY);
    drawLeftFoot(textureShader, leftLegX, leftLegY);


}


void Avatar::drawFace(Shader& shader) {
    drawMouth(shader);
    drawEyes(shader);
    drawNose(shader);
    drawHair(shader);
}


void Avatar::drawEyes(Shader& shader) {
    if (eyeTexture == 0) {
        eyeTexture = loadTexture("Eyes/eyes1.png");
    }

    // enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float eyesWidth = 0.26f;
    float eyesHeight = 0.12f; 
    float eyesCenterX = 0.0f; 
    float eyesCenterY = 0.52f; 

    float vertices[] = {
        eyesCenterX - eyesWidth / 2, eyesCenterY - eyesHeight / 2, 0.0f, 0.0f, // bottom-left
        eyesCenterX + eyesWidth / 2, eyesCenterY - eyesHeight / 2, 1.0f, 0.0f, // bottom-right
        eyesCenterX + eyesWidth / 2, eyesCenterY + eyesHeight / 2, 1.0f, 1.0f, // top-right
        eyesCenterX - eyesWidth / 2, eyesCenterY + eyesHeight / 2, 0.0f, 1.0f  // top-left
    };

    unsigned int indices[] = {
        0, 1, 2, // first triangle
        0, 2, 3  // second triangle
    };

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

    shader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, eyeTexture);
    shader.setInt("texture1", 0);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}


void Avatar::drawNose(Shader& shader) {
    if (noseTexture == 0) {
        noseTexture = loadTexture("Nose/nose3.png");
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float noseWidth = 0.08f;
    float noseHeight = 0.12f;
    float noseCenterX = 0.0f;
    float noseCenterY = 0.44f;

    float vertices[] = {
        noseCenterX - noseWidth / 2, noseCenterY - noseHeight / 2, 0.0f, 0.0f, // bottom-left
        noseCenterX + noseWidth / 2, noseCenterY - noseHeight / 2, 1.0f, 0.0f, // bottom-right
        noseCenterX + noseWidth / 2, noseCenterY + noseHeight / 2, 1.0f, 1.0f, // top-right
        noseCenterX - noseWidth / 2, noseCenterY + noseHeight / 2, 0.0f, 1.0f  // top-left
    };

    unsigned int indices[] = {
        0, 1, 2, // first triangle
        0, 2, 3  // second triangle
    };

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

    shader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, noseTexture);
    shader.setInt("texture1", 0);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}


void Avatar::drawMouth(Shader& shader) {
    if(mouthTexture == 0){
        mouthTexture = loadTexture("Lips/lips1.png");
    }

    // enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float mouthWidth = 0.13f; 
    float mouthHeight = 0.06f;
    float mouthCenterX = 0.0f;
    float mouthCenterY = 0.34f;

    float vertices[] = {
        mouthCenterX - mouthWidth / 2, mouthCenterY - mouthHeight / 2, 0.0f, 0.0f, // bottom-left
        mouthCenterX + mouthWidth / 2, mouthCenterY - mouthHeight / 2, 1.0f, 0.0f, // bottom-right
        mouthCenterX + mouthWidth / 2, mouthCenterY + mouthHeight / 2, 1.0f, 1.0f, // top-right
        mouthCenterX - mouthWidth / 2, mouthCenterY + mouthHeight / 2, 0.0f, 1.0f  // top-left
    };

    unsigned int indices[] = {
        0, 1, 2, // first triangle
        0, 2, 3  // second triangle
    };

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

    shader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mouthTexture);
    shader.setInt("texture1", 0);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}


void Avatar::drawHair(Shader& shader) {
    if (hairTexture == 0) {
        hairTexture = loadTexture("hair/hair13.png");
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float hairWidth = 0.8f; 
    float hairHeight = 0.9f;
    float hairCenterX = 0.0f;
    float hairCenterY = 0.35f;

    float vertices[] = {
        hairCenterX - hairWidth / 2, hairCenterY - hairHeight / 2, 0.0f, 0.0f,
        hairCenterX + hairWidth / 2, hairCenterY - hairHeight / 2, 1.0f, 0.0f,
        hairCenterX + hairWidth / 2, hairCenterY + hairHeight / 2, 1.0f, 1.0f,
        hairCenterX - hairWidth / 2, hairCenterY + hairHeight / 2, 0.0f, 1.0f
    };

    unsigned int indices[] = {
        0, 1, 2, // first triangle
        0, 2, 3  // second triangle
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

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    shader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hairTexture);
    shader.setInt("texture1", 0);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);

    glDisable(GL_BLEND);
}


void Avatar::drawClothes(Shader& avatarShader, Shader& textureShader, std::string texture) {

        if (clothesTexture == 0) {
            clothesTexture = loadTexture("T-shirts/shirt.png");
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        float clothesWidth = 1.0f;
        float clothesHeight = 0.7f;
        float clothesCenterX = 0.0f;
        float clothesCenterY = -0.08f;

        float vertices[] = {
            clothesCenterX - clothesWidth / 2, clothesCenterY - clothesHeight / 2, 0.0f, 0.0f,
            clothesCenterX + clothesWidth / 2, clothesCenterY - clothesHeight / 2, 1.0f, 0.0f,
            clothesCenterX + clothesWidth / 2, clothesCenterY + clothesHeight / 2, 1.0f, 1.0f,
            clothesCenterX - clothesWidth / 2, clothesCenterY + clothesHeight / 2, 0.0f, 1.0f
        };

        unsigned int indices[] = {
            0, 1, 2, // first triangle
            0, 2, 3  // second triangle
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

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        textureShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, clothesTexture);
        textureShader.setInt("texture1", 0);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteVertexArrays(1, &VAO);

        glDisable(GL_BLEND);
    
};
*/

void Avatar::setSkinColor(float r, float g, float b) {
    skinColor[0] = r;
    skinColor[1] = g;
    skinColor[2] = b;
}


void Avatar::setMouthTexture(GLuint textureID) {
    mouthTexture = textureID;
}

void Avatar::setEyeTexture(GLuint textureID) {
    eyeTexture = textureID;
}

void Avatar::setNoseTexture(GLuint textureID) {
    noseTexture = textureID;
}

void Avatar::setDressTexture(GLuint textureID) {
    dressTexture = textureID;
}

void Avatar::setTshirtTexture(GLuint textureID) {
    tshirtTexture = textureID;
}

void Avatar::setPantsTexture(GLuint textureID) {
    pantsTexture = textureID;
}

void Avatar::setHairTexture(GLuint textureID) {
    hairTexture = textureID;
}


void Avatar::drawStudent(Shader& shader) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float hairWidth = 0.1f;
    float hairHeight = 0.3f;
    float hairCenterX = 0.5f;
    float hairCenterY = 0.5f;

    float vertices[] = {
    -1.0f,  0.9f,       0.0f,  0.0f,
    -0.5f,  0.9f,       1.0f,  0.0f,
    -1.0f,  1.0f,       0.0f,  1.0f,
    -0.5f,  1.0f,       1.0f,  1.0f
    };

    unsigned int indices[] = {
        0, 1, 2,
        1, 3, 2 
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

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    shader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, studentTexture);
    shader.setInt("texture1", 0);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);

    glDisable(GL_BLEND);
}