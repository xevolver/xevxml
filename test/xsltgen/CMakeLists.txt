cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated")
set(CMAKE_PICOJSON_DIR $ENV{PICOJSON_HOME})

include_directories(${CMAKE_PICOJSON_DIR}/include)
add_executable(xsltgen  xsltgen.cpp)
