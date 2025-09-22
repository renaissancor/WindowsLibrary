¦§¦¡Execute
¦§¦¡Library
¦¢  ¦¢  CMakeLists.txt
¦¢  ¦¢
¦¢  ¦§¦¡Include
¦¢  ¦¢      NewTracer.h
¦¢  ¦¢      Profiler.h
¦¢  ¦¢
¦¢  ¦¦¦¡Sources
¦¢          NewTracer.cpp
¦¢          Profiler.cpp
¦¢
¦§¦¡Sources
¦¢      CMakeLists.txt
¦¢      main.cpp
¦¢
¦¦¦¡Testing
        CMakeLists.txt
        TestProfiler.cpp 


# Project Import 

```cmake
add_subdirectory(path/to/Library)
target_link_libraries(MyServer PRIVATE WindowsLibrary)
```


RUNTIME_OUTPUT_DIRECTORY ¡æ exe

LIBRARY_OUTPUT_DIRECTORY ¡æ dll, shared lib

ARCHIVE_OUTPUT_DIRECTORY ¡æ static lib


# Re build 

```bash
# Ä³½Ã/ºôµå Æú´õ »èÁ¦
Remove-Item -Recurse -Force .vs
Remove-Item -Recurse -Force BuildX86
Remove-Item -Recurse -Force BuildX64

# CMake configure
cmake -S . -B BuildX86 -G Ninja -DCMAKE_BUILD_TYPE=Release

# ºôµå
cmake --build BuildX86 --config Release
```