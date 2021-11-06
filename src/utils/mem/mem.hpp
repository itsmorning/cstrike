#ifndef UTILS_MEM_HPP_
#define UTILS_MEM_HPP_

#ifdef _WIN32
#include <windows.h>
#else
#include <link.h>
#include <string.h>
#include <sstream>
#include <iterator>

struct dlinfo_t {
    const char *library = nullptr;
    uintptr_t address = 0;
    size_t size = 0;
};
#endif

#include <vector>
#include <string>

namespace mem {
    #ifndef _WIN32
    std::vector<uintptr_t> find_matches(const std::string &pattern, uintptr_t address, size_t size);
    std::vector<uintptr_t> find_matches_in_module(const char *module_name, const std::string &pattern);
    inline std::vector<dlinfo_t> libraries;
    bool get_library_information(const char *library, uintptr_t *address, size_t *size);
    std::vector<std::pair<uint8_t, bool>> get_pattern_data(const std::string &pattern);
    bool compare_bytes(uintptr_t addr, const std::vector<std::pair<uint8_t, bool>> &patternData);
    #endif

    uintptr_t get_pattern(const char *module_name, const std::string &pattern);

    template <typename t = void *>
    constexpr t get_vfunc(void *_this, size_t index) {
        return (*static_cast<t **>(_this))[index];
    }

    template <typename t, typename... args_t>
    constexpr t call_vfunc(void *_this, size_t index, args_t... args) {
        using virtual_fn = t(*)(void *, decltype(args)...);
        return (*static_cast<virtual_fn **>(_this))[index](_this, args...);
    }
}

#endif