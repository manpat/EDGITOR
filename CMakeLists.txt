# CMAKE file for EDGITOR by Alepacho

cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(EDGITOR)

set(SOURCE_DIR "${PROJECT_SOURCE_DIR}/src/")
set(RESOURCES_PATH "${PROJECT_SOURCE_DIR}/resources/")

file(GLOB SOURCE
    ${SOURCE_DIR}/*.cpp
    ${SOURCE_DIR}/*.h
)

IF(APPLE)
    set(CMAKE_FIND_FRAMEWORK ONLY)
    find_library(SDL2_LIBRARY SDL2)
    find_library(SDL2_TTF_LIBRARY SDL2_ttf)
    
    add_executable(${PROJECT_NAME} ${SOURCE})
    
    target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY} ${SDL2_TTF_LIBRARY})
ELSEIF(WIN32)
	message("RESOURCE_PATH: " ${RESOURCE_PATH})

	find_program(NUGET nuget)
	if(NOT NUGET)
		message(FATAL "CMake could not find the nuget command line tool. Please install it!")
	else()
		set(SDL_VER "2.0.12")
		set(SDL_ttf_VER "2.0.15")
		
		set(SDL2_LIB_PATH "${CMAKE_BINARY_DIR}/packages/sdl2.nuget.${SDL_VER}/build/native/lib/x64/dynamic")
		set(SDL2_ttf_LIB_PATH "${CMAKE_BINARY_DIR}/packages/sdl2_ttf.nuget.${SDL_ttf_VER}/build/native/lib/x64/dynamic")
		
		configure_file(packages.config packages.config COPYONLY)
		
		execute_process(COMMAND 
			${NUGET} restore packages.config -SolutionDirectory ${CMAKE_BINARY_DIR}
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		)
		
		add_executable(${PROJECT_NAME} ${SOURCE})
		
		include_directories(${PROJECT_NAME} "${CMAKE_BINARY_DIR}/packages/sdl2.nuget.${SDL_VER}/build/native/include")
		include_directories(${PROJECT_NAME} "${CMAKE_BINARY_DIR}/packages/sdl2_ttf.nuget.${SDL_ttf_VER}/build/native/include")
		target_link_libraries(${PROJECT_NAME} "${SDL2_LIB_PATH}/SDL2.lib;${SDL2_LIB_PATH}/SDL2main.lib;${SDL2_ttf_LIB_PATH}/SDL2_ttf.lib")
		
		configure_file("${CMAKE_BINARY_DIR}/packages/sdl2.nuget.redist.${SDL_VER}/build/native/bin/x64/dynamic/SDL2.dll"
			"${CMAKE_BINARY_DIR}/Debug/SDL2.dll" COPYONLY)
		configure_file("${CMAKE_BINARY_DIR}/packages/sdl2_ttf.nuget.redist.${SDL_ttf_VER}/build/native/bin/x64/dynamic/SDL2_ttf.dll"
			"${CMAKE_BINARY_DIR}/Debug/SDL2_ttf.dll" COPYONLY)
		configure_file("${CMAKE_BINARY_DIR}/packages/sdl2_ttf.nuget.redist.${SDL_ttf_VER}/build/native/bin/x64/dynamic/libfreetype-6.dll"
			"${CMAKE_BINARY_DIR}/Debug/libfreetype-6.dll" COPYONLY)
		configure_file("${CMAKE_BINARY_DIR}/packages/sdl2_ttf.nuget.redist.${SDL_ttf_VER}/build/native/bin/x64/dynamic/zlib1.dll"
			"${CMAKE_BINARY_DIR}/Debug/zlib1.dll" COPYONLY)
	endif()

	target_compile_definitions(${PROJECT_NAME} PRIVATE RESOURCES_PATH="${RESOURCES_PATH}")
ELSEIF(LINUX)

ENDIF()