cmake_minimum_required(VERSION 3.17)
project(curses_firework C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_BUILD_TYPE Release)

add_compile_options("-Wall")
add_compile_options("-Wextra")
add_compile_options("-pedantic")
add_compile_options("-Werror")
add_compile_options("-O3")

add_executable(curses_firework main.c)
target_link_libraries(curses_firework ncurses)
