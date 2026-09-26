# Ethicalbot

Ethicalbot is a desktop application for safe cleanup and system monitoring, built with Qt and a Rust-based core engine.

## Project structure

- SmartCleaner/
  - assets/style.qss
  - CMakeLists.txt
  - core_engine/Cargo.toml
  - core_engine/src/lib.rs
  - src/main.cpp
  - src/rust_core.h
  - src/platform_native.h
  - src/platform_native.cpp
  - src/scanner_watcher.h
  - src/scanner_watcher.cpp
  - src/mainwindow.h
  - src/mainwindow.cpp

## Build

1. Install Qt 6 development packages.
2. Configure the project with CMake:

   cmake -S . -B build

3. Build the application:

   cmake --build build

4. Run the desktop app:

   ./build/SmartCleaner

## Notes

This project keeps the safe-cleanup prototype direction while preparing a native Qt implementation with a Rust core for scanning and cleanup logic.
