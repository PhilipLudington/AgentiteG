#!/usr/bin/env python
"""
AgentiteG Build Script

Build commands:
    scons platform=linux -j4
    scons platform=windows -j4
    scons platform=macos -j4

    # Debug build
    scons platform=linux target=template_debug -j4
"""

import os
import sys

# Try to find godot-cpp
godot_cpp_path = "godot-cpp"
if not os.path.exists(godot_cpp_path):
    print("Error: godot-cpp not found. Run: git submodule update --init --recursive")
    sys.exit(1)

env = SConscript("godot-cpp/SConstruct")

# Add our source paths
env.Append(CPPPATH=["src/"])

# Enable C++17 for std::optional, etc.
if env["platform"] == "windows":
    env.Append(CXXFLAGS=["/std:c++17"])
else:
    env.Append(CXXFLAGS=["-std=c++17"])

# Gather all source files
sources = []
for root, dirs, files in os.walk("src"):
    for file in files:
        if file.endswith(".cpp"):
            sources.append(os.path.join(root, file))

# Library name
lib_name = "libagentiteg{}{}".format(env["suffix"], env["SHLIBSUFFIX"])
lib_path = os.path.join("project", "addons", "agentiteg", "bin", lib_name)

library = env.SharedLibrary(
    lib_path,
    source=sources,
)

Default(library)

# Help text
Help("""
AgentiteG Build System

Targets:
    scons               Build the library
    scons -c            Clean build files

Options:
    platform=<p>        Target platform (linux, windows, macos)
    target=<t>          Build target (template_debug, template_release)
    arch=<a>            Architecture (x86_64, arm64)

Examples:
    scons platform=linux -j4
    scons platform=macos arch=arm64 -j4
""")
