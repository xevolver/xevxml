cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated")
set(CMAKE_ROSE_DIR $ENV{ROSE_HOME})
find_package( Boost 1.40 COMPONENTS program_options REQUIRED )

include_directories(../../include ${CMAKE_ROSE_DIR}/include ${Boost_INCLUDE_DIR})
link_directories(${CMAKE_ROSE_DIR}/lib)
add_executable(defuse  defuse.cpp)
target_link_libraries(defuse xevxml  rose xalan-c xerces-c  ${Boost_LIBRARIES})
