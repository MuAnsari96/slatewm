cmake_minimum_required(VERSION 3.3)
project(slatewm)

set(CMAKE_BINARY_DIR bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# Set g++ flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Link Xlib
find_package(X11 REQUIRED)
include_directories (${X11_INCLUDE_DIR})
link_libraries(${X11_LIBRARIES})

# Source files
set(SOURCE_FILES src/main.cpp src/slate.cpp src/slate.h src/event_handler.h)

# Create executable
add_executable(slatewm ${SOURCE_FILES})