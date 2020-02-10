# Mafia Formats Project 
# Author: Roman Dobias

# Usage:
# Use this repository as a git-submodule or hard-copy the directory to your project.
# Then, use add_subdirectory with path to the project and use exported targets in
# your projects.
# All file format parsers are aggregated in a single target 'MFormats::Formats'.

cmake_minimum_required(VERSION 3.10)
project(MFormats)

add_subdirectory(src)
