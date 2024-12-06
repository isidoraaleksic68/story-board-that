#include "Avatar.h"
#include <GL/glew.h>
#include <cmath>
#include <iostream>
#include "stb_image.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Avatar::Avatar() {
    // Default values
    skinColor[0] = 1.0f; skinColor[1] = 0.8f; skinColor[2] = 0.6f; // Skin color
    eyeColor[0] = 0.0f; eyeColor[1] = 0.0f; eyeColor[2] = 0.0f;    // Eye color
    hairColor[0] = 0.0f; hairColor[1] = 0.0f; hairColor[2] = 0.0f; // Hair color
    hairStyle = "Short";
    outfitStyle = "Casual";
    outfitColor[0] = 0.0f; outfitColor[1] = 0.0f; outfitColor[2] = 1.0f; // Outfit color
    mouthTexture = 0;
    noseTexture = 0;
    eyeTexture = 0;
    dressTexture = 0;
    tshirtTexture = 0;
    pantsTexture = 0;
}

void Avatar::draw(Shader& shader, Shader& hairShader, float windowWidth, float windowHeight, float scrollOffset) {
    glPushMatrix();
    float avatarWidth = 1.0f * scrollOffset;
    float avatarHeight = 1.5f * scrollOffset; 
    float scaleX = avatarWidth / windowWidth;
    float scaleY = avatarHeight / windowHeight;
    glScalef(scaleX, scaleY, 1.0f);

    float offsetX = windowWidth / 2.0f;
    float offsetY = windowHeight / 2.0f;
    glTranslatef(offsetX, offsetY, 0.0f);

    shader.setVec3("skinColor", skinColor[0], skinColor[1], skinColor[2]);
    shader.setVec3("eyeColor", eyeColor[0], eyeColor[1], eyeColor[2]);
    shader.setVec3("hairColor", hairColor[0], hairColor[1], hairColor[2]);
    shader.setVec3("outfitColor", outfitColor[0], outfitColor[1], outfitColor[2]);

    float faceColor[] = { 1.0, 0.8, 0.6 };
    float skinColor[] = { 1.2, 0.8, 0.5 };
    drawNeck(shader, skinColor);
    drawHead(shader, faceColor);
    drawTorso(shader, skinColor);
    drawHands(shader,hairShader, skinColor);
    drawLegs(shader, skinColor);
    glPopMatrix();
}


GLuint Avatar::loadTextureCached(const std::string& filepath) {
    if (textureCache.find(filepath) != textureCache.end()) {
        return textureCache[filepath];
    }

    GLuint textureID = loadTexture(filepath.c_str());
    textureCache[filepath] = textureID;
    return textureID;
}


GLuint Avatar::loadTexture(const char* filepath) {
    // Generate texture ID and load texture data
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // Flip the texture vertically
    unsigned char* data = stbi_load(filepath, &width, &height, &nrChannels, 0);

    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Texture parameters
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


void Avatar::drawHead(Shader& shader, float color[]) {
    const float a = 0.15f;  // Adjusted width (x-axis)
    const float b = 0.24f;  // Adjusted height (y-axis)
    const int numVertices = 200; // Number of vertices for the ellipse

    float* vertices = new float[numVertices * 6]; // 2D positions + RGBA colors

    for (int i = 0; i < numVertices; ++i) {
        float angle = 2.0f * M_PI * i / numVertices;
        float x = a * cos(angle);
        float y = b * sin(angle);

        // Center the head at (0, 0) with some space around it (e.g., Y = 0.5 for positioning)
        vertices[i * 6] = x;
        vertices[i * 6 + 1] = y + 0.5f; // Centered on Y = 0.5 (adjust for more space if needed)
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
    const float width = 0.15f;   // Adjusted width of the neck
    const float height = 0.25f;  // Adjusted height of the neck

    // Base Y position for the neck should align below the head
    float neckBaseY = 0.2f;  // Adjusted to create more space between head and neck

    float vertices[] = {
        // X, Y, R, G, B, A
        -width / 2.0f, neckBaseY, color[0], color[1], color[2], 2.0f, // Left bottom
         width / 2.0f, neckBaseY, color[0], color[1], color[2], 2.0f, // Right bottom
         width / 2.0f, neckBaseY + height, color[0], color[1], color[2], 2.0f, // Right top
        -width / 2.0f, neckBaseY + height, color[0], color[1], color[2], 2.0f  // Left top
    };

    unsigned int indices[] = {
        0, 1, 2, // First triangle
        2, 3, 0  // Second triangle
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
    // Adjusted dimensions for a smaller torso to fit space for the legs
    float vertices[] = {
        // Upper part (shoulders)
        -0.2f, 0.22f, color[0], color[1], color[2], 1.0f, // Left shoulder
         0.2f, 0.22f, color[0], color[1], color[2], 1.0f, // Right shoulder

         // Waist (reduced height)
         -0.15f, -0.3f, color[0], color[1], color[2], 1.0f, // Left waist
          0.15f, -0.3f, color[0], color[1], color[2], 1.0f, // Right waist

          // Lower part (bottom, reduced height)
          -0.2f, -0.5f, color[0], color[1], color[2], 1.0f, // Left bottom
           0.2f, -0.5f, color[0], color[1], color[2], 1.0f, // Right bottom
    };

    unsigned int indices[] = {
        // Upper part (shoulders to waist)
        0, 1, 2,
        2, 1, 3,

        // Waist to bottom
        2, 3, 4,
        4, 3, 5,
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

void drawCircle(Shader& shader, float centerX, float centerY, float radius, float eyeColor[3]) {
    const int numVertices = 100;
    float* vertices = new float[numVertices * 6];
    for (int i = 0; i < numVertices; ++i) {
        float angle = 2.0f * M_PI * i / numVertices;
        float x = centerX + radius * cos(angle);
        float y = centerY + radius * sin(angle);

        vertices[i * 6] = x;
        vertices[i * 6 + 1] = y;
        vertices[i * 6 + 2] = eyeColor[0]; // R
        vertices[i * 6 + 3] = eyeColor[1]; // G
        vertices[i * 6 + 4] = eyeColor[2]; // B
        vertices[i * 6 + 5] = 1.0f; // A
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

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    delete[] vertices;
}



void Avatar::drawLeftHand(Shader& shader, float leftArmEndX, float leftArmEndY) {

    static GLuint mouthTexture = 0;

    // Load the mouth texture only once
    if (mouthTexture == 0) {
        mouthTexture = loadTexture("hands/leva.png");
    }


    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Vertices for a textured quad at the mouth position
    float eyesWidth = 0.18f;  // Adjust width of the mouth
    float eyesHeight = 0.22f; // Adjust height of the mouth

    float vertices[] = {
        // Positions         // Texture Coords
        leftArmEndX+0.07- eyesWidth / 2, leftArmEndY-0.09 - eyesHeight / 2, 0.0f, 0.0f, // Bottom-left
        leftArmEndX+0.07 + eyesWidth / 2, leftArmEndY-0.09 - eyesHeight / 2, 1.0f, 0.0f, // Bottom-right
        leftArmEndX+0.07 + eyesWidth / 2, leftArmEndY-0.09 + eyesHeight / 2, 1.0f, 1.0f, // Top-right
        leftArmEndX+0.07 - eyesWidth / 2, leftArmEndY-0.09 + eyesHeight / 2, 0.0f, 1.0f  // Top-left
    };

    unsigned int indices[] = {
        0, 1, 2, // First triangle
        0, 2, 3  // Second triangle
    };

    unsigned int VAO, VBO, EBO;

    // Generate and bind VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Generate and bind VBO
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Generate and bind EBO
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Define vertex attributes
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float))); // Texture coordinates
    glEnableVertexAttribArray(1);

    // Use the shader program and set up texture
    shader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mouthTexture);
    shader.setInt("texture1", 0);

    // Draw the textured quad
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Cleanup
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
};


void Avatar::drawRightHand(Shader& shader, float rightArmEndX, float rightArmEndY) {

    static GLuint mouthTexture = 0;

    // Load the mouth texture only once
    if (mouthTexture == 0) {
        mouthTexture = loadTexture("hands/desna.png");
    }


    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Vertices for a textured quad at the mouth position
    float eyesWidth = 0.18f;  // Adjust width of the mouth
    float eyesHeight = 0.22f; // Adjust height of the mouth

    float vertices[] = {
        // Positions         // Texture Coords
        rightArmEndX - 0.07 - eyesWidth / 2, rightArmEndY - 0.09 - eyesHeight / 2, 0.0f, 0.0f, // Bottom-left
        rightArmEndX - 0.07 + eyesWidth / 2, rightArmEndY - 0.09 - eyesHeight / 2, 1.0f, 0.0f, // Bottom-right
        rightArmEndX - 0.07 + eyesWidth / 2, rightArmEndY - 0.09 + eyesHeight / 2, 1.0f, 1.0f, // Top-right
        rightArmEndX - 0.07 - eyesWidth / 2, rightArmEndY - 0.09 + eyesHeight / 2, 0.0f, 1.0f  // Top-left
    };

    unsigned int indices[] = {
        0, 1, 2, // First triangle
        0, 2, 3  // Second triangle
    };

    unsigned int VAO, VBO, EBO;

    // Generate and bind VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Generate and bind VBO
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Generate and bind EBO
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Define vertex attributes
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float))); // Texture coordinates
    glEnableVertexAttribArray(1);

    // Use the shader program and set up texture
    shader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mouthTexture);
    shader.setInt("texture1", 0);

    // Draw the textured quad
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Cleanup
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);

};


void Avatar::drawHands(Shader& shader, Shader& hairShader, float color[]) {
    float shoulderY = 0.2f; // Shoulders' y-coordinate
    float armLength = 0.6f; // Length of the arm
    float armWidth = 0.1f;  // Width of the arm

    // Left arm
    float leftShoulderX = -0.2f; // X-coordinate for left shoulder
    float leftArmEndX = leftShoulderX - armLength * cos(M_PI / 2.8f); // 30 degrees
    float leftArmEndY = shoulderY - armLength * sin(M_PI / 2.8f);

    float leftArmVertices[] = {
        leftShoulderX, shoulderY, color[0], color[1], color[2], 1.0f, // Shoulder top
        leftShoulderX + armWidth, shoulderY, color[0], color[1], color[2], 1.0f, // Shoulder bottom
        leftArmEndX + armWidth, leftArmEndY,color[0], color[1], color[2], 1.0f, // Hand top
        leftArmEndX, leftArmEndY, color[0], color[1], color[2], 1.0f // Hand bottom
    };

    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    drawRectangle(shader, leftArmVertices, indices, 6);

    // Right arm (mirrored along Y-axis)
    float rightShoulderX = 0.2f;
    float rightArmEndX = rightShoulderX + armLength * cos(M_PI / 2.8f);
    float rightArmEndY = shoulderY - armLength * sin(M_PI / 2.8f);

    float rightArmVertices[] = {
        rightShoulderX, shoulderY,color[0], color[1], color[2], 1.0f,
        rightShoulderX - armWidth, shoulderY, color[0], color[1], color[2], 1.0f,
        rightArmEndX - armWidth, rightArmEndY, color[0], color[1], color[2], 1.0f,
        rightArmEndX, rightArmEndY, color[0], color[1], color[2], 1.0f
    };

    drawRectangle(shader, rightArmVertices, indices, 6);

    // Draw hands as circles

    rightArmEndX = rightShoulderX + armLength * cos(M_PI / 2.8f);
    rightArmEndY = shoulderY - armLength * sin(M_PI / 2.8f);
    leftArmEndX = leftShoulderX - armLength * cos(M_PI / 2.8f); // 30 degrees
    leftArmEndY = shoulderY - armLength * sin(M_PI / 2.8f);


    //drawCircle(shader, leftArmEndX, leftArmEndY, 0.06f,color);
    //drawCircle(shader, rightArmEndX, rightArmEndY, 0.06f, color);
    
    drawLeftHand(hairShader, leftArmEndX, leftArmEndY);
    drawRightHand(hairShader, rightArmEndX, rightArmEndY);


}



void Avatar::drawLegs(Shader& shader, float color[]) {
    float torsoBottomY = -0.5f; // Bottom of the torso
    float legLength = 0.5f;     // Length of each leg
    float legWidth = 0.15f;     // Width of each leg
    float legGap = 0.1f;        // Gap between the legs

    // Left leg
    float leftLegX = -legWidth - legGap / 2.0f;
    float leftLegY = torsoBottomY - legLength;

    float leftLegVertices[] = {
        leftLegX, torsoBottomY, color[0], color[1], color[2], 1.0f, // Top left
        leftLegX + legWidth, torsoBottomY, color[0], color[1], color[2], 1.0f, // Top right
        leftLegX + legWidth, leftLegY,color[0], color[1], color[2], 1.0f, // Bottom right
        leftLegX, leftLegY,color[0], color[1], color[2], 1.0f // Bottom left
    };

    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    drawRectangle(shader, leftLegVertices, indices, 6);

    // Right leg
    float rightLegX = legGap / 2.0f;
    float rightLegY = leftLegY;

    float rightLegVertices[] = {
        rightLegX, torsoBottomY,color[0], color[1], color[2], 1.0f,
        rightLegX + legWidth, torsoBottomY,color[0], color[1], color[2], 1.0f,
        rightLegX + legWidth, rightLegY,color[0], color[1], color[2], 1.0f,
        rightLegX, rightLegY,color[0], color[1], color[2], 1.0f
    };

    drawRectangle(shader, rightLegVertices, indices, 6);
}



void Avatar::drawFace(Shader& shader) {
    float eyeColor[] = { 0.01f,0.01f,0.01f };
    drawMouth(shader);
    drawEyes(shader);
    drawNose(shader);
    drawHair(shader);
}


void Avatar::drawEyes(Shader& shader) {
    if (eyeTexture == 0) {
        eyeTexture = loadTexture("Eyes/eyes1.png");
    }


    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Vertices for a textured quad at the mouth position
    float eyesWidth = 0.26f;  // Adjust width of the mouth
    float eyesHeight = 0.12f; // Adjust height of the mouth
    float eyesCenterX = 0.0f; // Center X position of the mouth
    float eyesCenterY = 0.52f; // Center Y position of the mouth

    float vertices[] = {
        // Positions         // Texture Coords
        eyesCenterX - eyesWidth / 2, eyesCenterY - eyesHeight / 2, 0.0f, 0.0f, // Bottom-left
        eyesCenterX + eyesWidth / 2, eyesCenterY - eyesHeight / 2, 1.0f, 0.0f, // Bottom-right
        eyesCenterX + eyesWidth / 2, eyesCenterY + eyesHeight / 2, 1.0f, 1.0f, // Top-right
        eyesCenterX - eyesWidth / 2, eyesCenterY + eyesHeight / 2, 0.0f, 1.0f  // Top-left
    };

    unsigned int indices[] = {
        0, 1, 2, // First triangle
        0, 2, 3  // Second triangle
    };

    unsigned int VAO, VBO, EBO;

    // Generate and bind VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Generate and bind VBO
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Generate and bind EBO
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Define vertex attributes
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float))); // Texture coordinates
    glEnableVertexAttribArray(1);

    // Use the shader program and set up texture
    shader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, eyeTexture);
    shader.setInt("texture1", 0);

    // Draw the textured quad
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Cleanup
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}

void Avatar::drawEyebrow(Shader& shader, float startX, float startY, float length, float lineWidth) {
    float vertices[] = {
        startX, startY, 0.0f, 0.0f, 0.0f, 1.0f, // Start point (black color)
        startX + length, startY, 0.0f, 0.0f, 0.0f, 1.0f  // End point (black color)
    };

    // Set the line width for the eyebrow thickness
    glLineWidth(lineWidth);

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glUseProgram(shader.getID());
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, 2);

    glBindVertexArray(0);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}


void Avatar::drawNose(Shader& shader) {
    if (noseTexture == 0) {
        noseTexture = loadTexture("Nose/nose3.png");
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Vertices for a textured quad at the mouth position
    float mouthWidth = 0.08f;  // Adjust width of the mouth
    float mouthHeight = 0.12f; // Adjust height of the mouth
    float mouthCenterX = 0.0f; // Center X position of the mouth
    float mouthCenterY = 0.44f; // Center Y position of the mouth

    float vertices[] = {
        // Positions         // Texture Coords
        mouthCenterX - mouthWidth / 2, mouthCenterY - mouthHeight / 2, 0.0f, 0.0f, // Bottom-left
        mouthCenterX + mouthWidth / 2, mouthCenterY - mouthHeight / 2, 1.0f, 0.0f, // Bottom-right
        mouthCenterX + mouthWidth / 2, mouthCenterY + mouthHeight / 2, 1.0f, 1.0f, // Top-right
        mouthCenterX - mouthWidth / 2, mouthCenterY + mouthHeight / 2, 0.0f, 1.0f  // Top-left
    };

    unsigned int indices[] = {
        0, 1, 2, // First triangle
        0, 2, 3  // Second triangle
    };

    unsigned int VAO, VBO, EBO;

    // Generate and bind VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Generate and bind VBO
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Generate and bind EBO
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Define vertex attributes
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float))); // Texture coordinates
    glEnableVertexAttribArray(1);

    // Use the shader program and set up texture
    shader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, noseTexture);
    shader.setInt("texture1", 0);

    // Draw the textured quad
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Cleanup
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}


void Avatar::drawMouth(Shader& shader) {
    if(mouthTexture == 0){
        mouthTexture = loadTexture("Lips/lips1.png"); // Load texture every time
    }

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Vertices for a textured quad at the mouth position
    float mouthWidth = 0.13f;  // Adjust width of the mouth
    float mouthHeight = 0.06f; // Adjust height of the mouth
    float mouthCenterX = 0.0f; // Center X position of the mouth
    float mouthCenterY = 0.34f; // Center Y position of the mouth

    float vertices[] = {
        // Positions         // Texture Coords
        mouthCenterX - mouthWidth / 2, mouthCenterY - mouthHeight / 2, 0.0f, 0.0f, // Bottom-left
        mouthCenterX + mouthWidth / 2, mouthCenterY - mouthHeight / 2, 1.0f, 0.0f, // Bottom-right
        mouthCenterX + mouthWidth / 2, mouthCenterY + mouthHeight / 2, 1.0f, 1.0f, // Top-right
        mouthCenterX - mouthWidth / 2, mouthCenterY + mouthHeight / 2, 0.0f, 1.0f  // Top-left
    };

    unsigned int indices[] = {
        0, 1, 2, // First triangle
        0, 2, 3  // Second triangle
    };

    unsigned int VAO, VBO, EBO;

    // Generate and bind VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Generate and bind VBO
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Generate and bind EBO
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Define vertex attributes
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float))); // Texture coordinates
    glEnableVertexAttribArray(1);

    // Use the shader program and set up texture
    shader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mouthTexture);
    shader.setInt("texture1", 0);

    // Draw the textured quad
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Cleanup
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}






void Avatar::drawHair(Shader& shader) {
    static GLuint hairTexture = 0;
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
        0, 1, 2, // First triangle
        0, 2, 3  // Second triangle
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

    // Draw the hair
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Cleanup
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);

    glDisable(GL_BLEND);
}


void Avatar::drawTshirt(Shader& avatarShader, Shader& textureShader, float color[], std::string texture) {
    if (texture != "") {
        if (tshirtTexture == 0) {
            tshirtTexture = loadTexture("T-shirts/shirt.png");
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        float hairWidth = 1.0f;
        float hairHeight = 0.7f;
        float hairCenterX = 0.0f;
        float hairCenterY = -0.08f;

        float vertices[] = {
            hairCenterX - hairWidth / 2, hairCenterY - hairHeight / 2, 0.0f, 0.0f,
            hairCenterX + hairWidth / 2, hairCenterY - hairHeight / 2, 1.0f, 0.0f,
            hairCenterX + hairWidth / 2, hairCenterY + hairHeight / 2, 1.0f, 1.0f,
            hairCenterX - hairWidth / 2, hairCenterY + hairHeight / 2, 0.0f, 1.0f
        };

        unsigned int indices[] = {
            0, 1, 2, // First triangle
            0, 2, 3  // Second triangle
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

        // Vertex attributes for positions and texture coordinates
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Use the hair shader and bind the hair texture
        textureShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tshirtTexture);
        textureShader.setInt("texture1", 0);

        // Draw the hair
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Cleanup
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteVertexArrays(1, &VAO);

        glDisable(GL_BLEND);
    }
    else {
        drawTorso(avatarShader, color);
    }
    
};


void Avatar::drawPants(Shader& avatarShader, Shader& textureShader, float color[], std::string texture) {
    if (texture != "") {
        if (pantsTexture == 0) {
            pantsTexture = loadTexture("Pants/brownpants.png");
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        float hairWidth = 0.53f;
        float hairHeight = 0.9f;
        float hairCenterX = -0.03f;
        float hairCenterY = -0.8f;

        float vertices[] = {
            hairCenterX - hairWidth / 2, hairCenterY - hairHeight / 2, 0.0f, 0.0f,
            hairCenterX + hairWidth / 2, hairCenterY - hairHeight / 2, 1.0f, 0.0f,
            hairCenterX + hairWidth / 2, hairCenterY + hairHeight / 2, 1.0f, 1.0f,
            hairCenterX - hairWidth / 2, hairCenterY + hairHeight / 2, 0.0f, 1.0f
        };

        unsigned int indices[] = {
            0, 1, 2, // First triangle
            0, 2, 3  // Second triangle
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

        // Vertex attributes for positions and texture coordinates
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Use the hair shader and bind the hair texture
        textureShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, pantsTexture);
        textureShader.setInt("texture1", 0);

        // Draw the hair
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Cleanup
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteVertexArrays(1, &VAO);

        glDisable(GL_BLEND);
    }
    else {
        drawTorso(avatarShader, color);
    }
};


void Avatar::drawSkirt(Shader& avatarShader, Shader& textureShader, float color[], std::string texture) {
};


void Avatar::drawDress(Shader& avatarShader, Shader& textureShader, float color[], std::string texture) {
    if (texture != "") {
        if (dressTexture == 0) {
            dressTexture = loadTexture("Dresses/dress1.png");
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        float hairWidth = 0.5f;
        float hairHeight = 0.9f;
        float hairCenterX = -0.01f;
        float hairCenterY = -0.23f;

        float vertices[] = {
            hairCenterX - hairWidth / 2, hairCenterY - hairHeight / 2, 0.0f, 0.0f,
            hairCenterX + hairWidth / 2, hairCenterY - hairHeight / 2, 1.0f, 0.0f,
            hairCenterX + hairWidth / 2, hairCenterY + hairHeight / 2, 1.0f, 1.0f,
            hairCenterX - hairWidth / 2, hairCenterY + hairHeight / 2, 0.0f, 1.0f
        };

        unsigned int indices[] = {
            0, 1, 2, // First triangle
            0, 2, 3  // Second triangle
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

        // Vertex attributes for positions and texture coordinates
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Use the hair shader and bind the hair texture
        textureShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, dressTexture);
        textureShader.setInt("texture1", 0);

        // Draw the hair
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Cleanup
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteVertexArrays(1, &VAO);

        glDisable(GL_BLEND);
    }
    else {
        drawTorso(avatarShader, color);
    }
};



// Podesi boju kože avatara
void Avatar::setSkinColor(float r, float g, float b) {
    skinColor[0] = r;
    skinColor[1] = g;
    skinColor[2] = b;
}

// Podesi boju oèiju avatara
void Avatar::setEyeColor(float r, float g, float b) {
    eyeColor[0] = r;
    eyeColor[1] = g;
    eyeColor[2] = b;
}

// Podesi boju kose avatara
void Avatar::setHairColor(float r, float g, float b) {
    hairColor[0] = r;
    hairColor[1] = g;
    hairColor[2] = b;
}

// Podesi stil kose avatara
void Avatar::setHairStyle(const std::string& style) {
    hairStyle = style;
}

// Podesi stil odeæe avatara
void Avatar::setOutfitStyle(const std::string& style) {
    outfitStyle = style;
}

// Podesi boju odeæe avatara
void Avatar::setOutfitColor(float r, float g, float b) {
    outfitColor[0] = r;
    outfitColor[1] = g;
    outfitColor[2] = b;
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