cmake_minimum_required(VERSION 3.15)
project(DataStructure)

set(CMAKE_CXX_STANDARD 17)

# Include headers from include/
include_directories(include)

# Add your source files
add_executable(DataStructure
    src/main.cpp
    src/check.cpp
    # add more cpp files here if needed
)
