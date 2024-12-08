#pragma once

#include <boost/asio.hpp>

#include <iostream>

#include "intro/metadata.hpp"

#define LOG_INFO_HELPER(msg) std::cout << msg;

#define LOG_ERROR_HELPER(msg) std::cerr << msg;

#define LOG_INFO(...) FOR_EACH(LOG_INFO_HELPER, "INFO: ", __VA_ARGS__, '\n')

#define LOG_ERROR(...) FOR_EACH(LOG_ERROR_HELPER, "ERROR: ", __VA_ARGS__, '\n')