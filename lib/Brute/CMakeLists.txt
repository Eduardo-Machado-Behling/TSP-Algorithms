cmake_minimum_required(VERSION 3.18)

# must be Folder name
project("Brute" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCE_FILES
    CONFIGURE_DEPENDS  # Important for CMake to re-run if files change
    ./src/*.cpp
)

add_library(${PROJECT_NAME} SHARED 
    ${SOURCE_FILES}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE LIB_EXPORT)

target_include_directories(${PROJECT_NAME} PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

