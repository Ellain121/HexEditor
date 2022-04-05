cmake_minimum_required(VERSION 3.0.2)
project(hex_editor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_LINK_LIBS libncurses.so.6)
link_directories(/lib/x86_64-linux-gnu)

include_directories(include)
file(GLOB SOURCES "src/*.cpp")

add_executable(hex_editor ${SOURCES})
target_link_libraries(hex_editor ${PROJECT_LINK_LIBS})