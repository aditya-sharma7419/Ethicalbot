cmake_minimum_required(VERSION 3.21)
project(AvastCleaner LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core)

add_custom_target(cargo_build ALL
    COMMAND cargo build --release
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/core_engine
)

add_executable(AvastCleaner
    src/main.cpp
    src/mainwindow.cpp
    src/scanner_watcher.cpp
    src/platform_native.cpp
)

target_link_libraries(AvastCleaner PRIVATE
    Qt6::Widgets
    Qt6::Core
    ${CMAKE_SOURCE_DIR}/core_engine/target/release/${CMAKE_STATIC_LIBRARY_PREFIX}core_engine${CMAKE_STATIC_LIBRARY_SUFFIX}
)

if(WIN32)
    target_link_libraries(AvastCleaner PRIVATE Srsetp32.lib Shell32.lib)
endif()

# Automatically copy assets folder (containing style.qss) to output directory
add_custom_command(TARGET AvastCleaner POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:AvastCleaner>/assets
)