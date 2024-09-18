cmake_minimum_required(VERSION 3.5)
project(GRASP)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

cmake_policy(SET CMP0167 NEW)
add_subdirectory(Game)
add_subdirectory(GRASP)
