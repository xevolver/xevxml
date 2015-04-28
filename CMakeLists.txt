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

install_files( /docs/ FILES ${PROJECT_SOURCE_DIR}/docs/doxygen/html)
endif (DOXYGEN_FOUND)

install_files( /lib/xev-trans/ FILES ${PROJECT_SOURCE_DIR}/xev-trans/lib/)
install_files( /include/ FILES ${PROJECT_SOURCE_DIR}/include/)
install_files( /docs/ FILES ${PROJECT_SOURCE_DIR}/README.md)

enable_testing()

file(GLOB files ${PROJECT_SOURCE_DIR}/test/identity/input/*.f90
  ${PROJECT_SOURCE_DIR}/test/unparse/input/*.c)
foreach(absname ${files})
  get_filename_component(filename ${absname} NAME)
  add_test(NAME "identity-${filename}"
    COMMAND sh ${PROJECT_SOURCE_DIR}/test/identity/input/test.sh ${filename}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test/identity/input/
    )
endforeach()

add_test(
  NAME translation-test
  COMMAND make -C ${PROJECT_SOURCE_DIR}/xev-trans/test check-all
  )
