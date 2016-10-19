cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated")
set(UTIL xevutil.c xevutil_xml.c xevutil_strobj.c xevutil_langstr.c xevparse_util.c)
LIST(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
add_executable(xevpreparse xevpreparse.c xevparse_pre_f.c ${UTIL})
add_executable(xevpostparse xevpostparse.c xevparse_postf.c xevparse_x.c xevparse_info.c ${UTIL})

add_executable(xevpreunparse xevpreunparse.c xevunparse_pre_f.c xevunparse_x.c ${UTIL})
add_executable(xevpostunparse xevpostunparse.c xevunparse_postf.c xevunparse_x.c ${UTIL})

add_executable(xevparse xevparse.c ${UTIL})
add_executable(xevunparse xevunparse.c ${UTIL})

add_executable(xevrestruct
xevrestruct.c
xev_constr_block.c
xev_destr_block.c
xevparse_contplus.c
xevparse_contminus.c
xev_constr_line.c
xev_destr_line.c
xev_concat_cont.c ${UTIL})

add_executable(xml_indent xml_indent.c ${UTIL})
add_executable(xevutil_continu xevutil_continu.c ${UTIL})

add_executable(xevtgen xevtgen.c xevtgen_write_template.c ${UTIL})

set_target_properties(xevpreparse      PROPERTIES LINKER_LANGUAGE C)
set_target_properties(xevpostparse     PROPERTIES LINKER_LANGUAGE C)
set_target_properties(xevpreunparse    PROPERTIES LINKER_LANGUAGE C)
set_target_properties(xevpostunparse   PROPERTIES LINKER_LANGUAGE C)
set_target_properties(xevparse         PROPERTIES LINKER_LANGUAGE C)
set_target_properties(xevunparse       PROPERTIES LINKER_LANGUAGE C)
set_target_properties(xevrestruct      PROPERTIES LINKER_LANGUAGE C)
set_target_properties(xml_indent       PROPERTIES LINKER_LANGUAGE C)
set_target_properties(xevutil_continu  PROPERTIES LINKER_LANGUAGE C)
set_target_properties(xevtgen          PROPERTIES LINKER_LANGUAGE C)