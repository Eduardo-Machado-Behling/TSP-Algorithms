#ifndef SHARED_LIBRARY_LOADER_H
#define SHARED_LIBRARY_LOADER_H

#include <functional>
#include <stdexcept>
#include <string>


// Platform-specific includes
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

/**
 * @class SharedLibrary
 * @brief A cross-platform wrapper for dynamically loading shared libraries
 * (.dll or .so).
 *
 * This class abstracts the platform-specific details of loading a shared
 * library and retrieving function pointers from it.
 */
class SharedLibrary {
public:
  /**
   * @brief Loads a shared library from the given path.
   * @param path The path to the shared library file.
   * @throws std::runtime_error if the library cannot be loaded.
   */
  explicit SharedLibrary(const std::string &path) {
#ifdef _WIN32
    m_handle = LoadLibraryA(path.c_str());
    if (!m_handle) {
      throw std::runtime_error(
          "Failed to load library: " + path +
          " - Error code: " + std::to_string(GetLastError()));
    }
#else
    m_handle = dlopen(path.c_str(), RTLD_LAZY);
    if (!m_handle) {
      throw std::runtime_error("Failed to load library: " + path + " - " +
                               dlerror());
    }
#endif
  }

  /**
   * @brief Unloads the library.
   */
  ~SharedLibrary() {
#ifdef _WIN32
    if (m_handle) {
      FreeLibrary(static_cast<HMODULE>(m_handle));
    }
#else
    if (m_handle) {
      dlclose(m_handle);
    }
#endif
  }

  // Disable copy constructor and assignment operator to prevent handle
  // mismanagement.
  SharedLibrary(const SharedLibrary &) = delete;
  SharedLibrary &operator=(const SharedLibrary &) = delete;

  /**
   * @brief Retrieves a function pointer from the loaded library.
   * @tparam T The function signature type (e.g., `void(int, float)`).
   * @param functionName The name of the function to retrieve.
   * @return A std::function wrapper around the loaded function.
   * @throws std::runtime_error if the function cannot be found.
   */
  template <typename T>
  std::function<T> getFunction(const std::string &functionName) {
#ifdef _WIN32
    FARPROC proc =
        GetProcAddress(static_cast<HMODULE>(m_handle), functionName.c_str());
#else
    void *proc = dlsym(m_handle, functionName.c_str());
#endif
    if (!proc) {
      throw std::runtime_error("Function not found: " + functionName);
    }
    return reinterpret_cast<T *>(proc);
  }

private:
#ifdef _WIN32
  void *m_handle = nullptr; // Using HMODULE (which is a void*)
#else
  void *m_handle = nullptr;
#endif
};

#endif // SHARED_LIBRARY_LOADER_H
