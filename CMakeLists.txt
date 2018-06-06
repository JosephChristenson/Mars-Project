cmake_minimum_required(VERSION 3.10)
project(mars_project)

set(CMAKE_CXX_STANDARD 11)

add_executable(mars_project cpp/mars-project.cpp cpp/EGM.cpp cpp/EGM.h cpp/PGM.cpp cpp/PGM.h cpp/COLOR.cpp cpp/COLOR.h)