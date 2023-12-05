"""
Config file for Conan 2.0
"""

from conan import ConanFile


class PacMan(ConanFile):
    requires = "range-v3/0.12.0", "fmt/10.1.1", "ctre/3.8.1", "ms-gsl/4.0.0"
    generators = "CMakeDeps", "CMakeToolchain"
    settings = "build_type"
