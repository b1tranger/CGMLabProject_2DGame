#include "VideoPlayer.h"

#define NOMINMAX
#include <windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mmsystem.h>
#include <glad/glad.h>
#include <iostream>
#include <algorithm>

VideoPlayer introVideoPlayer;

VideoPlayer::VideoPlayer()
    : m_pReader(nullptr)
    , m_textureID(0)
    , m_width(0)
    , m_height(0)
    , m_isPlaying(false)
    , m_hasFinished(false)
    , m_elapsedTime(0.0f)
    , m_duration(15.0f)
    , m_lastSampleTimeHns(0)
{
}

VideoPlayer::~VideoPlayer() {
    close();
}

void VideoPlayer::decodeAudio(const std::wstring& filepath) {
    m_audioWavBuffer.clear();

    IMFSourceReader* pAudioReader = nullptr;
    HRESULT hr = MFCreateSourceReaderFromURL(filepath.c_str(), nullptr, &pAudioReader);
    if (FAILED(hr) || !pAudioReader) return;

    IMFMediaType* pAudioType = nullptr;
    MFCreateMediaType(&pAudioType);
    pAudioType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
    pAudioType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
    hr = pAudioReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, pAudioType);
    pAudioType->Release();

    if (FAILED(hr)) {
        pAudioReader->Release();
        return;
    }

    IMFMediaType* pCurrentType = nullptr;
    pAudioReader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pCurrentType);
    WAVEFORMATEX* pWave = nullptr;
    UINT32 cbWave = 0;
    MFCreateWaveFormatExFromMFMediaType(pCurrentType, &pWave, &cbWave);
    pCurrentType->Release();

    std::vector<uint8_t> pcmData;
    while (true) {
        DWORD streamIndex, flags;
        LONGLONG llTimeStamp;
        IMFSample* pSample = nullptr;
        hr = pAudioReader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, &streamIndex, &flags, &llTimeStamp, &pSample);
        if (FAILED(hr) || (flags & MF_SOURCE_READERF_ENDOFSTREAM)) {
            if (pSample) pSample->Release();
            break;
        }
        if (pSample) {
            IMFMediaBuffer* pBuffer = nullptr;
            pSample->ConvertToContiguousBuffer(&pBuffer);
            if (pBuffer) {
                BYTE* pData = nullptr;
                DWORD maxLen = 0, curLen = 0;
                pBuffer->Lock(&pData, &maxLen, &curLen);
                pcmData.insert(pcmData.end(), pData, pData + curLen);
                pBuffer->Unlock();
                pBuffer->Release();
            }
            pSample->Release();
        }
    }

    if (pWave && !pcmData.empty()) {
        uint32_t dataSize = static_cast<uint32_t>(pcmData.size());
        uint32_t totalSize = 36 + dataSize;
        m_audioWavBuffer.reserve(44 + dataSize);

        auto pushStr = [&](const char* s) { while (*s) m_audioWavBuffer.push_back((uint8_t)*s++); };
        auto push32 = [&](uint32_t val) {
            m_audioWavBuffer.push_back(val & 0xFF);
            m_audioWavBuffer.push_back((val >> 8) & 0xFF);
            m_audioWavBuffer.push_back((val >> 16) & 0xFF);
            m_audioWavBuffer.push_back((val >> 24) & 0xFF);
        };
        auto push16 = [&](uint16_t val) {
            m_audioWavBuffer.push_back(val & 0xFF);
            m_audioWavBuffer.push_back((val >> 8) & 0xFF);
        };

        pushStr("RIFF");
        push32(totalSize);
        pushStr("WAVE");
        pushStr("fmt ");
        push32(16);
        push16(pWave->wFormatTag);
        push16(pWave->nChannels);
        push32(pWave->nSamplesPerSec);
        push32(pWave->nAvgBytesPerSec);
        push16(pWave->nBlockAlign);
        push16(pWave->wBitsPerSample);
        pushStr("data");
        push32(dataSize);
        m_audioWavBuffer.insert(m_audioWavBuffer.end(), pcmData.begin(), pcmData.end());
    }

    if (pWave) CoTaskMemFree(pWave);
    pAudioReader->Release();
}

bool VideoPlayer::open(const std::wstring& filepath) {
    close();

    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    MFStartup(MF_VERSION);

    // Decode audio first so it can play synchronously
    decodeAudio(filepath);

    // Create SourceReader with video processing enabled
    IMFAttributes* pAttributes = nullptr;
    MFCreateAttributes(&pAttributes, 1);
    pAttributes->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, TRUE);

    HRESULT hr = MFCreateSourceReaderFromURL(filepath.c_str(), pAttributes, &m_pReader);
    pAttributes->Release();

    if (FAILED(hr) || !m_pReader) {
        std::cerr << "VideoPlayer: Failed to open " << std::string(filepath.begin(), filepath.end()) << std::endl;
        return false;
    }

    // Configure Video Stream to RGB32
    IMFMediaType* pVideoType = nullptr;
    MFCreateMediaType(&pVideoType);
    pVideoType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    pVideoType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32);
    hr = m_pReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, nullptr, pVideoType);
    pVideoType->Release();

    if (FAILED(hr)) {
        std::cerr << "VideoPlayer: Failed to configure RGB32 output!" << std::endl;
        close();
        return false;
    }

    // Retrieve Width, Height and Duration
    IMFMediaType* pCurrentType = nullptr;
    m_pReader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, &pCurrentType);
    UINT32 w = 0, h = 0;
    MFGetAttributeSize(pCurrentType, MF_MT_FRAME_SIZE, &w, &h);
    m_width = static_cast<int>(w);
    m_height = static_cast<int>(h);
    pCurrentType->Release();

    PROPVARIANT var;
    PropVariantInit(&var);
    hr = m_pReader->GetPresentationAttribute(MF_SOURCE_READER_MEDIASOURCE, MF_PD_DURATION, &var);
    if (SUCCEEDED(hr)) {
        m_duration = static_cast<float>(var.uhVal.QuadPart) / 10000000.0f;
    } else {
        m_duration = 15.0f;
    }
    PropVariantClear(&var);

    // Allocate OpenGL Texture for video frames
    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ONE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_BGRA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);

    m_isPlaying = true;
    m_hasFinished = false;
    m_elapsedTime = 0.0f;
    m_lastSampleTimeHns = 0;

    // Start audio playback if decoded
    if (!m_audioWavBuffer.empty()) {
        PlaySoundA(reinterpret_cast<LPCSTR>(m_audioWavBuffer.data()), NULL, SND_MEMORY | SND_ASYNC);
    }

    // Pre-decode the first frame so the screen isn't black
    update(0.001f);

    std::cout << "VideoPlayer: Playing " << m_width << "x" << m_height << " intro (" << m_duration << "s)" << std::endl;
    return true;
}

void VideoPlayer::update(float deltaTime) {
    if (!m_isPlaying || !m_pReader) return;

    m_elapsedTime += deltaTime;
    if (m_elapsedTime >= m_duration) {
        m_hasFinished = true;
        m_isPlaying = false;
        return;
    }

    LONGLONG currentTargetHns = static_cast<LONGLONG>(m_elapsedTime * 10000000.0f);

    while (m_lastSampleTimeHns <= currentTargetHns && m_isPlaying) {
        DWORD streamIndex = 0, flags = 0;
        LONGLONG llTimeStamp = 0;
        IMFSample* pSample = nullptr;

        HRESULT hr = m_pReader->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, &streamIndex, &flags, &llTimeStamp, &pSample);
        if (FAILED(hr) || (flags & MF_SOURCE_READERF_ENDOFSTREAM)) {
            if (pSample) pSample->Release();
            m_hasFinished = true;
            m_isPlaying = false;
            break;
        }

        if (pSample) {
            m_lastSampleTimeHns = llTimeStamp;
            IMFMediaBuffer* pBuffer = nullptr;
            pSample->ConvertToContiguousBuffer(&pBuffer);
            if (pBuffer) {
                BYTE* pData = nullptr;
                DWORD maxLen = 0, curLen = 0;
                pBuffer->Lock(&pData, &maxLen, &curLen);

                // Upload decoded BGRA frame buffer to GPU texture
                glBindTexture(GL_TEXTURE_2D, m_textureID);
                glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_BGRA, GL_UNSIGNED_BYTE, pData);
                glBindTexture(GL_TEXTURE_2D, 0);

                pBuffer->Unlock();
                pBuffer->Release();
            }
            pSample->Release();
        } else {
            break;
        }
    }
}

void VideoPlayer::skip() {
    PlaySoundA(NULL, NULL, 0); // Stop sound
    m_hasFinished = true;
    m_isPlaying = false;
}

void VideoPlayer::close() {
    PlaySoundA(NULL, NULL, 0);
    if (m_textureID) {
        glDeleteTextures(1, &m_textureID);
        m_textureID = 0;
    }
    if (m_pReader) {
        m_pReader->Release();
        m_pReader = nullptr;
    }
    m_isPlaying = false;
    m_hasFinished = true;
    m_audioWavBuffer.clear();
}

float VideoPlayer::getProgress() const {
    if (m_duration <= 0.0f) return 1.0f;
    return std::clamp(m_elapsedTime / m_duration, 0.0f, 1.0f);
}
