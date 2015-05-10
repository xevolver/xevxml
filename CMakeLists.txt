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

file(GLOB sources ${PROJECT_SOURCE_DIR}/test/xsltgen/*.f90)
foreach(f90absname ${sources})
  get_filename_component(f90name ${f90absname} NAME)
  file(GLOB files ${PROJECT_SOURCE_DIR}/test/xsltgen/${f90name}.xev.d/*.json)
  foreach(absname ${files})
    get_filename_component(filename ${absname} NAME)
    add_test(NAME "xsltgen-${f90name}-${filename}"
      COMMAND sh ${PROJECT_SOURCE_DIR}/test/xsltgen/test.sh ${f90name} ${filename}
      WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test/xsltgen
      )
  endforeach()
endforeach()


add_test(NAME "test-find_directive-1"
      COMMAND make check
      WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/xev-trans/test/find/test-find_directive-1
      )


add_test(NAME "input-src"
    COMMAND make input-src
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/xev-trans/test
    )

file(GLOB files ${PROJECT_SOURCE_DIR}/xev-trans/test/*.f90.xev.d/test-*/*.xsl)
foreach(absname ${files})
  get_filename_component(filename ${absname} NAME)
  get_filename_component(dirname ${absname} DIRECTORY)
  add_test(NAME "trans-${filename}"
    COMMAND make check
    WORKING_DIRECTORY ${dirname}
    )
endforeach()

file(GLOB files ${PROJECT_SOURCE_DIR}/xev-trans/test/*.f90)
foreach(absname ${files})
  get_filename_component(filename ${absname} NAME)
  add_test(NAME "xev-trans-identity-${filename}"
    COMMAND sh test.sh ${absname}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test/identity/
    )
endforeach()

file(GLOB files ${PROJECT_SOURCE_DIR}/test/identity/input/*.f90
  ${PROJECT_SOURCE_DIR}/test/identity/input/*.c
  ${PROJECT_SOURCE_DIR}/test/identity/input/*.f
  ${PROJECT_SOURCE_DIR}/test/identity/input/*.f03)
foreach(absname ${files})
  get_filename_component(filename ${absname} NAME)
  add_test(NAME "identity-${filename}"
    COMMAND sh ${PROJECT_SOURCE_DIR}/test/identity/test.sh ${absname}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test/identity/
    )
endforeach()


#add_test(
#  NAME translation-test
#  COMMAND make -C ${PROJECT_SOURCE_DIR}/xev-trans/test check-all
#  )
