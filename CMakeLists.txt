cmake_minimum_required(VERSION 4.0)
project(Dice)

set(CMAKE_CXX_STANDARD 26)

add_executable(Dice main.cpp
        core/Dice.cpp
        core/Dice.h)
