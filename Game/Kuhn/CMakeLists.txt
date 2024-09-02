# Add the Kuhn library as a static library
add_library(Kuhn STATIC Game.cpp)

# Include directories for Kuhn Poker library
target_include_directories(Kuhn
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)