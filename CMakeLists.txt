cmake_minimum_required(VERSION 3.15)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

project(ProjectPrime8 VERSION 0.1.0 LANGUAGES CXX)

# ----------------------------------------
# Prefijo de instalación (Linux)
# ----------------------------------------
if (UNIX AND NOT WIN32)
    set(CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "Install prefix" FORCE)
endif()

# ----------------------------------------
# C++
# ----------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ----------------------------------------
# Fuentes
# ----------------------------------------
file(GLOB_RECURSE SOURCES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

# ----------------------------------------
# Includes
# ----------------------------------------
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/vendor/include
)

# ----------------------------------------
# Ruta de recursos (compile-time)
# ----------------------------------------
option(USE_SOURCE_ASSETS "Usar los assets del árbol fuente (para desarrollo)" ON)

if (WIN32)
    # En Windows se usan assets locales
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        RESOURCE_PATH="assets/"
    )
else()
    if (USE_SOURCE_ASSETS)
        # Durante desarrollo usar assets del árbol fuente
        target_compile_definitions(${PROJECT_NAME} PRIVATE
            RESOURCE_PATH="${CMAKE_SOURCE_DIR}/assets/"
        )
    else()
        # En Linux instalado
        target_compile_definitions(${PROJECT_NAME} PRIVATE
            RESOURCE_PATH="/usr/share/${PROJECT_NAME}/assets/"
        )
    endif()
endif()

# ----------------------------------------
# Raylib
# ----------------------------------------
if (WIN32)
    include(FetchContent)

    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.0
    )

    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)
    set(BUILD_TESTING  OFF CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(raylib)

    target_link_libraries(${PROJECT_NAME} PRIVATE
        raylib
        winmm
        gdi32
        opengl32
    )

elseif (UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/vendor/lib/libraylib.a
        GL
        X11
        pthread
        dl
        m
    )
endif()

# ----------------------------------------
# Copiar assets al build (Windows / debug)
# ----------------------------------------
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)

# Copiar también las traducciones (locale) al directorio del ejecutable
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/locale
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/locale
)

# ----------------------------------------
# Instalación
# ----------------------------------------
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/assets
    DESTINATION share/${PROJECT_NAME}
)

# Instalar también las traducciones
install(DIRECTORY ${CMAKE_SOURCE_DIR}/locale
    DESTINATION share/${PROJECT_NAME}
)

# ----------------------------------------
# Uninstall (make uninstall)
# ----------------------------------------
add_custom_target(uninstall
    COMMAND /usr/bin/sudo rm -rf ${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}
    COMMAND /usr/bin/sudo rm -f ${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}
    COMMENT "Removing ${PROJECT_NAME} from ${CMAKE_INSTALL_PREFIX} (may ask for sudo password)"
)

# ----------------------------------------
# CPack (Packaging)
# ----------------------------------------

set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "ProjectPrime8")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "ProjectPrime8 - Retro game engine")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_CONTACT "dev@projectprime8.com")

# Nombre del archivo generado
set(CPACK_PACKAGE_FILE_NAME
    "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_NAME}"
)

# Qué formatos generar
set(CPACK_GENERATOR "ZIP;TGZ;DEB")

# --- Configuración específica para DEB ---
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "ProjectPrime8 Team")
set(CPACK_DEBIAN_PACKAGE_SECTION "games")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libstdc++6, libgl1, libx11-6")

# El prefijo debe ser /usr para .deb
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr")

include(CPack)