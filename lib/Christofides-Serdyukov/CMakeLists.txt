# Define a minimal project for the library.
# This helps keep the library self-contained.
project("Christofides-Serdyukov" LANGUAGES CXX CUDA)

# Set the C++ and CUDA standards for code within this library.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_STANDARD 17)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

# Ensure the CUDA Toolkit is available.
find_package(CUDAToolkit REQUIRED)

# Add the library target.
# 'SHARED' specifies to build a shared library (.dll on Windows, .so on Linux).
add_library(${PROJECT_NAME} SHARED
    src/source.cu
)

# Define a preprocessor macro when building THIS library.
# This is used in cuda_lib.h to switch between dllexport and dllimport on Windows.
target_compile_definitions(${PROJECT_NAME} PRIVATE CUDA_LIB_EXPORT)

# Specify the include directories for this library.
# PUBLIC: This is critical. It means that any target that links against
# cuda_lib will automatically inherit these include directories.
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include> # Used when creating an install package.
)

# Link this library against the CUDA runtime library.
# PUBLIC: This also propagates the linking dependency. If main_app links to
# cuda_lib, it will also be linked against the CUDA runtime.
target_link_libraries(${PROJECT_NAME} PUBLIC CUDA::cudart)