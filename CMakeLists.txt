cmake_minimum_required(VERSION 3.3)
project(slatewm)

set(CMAKE_BINARY_DIR bin)
set(EXECUTABLE_OUTPUT_PATH bin)

# Set g++ flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lpthread")

#INCLUDE_DIRECTORIES ( ${CZMQ_INCLUDE_DIR} )
#LINK_DIRECTORIES ( ${CZMQ_LIBRARY} )

#INCLUDE_DIRECTORIES ( ${ZMQ_INCLUDE_DIRECTORY} )
#LINK_DIRECTORIES ( ${ZMQ_LIBRARY} )


find_library(ZMQ_LIB libzmq PATHS C:/bin/zmq/libzmq/bin/x64/Release/v140/static)
find_path (ZMQ_INCLUDE_DIR zmq.hpp PATHS C:/bin/zmq/libzmq/include)
include_directories (${ZMQ_INCLUDE_DIR})

# Source files
set(SOURCE_FILES src/main.cpp src/slate.cpp src/slate.h src/client_handler.h src/client_handler.cpp lib/json.h src/util/message.cpp src/util/message.h src/layout/tile.cpp src/layout/tile.h src/layout/workspace.cpp src/layout/workspace.h src/layout/client.cpp src/layout/client.h src/util/util.cpp src/util/util.h)

# Create executable
add_executable(slatewm ${SOURCE_FILES})

# Link extra libs
target_link_libraries(slatewm ${ZMQ_LIB})
