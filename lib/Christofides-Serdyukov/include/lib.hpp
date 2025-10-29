#ifndef LIB_HPP
#define LIB_HPP

#include <cstddef>

#if defined(_WIN32)
#ifdef LIB_EXPORT
#define LIB_API __declspec(dllexport)
#else
#define LIB_API __declspec(dllimport)
#endif
#else
#define LIB_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

LIB_API size_t TSP(int *output, const float *h_adjMatrix, size_t numVerts);

#ifdef __cplusplus
}
#endif

#endif // LIB_HPP
