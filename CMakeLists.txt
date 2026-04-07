cmake_minimum_required(VERSION 3.14)

project(mirror LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network NetworkAuth)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(DCMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE WIDGETS
  src/widgets/*.cpp
)

add_executable(${PROJECT_NAME}
  src/main.cpp
  src/main_window.cpp
  ${WIDGETS}
)

# Link Qt6 libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
  Qt6::Core
  Qt6::Gui
  Qt6::Widgets
  Qt6::Network
  Qt6::NetworkAuth
)
