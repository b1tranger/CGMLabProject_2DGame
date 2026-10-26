cmake_minimum_required(VERSION 3.15)
project(TheWenderer LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link directories
link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

# Source files
set(SOURCES
    src/main.cpp
    src/VideoPlayer.cpp
    src/Audio.cpp
    src/Renderer.cpp
    src/World.cpp
    src/UIManager.cpp
    src/LadderScene.cpp
    src/SkyFallScene.cpp
    src/glad.c
)

# Target executable
add_executable(TheWenderer ${SOURCES})

# Target libraries
target_link_libraries(TheWenderer
    glfw3
    opengl32
    gdi32
    winmm
    mfplat
    mfreadwrite
    mfuuid
    ole32
)

# Copy dlls to output directory after build
file(GLOB RUNTIME_DLLS "${CMAKE_CURRENT_SOURCE_DIR}/lib/*.dll")
add_custom_command(TARGET TheWenderer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${RUNTIME_DLLS}
    $<TARGET_FILE_DIR:TheWenderer>
)
