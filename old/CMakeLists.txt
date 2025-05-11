cmake_minimum_required(VERSION 3.10)
project(BigNumbersBoostAlgo)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED)

add_subdirectory(boosted_algorithms)

add_executable(main boosted_algorithms/main.cpp)
target_link_libraries(main PRIVATE boosted_algorithms)