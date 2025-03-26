cmake_minimum_required(VERSION 3.27)
project(task-cli)

set(CMAKE_CXX_STANDARD 23)
add_executable(task-cli program.cpp status.hpp status.cpp table.hpp table.cpp task.hpp task.cpp time.hpp time.cpp)