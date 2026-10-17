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
    src/glad.c
)

# Target executable
add_executable(TheWenderer ${SOURCES})

# Target libraries
target_link_libraries(TheWenderer
    glfw3
    opengl32
    gdi32
)

# Copy glfw3.dll to output directory after build
add_custom_command(TARGET TheWenderer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw3.dll"
    $<TARGET_FILE_DIR:TheWenderer>
)
