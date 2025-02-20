cmake_minimum_required(VERSION 3.28)

project(game LANGUAGES CXX)

find_package(Vulkan REQUIRED)
find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-static)
find_package(glm REQUIRED)

file(GLOB_RECURSE GAME_SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp" "${CMAKE_SOURCE_DIR}/src/*.hpp")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

add_executable(game ${GAME_SOURCES})
target_link_libraries(game PUBLIC Vulkan::Vulkan SDL3::SDL3-static glm::glm)
