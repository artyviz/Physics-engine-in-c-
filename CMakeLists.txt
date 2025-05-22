cmake_minimum_required(VERSION 3.10)
project(PhyEng)

set(CMAKE_CXX_STANDARD 17)
include_directories(include src)
file(GLOB SOURCES "src/*.cpp")
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
add_executable(phyeng ${SOURCES})
target_link_libraries(phyeng sfml-graphics sfml-window sfml-system)
