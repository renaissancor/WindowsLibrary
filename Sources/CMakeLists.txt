# Root\\Sources CMakeLists.txt 

add_executable(MainApp 
	main.cpp
	TestProfiler.cpp 
)

target_link_libraries(MainApp PRIVATE WindowsLibrary)

target_include_directories(MainApp PRIVATE 
    ${CMAKE_SOURCE_DIR}/Library/Include
    ${CMAKE_SOURCE_DIR}/Sources
)