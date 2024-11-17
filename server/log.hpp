#pragma once

#include <boost/asio.hpp>

#include <iostream>
#include <string_view>

#include "intro/metadata.hpp"

#define LOG_INFO_HELPER(msg) std::cout << msg;

#define LOG_ERROR_HELPER(msg) std::cerr << msg;

#define LOG_INFO(...) FOR_EACH(LOG_INFO_HELPER, "INFO: ", __VA_ARGS__, '\n')

#define LOG_ERROR(...) FOR_EACH(LOG_ERROR_HELPER, "ERROR: ", __VA_ARGS__, '\n')

void fail(std::string_view where, boost::system::error_code error)
{
    if (error == boost::asio::error::operation_aborted)
    {
        return;
    }

    std::cerr << where << " " << error.message() << std::endl;
}

template <typename... Args>
void info(Args &&...args)
{
    ((std::cout << args << ' '), ...) << std::endl;
}