#####################################################################
# XALAN
FIND_PATH(CMAKE_XALAN_INCLUDE NAMES xalanc/Include/XalanVersion.hpp
  PATHS
  $ENV{CMAKE_XALAN_INCLUDE_PATH}
  ${CMAKE_XALAN_INCLUDE_PATH}
  ${CMAKE_INCLUDE_PATH}
  /usr/local/include
  /usr/include
  )
FIND_LIBRARY(CMAKE_XALAN_LIBRARY NAMES xalan-c
  PATHS
  $ENV{CMAKE_XALAN_LIBRARY_PATH}
  ${CMAKE_XALAN_LIBRARY_PATH}
  ${CMAKE_LIBRARY_PATH}
  /usr/lib
  /usr/local/lib
  )

IF(CMAKE_XALAN_INCLUDE AND CMAKE_XALAN_LIBRARY)
  SET(CMAKE_XALAN_FOUND TRUE)
ELSE(CMAKE_XALAN_INCLUDE AND CMAKE_XALAN_LIBRARY)
  SET(CMAKE_XALAN_FOUND FALSE)
ENDIF(CMAKE_XALAN_INCLUDE AND CMAKE_XALAN_LIBRARY)

IF(CMAKE_XALAN_FOUND)
  MESSAGE(STATUS "Found Xalan-C")
ELSE(CMAKE_XALAN_FOUND)
  MESSAGE(FATAL_ERROR "Couldn't find Xalan-C")
ENDIF(CMAKE_XALAN_FOUND)

MARK_AS_ADVANCED(CMAKE_XALAN_INCLUDE CMAKE_XALAN_LIBRARY)