#ifndef COMMON_HPP_
#define COMMON_HPP_

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#include <iostream>
#include <thread>
#include <string>
#include <cstdint>
#include <chrono>

using namespace std::chrono_literals;

#include "utils/utils.hpp"

#endif