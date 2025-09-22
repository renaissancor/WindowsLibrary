# Root CMakeLists.txt 

cmake_minimum_required(VERSION 3.10)
project(WindowsLibrary)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(Library)
add_subdirectory(Sources)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Execute/${CMAKE_VS_PLATFORM_NAME})
