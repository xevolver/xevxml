CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated -g")
SET(CMAKE_XEVXML_PREFIX "${CMAKE_INSTALL_PREFIX}")

LIST(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
FIND_PACKAGE( Boost 1.40 COMPONENTS system filesystem iostreams REQUIRED )
FIND_PACKAGE( Picojson )
FIND_PACKAGE( Rose )
FIND_PACKAGE( Xerces )
FIND_PACKAGE( Xalan )

GET_FILENAME_COMPONENT(ROSE_LIBDIR ${CMAKE_ROSE_LIBRARY} DIRECTORY)

#####################################################################################
CONFIGURE_FILE(
  "${PROJECT_SOURCE_DIR}/src/config.h.in"
  "${PROJECT_SOURCE_DIR}/src/config.h"
  )

CONFIGURE_FILE(
  "${PROJECT_SOURCE_DIR}/test/identity/test.sh.in"
  "${PROJECT_SOURCE_DIR}/test/identity/test.sh"
  )

CONFIGURE_FILE(
  "${PROJECT_SOURCE_DIR}/test/xsltgen/test.sh.in"
  "${PROJECT_SOURCE_DIR}/test/xsltgen/test.sh"
  )

CONFIGURE_FILE(
  "${PROJECT_SOURCE_DIR}/xev-trans/lib/Makefile-xev.mk.in"
  "${PROJECT_SOURCE_DIR}/xev-trans/lib/Makefile-xev.mk"
  )

ADD_LIBRARY(xevxml STATIC
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

INCLUDE_DIRECTORIES(../include
  ${CMAKE_ROSE_INCLUDE}
  ${CMAKE_JSON_INCLUDE}
  ${CMAKE_XERCES_INCLUDE}
  ${CMAKE_XALAN_INCLUDE}
  ${Boost_INCLUDE_DIR}
  )

#LINK_DIRECTORIES(
#  ${CMAKE_ROSE_LIBRARY}
#  ${CMAKE_XERCES_LIBRARY}
#  ${CMAKE_XALAN_LIBRARY}
#  )

ADD_EXECUTABLE(src2xml  src2xml.cpp)
ADD_EXECUTABLE(xml2src  xml2src.cpp)
ADD_EXECUTABLE(xsltexec xslt.cpp)
ADD_EXECUTABLE(xsltgen xsltgen.cpp)
ADD_EXECUTABLE(xmlrebuild xmlrebuild.cpp src2xml.cpp)

TARGET_LINK_LIBRARIES(src2xml xevxml  ${CMAKE_ROSE_LIBRARY} ${CMAKE_XALAN_LIBRARY} ${CMAKE_XERCES_LIBRARY} ${Boost_LIBRARIES})
TARGET_LINK_LIBRARIES(xml2src xevxml  ${CMAKE_ROSE_LIBRARY} ${CMAKE_XALAN_LIBRARY} ${CMAKE_XERCES_LIBRARY} ${Boost_LIBRARIES})
TARGET_LINK_LIBRARIES(xsltexec xevxml  ${CMAKE_ROSE_LIBRARY} ${CMAKE_XALAN_LIBRARY} ${CMAKE_XERCES_LIBRARY} ${Boost_LIBRARIES})
TARGET_LINK_LIBRARIES(xmlrebuild xevxml  ${CMAKE_ROSE_LIBRARY} ${CMAKE_XALAN_LIBRARY} ${CMAKE_XERCES_LIBRARY} ${Boost_LIBRARIES})
SET_TARGET_PROPERTIES(xmlrebuild PROPERTIES COMPILE_DEFINITIONS XEV_COMPILE4XMLREBUILD)

INSTALL_PROGRAMS( /bin FILES src2xml xml2src xsltexec xsltgen xmlrebuild)
INSTALL_FILES( /lib FILES libxevxml.a)
