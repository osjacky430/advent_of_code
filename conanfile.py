"""
Config file for Conan 2.0
"""

from conan import ConanFile


class PacMan(ConanFile):
    requires = "range-v3/0.12.0", "fmt/10.0.0", "ctre/3.7.1"
    tool_requires = "cmake/3.25.0"
    generators = "CMakeDeps", "CMakeToolchain"
    settings = "build_type"
