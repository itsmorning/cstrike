#include "mem.hpp"

#ifndef _WIN32
std::vector<uintptr_t> mem::find_matches(const std::string &pattern, uintptr_t address, size_t size) {
    const auto pattern_data = get_pattern_data(pattern);
    const auto first_byte = pattern_data.front();

    if (first_byte.second) {
        throw std::runtime_error(R"(First pattern byte can't be '?' or '??')");
    }

    if (size < pattern_data.size()) {
        throw std::runtime_error("Pattern size can't be greater than scan size");
    }

    std::vector<uintptr_t> data;

    for (size_t i = 0; i <= size - pattern_data.size(); i++) {
        if (*(uint8_t *)(address + i) == first_byte.first && compare_bytes(address + i, pattern_data)) {
            data.emplace_back(address + i);
        }
    }

    return data;
}

std::vector<uintptr_t> mem::find_matches_in_module(const char *module_name, const std::string &pattern) {
    uintptr_t base_address;
    size_t mem_size;

    if (!get_library_information(module_name, &base_address, &mem_size)){
        throw std::runtime_error("Could Not Get info for Module");
    }

    return find_matches(pattern, base_address, mem_size);
}

bool mem::get_library_information(const char *library, uintptr_t *address, size_t *size) {
    if (libraries.size() == 0) {
        dl_iterate_phdr([] (struct dl_phdr_info* info, size_t, void*) {
            dlinfo_t library_info = {};

            library_info.library = info->dlpi_name;
            library_info.address = info->dlpi_addr + info->dlpi_phdr[0].p_vaddr;
            library_info.size = info->dlpi_phdr[0].p_memsz;

            libraries.push_back(library_info);

            return 0;
        }, nullptr);
    }

    for (const dlinfo_t& current: libraries) {
        if (!strcasestr(current.library, library))
            continue;

        if (address)
            *address = current.address;

        if (size)
            *size = current.size;

        return true;
    }

    return false;
}

std::vector<std::pair<uint8_t, bool>> mem::get_pattern_data(const std::string &pattern) {
    std::istringstream iss(pattern);
    std::vector<std::pair<uint8_t, bool>> data;

    for (auto &it : std::vector<std::string>{ std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>() }) {
        data.emplace_back(std::strtol(it.c_str(), nullptr, 16), it == "?" || it == "??");
    }

    return data;
}

bool mem::compare_bytes(uintptr_t address, const std::vector<std::pair<uint8_t, bool>> &pattern_data) {
    for (size_t i = 0; i < pattern_data.size(); i++) {
        const auto data = pattern_data.at(i);

        if (data.second) {
            continue;
        }

        if (*(uint8_t*)(address + i) != data.first) {
            return false;
        }
    }

    return true;
}
#endif

uintptr_t mem::get_pattern(const char* module_name, const std::string &pattern) {
#ifdef _WIN32
const auto module_handle = GetModuleHandleA(module_name);

    if (!module_handle)
        return nullptr;

    static auto pattern_to_byte = [](const char* pattern) {
        auto bytes = std::vector<int>{};
        auto start = const_cast<char*>(pattern);
        auto end = const_cast<char*>(pattern) + std::strlen(pattern);

        for (auto current = start; current < end; ++current) {
            if (*current == '?') {
                ++current;

                if (*current == '?')
                    ++current;

                bytes.push_back(-1);
            }
            else {
                bytes.push_back(std::strtoul(current, &current, 16));
            }
        }
        return bytes;
    };

    auto dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(module_handle);
    auto nt_headers =
        reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<uintptr_t*>(module_handle) + dos_header->e_lfanew);

    auto size_of_image = nt_headers->OptionalHeader.SizeOfImage;
    auto pattern_bytes = pattern_to_byte(pattern.c_str());
    auto scan_bytes = reinterpret_cast<uintptr_t*>(module_handle);

    auto s = pattern_bytes.size();
    auto d = pattern_bytes.data();

    for (auto i = 0ul; i < size_of_image - s; ++i) {
        bool found = true;

        for (auto j = 0ul; j < s; ++j) {
            if (scan_bytes[i + j] != d[j] && d[j] != -1) {
                found = false;
                break;
            }
        }
        if (found)
            return &scan_bytes[i];
    }

    throw std::runtime_error(std::string("Wrong signature: ") + signature);
#else
const auto matches = find_matches_in_module(module_name, pattern);

    if (matches.empty()) {
        return 0;
    }

    return matches.front();
#endif
}