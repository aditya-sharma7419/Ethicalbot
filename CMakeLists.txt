cmake_minimum_required(VERSION 3.16)

project(AvastSmartCleaner LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Widgets QUIET)
if(Qt6_FOUND)
  set(QT_WIDGETS_LIB Qt6::Widgets)
else()
  find_package(Qt5 REQUIRED COMPONENTS Widgets)
  set(QT_WIDGETS_LIB Qt5::Widgets)
endif()

add_executable(AvastSmartCleaner
  src/main.cpp
  src/MainWindow.cpp
  src/MainWindow.h
  src/SafeCleanerEngine.cpp
  src/SafeCleanerEngine.h
)

target_link_libraries(AvastSmartCleaner PRIVATE ${QT_WIDGETS_LIB})

target_include_directories(AvastSmartCleaner PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
