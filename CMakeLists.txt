# cmake settings
cmake_minimum_required(VERSION 3.5)
set(CMAKE_BUILD_TYPE "Debug")

# project settings
project(Game VERSION 0 LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/build")

# project dependencies
find_package(SDL2 REQUIRED)
find_package(Vulkan REQUIRED)

# project structure
add_subdirectory("${PROJECT_SOURCE_DIR}/src") # where our game code lives :D
