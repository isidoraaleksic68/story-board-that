#ifndef SLIDES_H
#define SLIDES_H

#include <vector>
#include "Shader.h"
#include <GL/glew.h>

class Slides {
public:
    Slides(Shader& shader,Shader& avatarShader, Shader& textureShader);
    ~Slides();

    void render(int windowWidth, int windowHeight);
    void handleMouseClick(double mouseX, double mouseY, int windowWidth, int windowHeight);
    void addSlide();
    void renderSlidesBackground();
    void renderNextButton(float x, float y, float width, float height);
    void renderPreviousButton(float x, float y, float width, float height);

private:
    struct Slide {
        float x, y, width, height;
        GLuint VAO, VBO, EBO;
        GLuint scene;
        GLuint textureId;

    };

    Shader& avatarShader;
    Shader& shader;
    Shader& textureShader;

    std::vector<Slide> slides;
    std::vector<Slide> activeSlides;

    int selectedSlideIndex;
    int totalSlidesPage;
    int currentSlidesPage;
    int numberOfSlides;

    void createSlide(float x, float y, float width, float height);
    void createAddSlide(float x, float y, float width, float height);
    void setupBuffers(Slide& slide);
    void setupBuffersTexture(Slide& slide);
    int calculateTotalPages();
    int getSlidesCount();
    std::vector<Slide> getActiveSlides();
};

#endif
