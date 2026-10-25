#pragma once

#include "Common.h"

class Renderer {
public:
    Renderer();
    ~Renderer() = default;

    bool init();
    void cleanup();

    unsigned int loadTexture(const char* filepath);
    void drawQuad(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection,
                  const glm::vec4& color, bool useTexture, const glm::vec4& uvRect, unsigned int textureID, bool discardAlpha = true);
    void drawCircle(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection,
                    const glm::vec4& color, float rimPower = 2.5f);
    void drawText(const std::string& text, float startX, float startY, float charWidth, float charHeight,
                  const glm::vec4& color, const glm::mat4& projection, const glm::mat4& view = glm::mat4(1.0f));

    unsigned int getProgram() const { return shaderProgram; }
    unsigned int getFontTexture() const { return fontTextureID; }

private:
    unsigned int quadVAO;
    unsigned int quadVBO;
    unsigned int shaderProgram;
    unsigned int fontTextureID;

    GLint uMVPLoc;
    GLint uUVRectLoc;
    GLint uColorLoc;
    GLint uUseTextureLoc;
    GLint uDiscardAlphaLoc;
    GLint uIsCircleLoc;
    GLint uRimPowerLoc;

    unsigned int compileShaders(const char* vSource, const char* fSource);
    void setupQuadGeometry();
};

extern Renderer gameRenderer;
