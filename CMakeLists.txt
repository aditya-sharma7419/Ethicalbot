cmake_minimum_required(VERSION 3.21)

project(Ethicalbot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# The real project lives under SmartCleaner/. This wrapper prevents VS Code
# from configuring the stale legacy files at the workspace root.
add_subdirectory(SmartCleaner)
