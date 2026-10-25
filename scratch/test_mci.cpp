#include <windows.h>
#include <mmsystem.h>
#include <iostream>

void checkAudio(const char* alias, const char* path) {
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "open \"%s\" type mpegvideo alias %s", path, alias);
    MCIERROR err = mciSendStringA(cmd, NULL, 0, NULL);
    std::cout << "Open " << alias << ": " << err << std::endl;
    if (err == 0) {
        char status[128] = {0};
        snprintf(cmd, sizeof(cmd), "status %s length", alias);
        mciSendStringA(cmd, status, sizeof(status), NULL);
        std::cout << "Length of " << alias << " (ms): " << status << std::endl;
        snprintf(cmd, sizeof(cmd), "close %s", alias);
        mciSendStringA(cmd, NULL, 0, NULL);
    }
}

int main() {
    checkAudio("splat", "assets/enemies/boss_demon_slime/universfield-wet-splat-impact-567197.mp3");
    checkAudio("bubbling", "assets/enemies/boss_demon_slime/freesound_community-acid-bubbling-105397.mp3");
    checkAudio("roar", "assets/enemies/boss_demon_slime/audiopapkin-monster-roar-ps-o24-333057.mp3");
    checkAudio("bang", "assets/enemies/boss_demon_slime/dragon-studio-distant-bang-472364.mp3");
    return 0;
}
