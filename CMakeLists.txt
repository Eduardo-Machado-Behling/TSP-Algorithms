# Specify the minimum version of CMake required.
# Modern CUDA support was significantly improved around version 3.18.
cmake_minimum_required(VERSION 3.18)

# Define the project name and specify its languages.
# This is the key step! It tells CMake to find and enable both the
# C++ compiler (g++, clang++, msvc) and the CUDA compiler (nvcc).
project(TSP LANGUAGES CXX CUDA)

# Set the C++ standard you want to use.
# This applies the standard to all CXX targets in the project.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

set(CMAKE_CUDA_ARCHITECTURES "52;75;86" CACHE STRING "CUDA architectures to build for (e.g., '75;86')")


# Find the CUDA Toolkit package. This provides access to CUDA-specific
# variables and libraries. The modern way is to use find_package.
# This also ensures that the toolkit is actually installed.
find_package(CUDAToolkit REQUIRED)

# Add your executable target.
# Include all source files, whether they are .cpp or .cu.
# CMake is smart enough to use the correct compiler for each file.
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

# Create an empty list to hold the names of all library targets we find.
set(ALL_LIBRARY_TARGETS "")

# Find all immediate subdirectories within the 'lib' folder.
file(GLOB LIB_SUBDIRS LIST_DIRECTORIES true "${CMAKE_CURRENT_SOURCE_DIR}/lib/*")

# Loop through each directory found.
foreach(LIB_DIR ${LIB_SUBDIRS})
    # Check if the item is actually a directory and contains a CMakeLists.txt.
    if(IS_DIRECTORY ${LIB_DIR} AND EXISTS "${LIB_DIR}/CMakeLists.txt")
        # Extract the folder name to use as the library's target name.
        get_filename_component(LIB_NAME ${LIB_DIR} NAME)
        message(STATUS "Found and adding library: ${LIB_NAME}")

        # Add the subdirectory. CMake will process its CMakeLists.txt,
        # creating a target with the name we just extracted.
        add_subdirectory(${LIB_DIR})

        # Add the new library target name to our list.
        list(APPEND ALL_LIBRARY_TARGETS ${LIB_NAME})
    endif()
endforeach()

# Link the executable against ALL the shared libraries we discovered.
if(ALL_LIBRARY_TARGETS)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${ALL_LIBRARY_TARGETS})
endif()



# --- Post-Build Steps for Copying DLLs/SOs ---
if(WIN32)
    # Loop through each library target and create a command to copy its DLL.
    foreach(LIB_TARGET ${ALL_LIBRARY_TARGETS})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:${LIB_TARGET}>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
            COMMENT "Copying ${LIB_TARGET}.dll to executable directory"
        )
    endforeach()
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