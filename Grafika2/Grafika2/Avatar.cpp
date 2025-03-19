#include "Avatar.h"
#include <GL/glew.h>
#include <cmath>
#include <iostream>
#include "stb_image.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Avatar::Avatar() {
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
}

void Avatar::draw(Shader& shader, Shader& textureShader, float windowWidth, float windowHeight) {
    drawNeck(shader, skinColor);
    drawHead(shader, faceColor);
    drawTorso(shader, skinColor);
    drawArms(shader, textureShader, skinColor);
    drawLegs(shader, textureShader, skinColor);
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