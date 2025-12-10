cmake_minimum_required(VERSION 3.10)
project(ProjectPrime8)

# Usar C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Carpetas de librerías e includes
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/vendor/include
)

# Recoger todos los .cpp de src/
file(GLOB_RECURSE SOURCES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

# Crear el ejecutable
add_executable(${PROJECT_NAME} ${SOURCES})

# Definir macro ASSET_PATH con la ruta donde se instalarán los assets
add_definitions(-DASSET_PATH="${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}/assets")

# Carpeta con librerías precompiladas (Linux .a/.so, Windows .lib/.dll)
link_directories(${CMAKE_SOURCE_DIR}/vendor/lib)

# Dependencias por plataforma
if(WIN32)
    message(STATUS "Configurando para Windows")
    target_link_libraries(${PROJECT_NAME}
        raylib
        winmm
        gdi32
    )
elseif(UNIX)
    message(STATUS "Configurando para Linux/Unix")
    target_link_libraries(${PROJECT_NAME}
        ${CMAKE_SOURCE_DIR}/vendor/lib/libraylib.a
        GL
        X11
    )
endif()

# Copiar assets al build automáticamente para pruebas locales antes de instalar 
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)

# Instalar el binario
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

# Instalar la carpeta de assets
install(DIRECTORY ${CMAKE_SOURCE_DIR}/assets
        DESTINATION share/${PROJECT_NAME})
