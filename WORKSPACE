load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

local_repository(
    name = "gcpp-project",
    path = ".",
)


# CLANG-TIDY-INTEGRATION (experimental) - https://github.com/erenon/bazel_clang_tidy

#http_archive(
#    name = "bazel_clang_tidy",
#    sha256 = "2c2d7e290d78c0020c60f07cec6e3820cce64c285153de8e96637027ee5e3b8e",
#    strip_prefix = "bazel_clang_tidy-31d62bf825a94468b3d35c5ffd4e014e1c0ff566",
#    url = "https://github.com/erenon/bazel_clang_tidy/archive/31d62bf825a94468b3d35c5ffd4e014e1c0ff566.tar.gz",
#)
# BUILD WITH clang-tidy: bazel build //... --config clang-tidy

#local_repository(
#    name = "local_bazel_clang_tidy",
#    path = "libs/erenon_bazel_clang_tidy",
#)


#####################################################
# tooling for vscode (generate compile_commands.json)
#####################################################

# Hedron's Compile Commands Extractor for Bazel
# https://github.com/hedronvision/bazel-compile-commands-extractor
http_archive(
    name = "hedron_compile_commands",
    sha256 = "7a42f4a4a75a22476eb5eb0bc8fa0294034f238f007f11b7befb8f05ced1f552",
    # Replace the commit hash in both places (below) with the latest, rather than using the stale one here.
    # Even better, set up Renovate and let it do the work for you (see "Suggestion: Updates" in the README).
    url = "https://github.com/hedronvision/bazel-compile-commands-extractor/archive/a69b071b78120a6cdadc9f6a9968ed36c2206556.tar.gz",
    strip_prefix = "bazel-compile-commands-extractor-a69b071b78120a6cdadc9f6a9968ed36c2206556",
    # When you first run this tool, it'll recommend a sha256 hash to put here with a message like: "DEBUG: Rule 'hedron_compile_commands' indicated that a canonical reproducible form can be obtained by modifying arguments sha256 = ..."
)
load("@hedron_compile_commands//:workspace_setup.bzl", "hedron_compile_commands_setup")
hedron_compile_commands_setup()
# RUN: bazel run @hedron_compile_commands//:refresh_all