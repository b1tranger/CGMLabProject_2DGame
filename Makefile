# Makefile for The Wenderer (CGM Lab Project)

win:
	@if not exist "build" mkdir build
	@if not exist "build\glfw3.dll" copy "lib\glfw3.dll" "build\glfw3.dll" >nul
	g++.exe -std=c++17 -Wall -Wextra -O2 -fdiagnostics-color=always -I./include ./src/main.cpp ./src/glad.c -o ./build/main.exe -Llib -lglfw3 -lopengl32 -lgdi32
	./build/main.exe

linux:
	@mkdir -p build
	g++ -std=c++17 -Wall -Wextra -O2 -fdiagnostics-color=always -I./include ./src/main.cpp ./src/glad.c -o ./build/main -Llib -lglfw -lGL -lXrandr -lX11 -lrt -ldl
	./build/main

clean:
	@if exist "build\main.exe" del /q "build\main.exe"
	@if exist "build\*.o" del /q "build\*.o"
