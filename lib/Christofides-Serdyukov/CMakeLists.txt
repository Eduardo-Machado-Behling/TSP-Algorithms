cmake_minimum_required(VERSION 3.18)
project("Christofides-Serdyukov" LANGUAGES CXX)

add_library(${PROJECT_NAME} SHARED 
    src/main.cpp
    src/MST.cpp
    src/MWPMatching.cpp
)

target_compile_definitions(${PROJECT_NAME} PRIVATE LIB_EXPORT)

target_include_directories(${PROJECT_NAME} PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_subdirectory(vendor/blossom5)

target_link_libraries(${PROJECT_NAME} PRIVATE blossom5)