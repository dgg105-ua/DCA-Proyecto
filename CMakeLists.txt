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


include(${CMAKE_SOURCE_DIR}/Dependencies.cmake)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        raylib
        winmm
        gdi32
        opengl32
    )

else()
    
    find_package(raylib QUIET)

    target_link_libraries(${PROJECT_NAME} PRIVATE
        raylib
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
if(WIN32)
    # En Windows: ejecutable y assets juntos en la raíz del paquete
    install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION .
    )
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/assets
        DESTINATION .
    )
else()
    # En Linux: estructura estándar FHS
    install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
    )
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/assets
        DESTINATION share/${PROJECT_NAME}
    )
endif()

# Instalar también las traducciones
if(NOT WIN32)
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/locale
        DESTINATION share/${PROJECT_NAME}
    )
endif()


if(TARGET uninstall)
    message(STATUS "Target 'uninstall' already exists (from a dependency); defining 'uninstall_project' for this project")
    set(_PP8_UNINSTALL_TARGET uninstall_project)
else()
    set(_PP8_UNINSTALL_TARGET uninstall)
endif()

set(_PP8_BINARY_NAME "${PROJECT_NAME}")
if(WIN32)
    set(_PP8_BINARY_NAME "${PROJECT_NAME}.exe")
endif()

add_custom_target(${_PP8_UNINSTALL_TARGET}
    COMMAND ${CMAKE_COMMAND} -E remove -f "${CMAKE_INSTALL_PREFIX}/bin/${_PP8_BINARY_NAME}"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}"
    COMMENT "Removing ${PROJECT_NAME} from ${CMAKE_INSTALL_PREFIX}"
)

# ----------------------------------------
# CPack (Packaging)
# ----------------------------------------

set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "ProjectPrime8")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Videojuego Project Prime-8")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")

# Nombre del archivo generado
set(CPACK_PACKAGE_FILE_NAME
    "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_NAME}"
)

# Establecer generadores según el sistema operativo
if(WIN32)
    set(CPACK_GENERATOR "ZIP;NSIS")
else()
    set(CPACK_GENERATOR "ZIP;TGZ;DEB")
endif()

# Configuración específica para DEB
if(UNIX AND NOT WIN32)
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Sector-8")
    set(CPACK_DEBIAN_PACKAGE_SECTION "games")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libstdc++6, libgl1, libx11-6")
    set(CPACK_PACKAGING_INSTALL_PREFIX "/usr")
endif()

# Configuración específica para Windows (NSIS)
if(WIN32)
    set(CPACK_NSIS_DISPLAY_NAME "ProjectPrime8")
    set(CPACK_NSIS_PACKAGE_NAME "ProjectPrime8")
    set(CPACK_NSIS_MODIFY_PATH OFF)
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
    # Por defecto: C:\Program Files\ProjectPrime8
    set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES")
endif()

include(CPack)