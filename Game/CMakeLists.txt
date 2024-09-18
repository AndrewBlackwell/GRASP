find_package(Boost REQUIRED COMPONENTS serialization filesystem)

add_executable(game main.cpp)

target_link_libraries(game Kuhn Trainer Agent)
target_include_directories(game PRIVATE ../cmdline)

add_subdirectory(Kuhn)