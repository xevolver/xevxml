#####################################################################
# Document generation

FIND_PACKAGE (Doxygen)
IF (DOXYGEN_FOUND)

SET(DOXYGEN_INPUT docs/doxygen/Doxygen.conf)
SET(DOXYGEN_OUTPUT docs/doxygen/)

ADD_CUSTOM_COMMAND(
  OUTPUT ${DOXYGEN_OUTPUT}
#  COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation..."
  COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT}
#  COMMAND ${CMAKE_COMMAND} -E echo "Done."
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS ${DOXYGEN_INPUT}
  )

ADD_CUSTOM_TARGET(apidoc ALL DEPENDS ${DOXYGEN_OUTPUT})

ADD_CUSTOM_TARGET(apidoc_forced
#  COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation..."
  COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT}
#  COMMAND ${CMAKE_COMMAND} -E echo "Done."
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

INSTALL_FILES( /share/${PROJECT_NAME}/html/ FILES ${PROJECT_SOURCE_DIR}/docs/doxygen/html/)
INSTALL_FILES( /share/${PROJECT_NAME}/tutorial/ FILES ${PROJECT_SOURCE_DIR}/docs/tex/)
ENDIF (DOXYGEN_FOUND)
