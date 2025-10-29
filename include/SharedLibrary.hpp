#ifndef SHARED_LIBRARY_HPP
#define SHARED_LIBRARY_HPP

#include <functional>
#include <stdexcept>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

class SharedLibrary {
public:
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

  SharedLibrary(const SharedLibrary &) = delete;
  SharedLibrary &operator=(const SharedLibrary &) = delete;

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
  void *m_handle = nullptr;
};

#endif // SHARED_LIBRARY_HPP
