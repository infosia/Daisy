# Defines the gtest & gtest_main libraries.  User tests should link
# with one of them.
function(cxx_library_with_type name type cxx_flags)
  # type can be either STATIC or SHARED to denote a static or shared
  # library.  ARGN refers to additional arguments after 'cxx_flags'.
  add_library(${name} ${type} ${ARGN})
  if (cxx_flags)
    set_target_properties(${name}
      PROPERTIES
      COMPILE_FLAGS "${cxx_flags}")
  endif()
endfunction()

########################################################################
#
# Helper functions for creating build targets.

function(cxx_shared_library name cxx_flags)
  cxx_library_with_type(${name} SHARED "${cxx_flags}" ${ARGN})
endfunction()

function(cxx_library name cxx_flags)
  cxx_library_with_type(${name} "" "${cxx_flags}" ${ARGN})
endfunction()

# cxx_executable_with_flags(name cxx_flags libs srcs...)
#
# Creates a named C++ executable that depends on the given libraries
# and is built from the given source files with the given compiler
# flags.
function(cxx_executable_with_flags name cxx_flags libs)
  add_executable(${name} ${ARGN})
  if (cxx_flags)
    set_target_properties(${name}
      PROPERTIES
      COMPILE_FLAGS "${cxx_flags}")
  endif()
  
  target_link_libraries(${name} ${libs})
  #   # To support mixing linking in static and dynamic libraries, link each
  #   # library in with an extra call to target_link_libraries.
  #   foreach (lib "${libs}")
  #     target_link_libraries(${name} ${lib})
  #   endforeach()
endfunction()

# cxx_executable(name dir lib srcs...)
#
# Creates a named target that depends on the given libs and is built
# from the given source files.  dir/name.cpp is implicitly included in
# the source file list.
function(cxx_executable name dir libs)
  cxx_executable_with_flags(
    ${name} "${CMAKE_CXX_FLAGS}" "${libs}" "${dir}/${name}.cpp" ${ARGN})
endfunction()

find_package(GTest REQUIRED)

# cxx_test_with_flags(name cxx_flags libs srcs...)
#
# Creates a named C++ test that depends on the given libs and is built
# from the given source files with the given compiler flags.
function(cxx_test_with_flags name cxx_flags libs)
  list(APPEND libs ${GTEST_BOTH_LIBRARIES})
  cxx_executable_with_flags(${name} "${cxx_flags}" "${libs}" ${ARGN})
  set_property(TARGET ${name}
    APPEND PROPERTY INCLUDE_DIRECTORIES "${GTEST_INCLUDE_DIRS}")
  add_test(${name} ${name})
endfunction()

# cxx_test(name libs srcs...)
#
# Creates a named test target that depends on the given libs and is
# built from the given source files.  Unlike cxx_test_with_flags,
# name.cpp is already implicitly included in the source file list.
function(cxx_test name dir libs)
  cxx_test_with_flags("${name}" "${CMAKE_CXX_FLAGS}" "${libs}"
    "${dir}/${name}.cpp" ${ARGN})
endfunction()

