#
# Copyright (c) 2015 by Kota Iguchi. All Rights Reserved.
# 
# Licensed under the Apache License, Version 2.0 (the "License"); you
# may not use this software or any of it's contents except in
# compliance with the License. The full text of the license is in the
# file LICENSE.txt in the top-level directory of this project, or you
# may obtain a copy of the License at
# 
# http://www.apache.org/licenses/LICENSE-2.0
#

#
# Check if Xcode is installed
#
function(check_xcode)
  find_program(XCODEBUILD xcodebuild)
  if ("${XCODEBUILD}" MATCHES "-NOTFOUND$")
    message(FATAL_ERROR "The xcodebuild program could not be found.")
  endif()

  find_program(XCODE-SELECT xcode-select)
  if ("${XCODE-SELECT}" MATCHES "-NOTFOUND$")
    message(FATAL_ERROR "The xcode-select program could not be found.")
  endif()

  find_program(XCRUN xcrun)
  if ("${XCRUN}" MATCHES "-NOTFOUND$")
    message(FATAL_ERROR "The xcrun program could not be found.")
  endif()
endfunction()

#
# Setup xcode environment
#
# sdk should be one of "macosx", "iphoneos" or "iphonesimulator"
#
function(config_xcode arg)
  string(TOLOWER "${arg}" sdk)

  execute_process(COMMAND ${XCODEBUILD} -sdk ${sdk} -version Path
    OUTPUT_VARIABLE XCODE_SDKROOT
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  execute_process(COMMAND ${XCODE-SELECT} -print-path
    OUTPUT_VARIABLE XCODE_DEVELOPER_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  set(XCODE_FRAMEWORK_DIRS "${XCODE_SDKROOT}/System/Library/Frameworks" "${XCODE_DEVELOPER_DIR}/Library/Frameworks")
  list(REMOVE_DUPLICATES XCODE_FRAMEWORK_DIRS)

  list(INSERT CMAKE_SYSTEM_FRAMEWORK_PATH 0 ${XCODE_FRAMEWORK_DIRS})
  list(REMOVE_DUPLICATES CMAKE_SYSTEM_FRAMEWORK_PATH)
  set(CMAKE_SYSTEM_FRAMEWORK_PATH ${CMAKE_SYSTEM_FRAMEWORK_PATH} PARENT_SCOPE)

  if ("${sdk}" MATCHES "iphoneos" OR "${sdk}" MATCHES "iphonesimulator")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -arch ${ARCH} -isysroot ${XCODE_SDKROOT} -I${XCODE_SDKROOT}/usr/include" PARENT_SCOPE)
  endif()

endfunction()

