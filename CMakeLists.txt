cmake_minimum_required(VERSION 3.5.1)

project(TTS)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
find_package(Sanitizers) # Address sanitizer.

# threads
find_package(Threads)

# Add custom build type DebugOpt
message("* Adding build types...")
IF (MSVC)
SET(CMAKE_CXX_FLAGS_DEBUGOPT
    "-DDEBUG /DEBUG /O2"
    CACHE STRING "Flags used by the C++ compiler during coverage builds."
    FORCE )
SET(CMAKE_C_FLAGS_DEBUGOPT
    "-DDEBUG /DEBUG /O2"
    CACHE STRING "Flags used by the C compiler during coverage builds."
    FORCE )
ELSE () # Assume gcc
SET(CMAKE_CXX_FLAGS_DEBUGOPT
    "-g -O2 -fno-omit-frame-pointer"
    CACHE STRING "Flags used by the C++ compiler during coverage builds."
    FORCE )
SET(CMAKE_C_FLAGS_DEBUGOPT
    "-g -O2 -fno-omit-frame-pointer"
    CACHE STRING "Flags used by the C compiler during coverage builds."
    FORCE )
ENDIF()

SET(CMAKE_EXE_LINKER_FLAGS_DEBUGOPT
    ""
    CACHE STRING "Flags used for linking binaries during coverage builds."
    FORCE )
SET(CMAKE_SHARED_LINKER_FLAGS_DEBUGOPT
    ""
    CACHE STRING "Flags used by the shared libraries linker during coverage builds."
    FORCE )
MARK_AS_ADVANCED(
    CMAKE_CXX_FLAGS_DEBUGOPT
    CMAKE_C_FLAGS_DEBUGOPT
    CMAKE_EXE_LINKER_FLAGS_DEBUGOPT
    CMAKE_SHARED_LINKER_FLAGS_DEBUGOPT )

IF(NOT CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE Release
       CACHE STRING "Choose the type of build : None Debug Release RelWithDebInfo MinSizeRel DebugOpt."
       FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)
message("* Current build type is : ${CMAKE_BUILD_TYPE}")

# C++11
set (CMAKE_CXX_STANDARD 11)

# PIC
set (CMAKE_POSITION_INDEPENDENT_CODE ON)

include_directories(
    ${CMAKE_SOURCE_DIR}/src
)

set (CORE_SOURCE
    ${CMAKE_SOURCE_DIR}/src/main.cc
    ${CMAKE_SOURCE_DIR}/src/tf_synthesizer.cc
    ${CMAKE_SOURCE_DIR}/src/audio_util.cc
    )

link_directories(
    ${TENSORFLOW_BUILD_DIR}
    )

add_executable( tts
    ${CORE_SOURCE}
    )

target_include_directories(tts
    # TensorFlow
    PUBLIC ${TENSORFLOW_DIR}

    # for array_ops.h
    PUBLIC ${TENSORFLOW_DIR}/bazel-genfiles

    # headers for external packages
    PUBLIC ${TENSORFLOW_EXTERNAL_DIR}/external/protobuf_archive/src
    PUBLIC ${TENSORFLOW_EXTERNAL_DIR}/external/eigen_archive
    PUBLIC ${TENSORFLOW_EXTERNAL_DIR}/external/nsync/public
    PUBLIC ${TENSORFLOW_EXTERNAL_DIR}/external/com_google_absl

    # this project
    PUBLIC ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries( tts
    # TensorFlow C++
    tensorflow_cc

    ${TTS_EXT_LIBS}
    ${CMAKE_THREAD_LIBS_INIT}
    ${CMAKE_DL_LIBS}
    )


# Increase warning level for clang.
IF (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(tts PRIVATE -Weverything -Werror -Wno-padded -Wno-c++98-compat-pedantic -Wno-documentation -Wno-documentation-unknown-command)
ENDIF ()

add_sanitizers(tts)

# [VisualStudio]
if (WIN32)
  # Set `tts` as a startup project for VS IDE
  if (CMAKE_VERSION VERSION_GREATER 3.6.0)
    set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT tts)
  endif ()

  # For easier debugging in VS IDE(cmake 3.8.0 or later required)
  if (CMAKE_VERSION VERSION_GREATER 3.8.0)
    # Set working directory to $lucia git repo root.
    set_target_properties(tts PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
  endif ()
endif ()
