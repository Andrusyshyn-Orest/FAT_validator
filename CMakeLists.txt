cmake_minimum_required(VERSION 3.15)

set(PROJECT_NAME checking_fat)
project(${PROJECT_NAME} C CXX) # project(${PROJECT_NAME} C CXX ASM)

set(CMAKE_CXX_STANDARD 17)

add_executable(checking_fat src/main.cpp src/fats.cpp include/fats.hpp src/dir_entry.cpp include/dir_entry.hpp include/boot_sector.hpp include/structs.hpp include/fats_versions.hpp include/print_dir.hpp include/trim.hpp src/trim.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE include)
