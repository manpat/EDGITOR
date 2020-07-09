# CMAKE file for EDGITOR by Alepacho
#
# macOS (x86_64):   cmake -B build -G Xcode
#   * don't forget to place required frameworks in this folder: /Library/Frameworks
# Windows 10:       cmake -B build -G "Visual Studio 16 2019 Win64"
#   * not done yet 

cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(EDGITOR)

file(GLOB SOURCE
    src/*.cpp
    src/*.h
)

# adding SDL2
IF(APPLE)
    set(CMAKE_FIND_FRAMEWORK ONLY)
    find_library(SDL2_LIBRARY SDL2)
    find_library(SDL2_TTF_LIBRARY SDL2_ttf)
    
    add_executable(${PROJECT_NAME} ${SOURCE})
    
    target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY} ${SDL2_TTF_LIBRARY})
ELSEIF(WIN32)
    
    include_directories(${PROJECT_NAME} ${SDL2_INCLUDE_DIR})
    #target_link_libraries(${PROJECT_NAME} SDL2main SDL2)

    add_executable(${PROJECT_NAME} ${SOURCE})
ELSEIF(LINUX)

ENDIF()