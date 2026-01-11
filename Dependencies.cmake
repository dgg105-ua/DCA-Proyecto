
include(FetchContent)

# Try to find an installed raylib package quietly. If not found we will
# fetch and build raylib via FetchContent below. Using QUIET avoids printing
# a warning when raylib isn't installed (it's expected in many dev setups).
find_package(raylib QUIET)

if (NOT raylib_FOUND)
	FetchContent_Declare(
		raylib
		GIT_REPOSITORY https://github.com/raysan5/raylib
		GIT_TAG 5.5
		GIT_SHALLOW ON
	)
	
	set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
	set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

	# Inform the user that raylib will be fetched/built (can take some time)
	message(STATUS "Descargando raylib con shallow clone (mucho más rápido)...")

	FetchContent_MakeAvailable(raylib)	
	
endif()