cmake_minimum_required(VERSION 3.15)
set(PROJECT_NAME checking_fat)
project(${PROJECT_NAME} C)

set(CMAKE_C_STANDARD 99)

add_executable(checking_fat src/main.c include/structs.h include/fats.h src/fats.c src/boot_sector.c include/boot_sector.h src/root_dir.c include/root_dir.h src/dir_entry.c include/dir_entry.h)

target_include_directories(${PROJECT_NAME} PRIVATE options_parser include)
