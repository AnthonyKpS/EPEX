cmake_minimum_required(VERSION 3.17)
project(epex C)

set(CMAKE_C_STANDARD 11)

add_executable(epex main.c src/init/init.c src/io/io.h src/io/io.c)