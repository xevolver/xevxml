cmake_minimum_required(VERSION 2.8)
add_subdirectory(src)
add_subdirectory(test)

find_package(Doxygen)
if (DOXYGEN_FOUND)

SET(DOXYGEN_INPUT docs/doxygen/Doxygen.conf)
SET(DOXYGEN_OUTPUT docs/doxygen/)

ADD_CUSTOM_COMMAND(
  OUTPUT ${DOXYGEN_OUTPUT}
  COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation..."
  COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT}
  COMMAND ${CMAKE_COMMAND} -E echo "Done."
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS ${DOXYGEN_INPUT}
  )

ADD_CUSTOM_TARGET(apidoc ALL DEPENDS ${DOXYGEN_OUTPUT})

ADD_CUSTOM_TARGET(apidoc_forced
  COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation..."
  COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT}
  COMMAND ${CMAKE_COMMAND} -E echo "Done."
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

endif (DOXYGEN_FOUND)

install_files( /lib FILES ${PROJECT_SOURCE_DIR}/test/xslt/rule)
install_files( /include FILES ${PROJECT_SOURCE_DIR}/include/)
