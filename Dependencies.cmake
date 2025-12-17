include(FetchContent)

find_package(raylib)

if (NOT raylib_FOUND)
	FetchContent_Declare(
		raylib
		GIT_REPOSITORY https://github.com/raysan5/raylib
		GIT_TAG 5.0
	)
	
	set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
	set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

	FetchContent_MakeAvailable(raylib)	
	
endif()