#include "Renderer.h"
#include "font8x8_basic.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>
#include <vector>

Renderer gameRenderer;

namespace {
    const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 uMVP;
uniform vec4 uUVRect;

void main()
{
    gl_Position = uMVP * vec4(aPos, 1.0);
    TexCoord = vec2(uUVRect.x + aTexCoord.x * uUVRect.z,
                    uUVRect.y + aTexCoord.y * uUVRect.w);
}
)";

    const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D uTexture;
uniform vec4 uColor;
uniform bool uUseTexture;
uniform bool uDiscardAlpha;
uniform bool uIsCircle;
uniform float uRimPower;

void main()
{
    if (uIsCircle) {
        vec2 p = TexCoord - vec2(0.5);
        float dist = length(p);
        if (dist > 0.5) discard;

        // Anti-aliased outer boundary edge
        float edge = 1.0 - smoothstep(0.482, 0.50, dist);

        // Glowing force-field outer rim
        float rim = smoothstep(0.32, 0.49, dist);
        rim = pow(rim, max(0.2, uRimPower * 0.5));

        // Translucent inner energy core
        float core = smoothstep(0.50, 0.0, dist) * 0.22;

        float alpha = (core + rim * 0.85) * edge * uColor.a;
        // Brighter electric cyan/white highlight on the energy rim
        vec3 col = mix(uColor.rgb, vec3(0.85, 0.96, 1.0), rim * 0.75);
        FragColor = vec4(col, alpha);
    } else if (uUseTexture) {
        vec4 texColor = texture(uTexture, TexCoord);
        if (uDiscardAlpha && texColor.a < 0.12) discard;
        FragColor = texColor * uColor;
    } else {
        FragColor = uColor;
    }
}
)";
}

Renderer::Renderer()
    : quadVAO(0), quadVBO(0), shaderProgram(0), fontTextureID(0),
      uMVPLoc(-1), uUVRectLoc(-1), uColorLoc(-1), uUseTextureLoc(-1), uDiscardAlphaLoc(-1),
      uIsCircleLoc(-1), uRimPowerLoc(-1) {}

bool Renderer::init() {
    shaderProgram = compileShaders(vertexShaderSource, fragmentShaderSource);
    if (!shaderProgram) return false;

    uMVPLoc          = glGetUniformLocation(shaderProgram, "uMVP");
    uUVRectLoc       = glGetUniformLocation(shaderProgram, "uUVRect");
    uColorLoc        = glGetUniformLocation(shaderProgram, "uColor");
    uUseTextureLoc   = glGetUniformLocation(shaderProgram, "uUseTexture");
    uDiscardAlphaLoc = glGetUniformLocation(shaderProgram, "uDiscardAlpha");
    uIsCircleLoc     = glGetUniformLocation(shaderProgram, "uIsCircle");
    uRimPowerLoc     = glGetUniformLocation(shaderProgram, "uRimPower");

    setupQuadGeometry();

    // Generate 128x64 font texture (16 columns x 8 rows of 8x8 glyphs)
    const int FONT_TEX_W = 128;
    const int FONT_TEX_H = 64;
    std::vector<unsigned char> fontPixels(FONT_TEX_W * FONT_TEX_H * 4, 0);

    for (int charIdx = 0; charIdx < 128; ++charIdx) {
        int glyphCol = charIdx % 16;
        int glyphRow = charIdx / 16;
        int startX = glyphCol * 8;
        int startY = glyphRow * 8;

        for (int py = 0; py < 8; ++py) {
            unsigned char rowBits = static_cast<unsigned char>(font8x8_basic[charIdx][py]);
            for (int px = 0; px < 8; ++px) {
                bool bitSet = (rowBits >> px) & 1;
                int pixelX = startX + px;
                int pixelY = startY + py;
                int idx = (pixelY * FONT_TEX_W + pixelX) * 4;
                if (bitSet) {
                    fontPixels[idx + 0] = 255;
                    fontPixels[idx + 1] = 255;
                    fontPixels[idx + 2] = 255;
                    fontPixels[idx + 3] = 255;
                } else {
                    fontPixels[idx + 0] = 0;
                    fontPixels[idx + 1] = 0;
                    fontPixels[idx + 2] = 0;
                    fontPixels[idx + 3] = 0;
                }
            }
        }
    }

    glGenTextures(1, &fontTextureID);
    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, FONT_TEX_W, FONT_TEX_H, 0, GL_RGBA, GL_UNSIGNED_BYTE, fontPixels.data());

    return true;
}

void Renderer::setupQuadGeometry() {
    float quadVertices[] = {
        //  X      Y     Z     U     V
        -0.5f,  0.5f, 0.0f,  0.0f, 0.0f,
         0.5f,  0.5f, 0.0f,  1.0f, 0.0f,
         0.5f, -0.5f, 0.0f,  1.0f, 1.0f,

        -0.5f,  0.5f, 0.0f,  0.0f, 0.0f,
         0.5f, -0.5f, 0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

unsigned int Renderer::loadTexture(const char* filepath) {
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filepath, &width, &height, &nrChannels, 4);
    if (!data) {
        std::cerr << "Failed to load texture file: " << filepath << std::endl;
        return 0;
    }

    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);

    return textureID;
}

void Renderer::drawQuad(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection,
                        const glm::vec4& color, bool useTexture, const glm::vec4& uvRect, unsigned int textureID, bool discardAlpha) {
    glUseProgram(shaderProgram);
    glm::mat4 mvp = projection * view * model;
    glUniformMatrix4fv(uMVPLoc, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform4fv(uColorLoc, 1, glm::value_ptr(color));
    glUniform1i(uUseTextureLoc, useTexture ? 1 : 0);
    glUniform1i(uDiscardAlphaLoc, discardAlpha ? 1 : 0);
    if (uIsCircleLoc != -1) glUniform1i(uIsCircleLoc, 0);
    glUniform4fv(uUVRectLoc, 1, glm::value_ptr(uvRect));

    if (useTexture && textureID != 0) {
        glBindTexture(GL_TEXTURE_2D, textureID);
    }

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Renderer::drawCircle(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection,
                          const glm::vec4& color, float rimPower) {
    glUseProgram(shaderProgram);
    glm::mat4 mvp = projection * view * model;
    glUniformMatrix4fv(uMVPLoc, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform4fv(uColorLoc, 1, glm::value_ptr(color));
    glUniform1i(uUseTextureLoc, 0);
    glUniform1i(uDiscardAlphaLoc, 0);
    if (uIsCircleLoc != -1) glUniform1i(uIsCircleLoc, 1);
    if (uRimPowerLoc != -1) glUniform1f(uRimPowerLoc, rimPower);
    glUniform4f(uUVRectLoc, 0.0f, 0.0f, 1.0f, 1.0f);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    if (uIsCircleLoc != -1) glUniform1i(uIsCircleLoc, 0);
}

unsigned int Renderer::compileShaders(const char* vSource, const char* fSource) {
    int success;
    char infoLog[512];

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vSource, nullptr);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        return 0;
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fSource, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        glDeleteShader(vertexShader);
        return 0;
    }

    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

void Renderer::drawText(const std::string& text, float startX, float startY, float charWidth, float charHeight,
                        const glm::vec4& color, const glm::mat4& projection, const glm::mat4& view)
{
    float curX = startX;
    float curY = startY;

    for (char c : text) {
        if (c == '\n') {
            curY -= charHeight * 1.35f;
            curX = startX;
            continue;
        }

        unsigned char uc = static_cast<unsigned char>(c);
        if (uc >= 128) uc = '?';

        int glyphCol = uc % 16;
        int glyphRow = uc / 16;

        float u  = static_cast<float>(glyphCol * 8) / 128.0f;
        float v  = static_cast<float>(glyphRow * 8) / 64.0f;
        float du = 8.0f / 128.0f;
        float dv = 8.0f / 64.0f;

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(curX + charWidth * 0.5f, curY + charHeight * 0.5f, 0.0f));
        model = glm::scale(model, glm::vec3(charWidth, charHeight, 1.0f));

        drawQuad(model, view, projection, color, true, glm::vec4(u, v, du, dv), fontTextureID);

        curX += charWidth;
    }
}

void Renderer::cleanup() {
    if (quadVAO) glDeleteVertexArrays(1, &quadVAO);
    if (quadVBO) glDeleteBuffers(1, &quadVBO);
    if (shaderProgram) glDeleteProgram(shaderProgram);
    if (fontTextureID) glDeleteTextures(1, &fontTextureID);
}
