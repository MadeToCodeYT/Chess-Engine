cmake_minimum_required(VERSION 3.15)
project(ChessEngine CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # helps IntelliSense/clangd pick up flags

# Search for raylib instead of hardcoding a path.
# HINTS gives find_path/find_library extra places to look;
# it'll still check standard system locations too.
find_path(RAYLIB_INCLUDE_DIR raylib.h
    HINTS
        "C:/raylib/include"
        "$ENV{RAYLIB_ROOT}/include"
)

find_library(RAYLIB_LIBRARY
    NAMES raylib libraylib
    HINTS
        "C:/raylib/lib"
        "$ENV{RAYLIB_ROOT}/lib"
)

if (NOT RAYLIB_INCLUDE_DIR OR NOT RAYLIB_LIBRARY)
    message(FATAL_ERROR
        "raylib not found. Set RAYLIB_ROOT environment variable to your raylib "
        "install folder (the one containing include/ and lib/), or pass "
        "-DRAYLIB_INCLUDE_DIR=... -DRAYLIB_LIBRARY=... directly to cmake."
    )
endif()

message(STATUS "Found raylib include: ${RAYLIB_INCLUDE_DIR}")
message(STATUS "Found raylib library: ${RAYLIB_LIBRARY}")

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE "${RAYLIB_INCLUDE_DIR}")
target_link_libraries(${PROJECT_NAME} PRIVATE
    "${RAYLIB_LIBRARY}"
    opengl32
    gdi32
    winmm
)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)
endif()
