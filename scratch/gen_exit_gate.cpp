#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../include/stb_image_write.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>

static void blendPixel(std::vector<uint8_t>& buf, int sheetW, int sheetH,
                       int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    if (x < 0 || x >= sheetW || y < 0 || y >= sheetH || a == 0) return;
    int idx = (y * sheetW + x) * 4;
    float af = a / 255.0f;
    float ba = buf[idx + 3] / 255.0f;
    float outA = af + ba * (1.0f - af);
    if (outA > 0.0f) {
        buf[idx + 0] = static_cast<uint8_t>(std::min(255.0f, (r * af + buf[idx + 0] * ba * (1.0f - af)) / outA));
        buf[idx + 1] = static_cast<uint8_t>(std::min(255.0f, (g * af + buf[idx + 1] * ba * (1.0f - af)) / outA));
        buf[idx + 2] = static_cast<uint8_t>(std::min(255.0f, (b * af + buf[idx + 2] * ba * (1.0f - af)) / outA));
        buf[idx + 3] = static_cast<uint8_t>(std::min(255.0f, outA * 255.0f));
    }
}

int main() {
    const int FRAME_W = 64;
    const int FRAME_H = 64;
    const int NUM_FRAMES = 4;
    const int SHEET_W = FRAME_W * NUM_FRAMES;
    const int SHEET_H = FRAME_H;

    std::vector<uint8_t> pixels(SHEET_W * SHEET_H * 4, 0);

    for (int f = 0; f < NUM_FRAMES; ++f) {
        int ox = f * FRAME_W;
        float phase = (static_cast<float>(f) / NUM_FRAMES) * 6.2831853f;
        float pulse = 0.85f + 0.15f * std::sin(phase);

        // 1. Soft radial emerald-cyan portal glow behind the gate
        for (int y = 0; y < FRAME_H; ++y) {
            for (int x = 0; x < FRAME_W; ++x) {
                float dx = x - 31.5f;
                float dy = y - 34.0f;
                float d = std::hypot(dx, dy);
                if (d < 30.0f) {
                    float factor = std::pow(1.0f - d / 30.0f, 2.0f) * pulse;
                    uint8_t a = static_cast<uint8_t>(factor * 115.0f);
                    blendPixel(pixels, SHEET_W, SHEET_H, ox + x, y, 40, 245, 195, a);
                }
            }
        }

        // 2. Glowing open doorway portal interior (arched opening: x in [21, 42], y in [18, 53])
        for (int y = 14; y <= 53; ++y) {
            for (int x = 20; x <= 43; ++x) {
                float dx = x - 31.5f;
                bool inArch = (y >= 26) || ((dx * dx) / (11.5f * 11.5f) + ((26.0f - y) * (26.0f - y)) / (11.5f * 11.5f) <= 1.0f);
                if (!inArch) continue;

                float normDist = std::abs(dx) / 11.5f;
                float swirl = 0.5f + 0.5f * std::sin(y * 0.35f - phase + std::abs(dx) * 0.25f);
                float core = (1.0f - normDist * 0.55f) * (0.75f + 0.25f * swirl);

                uint8_t r = static_cast<uint8_t>(std::min(255.0f, 70.0f + 165.0f * core));
                uint8_t g = static_cast<uint8_t>(std::min(255.0f, 225.0f + 30.0f * core));
                uint8_t b = static_cast<uint8_t>(std::min(255.0f, 185.0f + 70.0f * core));
                blendPixel(pixels, SHEET_W, SHEET_H, ox + x, y, r, g, b, 245);
            }
        }

        // 3. Stone / Iron Archway Pillars & Arch Frame
        for (int y = 9; y <= 55; ++y) {
            for (int x = 13; x <= 50; ++x) {
                float dx = x - 31.5f;
                float dyArch = 26.0f - y;

                bool inOuter = false;
                bool inInner = false;

                if (y >= 26 && y <= 53) {
                    inOuter = (std::abs(dx) <= 16.5f);
                    inInner = (std::abs(dx) <= 11.5f);
                } else if (y < 26) {
                    float rOut = std::hypot(dx / 16.5f, dyArch / 16.5f);
                    float rIn  = std::hypot(dx / 11.5f, dyArch / 11.5f);
                    inOuter = (rOut <= 1.0f);
                    inInner = (rIn <= 1.0f);
                }

                // Threshold step at bottom (y in [53, 56])
                bool inThreshold = (y >= 53 && y <= 56 && std::abs(dx) <= 19.5f);

                if (inThreshold) {
                    bool border = (y == 53 || y == 56 || std::abs(dx) > 18.5f);
                    if (border) {
                        blendPixel(pixels, SHEET_W, SHEET_H, ox + x, y, 45, 55, 70, 255);
                    } else {
                        blendPixel(pixels, SHEET_W, SHEET_H, ox + x, y, 115, 130, 150, 255);
                    }
                } else if (inOuter && !inInner) {
                    // Stone blocks & golden keystone trim
                    float rOut = (y >= 26) ? (std::abs(dx) / 16.5f) : std::hypot(dx / 16.5f, dyArch / 16.5f);
                    float rIn  = (y >= 26) ? (std::abs(dx) / 11.5f) : std::hypot(dx / 11.5f, dyArch / 11.5f);
                    bool isRim = (rOut > 0.92f || rIn < 1.09f);
                    bool isKeystone = (y < 16 && std::abs(dx) < 3.5f);
                    bool isMortar = (y >= 26 && ((y - 26) % 7 == 0));

                    if (isKeystone) {
                        blendPixel(pixels, SHEET_W, SHEET_H, ox + x, y, 255, 205, 65, 255);
                    } else if (isRim) {
                        blendPixel(pixels, SHEET_W, SHEET_H, ox + x, y, 60, 75, 95, 255);
                    } else if (isMortar) {
                        blendPixel(pixels, SHEET_W, SHEET_H, ox + x, y, 75, 88, 108, 255);
                    } else {
                        uint8_t shade = static_cast<uint8_t>(125 + ((x * 3 + y * 5) % 24));
                        blendPixel(pixels, SHEET_W, SHEET_H, ox + x, y, shade, shade + 12, shade + 24, 255);
                    }
                }
            }
        }

        // 4. Two Swung-Open Gate Doors (angled outward to the left and right!)
        // Left open gate leaf: x from 6 to 17, perspective-angled from top y=24..53 down to outer y=20..56
        // Right open gate leaf: x from 46 to 57, symmetric
        for (int side = -1; side <= 1; side += 2) {
            for (int step = 0; step <= 11; ++step) {
                int x = (side < 0) ? (17 - step) : (46 + step);
                int topY = 25 - (step * 4) / 11;
                int botY = 53 + (step * 3) / 11;
                bool isFrameEdge = (step == 0 || step == 1 || step == 10 || step == 11);
                bool isBar = (step == 4 || step == 7);

                for (int y = topY; y <= botY; ++y) {
                    bool isHorizontalRail = (y == topY || y == topY + 1 || y == botY || y == botY - 1 || y == (topY + botY) / 2);
                    if (isFrameEdge || isHorizontalRail) {
                        // Golden-bronze & dark iron frame
                        bool highlight = (step == 10 || y == topY + 1);
                        if (highlight) {
                            blendPixel(pixels, SHEET_W, SHEET_H, ox + x, y, 245, 195, 75, 255);
                        } else {
                            blendPixel(pixels, SHEET_W, SHEET_H, ox + x, y, 165, 115, 42, 255);
                        }
                    } else if (isBar) {
                        // Iron gate vertical bars
                        blendPixel(pixels, SHEET_W, SHEET_H, ox + x, y, 95, 108, 125, 255);
                    } else {
                        // Semi-translucent dark backing between bars
                        blendPixel(pixels, SHEET_W, SHEET_H, ox + x, y, 28, 36, 48, 130);
                    }
                }
            }
        }

        // 5. Shimmering portal sparkles inside open doorway
        int sparkX[3] = { 27 + (f * 3) % 9, 34 - (f * 2) % 7, 31 + ((f % 2 == 0) ? -3 : 3) };
        int sparkY[3] = { 28 + (f * 5) % 16, 44 - (f * 4) % 14, 35 + (f * 3) % 10 };
        for (int s = 0; s < 3; ++s) {
            blendPixel(pixels, SHEET_W, SHEET_H, ox + sparkX[s], sparkY[s], 255, 255, 255, 255);
            blendPixel(pixels, SHEET_W, SHEET_H, ox + sparkX[s] - 1, sparkY[s], 190, 255, 240, 180);
            blendPixel(pixels, SHEET_W, SHEET_H, ox + sparkX[s] + 1, sparkY[s], 190, 255, 240, 180);
            blendPixel(pixels, SHEET_W, SHEET_H, ox + sparkX[s], sparkY[s] - 1, 190, 255, 240, 180);
            blendPixel(pixels, SHEET_W, SHEET_H, ox + sparkX[s], sparkY[s] + 1, 190, 255, 240, 180);
        }
    }

    stbi_write_png("assets/exit_gate.png", SHEET_W, SHEET_H, 4, pixels.data(), SHEET_W * 4);
    stbi_write_png("TheWenderer/assets/exit_gate.png", SHEET_W, SHEET_H, 4, pixels.data(), SHEET_W * 4);
    return 0;
}

