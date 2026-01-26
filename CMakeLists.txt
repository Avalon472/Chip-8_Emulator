cmake_minimum_required(VERSION 3.10)

project(chip8_emulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)

add_executable(chip8
    src/main.cpp
    src/Chip8.cpp
)

target_include_directories(chip8 PRIVATE
    ${SDL2_INCLUDE_DIRS}
)

target_link_libraries(chip8
    ${SDL2_LIBRARIES}
)
