#pragma once

#include "intro/metadata.hpp"
#include "log.hpp"

#include <CLI/CLI.hpp>

template<typename CLIConfig>
CLIConfig parseCLI(int argc, const char *argv[])
{
    CLI::App app{};

    CLIConfig config{};

    metadata::forEachField(config, [&](const auto &field) {
        app.add_option(
            std::string("--") +
                std::string(std::get<metadata::Field::Name>(field)),
            std::get<metadata::Field::Ref>(field));
    });

    try {
        app.parse(argc, argv);
    }
    catch (const ::CLI::ParseError &e) {
        info("Bad command line arguments. Error is ", e.get_name());
        std::exit(app.exit(e));
    }

    return config;
}