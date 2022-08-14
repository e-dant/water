cmake_minimum_required(VERSION 3.16)

# Add project_options v0.24.0
# https://github.com/cpp-best-practices/project_options
# for FetchContent_Declare
# for FetchContent_MakeAvailable
include(FetchContent)
FetchContent_Declare(
  _project_options 
  URL https://github.com/aminya/project_options/archive/refs/tags/v0.24.0.zip)
FetchContent_MakeAvailable(_project_options)
# for project_options and other utilities
include(${_project_options_SOURCE_DIR}/Index.cmake)
# for dynamic_project_options
include(${_project_options_SOURCE_DIR}/src/DynamicProjectOptions.cmake)

# uncomment to enable vcpkg:
# # Setup vcpkg - should be called before defining project()
# run_vcpkg()

project(
  argparse
  VERSION 0.0.1
  DESCRIPTION "water::argparse (Common, generally standalone utilities for C++ projects)"
  HOMEPAGE_URL "https://edant.io"
  LANGUAGES CXX)

set(GIT_SHA
    "Unknown"
    CACHE STRING "SHA this build was generated from")

string(
  SUBSTRING "${GIT_SHA}"
            0
            8
            GIT_SHORT_SHA)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

dynamic_project_options(
  ENABLE_CACHE
  # ${ENABLE_CPPCHECK}
  # ${ENABLE_CLANG_TIDY}
  # ENABLE_VS_ANALYSIS
  # ENABLE_CONAN
  # ENABLE_INTERPROCEDURAL_OPTIMIZATION
  # ENABLE_NATIVE_OPTIMIZATION
  # ${ENABLE_DOXYGEN}
  # ${ENABLE_COVERAGE}
  ENABLE_SANITIZER_ADDRESS
  ENABLE_SANITIZER_UNDEFINED_BEHAVIOR
  #ENABLE_SANITIZER_THREAD
  #ENABLE_SANITIZER_MEMORY
  # PCH_HEADERS
  # WARNINGS_AS_ERRORS
  # ENABLE_INCLUDE_WHAT_YOU_USE
  # ENABLE_USER_LINKER
  # ENABLE_BUILD_WITH_TIME_TRACE
  # ENABLE_UNITY
  # passes a profile to conan: see https://docs.conan.io/en/latest/reference/profiles.html
  # CONAN_PROFILE ${profile_path}
  # Note: PCH is disabled by default in developer mode because these headers become
  # globally included and they can mask other errors
  ENABLE_PCH
  PCH_HEADERS
  # This is a list of headers to pre-compile, here are some common ones
  <vector>
  <string>
  ENABLE_CONAN
  # CONAN_OPTIONS    # Extra options to pass to conan
  # MSVC_WARNINGS    # Override the defaults for the MSVC warnings
  # CLANG_WARNINGS   # Override the defaults for the CLANG warnings
  # GCC_WARNINGS     # Override the defaults for the GCC warnings
)

add_executable(${PROJECT_NAME} ../../example/example.cpp)
# link with project_options/warnings
target_link_libraries(${PROJECT_NAME} PRIVATE project_options project_warnings)

## Find dependencies:
#set(DEPENDENCIES_CONFIGURED fmt Eigen3)
#
#foreach(DEPENDENCY ${DEPENDENCIES_CONFIGURED})
#  find_package(${DEPENDENCY} CONFIG REQUIRED)
#endforeach()

## Link dependencies
#target_link_system_libraries(
#  main
#  PRIVATE
#  fmt::fmt
#  Eigen3::Eigen
#)

# Package the project
package_project(
  TARGETS ${PROJECT_NAME}
  # FIXME: this does not work! CK
  # PRIVATE_DEPENDENCIES_CONFIGURED project_options project_warnings
)

# Experience shows that explicit package naming can help make it easier to sort
# out potential ABI related issues before they start, while helping you
# track a build to a specific GIT SHA
set(CPACK_PACKAGE_FILE_NAME
    "${CMAKE_PROJECT_NAME}-${CMAKE_PROJECT_VERSION}-${GIT_SHORT_SHA}-${CMAKE_SYSTEM_NAME}-${CMAKE_BUILD_TYPE}-${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}"
)

include(CPack)