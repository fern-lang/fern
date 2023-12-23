#
# newFernTarget(
#   name
#   source1 [source2 source3 ...]
#
#   [BINARY] (ignores STATIC)
#   [STATIC]
#   [INCLUDE_INSTALL_DIR [/path/to/dir]] (default: '${PREFIX}/include')
#   [INSTALL_DIR [/path/to/dir]] (default: '${PREFIX}/bin' or '${PREFIX}/lib')
#   [LINK_LIBS [lib1 lib2 ...]]
# )
#
#
# name
#   Name of target
#
# sources...
#   Sources for target
#
# LINK_LIBS libs...
#   Libraries to link with
#
#
function(newFernTarget targetName)
  cmake_parse_arguments(FT "STATIC;BINARY;NO_COMPILE_DB" "INCLUDE_INSTALL_DIR;INSTALL_DIR" "LINK_LIBS" ${ARGN})

  if (__FT_${targetName}_exists)
    message(FATAL_ERROR "newFernTarget: Target `${targetName}` already exists")
  endif()

  # get all the sources in the arguments
  set(FT_SOURCES ${FT_UNPARSED_ARGUMENTS})

  get_filename_component(base_dir ${CMAKE_CURRENT_SOURCE_DIR} NAME)

  file(GLOB_RECURSE FT_HEADERS
      ${PROJECT_SOURCE_DIR}/include/${base_dir}/*.h
      ${PROJECT_SOURCE_DIR}/include/${base_dir}/*.hpp
      ${PROJECT_SOURCE_DIR}/include/*.h
      ${PROJECT_SOURCE_DIR}/include/*.hpp)
  
  set(FT_ALL_SOURCES ${FT_SOURCES} ${FT_HEADERS})

  if(FT_BINARY)
    add_executable(${targetName} ${FT_ALL_SOURCES} ${BACKWARD_ENABLE})
  elseif(FT_STATIC)
    add_library(${targetName} ${FT_ALL_SOURCES} ${BACKWARD_ENABLE})
  else()
    add_library(${targetName} SHARED ${FT_ALL_SOURCES} ${BACKWARD_ENABLE})
  endif()

  # deprecated: add_backward(${targetName})
  target_link_libraries(${targetName} PUBLIC Backward::Backward)

  target_include_directories(
    ${targetName}
    PUBLIC ${PROJECT_SOURCE_DIR}/include/
    PUBLIC ${PROJECT_SOURCE_DIR}/include/${base_dir}
  )

  # Add vendor libraries
  target_link_directories(
    ${targetName}
    PRIVATE cxxopts::cxxopts
    PRIVATE nlohmann_json::nlohmann_json
    PRIVATE fmt::fmt
    PRIVATE Roots::Roots  
    PRIVATE LLVM
  )

  if(FT_LINK_LIBS)
    target_link_libraries(
      ${targetName}
      PUBLIC ${FT_LINK_LIBS}
      PUBLIC ${CMAKE_DL_LIBS}
    )
  endif()

  if(FT_BINARY)
    set_target_properties(
      ${targetName}
      PROPERTIES
      OUTPUT_NAME ${targetName}
      RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin"
      INSTALL_RPATH_USE_LINK_PATH ON
    )

    install(
      TARGETS ${targetName}
      RUNTIME
        DESTINATION bin/${FT_INSTALL_DIR}
        COMPONENT Binaries
      PUBLIC_HEADER
        DESTINATION include/${FT_INCLUDE_INSTALL_DIR}
    )
  else()
    set_target_properties(
      ${targetName}
      PROPERTIES
      PREFIX "libFern"
      OUTPUT_NAME ${targetName}
      RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib/${FT_LIBRARY_INSTALL_DIR}"
      INSTALL_RPATH_USE_LINK_PATH ON
    )

    install(
      TARGETS ${targetName}
      LIBRARY
        DESTINATION lib/${FT_INSTALL_DIR}
        COMPONENT Libraries
      PUBLIC_HEADER
        DESTINATION include/${FT_INCLUDE_INSTALL_DIR}
    )
  endif()

  set(__FT_${targetName}_exists TRUE PARENT_SCOPE)
  set(__FT_${targetName}_base_dir ${base_dir} PARENT_SCOPE)
  set(__FT_${targetName}_link_libs ${FT_LINK_LIBS} PARENT_SCOPE)
  set(__FT_${targetName}_is_binary ${FT_BINARY} PARENT_SCOPE)
endfunction()

#
# newFernTest(
#   name
#   
#   [AGAINST [target]]
#   [BENCH|TEST]
#   [SOURCES [source1 source2 ...]]
# )
#
#
# name
#   Name of test/benchmark
#
# AGAINST target
#   Target to test/benchmark against
#
# BENCH
#   Build benchmark
#
# TEST
#   Build test, overrides BENCH
#
# SOURCES sources...
#   Sources for test/benchmark
#
function(newFernTest testName)
  cmake_parse_arguments(FT "TEST;BENCH" "AGAINST" "SOURCES" ${ARGN})

  if(NOT __FT_${FT_AGAINST}_exists)
    message(FATAL_ERROR "newFernTest: Target `${FT_AGAINST}` does not exist")
  endif()

  if(__FT_${testName}_exists)
    message(FATAL_ERROR "newFernTest: Test `${testName}` already exists")
  endif()

  add_executable(
    ${testName}
    ${FT_SOURCES}
    ${BACKWARD_ENABLE}
  )

  add_backward(${testName})

  target_include_directories(
    ${testName}
    PUBLIC ${PROJECT_SOURCE_DIR}/include/
    PUBLIC ${PROJECT_SOURCE_DIR}/include/${__FT_${FT_AGAINST}_base_dir}
  )

  # Add vendor libraries
  if(FT_TEST)
    target_link_libraries(
      ${testName}
      PRIVATE nlohmann_json::nlohmann_json
      PRIVATE fmt::fmt
      PRIVATE Roots::Roots
      PRIVATE Catch2::Catch2WithMain
      PRIVATE LLVM
    )
  elseif(FT_BENCH)
    target_link_libraries(
      ${testName}
      PRIVATE nlohmann_json::nlohmann_json
      PRIVATE fmt::fmt
      PRIVATE Roots::Roots
      PRIVATE nano
      PRIVATE LLVM
    )
  endif()

  if(NOT __FT_${FT_AGAINST}_is_binary)
    target_link_libraries(
      ${testName}
      PRIVATE ${FT_AGAINST}
    )
  endif()

  if(FT_BENCH)
    # For benchmarks, 
    set_target_properties(
      ${testName}
      PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bench"
    )

    if (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
      install(
        TARGETS ${testName}
        RUNTIME
          DESTINATION bench
          COMPONENT Benchmarks
      )
    endif()

    return()
  endif()

  # For tests,
  set_target_properties(
    ${testName}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/test"
  )
  include(CTest)
  include(Catch)
  catch_discover_tests(
    ${testName}
    OUTPUT_DIR "${PROJECT_BINARY_DIR}/test"
  )

  # DEPRECATED: hacky way to get the test to run
  # add_test(
  #   NAME ${testName}
  #   COMMAND "${PROJECT_BINARY_DIR}/test/${testName}"
  # )
endfunction()
