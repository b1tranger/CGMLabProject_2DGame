#pragma once

#include <string>
#include <vector>
#include <cstdint>

struct IMFSourceReader;

class VideoPlayer {
public:
    VideoPlayer();
    ~VideoPlayer();

    bool open(const std::wstring& filepath);
    void update(float deltaTime);
    void close();
    void skip();

    bool isPlaying() const { return m_isPlaying; }
    bool hasFinished() const { return m_hasFinished; }
    unsigned int getTexture() const { return m_textureID; }
    unsigned int getTextureID() const { return m_textureID; }
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    float getProgress() const;
    float getDuration() const { return m_duration; }
    float getElapsedTime() const { return m_elapsedTime; }

private:
    IMFSourceReader* m_pReader;
    unsigned int m_textureID;
    int m_width;
    int m_height;
    bool m_isPlaying;
    bool m_hasFinished;
    float m_elapsedTime;
    float m_duration;
    int64_t m_lastSampleTimeHns; // in 100-ns units
    std::vector<uint8_t> m_audioWavBuffer;

    void decodeAudio(const std::wstring& filepath);
};

extern VideoPlayer introVideoPlayer;
