cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(santa_workshop_2019 CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -pedantic)

add_library(optimize_lib STATIC
    src/Chromosome.cc
    src/Loss.cc
    src/Pool.cc)
target_include_directories(optimize_lib PUBLIC include)

add_executable(optimize src/optimize.cc)
target_link_libraries(optimize PRIVATE optimize_lib)

add_executable(annealing src/annealing.cc)
target_link_libraries(annealing PRIVATE optimize_lib)
