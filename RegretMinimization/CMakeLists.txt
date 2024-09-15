add_executable(run main.cpp)

target_include_directories(run PRIVATE ../cmdline)
target_link_libraries(run Trainer Kuhn)

add_subdirectory(Agent)
add_subdirectory(Trainer)