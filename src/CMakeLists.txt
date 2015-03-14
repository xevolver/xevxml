cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated")
set(CMAKE_ROSE_DIR $ENV{ROSE_HOME})
find_package( Boost 1.40 COMPONENTS program_options REQUIRED )

add_library(xevxml STATIC
  xmlutils.cpp
  type.cpp
  rose2xml.cpp
  xml2rose.cpp
  preproc.cpp
  unparse.cpp
  expr.cpp
  stmt.cpp
  decl.cpp
  supp.cpp
  value.cpp
)
include_directories(../include ${CMAKE_ROSE_DIR}/include ${Boost_INCLUDE_DIR})
link_directories(${CMAKE_ROSE_DIR}/lib)
add_executable(src2xml  src2xml.cpp)
add_executable(xml2src  xml2src.cpp)
add_executable(xsltexec xslt.cpp)

target_link_libraries(src2xml xevxml  rose xalan-c xerces-c  ${Boost_LIBRARIES})
target_link_libraries(xml2src xevxml  rose xalan-c xerces-c  ${Boost_LIBRARIES})
target_link_libraries(xsltexec xevxml rose xalan-c xerces-c  ${Boost_LIBRARIES})
