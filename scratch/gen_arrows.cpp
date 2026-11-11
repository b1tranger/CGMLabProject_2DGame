#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../include/stb_image_write.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>

static void setPixel(std::vector<uint8_t>& buf, int sheetW, int sheetH,
                     int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    if (x < 0 || x >= sheetW || y < 0 || y >= sheetH) return;
    int idx = (y * sheetW + x) * 4;
    buf[idx + 0] = r;
    buf[idx + 1] = g;
    buf[idx + 2] = b;
    buf[idx + 3] = a;
}

static bool isArrowBody(int frameIdx, float lx, float ly, bool& isCore) {
    float rx = 0.0f, ry = 0.0f;
    if (frameIdx == 0) { rx = -lx; ry = -ly; }      // Left
    else if (frameIdx == 1) { rx = lx; ry = ly; }   // Right
    else if (frameIdx == 2) { rx = -ly; ry = lx; }  // Up
    else { rx = ly; ry = -lx; }                     // Down

    isCore = false;
    bool inside = false;

    // Shaft
    if (rx >= -17.0f && rx <= 7.0f && std::abs(ry) <= 5.0f) {
        inside = true;
        if (std::abs(ry) <= 2.4f) isCore = true;
    }

    // Arrowhead triangle
    if (rx >= 2.0f && rx <= 19.0f) {
        float maxY = 19.0f - rx;
        float minYInner = std::max(0.0f, (rx - 2.0f) * 0.40f);
        if (std::abs(ry) >= minYInner && std::abs(ry) <= maxY) {
            inside = true;
            if (std::abs(ry) <= maxY * 0.55f) isCore = true;
        }
    }

    // Trailing chevron wing
    float chRx = rx + 8.0f;
    if (chRx >= 2.0f && chRx <= 13.0f) {
        float chMaxY = 13.0f - chRx;
        if (std::abs(std::abs(ry) - chMaxY) <= 1.7f && chMaxY >= 3.0f) {
            inside = true;
        }
    }
    return inside;
}

int main() {
    const int FRAME_W = 64;
    const int FRAME_H = 64;
    const int NUM_FRAMES = 4;
    const int SHEET_W = FRAME_W * NUM_FRAMES;
    const int SHEET_H = FRAME_H;

    struct Col { uint8_t r, g, b; };
    const Col primary[4] = {
        {255, 140, 30},  // Left: Orange
        {30, 220, 255},  // Right: Cyan
        {40, 255, 110},  // Up: Emerald Green
        {255, 215, 30}   // Down: Golden Amber
    };
    const Col highlight[4] = {
        {255, 215, 145},
        {185, 245, 255},
        {195, 255, 210},
        {255, 245, 175}
    };

    std::vector<uint8_t> pixels(SHEET_W * SHEET_H * 4, 0);

    for (int f = 0; f < NUM_FRAMES; ++f) {
        int ox = f * FRAME_W;

        // Pass 1: 1.5px dark silhouette outline around the arrowhead only (no disc/box background)
        for (int y = 0; y < FRAME_H; ++y) {
            for (int x = 0; x < FRAME_W; ++x) {
                bool core = false;
                if (isArrowBody(f, x - 32.0f, y - 32.0f, core)) continue;

                bool nearArrow = false;
                for (int dy = -2; dy <= 2 && !nearArrow; ++dy) {
                    for (int dx = -2; dx <= 2; ++dx) {
                        if (dx * dx + dy * dy <= 3) {
                            bool dummy = false;
                            if (isArrowBody(f, (x + dx) - 32.0f, (y + dy) - 32.0f, dummy)) {
                                nearArrow = true;
                                break;
                            }
                        }
                    }
                }
                if (nearArrow) {
                    setPixel(pixels, SHEET_W, SHEET_H, ox + x, y, 14, 18, 26, 225);
                }
            }
        }

        // Pass 2: Arrowhead body & bright core
        for (int y = 0; y < FRAME_H; ++y) {
            for (int x = 0; x < FRAME_W; ++x) {
                bool core = false;
                if (isArrowBody(f, x - 32.0f, y - 32.0f, core)) {
                    if (core) {
                        setPixel(pixels, SHEET_W, SHEET_H, ox + x, y,
                                 highlight[f].r, highlight[f].g, highlight[f].b, 255);
                    } else {
                        setPixel(pixels, SHEET_W, SHEET_H, ox + x, y,
                                 primary[f].r, primary[f].g, primary[f].b, 255);
                    }
                }
            }
        }
    }

    stbi_write_png("assets/direction_arrows.png", SHEET_W, SHEET_H, 4, pixels.data(), SHEET_W * 4);
    stbi_write_png("TheWenderer/assets/direction_arrows.png", SHEET_W, SHEET_H, 4, pixels.data(), SHEET_W * 4);
    return 0;
}

