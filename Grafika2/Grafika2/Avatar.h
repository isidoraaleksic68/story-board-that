
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

    Shader& shader;
    Shader& textureShader;

public:
    Avatar(Shader& shader, Shader& textureShader);
    ~Avatar();

    struct BodyPart {
        float x, y, width, height;
        GLuint VAO, VBO, EBO;
        GLuint textureId;
        std::vector<float> vertices;       // Contains position and color per vertex
        std::vector<unsigned int> indices; // Indices for glDrawElements
    };

    struct Character {
        std::string name;
        std::vector<BodyPart> bodyParts;
    };


    std::vector<Character> characters;

    void setupHead(Character& character);
    void setupNeck(Character& character);
    void setupTorso(Character& character);
    void setupArms(Character& character);
    void setupLegs(Character& character);

    void setupNewCharacter();


    void setupBuffers(BodyPart& scenePart);
    void setupBuffersTexture(BodyPart& scenePart);


    void writeCharactersToJson(const std::vector<Avatar::Character>& characters, const std::string& filePath);
    std::vector<Character> readCharactersFromJson(const std::string& filePath);


    void drawCharacter(int characterIndex, float x, float y, float width, float height);

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

    void renderActiveCharacters(int currentMenuPage, int windowWidth, int windowHeight);
    int getTotalMenuPages();
    std::vector<Character> getAllCharacters();

    void drawRectangle(float x, float y, float width, float height);
};

#endif // AVATAR_H