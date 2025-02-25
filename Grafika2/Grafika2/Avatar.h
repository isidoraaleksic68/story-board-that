
#ifndef AVATAR_H
#define AVATAR_H

#include "Shader.h"
#include <string>
#include <unordered_map>

class Avatar {
private:
    float skinColor[3];
    float eyeColor[3];
    float hairColor[3];
    std::string hairStyle;
    std::string outfitStyle;
    float outfitColor[3];
    std::unordered_map<std::string, GLuint> textureCache;
    GLuint mouthTexture;
    GLuint eyeTexture;
    GLuint noseTexture;
    GLuint dressTexture;
    GLuint tshirtTexture;
    GLuint pantsTexture;
    GLuint studentTexture;
    GLuint hairTexture;

public:
    Avatar();
    void setSkinColor(float r, float g, float b);
    void setEyeColor(float r, float g, float b);
    void setHairColor(float r, float g, float b);
    void setHairStyle(const std::string& style);
    void setOutfitStyle(const std::string& style);
    void setOutfitColor(float r, float g, float b);

    void draw(Shader& shader, Shader& hairShader, float windowWidth, float windowHeight, float scrollOffset);
    void drawHead(Shader& shader, float color[]);
    void drawFace(Shader& shader);
    void drawEyes(Shader& shader);
    void drawMouth(Shader& shader);
    void drawNose(Shader& shader);
    void drawEyebrow(Shader& shader, float x, float y, float radius, float lineWidth);
    void drawHair(Shader& shader);
    void drawNeck(Shader& shader, float color[]);
    void drawTorso(Shader& shader, float color[]);
    void drawHands(Shader& shader, Shader& hairShader, float color[]);
    void drawLegs(Shader& shader, float color[]);
    void drawTshirt(Shader& avatarShader, Shader& textureShader, float color[], std::string texture);
    void drawPants(Shader& avatarShader, Shader& textureShader, float color[], std::string texture);
    void drawSkirt(Shader& avatarShader, Shader& textureShader, float color[], std::string texture);
    void drawDress(Shader& avatarShader, Shader& textureShader, float color[], std::string texture);
    void drawLeftHand(Shader& shader, float leftArmEndX, float leftArmEndY);
    void drawRightHand(Shader& shader, float rightArmEndX, float rightArmEndY);
    GLuint loadTexture(const char* filepath);
    GLuint loadTextureCached(const std::string& filepath);
    void setMouthTexture(GLuint textureID);
    void setEyeTexture(GLuint textureID);
    void setNoseTexture(GLuint textureID);
    void setDressTexture(GLuint textureID);
    void setTshirtTexture(GLuint textureID);
    void setPantsTexture(GLuint textureID);
    void setHairTexture(GLuint textureID);
    void drawStudent(Shader& shader);
};

#endif // AVATAR_H