#####################################################################
# ROSE
FIND_PATH(CMAKE_ROSE_INCLUDE NAMES rose.h
  PATHS
  $ENV{CMAKE_ROSE_INCLUDE_PATH}
  $ENV{CMAKE_INCLUDE_PATH}
  ${CMAKE_INCLUDE_PATH}
  $ENV{CMAKE_ROSE_INCLUDE_PATH}/rose
  $ENV{CMAKE_INCLUDE_PATH}/rose
  ${CMAKE_INCLUDE_PATH}/rose
  /usr/local/include
  /usr/local/include/rose
  /usr/include
  /usr/include/rose
  )

FIND_LIBRARY(CMAKE_ROSE_LIBRARY NAMES rose
  PATHS
  $ENV{CMAKE_ROSE_LIBRARY_PATH}
  $ENV{CMAKE_LIBRARY_PATH}
  ${CMAKE_LIBRARY_PATH}
  /usr/local/lib
  /usr/lib
  )

IF(CMAKE_ROSE_INCLUDE AND CMAKE_ROSE_LIBRARY)
  SET(CMAKE_ROSE_FOUND TRUE)
ELSE(CMAKE_ROSE_INCLUDE AND CMAKE_ROSE_LIBRARY)
  SET(CMAKE_ROSE_FOUND FALSE)
ENDIF(CMAKE_ROSE_INCLUDE AND CMAKE_ROSE_LIBRARY)

IF(CMAKE_ROSE_FOUND)
  MESSAGE(STATUS "Found \"rose.h\" at ${CMAKE_ROSE_INCLUDE}")
  MESSAGE(STATUS "Found \"librose.so\" at ${CMAKE_ROSE_LIBRARY}")
ELSE(CMAKE_ROSE_FOUND)
  MESSAGE(FATAL_ERROR "Couldn't find \"rose.h\" and/or \"librose.so\"")
ENDIF(CMAKE_ROSE_FOUND)

MARK_AS_ADVANCED(CMAKE_ROSE_INCLUDE CMAKE_ROSE_LIBRARY)
