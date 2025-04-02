#include "Slides.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"

// Vertex data for a rectangle with texture coordinates
GLfloat slideVertices[] = {
    // Positions       // Colors          // Texture Coords
    -0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f, // Top-left
     0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f, // Top-right
     0.5f, -0.5f, 0.0f,  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 0.0f, // Bottom-right
    -0.5f, -0.5f, 0.0f,  1.0f, 1.0f, 1.0f, 1.0f,  0.0f, 0.0f  // Bottom-left
};

GLuint indices[] = {
    0, 1, 2,
    2, 3, 0
};

Slides::Slides(Shader& shader, Shader& avatarShader, Shader& textureShader) 
    : shader(shader), avatarShader(avatarShader), textureShader(textureShader), selectedSlideIndex(0) {
    // Initialize with three slides: one left, one main in center, one add slide
    createSlide(0.0f, 0.2f, 1.1f, 1.1f);  // Main slide (center) (index=0)
    createSlide(-0.85f, 0.65f, 0.2f, 0.2f); // First slide (left side) (index=1)
    createAddSlide(-0.85f, 0.4f, 0.2f, 0.2f); // Add Slide button (below left slides) (index=2)
    totalSlidesPage = 1;
    currentSlidesPage = 1;
}

Slides::~Slides() {
    for (auto& slide : slides) {
        glDeleteVertexArrays(1, &slide.VAO);
        glDeleteBuffers(1, &slide.VBO);
        glDeleteBuffers(1, &slide.EBO);
    }
}

void Slides::createSlide(float x, float y, float width, float height) {
    Slide slide;
    slide.x = x;
    slide.y = y;
    slide.width = width;
    slide.height = height;
    slide.textureId = 0;

    setupBuffers(slide);

    slides.push_back(slide);

}


void Slides::createAddSlide(float x, float y, float width, float height) {
    // Iterate through the slides and remove the one with a non-zero textureID
    for (auto it = slides.begin(); it != slides.end(); ++it) {
        if (it->textureId != 0) {
            slides.erase(it);  // Remove the slide with non-zero texture ID
            break;  // Break after removing the first matching slide
        }
    }

    // Create and set up the new slide
    Slide slide;
    slide.x = x;
    slide.y = y;
    slide.width = width;
    slide.height = height;

    setupBuffersTexture(slide);

    slides.push_back(slide);  // Add the new slide
}

std::vector<Slides::Slide> Slides::getActiveSlides() {
    std::vector<Slides::Slide> activeSlides;
    int slideCount = slides.size();
    int devidedTo5 = slideCount % 5;

    activeSlides.push_back(slides[0]);
    activeSlides.push_back(slides[1]);

    int startIndex = (currentSlidesPage - 1) * 5; // Calculate the starting index
    int endIndex = startIndex + 5; // Get up to 5 slides per page

    //std::cout << "Slides count: " << slides.size() << std::endl;

    if (endIndex > slides.size()) {
        endIndex = slides.size();
    }

    for (int i = startIndex; i < endIndex; ++i) {
        if (i % 5 == 0 || i % 5 == 1) {
            continue;
        }
        activeSlides.push_back(slides[i]);
    }

    //std::cout << "Active slides count: " << activeSlides.size() << std::endl;

    return activeSlides;
}


void Slides::renderSlidesBackground() {
    float color[3];
    float centralColor[3];

    color[0] = 0.922f;  // Red (EB / 255)
    color[1] = 0.686f;  // Green (AF / 255)
    color[2] = 0.663f;  // Blue (A9 / 255)

    centralColor[0] = 0.9355f;  // Red
    centralColor[1] = 0.737f;   // Green
    centralColor[2] = 0.7165f;  // Blue


    // Corrected vertex order (Top-left, Bottom-left, Top-right, Bottom-right)
    float vertices[] = {
        -1.0f,  0.8f, color[0], color[1], color[2], 1.0f, // Top-left
        -1.0f,  -1.0f, color[0], color[1], color[2], 1.0f, // Bottom-left
         -0.7f,  0.8f, color[0], color[1], color[2], 1.0f, // Top-right
         -0.7f,  -1.0f, color[0], color[1], color[2], 1.0f  // Bottom-right
    };

    float centralVertices[] = {
        -0.7f,  0.8f, centralColor[0], centralColor[1], centralColor[2], 1.0f, // Top-left
        -0.7f,  -1.0f, centralColor[0], centralColor[1], centralColor[2], 1.0f, // Bottom-left
         1.0f,  0.8f, centralColor[0], centralColor[1], centralColor[2], 1.0f, // Top-right
         1.0f,  -1.0f, centralColor[0], centralColor[1], centralColor[2], 1.0f  // Bottom-right
    };

    // Corrected indices to form two triangles: (Top-left -> Bottom-left -> Top-right) & (Bottom-left -> Bottom-right -> Top-right)
    unsigned int indices[] = { 0, 1, 2, 1, 3, 2 };

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

    glUseProgram(avatarShader.getID());
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);


    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(centralVertices), centralVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glUseProgram(avatarShader.getID());
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


GLuint loadTexture(const char* filepath) {
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
    if (!data) {
        std::cerr << "Failed to load texture: " << filepath << std::endl;
        return 0; // Return 0 to indicate failure
    }

    stbi_image_free(data);
    return textureID;
}


void Slides::renderNextButton(float x, float y, float width, float height) {
    GLuint texture = loadTexture("arrows/right.png");
    if (texture == 0) {
        std::cerr << "Error: Failed to load Add Slide texture!" << std::endl;
        return;
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float vertices[] = {
        x - width / 2, y - height / 2, 0.0f, 0.0f,
        x + width / 2, y - height / 2, 1.0f, 0.0f,
        x + width / 2, y + height / 2, 1.0f, 1.0f,
        x - width / 2, y + height / 2, 0.0f, 1.0f
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
    glBindTexture(GL_TEXTURE_2D, texture);
    textureShader.setInt("texture1", 0);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
};

void Slides::renderPreviousButton(float x, float y, float width, float height) {
    GLuint texture = loadTexture("arrows/left.png");
    if (texture == 0) {
        std::cerr << "Error: Failed to load Add Slide texture!" << std::endl;
        return;
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float vertices[] = {
        x - width / 2, y - height / 2, 0.0f, 0.0f,
        x + width / 2, y - height / 2, 1.0f, 0.0f,
        x + width / 2, y + height / 2, 1.0f, 1.0f,
        x - width / 2, y + height / 2, 0.0f, 1.0f
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
    glBindTexture(GL_TEXTURE_2D, texture);
    textureShader.setInt("texture1", 0);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
};

void Slides::setupBuffersTexture(Slide& slide) {
    GLuint texture = loadTexture("slides/addslide.png");
    if (texture == 0) {
        std::cerr << "Error: Failed to load Add Slide texture!" << std::endl;
        return;
    }
    slide.textureId = texture;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float vertices[] = {
        slide.x - slide.width / 2, slide.y - slide.height / 2, 0.0f, 0.0f,
        slide.x + slide.width / 2, slide.y - slide.height / 2, 1.0f, 0.0f,
        slide.x + slide.width / 2, slide.y + slide.height / 2, 1.0f, 1.0f,
        slide.x - slide.width / 2, slide.y + slide.height / 2, 0.0f, 1.0f
    };

    unsigned int indices[] = {
        0, 1, 2, // first triangle
        0, 2, 3  // second triangle
    };

    glGenVertexArrays(1, &slide.VAO);
    glGenBuffers(1, &slide.VBO);
    glGenBuffers(1, &slide.EBO);

    glBindVertexArray(slide.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, slide.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, slide.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    textureShader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    textureShader.setInt("texture1", 0);

    glBindVertexArray(slide.VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
}


void Slides::setupBuffers(Slide& slide) {
    glGenVertexArrays(1, &slide.VAO);
    glGenBuffers(1, &slide.VBO);
    glGenBuffers(1, &slide.EBO);

    glBindVertexArray(slide.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, slide.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(slideVertices), slideVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, slide.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Texture coordinate attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(2);


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

int Slides::calculateTotalPages() {
    int totalSlides = getSlidesCount();
    int totalPages = totalSlides / 5;
    totalSlidesPage = totalPages + 1;
    return totalSlidesPage;
};

int Slides::getSlidesCount() {
    numberOfSlides = 0;
    for (size_t i = 0; i < slides.size(); i++) {
        Slide& slide = slides[i];
        if (slide.textureId != 0) {
            continue;
        }
        else {
            numberOfSlides += 1;
        }
    }
    return numberOfSlides - 1;
}


void Slides::render(int windowWidth, int windowHeight) {
    int totalPages = calculateTotalPages();
    //std::cout << "Total pages: " << totalPages << std::endl;
    std::vector<Slide> activeSlides = getActiveSlides();

    if (currentSlidesPage < totalPages) {
        renderNextButton(-0.8f, -0.85f, 0.1f, 0.15f);
    }

    if (currentSlidesPage > 1) {
        renderPreviousButton(-0.9f, -0.85f, 0.1f, 0.15f);
    }
    
    for (size_t i = 0; i < activeSlides.size(); i++) {
        Slide& slide = activeSlides[i];

        if (slide.textureId!=0) {
            textureShader.use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, slide.textureId); // Ensure texture is bound
            textureShader.setInt("texture1", 0);
        }
        else {
            shader.use(); 
            glUniform1i(glGetUniformLocation(shader.getID(), "useTexture"), false); // Set uniform to disable textures
        }

        glBindVertexArray(slide.VAO);

        // Transformations
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(slide.x, slide.y, 0.0f));
        model = glm::scale(model, glm::vec3(slide.width, slide.height, 1.0f));

        GLuint modelLoc = glGetUniformLocation(shader.getID(), "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);

        // Draw the slide
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
}

void Slides::handleMouseClick(double mouseX, double mouseY, int windowWidth, int windowHeight) {
    float normalizedX = (mouseX / windowWidth) * 2.0f - 1.0f;
    float normalizedY = 1.0f - (mouseY / windowHeight) * 2.0f;
    std::vector<Slide> activeSlides = getActiveSlides();
    int totalPages = calculateTotalPages();

    //next button detection -0.8f, -0.85f, 0.1f, 0.15f
    if (currentSlidesPage < totalPages) {
        if (normalizedX >= -0.8f - 0.1f / 2 && normalizedX <= -0.8f + 0.1f / 2 &&
            normalizedY >= -0.85f - 0.15 / 2 && normalizedY <= -0.85f + 0.15f / 2) {
            std::cout << "Next slides!\n";
            currentSlidesPage += 1;
        }
    }
    
    if (currentSlidesPage > 1) {
        if (normalizedX >= -0.9f - 0.1f / 2 && normalizedX <= -0.9f + 0.1f / 2 &&
            normalizedY >= -0.85f - 0.15 / 2 && normalizedY <= -0.85f + 0.15f / 2) {
            std::cout << "Previous slides!\n";
            currentSlidesPage -= 1;
        }
    }

    for (size_t i = 0; i < activeSlides.size(); i++) {
        Slide& slide = activeSlides[i];

        if (normalizedX >= slide.x - slide.width / 2 && normalizedX <= slide.x + slide.width / 2 &&
            normalizedY >= slide.y - slide.height / 2 && normalizedY <= slide.y + slide.height / 2) {

            if (i == activeSlides.size() - 1) { // If clicking the last slide (Add Slide button)
                addSlide();
                return;
            }

            selectedSlideIndex = i;
            std::cout << "Slide " << i << " selected!\n";
            return;
        }
    }
}

void Slides::addSlide() {
    std::vector<Slide> activeSlides = getActiveSlides();
    int slideCount = activeSlides.size(); // Exclude the "Add Slide" button from count
    int devidedTo5 = slideCount % 5;

   // std::cout << "Active slides count from add slide function: " << slideCount << std::endl;

    if (devidedTo5 == 0) {
        currentSlidesPage += 1;
        slideCount = 2;
        slides.push_back(slides[0]);
        slides.push_back(slides[1]);
    }

    float newY = 0.65f - (slideCount - 2 ) * 0.25f;
    float nextSlideY = 0.65f - (slideCount - 1) * 0.25f;


    createSlide(-0.85f, newY, 0.2f, 0.2f);
    createAddSlide(-0.85f, nextSlideY, 0.2f, 0.2f);
}
