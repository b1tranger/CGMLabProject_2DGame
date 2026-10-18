# Makefile for The Wenderer (CGM Lab Project)

win:
	g++.exe -std=c++17 -Wall -Wextra -O2 -fdiagnostics-color=always -I./include ./src/main.cpp ./src/glad.c -o ./build/main.exe -Llib -lglfw3 -lopengl32 -lgdi32
	./build/main.exe

linux:
	g++ -std=c++17 -Wall -Wextra -O2 -fdiagnostics-color=always -I./include ./src/main.cpp ./src/glad.c -o ./build/main -Llib -lglfw -lGL -lXrandr -lX11 -lrt -ldl
	./build/main

clean:
	@rm -f build/main.exe build/*.o 2>/dev/null || del /q "build\main.exe" "build\*.o" 2>nul || true
