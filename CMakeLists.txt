cmake_minimum_required(VERSION 3.28)
project(SDL_Game)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL3 CONFIG REQUIRED)
#find_package(imgui CONFIG REQUIRED)

add_executable(SDL_Game main.cpp)

target_link_libraries(SDL_Game PRIVATE SDL3::SDL3) #imgui::imgui
