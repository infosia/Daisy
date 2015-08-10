#
# Copyright (c) 2015 by Kota Iguchi. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

if (${PLATFORM} STREQUAL "android")
  set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_LIST_DIR}/cmake/toolchain.android.cmake")
endif()

cmake_minimum_required(VERSION 3.0.0)

project(Daisy)
set(Daisy_VERSION 0.1.0)

execute_process(COMMAND git symbolic-ref -q HEAD
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                OUTPUT_VARIABLE JERRY_GIT_BRANCH
                OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND git rev-parse HEAD
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                OUTPUT_VARIABLE JERRY_GIT_COMMIT
                OUTPUT_STRIP_TRAILING_WHITESPACE)
# Get build date
execute_process(COMMAND date +%d/%m/%Y
                OUTPUT_VARIABLE JERRY_BUILD_DATE
                OUTPUT_STRIP_TRAILING_WHITESPACE)

set(DEFINES_JERRY
    JERRY_BUILD_DATE="${JERRY_BUILD_DATE}"
    JERRY_COMMIT_HASH="${JERRY_GIT_COMMIT}"
    JERRY_BRANCH_NAME="${JERRY_GIT_BRANCH}")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

option(Daisy_ENABLE_TESTS "Enable tests" ON)

# Build shared library by default
set(LIBRARY_BUILD_TYPE SHARED)

set(REQUIRE_XCODE (${PLATFORM} STREQUAL "macosx" OR ${PLATFORM} STREQUAL "iphoneos" OR ${PLATFORM} STREQUAL "iphonesimulator"))
set(BUILD_FOR_IOS (${PLATFORM} STREQUAL "iphoneos" OR ${PLATFORM} STREQUAL "iphonesimulator"))

if (Daisy_ENABLE_TESTS)
# Google test helper functions and macros.
  include(${PROJECT_SOURCE_DIR}/cmake/gtest_utils.cmake)
  enable_testing()
endif()

if (${REQUIRE_XCODE})
  include(${PROJECT_SOURCE_DIR}/cmake/toolchain.xcode.cmake)

  include(CheckCXXCompilerFlag)
  CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
  if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
  endif()

  CHECK_CXX_COMPILER_FLAG("-stdlib=libc++" COMPILER_SUPPORTS_LIBCXX)
  if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
  else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
  endif()

  check_xcode()
  config_xcode(${PLATFORM})

  # Build static library for iOS
  if (${BUILD_FOR_IOS})
    set(LIBRARY_BUILD_TYPE STATIC)
  endif()
endif()

if (ANDROID)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

# Include directories
set(INCLUDE_CORE
    jerry-core
    jerry-core/lit
    jerry-core/rcs
    jerry-core/mem
    jerry-core/vm
    jerry-core/ecma/builtin-objects
    jerry-core/ecma/base
    jerry-core/ecma/operations
    jerry-core/parser/js
    jerry-core/parser/js/collections
    jerry-core/parser/regexp
    jerry-core/jrt)

file(GLOB SOURCE_CORE_API                   jerry-core/*.cpp jerry-core/*.h)
file(GLOB SOURCE_CORE_LIT                   jerry-core/lit/*.cpp jerry-core/lit/*.h)
file(GLOB SOURCE_CORE_RCS                   jerry-core/rcs/*.cpp jerry-core/rcs/*.h)
file(GLOB SOURCE_CORE_MEM                   jerry-core/mem/*.cpp jerry-core/mem/*.h)
file(GLOB SOURCE_CORE_VM                    jerry-core/vm/*.cpp jerry-core/vm/*.h)
file(GLOB SOURCE_CORE_ECMA_BUILTINS         jerry-core/ecma/builtin-objects/*.cpp jerry-core/ecma/builtin-objects/*.h)
file(GLOB SOURCE_CORE_ECMA_BASE             jerry-core/ecma/base/*.cpp jerry-core/ecma/base/*.h)
file(GLOB SOURCE_CORE_ECMA_OPERATIONS       jerry-core/ecma/operations/*.cpp jerry-core/ecma/operations/*.h)
file(GLOB SOURCE_CORE_PARSER_JS             jerry-core/parser/js/*.cpp jerry-core/parser/js/*.h)
file(GLOB SOURCE_CORE_PARSER_JS_COLLECTIONS jerry-core/parser/js/collections/*.cpp jerry-core/parser/js/collections/*.h)
file(GLOB SOURCE_CORE_PARSER_REGEXP         jerry-core/parser/regexp/*.cpp jerry-core/parser/regexp/*.h)
file(GLOB SOURCE_CORE_JRT                   jerry-core/jrt/*.cpp jerry-core/jrt/*.h)

set(SOURCE_CORE
    jerry-core/jerry.cpp
    ${SOURCE_CORE_API}
    ${SOURCE_CORE_LIT}
    ${SOURCE_CORE_RCS}
    ${SOURCE_CORE_MEM}
    ${SOURCE_CORE_VM}
    ${SOURCE_CORE_ECMA_BUILTINS}
    ${SOURCE_CORE_ECMA_BASE}
    ${SOURCE_CORE_ECMA_OPERATIONS}
    ${SOURCE_CORE_PARSER_JS}
    ${SOURCE_CORE_PARSER_JS_COLLECTIONS}
    ${SOURCE_CORE_PARSER_REGEXP}
    ${SOURCE_CORE_JRT})

source_group(JerryCore FILES ${SOURCE_CORE})

set(SOURCE_Daisy
  include/Daisy/daisy.hpp
  include/Daisy/detail/JSBase.hpp
  include/Daisy/detail/JSUtil.hpp
  src/detail/JSUtil.cpp
  include/Daisy/JSContextGroup.hpp
  src/JSContextGroup.cpp
  include/Daisy/JSContext.hpp
  src/JSContext.cpp
  include/Daisy/JSValue.hpp
  src/JSValue.cpp
  include/Daisy/JSString.hpp
  src/JSString.cpp
  include/Daisy/JSNumber.hpp
  src/JSNumber.cpp
  include/Daisy/JSBoolean.hpp
  src/JSBoolean.cpp
  include/Daisy/JSObject.hpp
  src/JSObject.cpp
  include/Daisy/JSClass.hpp
  src/JSClass.cpp
  include/Daisy/JSExportClass.hpp
  include/Daisy/JSExport.hpp
  )

add_library(Daisy ${LIBRARY_BUILD_TYPE}
  ${SOURCE_CORE}
  ${SOURCE_Daisy}
  )

target_include_directories(Daisy PUBLIC
  ${PROJECT_SOURCE_DIR}/include
  ${INCLUDE_CORE}
  )

include(GenerateExportHeader)
generate_export_header(Daisy)
target_compile_definitions(Daisy PRIVATE Daisy_EXPORTS ${DEFINES_JERRY})

set_property(TARGET Daisy PROPERTY VERSION ${Daisy_VERSION})
set_property(TARGET Daisy PROPERTY SOVERSION 0)
set_property(TARGET Daisy PROPERTY INTERFACE_Daisy_MAJOR_VERSION 0)
set_property(TARGET Daisy APPEND PROPERTY
  COMPATIBLE_INTERFACE_STRING Daisy_MAJOR_VERSION
  )

if (Daisy_ENABLE_TESTS)
  add_subdirectory(tests)
endif()
