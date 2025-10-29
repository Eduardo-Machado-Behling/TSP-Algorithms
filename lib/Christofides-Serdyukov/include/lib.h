#ifndef CUDA_LIB_H
#define CUDA_LIB_H

// Use standard integer types for cross-platform compatibility.
#include <cstddef>

// This block handles the necessary symbol exporting/importing for Windows DLLs.
// When we build the library, we define CUDA_LIB_EXPORT, so CUDA_LIB_API expands
// to __declspec(dllexport). When the main application includes this header,
// CUDA_LIB_EXPORT is not defined, so it expands to __declspec(dllimport).
#if defined(_WIN32)
#ifdef CUDA_LIB_EXPORT
#define CUDA_LIB_API __declspec(dllexport)
#else
#define CUDA_LIB_API __declspec(dllimport)
#endif
#else
// On Linux and macOS, this is not needed.
#define CUDA_LIB_API
#endif

// We wrap the function declarations in extern "C" to prevent C++ name mangling.
// This ensures that the function name in the compiled library is exactly
// "perform_vector_add", making it easy to find and link against.
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Adds two vectors element-wise on the GPU using CUDA.
 *
 * This function handles all CUDA memory management and kernel launching
 * internally.
 *
 * @param a Pointer to the first input host vector.
 * @param b Pointer to the second input host vector.
 * @param c Pointer to the output host vector where the result is stored.
 * @param n The number of elements in each vector.
 */
CUDA_LIB_API void TSP(float* out, const float* data, size_t rows, size_t cols);

#ifdef __cplusplus
}
#endif

#endif // CUDA_LIB_H
