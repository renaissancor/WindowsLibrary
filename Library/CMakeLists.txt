# Root\\Library CMakeLists.txt 

add_library(WindowsLibrary
    Sources/Profiler.cpp
    Sources/NewTracer.cpp
)

target_include_directories(WindowsLibrary PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
)
