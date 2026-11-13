.PHONY: all build run win linux package web package-web clean

all: build

build:
	powershell -NoProfile -ExecutionPolicy Bypass -File build/win_tasks.ps1 -Action build

run:
	./build/main.exe

win: build run

linux:
	g++ -std=c++17 -Wall -Wextra -O2 -fdiagnostics-color=always -I./include ./src/main.cpp ./src/Audio.cpp ./src/Renderer.cpp ./src/World.cpp ./src/UIManager.cpp ./src/LadderScene.cpp ./src/SkyFallScene.cpp ./src/glad.c -o ./build/main -Llib -lglfw -lGL -lXrandr -lX11 -lrt -ldl
	./build/main

package: build
	powershell -NoProfile -ExecutionPolicy Bypass -File build/win_tasks.ps1 -Action package

web:
	powershell -NoProfile -ExecutionPolicy Bypass -File build_web.ps1

package-web: web

clean:
	@rm -f build/main.exe build/*.o TheWenderer_Standalone.zip TheWenderer_Web_itch.zip 2>/dev/null || del /q "build\main.exe" "build\*.o" "TheWenderer_Standalone.zip" "TheWenderer_Web_itch.zip" 2>nul || true
