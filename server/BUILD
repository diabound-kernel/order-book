load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "lib-boost",
    srcs = glob(["*/*.cpp"]),
    hdrs = glob(["*/*.hpp"]),
    deps = [
        "@boost//:asio",
        "@boost//:beast",
    ],
    visibility = [
        "//app:__pkg__",
        "//test:__pkg__",
    ],
)

cc_binary(
    name = "server",
    srcs = [
        "main.cpp",
        "log.hpp",
        "cli.hpp",
        "order.hpp",
        "order_book.hpp",
        "server.hpp",
    ],
    deps = [
        "lib-boost",
        "//third_party/cli11",
        "//third_party/json",
    ],
)
