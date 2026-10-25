# Makefile for The Wenderer (CGM Lab Project)

win:
	g++.exe -std=c++17 -Wall -Wextra -O2 -fdiagnostics-color=always -static-libgcc -static-libstdc++ -I./include ./src/main.cpp ./src/VideoPlayer.cpp ./src/Audio.cpp ./src/Renderer.cpp ./src/World.cpp ./src/UIManager.cpp ./src/LadderScene.cpp ./src/SkyFallScene.cpp ./src/glad.c -o ./build/main.exe -Llib -lglfw3 -lopengl32 -lgdi32 -lwinmm -lmfplat -lmfreadwrite -lmfuuid -lole32
	./build/main.exe

linux:
	g++ -std=c++17 -Wall -Wextra -O2 -fdiagnostics-color=always -I./include ./src/main.cpp ./src/Audio.cpp ./src/Renderer.cpp ./src/World.cpp ./src/UIManager.cpp ./src/LadderScene.cpp ./src/SkyFallScene.cpp ./src/glad.c -o ./build/main -Llib -lglfw -lGL -lXrandr -lX11 -lrt -ldl
	./build/main


package:
	powershell -Command "New-Item -ItemType Directory -Force -Path dist/TheWenderer; Copy-Item build/main.exe dist/TheWenderer/TheWenderer.exe -Force; if (Test-Path 'TheWenderer') { Copy-Item build/main.exe TheWenderer/TheWenderer.exe -Force }; Copy-Item lib/glfw3.dll dist/TheWenderer/; if (Test-Path 'C:\msys64\usr\bin\msys-2.0.dll') { Copy-Item 'C:\msys64\usr\bin\msys-2.0.dll' dist/TheWenderer/ }; Copy-Item assets -Recurse dist/TheWenderer/; Compress-Archive -Path dist/TheWenderer -DestinationPath TheWenderer_Standalone.zip -Force; Remove-Item -Recurse -Force dist; Write-Host 'Standalone package created: TheWenderer_Standalone.zip'"

clean:
	@rm -f build/main.exe build/*.o TheWenderer_Standalone.zip 2>/dev/null || del /q "build\main.exe" "build\*.o" "TheWenderer_Standalone.zip" 2>nul || true
