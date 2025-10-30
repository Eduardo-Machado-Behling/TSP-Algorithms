cmake_minimum_required(VERSION 3.15)
project(blossom5)

set(CMAKE_CXX_STANDARD 17)

add_library(${PROJECT_NAME} STATIC
    PMinit.cpp
    PMinterface.cpp
    PMduals.cpp
    PMexpand.cpp
    PMmain.cpp
    PMrepair.cpp
    PMshrink.cpp
    misc.cpp
MinCost/MinCost.cpp   # <- important: implementation of DualMinCost lives here
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_definitions(${PROJECT_NAME} PRIVATE PERFECT_MATCHING_DOUBLE)


