cmake_minimum_required(VERSION 3.16)
project(BlackHoleSim)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.9
)

FetchContent_MakeAvailable(glfw)

find_package(OpenGL REQUIRED)

add_executable(bh-sim viewer.cpp)

target_link_libraries(bh-sim
    glfw
    OpenGL::GL
)

