cmake_minimum_required(VERSION 3.21)
project(SmartCleaner LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core)
find_program(CARGO_EXECUTABLE cargo REQUIRED)

set(RUST_CORE_LIB_PATH
    "${CMAKE_SOURCE_DIR}/core_engine/target/release/${CMAKE_STATIC_LIBRARY_PREFIX}core_engine${CMAKE_STATIC_LIBRARY_SUFFIX}"
)

add_custom_command(
    OUTPUT ${RUST_CORE_LIB_PATH}
    COMMAND ${CARGO_EXECUTABLE} build --release
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/core_engine
    DEPENDS
        ${CMAKE_SOURCE_DIR}/core_engine/Cargo.toml
        ${CMAKE_SOURCE_DIR}/core_engine/src/lib.rs
    COMMENT "Building Rust core_engine"
    VERBATIM
)

add_custom_target(rust_core ALL DEPENDS ${RUST_CORE_LIB_PATH})

add_executable(SmartCleaner
    src/main.cpp
    src/mainwindow.cpp
    src/scanner_watcher.cpp
    src/platform_native.cpp
)

add_dependencies(SmartCleaner rust_core)

target_include_directories(SmartCleaner PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(SmartCleaner PRIVATE
    Qt6::Widgets
    Qt6::Core
    ${RUST_CORE_LIB_PATH}
)

if(WIN32)
    target_link_libraries(SmartCleaner PRIVATE Srsetp32.lib Shell32.lib)
endif()

add_custom_command(TARGET SmartCleaner POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:SmartCleaner>/assets
)
