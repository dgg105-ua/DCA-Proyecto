# ----------------------------------------
# Tests (CTest)
# ----------------------------------------

# ----------------------------------------
# C++
# ----------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ----------------------------------------
# Ejecutable de tests
# ----------------------------------------
add_executable(ProjectPrime8_tests
    prime8_tests.cpp
)

# ----------------------------------------
# Ruta del proyecto (para localizar assets)
# ----------------------------------------
target_compile_definitions(ProjectPrime8_tests PRIVATE
    PROJECT_ROOT="${CMAKE_SOURCE_DIR}"
)

# ----------------------------------------
# Registrar tests en CTest
# ----------------------------------------
function(add_boost_case test_name boost_filter)
    add_test(
        NAME ${test_name}
        COMMAND ProjectPrime8_tests --run_test=${boost_filter}
    )
endfunction()

# ----------------------------------------
# Suite de tests
# ----------------------------------------

add_boost_case(assets_background_exists   Assets/background_exists)
add_boost_case(assets_floor_exists        Assets/floor_exists)
add_boost_case(assets_platform_exists     Assets/platform_exists)
add_boost_case(assets_lava_exists         Assets/lava_exists)
add_boost_case(assets_wallizq_exists      Assets/wallizq_exists)
add_boost_case(assets_wallder_exists      Assets/wallder_exists)
add_boost_case(assets_font_exists         Assets/font_exists)
add_boost_case(png_background_signature   PngValidity/background_png_signature)
add_boost_case(png_platform_signature     PngValidity/platform_png_signature)
add_boost_case(locale_es_mo_exists        Locales/es_mo_exists)
add_boost_case(locale_de_mo_exists        Locales/de_mo_exists)
add_boost_case(locale_fr_mo_exists        Locales/fr_mo_exists)
