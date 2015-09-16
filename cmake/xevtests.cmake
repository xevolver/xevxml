#####################################################################
# Test setting

enable_testing()

set(CTEST_CUSTOM_PRE_TEST "make -C ${PROJECT_SOURCE_DIR}/test/identity")

file(GLOB sources ${PROJECT_SOURCE_DIR}/test/xsltgen/*.f90)
foreach(f90absname ${sources})
  get_filename_component(f90name ${f90absname} NAME)
  file(GLOB files ${PROJECT_SOURCE_DIR}/test/xsltgen/${f90name}.xev.d/*.json)
  foreach(absname ${files})
    get_filename_component(filename ${absname} NAME)
    add_test(NAME "xsltgen-${f90name}-${filename}"
      COMMAND env XEV_TEST_MODE="ctest" sh ${PROJECT_SOURCE_DIR}/test/xsltgen/test.sh ${f90name} ${filename}
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
    COMMAND env XEV_TEST_MODE="ctest" sh ${PROJECT_SOURCE_DIR}/test/identity/test.sh ${absname}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test/identity/input/module
    )
endforeach()

file(GLOB files ${PROJECT_SOURCE_DIR}/test/identity/input/*.f90
  ${PROJECT_SOURCE_DIR}/test/identity/input/*.c
  ${PROJECT_SOURCE_DIR}/test/identity/input/*.f
  ${PROJECT_SOURCE_DIR}/test/identity/input/*.f03)
foreach(absname ${files})
  get_filename_component(filename ${absname} NAME)
  add_test(NAME "identity-${filename}"
    COMMAND env XEV_TEST_MODE="ctest" sh ${PROJECT_SOURCE_DIR}/test/identity/test.sh ${absname}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test/identity/input/module
    )
endforeach()


#add_test(
#  NAME translation-test
#  COMMAND make -C ${PROJECT_SOURCE_DIR}/xev-trans/test check-all
#  )
