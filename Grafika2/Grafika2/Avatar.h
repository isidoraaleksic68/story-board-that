
#ifndef AVATAR_H
#define AVATAR_H

#include "Shader.h"
#include <string>
#include <unordered_map>

class Avatar {
private:
    float skinColor[3];
    float faceColor[3];

    std::unordered_map<std::string, GLuint> textureCache;

    GLuint mouthTexture;
    GLuint eyeTexture;
    GLuint noseTexture;
    GLuint dressTexture;
    GLuint tshirtTexture;
    GLuint pantsTexture;
    GLuint studentTexture;
    GLuint hairTexture;
    GLuint clothesTexture;
    GLuint rightHandTexture;
    GLuint leftHandTexture;
    GLuint rightFootTexture;
    GLuint leftFootTexture;

public:
    Avatar();

    void draw(Shader& shader, Shader& textureShader, float windowWidth, float windowHeight);
    void drawFace(Shader& shader);
    void drawEyes(Shader& shader);
    void drawMouth(Shader& shader);
    void drawNose(Shader& shader);
    void drawHair(Shader& shader);

    void drawHead(Shader& shader, float color[]);
    void drawNeck(Shader& shader, float color[]);
    void drawTorso(Shader& shader, float color[]);
    void drawArms(Shader& shader, Shader& textureShader, float color[]);
    void drawLegs(Shader& shader, Shader& textureShader, float color[]);
    void drawLeftFoot(Shader& shader, float leftFootEndX, float leftFootEndY);
    void drawRightFoot(Shader& shader, float rightFootEndX, float rightFootEndY); 
    void drawLeftHand(Shader& shader, float leftArmEndX, float leftArmEndY);
    void drawRightHand(Shader& shader, float rightArmEndX, float rightArmEndY);


    void drawClothes(Shader& avatarShader, Shader& textureShader, std::string texture);

    GLuint loadTexture(const char* filepath);
    GLuint loadTextureCached(const std::string& filepath);
    

    void setSkinColor(float r, float g, float b);
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