cmake_minimum_required(VERSION 3.10)

# Requerido por raylib 5.0
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

project(ProjectPrime8 VERSION 0.1.0)

# =========================
# C++ CONFIG
# =========================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# =========================
# RAYLIB (Windows + Linux)
# =========================
find_package(raylib QUIET)

if(NOT raylib_FOUND)
    message(STATUS "Raylib no encontrada, descargando...")
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
    set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.0
    )
    FetchContent_MakeAvailable(raylib)
endif()

# =========================
# SOURCES
# =========================
file(GLOB_RECURSE PROJECT_SOURCES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

# =========================
# EXECUTABLE
# =========================
add_executable(ProjectPrime8 ${PROJECT_SOURCES})

# =========================
# DEFINICIONES (ASSETS)
# =========================
target_compile_definitions(ProjectPrime8 PRIVATE
    ASSET_PATH="./assets/"
)

# =========================
# INCLUDE PATHS (FIX MSVC)
# =========================
target_include_directories(ProjectPrime8 PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/vendor/include
)

# =========================
# LINK
# =========================
target_link_libraries(ProjectPrime8 PRIVATE raylib)

# =========================
# COPY ASSETS NEXT TO EXE
# =========================
add_custom_command(TARGET ProjectPrime8 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:ProjectPrime8>/assets
)

message(STATUS "ProjectPrime8 configurado correctamente (Windows + Linux)")

