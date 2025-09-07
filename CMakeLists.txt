cmake_minimum_required(VERSION 3.10)

project(VectorFieldGenerator)

include(FetchContent)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG c1ab645
    GIT_SHALLOW 1
)
FetchContent_MakeAvailable(raylib)

FetchContent_Declare(
  raylib_cpp
  GIT_REPOSITORY https://github.com/RobLoach/raylib-cpp.git
  GIT_TAG        6d9d02cd242e8a4f78b6f971afcf992fb7a417dd # v5.5.0
)
FetchContent_MakeAvailable(raylib_cpp)

# Add executable
add_executable(${PROJECT_NAME} main.cpp)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 11)

# Link Raylib to the executable
target_link_libraries(${PROJECT_NAME} PUBLIC raylib raylib_cpp)
