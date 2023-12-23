cmake_minimum_required(VERSION 3.19.6)

project(Fern C CXX)

# Honour CMAKE_CXX_STANDARD in try_compile(), needed for check_cxx_native_regex
if(POLICY CMP0067)
  cmake_policy(SET CMP0067 NEW)
endif()

# Convert relative paths to absolute paths for subdirectory `target_sources`
if(POLICY CMP0076)
  cmake_policy(SET CMP0076 NEW)
endif()

# Don't clobber existing variables when evaluating `option()` declarations
if(POLICY CMP0077)
  cmake_policy(SET CMP0077 NEW)
endif()

# Clobber existing timestamps of extracted files when using `ExternalProject_Add()`
if(POLICY CMP0086)
  cmake_policy(SET CMP0086 NEW)
endif()

# Allow for forced colored output
option(FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." ON)
if(${FORCE_COLORED_OUTPUT})
  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options(-fdiagnostics-color=always)
  elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options(-fcolor-diagnostics)
  endif()
endif()

# Prefix directory
if(DEFINED ENV{PREFIX_DIR} AND NOT "$ENV{PREFIX_DIR}" STREQUAL "")
  set(FERN_PREFIX_DIR "$ENV{PREFIX_DIR}")
else()
  set(FERN_PREFIX_DIR "$ENV{HOME}/.fern")
endif()
set(CMAKE_PREFIX_PATH "${CMAKE_INSTALL_PREFIX};${PROJECT_BINARY_DIR}")
add_definitions(-DINSTALL_PREFIX=${CMAKE_INSTALL_PREFIX})
message(STATUS "Using PREFIX = ${CMAKE_INSTALL_PREFIX}")

# Module setup
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_SOURCE_DIR}/cmake")

include(FernTargets)

# macOS `rpath` fix
set(ORIGIN "$ORIGIN")
if(APPLE)
  set(ORIGIN "@loader_path")
endif()
set(CMAKE_INSTALL_RPATH "${ORIGIN}/../lib/fern;${ORIGIN}/../lib;${ORIGIN}/.")
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_BUILD_WITH_INSTALL_NAME_DIR TRUE)
set(CMAKE_MACOSX_RPATH TRUE)

# C++ Standard
set(CMAKE_CXX_STANDARD 20 CACHE STRING "C++ standard to conform to")
set(CMAKE_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
    "Build type for Fern [Debug, RelWithDebInfo, Release]"
  )
  message(STATUS "No build type was specified, will default to ${CMAKE_BUILD_TYPE}")
endif()

# Build definitions
option(FERN_DEBUG "Enable debug mode" OFF)

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug" OR "${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
  message(STATUS "Building with debug info...")
  add_definitions(-DDEBUG_INFO)

  set(FERN_DEBUG ON CACHE BOOL "Enable debug mode" FORCE)
  add_compile_options(-g)
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "Release" OR "${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
  message(STATUS "Building with optimisations...")
  add_definitions(-DRELEASE)
endif()

# Compiler flags
option(FERN_ADDRESS_SANITIZER "Enable address sanitizer" OFF)

if(${FERN_ADDRESS_SANITIZER})
  message(STATUS "Building with address sanitizer...")
  add_definitions(-DADDRESS_SANITIZER)
  add_compile_options(-fsanitize=address)
  add_link_options(-fsanitize=address)
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_TYPE_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

# ccache support
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
  message(STATUS "Using ccache: ${CCACHE_PROGRAM}")
endif()

# Config
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "Export compile commands")
set(FERN_VERSION "0.1")
set(FERN_V_MAJOR 0)
set(FERN_V_MINOR 1)
set(FERN_V_PATCH 0)

execute_process(COMMAND date +"%Y-%m-%d" OUTPUT_VARIABLE FERN_BUILD_DATE OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND git rev-parse --short HEAD OUTPUT_VARIABLE FERN_BUILD_HASH OUTPUT_STRIP_TRAILING_WHITESPACE)
configure_file(
  "${PROJECT_SOURCE_DIR}/include/FernConfig.hpp.in"
  "${PROJECT_SOURCE_DIR}/include/FernConfig.hpp" @ONLY
)

# External dependencies
include(FetchContent)

FetchContent_Declare(
  catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.4.0
)

FetchContent_Declare(
  cxxopts
  GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
  GIT_TAG v3.1.1
)

FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.2
)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 10.0.0
)

FetchContent_Declare(
  roots
  GIT_REPOSITORY https://github.com/fern-lang/roots.git
  GIT_TAG main
)

FetchContent_Declare(
  backward
  GIT_REPOSITORY https://github.com/bombela/backward-cpp.git
  GIT_TAG master
)

FetchContent_MakeAvailable(catch2)
FetchContent_MakeAvailable(cxxopts)
FetchContent_MakeAvailable(json)
FetchContent_MakeAvailable(fmt)
FetchContent_MakeAvailable(roots)
FetchContent_MakeAvailable(backward)

# Testing
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
enable_testing()

# LLVM libraries
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Install CMake modules
install(DIRECTORY "${PROJECT_SOURCE_DIR}/cmake/"
  DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake/"
)

# Install Includes
install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/"
  DESTINATION "${CMAKE_INSTALL_PREFIX}/include/fern"
)

# Compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add source files
add_subdirectory(lib)
