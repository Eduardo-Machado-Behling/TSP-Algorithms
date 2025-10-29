cmake_minimum_required(VERSION 3.18)

project(TSP LANGUAGES CXX)

# Set the C++ standard you want to use.
# This applies the standard to all CXX targets in the project.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

add_executable(${PROJECT_NAME}
    src/main.cpp
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    "SHARED_EXTENSION_STR=\"${CMAKE_SHARED_LIBRARY_SUFFIX}\""
)

# Specify include directories so the compiler can find your header files.
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(ALL_LIBRARY_TARGETS "")

file(GLOB LIB_SUBDIRS LIST_DIRECTORIES true "${CMAKE_CURRENT_SOURCE_DIR}/lib/*")

foreach(LIB_DIR ${LIB_SUBDIRS})
    if(IS_DIRECTORY ${LIB_DIR} AND EXISTS "${LIB_DIR}/CMakeLists.txt")
        get_filename_component(LIB_NAME ${LIB_DIR} NAME)
        message(STATUS "Found and adding library: ${LIB_NAME}")

        add_subdirectory(${LIB_DIR})

        list(APPEND ALL_LIBRARY_TARGETS ${LIB_NAME})
    endif()
endforeach()

if(ALL_LIBRARY_TARGETS)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${ALL_LIBRARY_TARGETS})
endif()



# Check if a 'samples' directory exists at the project root.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/samples")
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/samples"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/samples"
        COMMENT "Copying samples directory to executable location."
    )
endif()