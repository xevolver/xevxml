#####################################################################
# picojson

# CMAKE_JSON_NOTFOUND
# CMAKE_JSON_INCLUDE

FIND_PATH(CMAKE_JSON_INCLUDE NAMES picojson.h
  PATHS
  $ENV{CMAKE_JSON_INCLUDE_PATH}
  $ENV{CMAKE_INCLUDE_PATH}
  ${CMAKE_INCLUDE_PATH}
  /usr/local/include
  /usr/include
  )

IF(CMAKE_JSON_INCLUDE)
  SET(CMAKE_JSON_FOUND TRUE)
ELSE(CMAKE_JSON_INCLUDE)
  SET(CMAKE_JSON_FOUND FALSE)
ENDIF(CMAKE_JSON_INCLUDE)

IF(CMAKE_JSON_FOUND)
  MESSAGE(STATUS "Found \"picojson.h\" at ${CMAKE_JSON_INCLUDE}")
ELSE(CMAKE_JSON_FOUND)
  MESSAGE(WARNING "Couldn't find \"picojson.h\"")
ENDIF(CMAKE_JSON_FOUND)

MARK_AS_ADVANCED(CMAKE_JSON_INCLUDE)