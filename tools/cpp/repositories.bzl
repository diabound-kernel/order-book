load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def com_github_CLIUtils_CLI11_repository():
    tag = "2.1.2"
    http_archive(
        name = "com_github_CLIUtils_CLI11",
        build_file = Label("//third_party/cli11:bazel.BUILD"),
        sha256 = "26291377e892ba0e5b4972cdfd4a2ab3bf53af8dac1f4ea8fe0d1376b625c8cb",
        strip_prefix = "CLI11-{}".format(tag),
        urls = ["https://github.com/CLIUtils/CLI11/archive/v{}.tar.gz".format(tag)],
    )

def com_github_nlohmann_json_repository():
    ## JSON
    http_archive(
        name = "com_github_nlohmann_json",
        build_file = Label("//third_party/json:bazel.BUILD"),
        sha256 = "6bea5877b1541d353bd77bdfbdb2696333ae5ed8f9e8cc22df657192218cad91",
        urls = ["https://github.com/nlohmann/json/releases/download/v3.9.1/include.zip"],
    )

def cpp_repositories(excludes = []):

    if "com_github_CLIUtils_CLI11" not in excludes:
        com_github_CLIUtils_CLI11_repository()

    if "com_github_nlohmann_json" not in excludes:
        com_github_nlohmann_json_repository()