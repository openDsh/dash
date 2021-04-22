# cmake/gitversion.cmake
cmake_minimum_required(VERSION 3.0.0)

message(STATUS "Resolving GIT Version")

set(_build_version "unknown")

find_package(Git)
if(GIT_FOUND)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
    WORKING_DIRECTORY "${local_dir}"
    OUTPUT_VARIABLE _build_version
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY "${local_dir}"
    OUTPUT_VARIABLE _build_branch
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  message( STATUS "GIT hash: ${_build_version}; branch: ${_build_branch}")
  execute_process(
  	COMMAND ${GIT_EXECUTABLE} diff --no-ext-diff --quiet
    WORKING_DIRECTORY "${local_dir}"
  	RESULT_VARIABLE ret
  )
  if(ret EQUAL "1")
    set(_build_changes "*")
  else()
    set(_build_changes "")
  endif()

else()
  message(STATUS "GIT not found")
endif()

# branch name
# git rev-parse --abbrev-ref HEAD
# changed
# git diff --no-ext-diff --quiet

string(TIMESTAMP _time_stamp)

configure_file(${base_directory}/src/gitversion.h.in ${include_directory}/gitversion.h @ONLY)
