# unsebu - libusb helper for C++
# Copyright (C) 2020 Ingo Ruhnke <grumbel@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.6)
project(unsebu VERSION 0.1.0)

set(TINYCMMC_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/external/tinycmmc/modules/")
find_package(tinycmmc CONFIG)
message(STATUS "tinycmmc module path: ${TINYCMMC_MODULE_PATH}")
list(APPEND CMAKE_MODULE_PATH ${TINYCMMC_MODULE_PATH})

option(BUILD_TESTS "Build test cases" OFF)

include(GNUInstallDirs)
include(MaximumWarnings)
include(ClangTidy)

find_package(PkgConfig)
find_package(fmt REQUIRED)
pkg_search_module(USB REQUIRED libusb-1.0 IMPORTED_TARGET)
pkg_search_module(UDEV REQUIRED libudev IMPORTED_TARGET)
pkg_search_module(DBUSGLIB REQUIRED dbus-glib-1 IMPORTED_TARGET)

function(build_dependencies)
  set(BUILD_TESTS OFF)

  find_package(logmich)
  if(NOT TARGET logmich::logmich)
    add_subdirectory(external/logmich/)
  endif()
endfunction()
build_dependencies()

file(GLOB UNSEBU_HEADER_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  include/unsebu/*.hpp)
file(GLOB UNSEBU_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  src/*.cpp)

add_library(unsebu STATIC ${UNSEBU_SOURCES})
set_target_properties(unsebu PROPERTIES PUBLIC_HEADER "${UNSEBU_HEADER_SOURCES}")
set_target_properties(unsebu PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF)
target_compile_options(unsebu PRIVATE ${WARNINGS_CXX_FLAGS})
target_include_directories(unsebu PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/unsebu/>
  $<INSTALL_INTERFACE:include>)
target_link_libraries(unsebu PUBLIC
  fmt::fmt
  PkgConfig::DBUSGLIB
  PkgConfig::USB
  PkgConfig::UDEV)

# if(BUILD_TESTS)
#   if(NOT TARGET GTest)
#     find_package(GTest REQUIRED)
#   endif()

#   file(GLOB TEST_UNSEBU_SOURCES test/*_test.cpp)
#   add_executable(test_unsebu ${TEST_UNSEBU_SOURCES})
#   set_target_properties(test_unsebu PROPERTIES
#     CXX_STANDARD 20
#     CXX_STANDARD_REQUIRED ON
#     CXX_EXTENSIONS OFF)
#   target_compile_options(test_unsebu PRIVATE ${WARNINGS_CXX_FLAGS})
#   target_include_directories(test_unsebu PRIVATE src/)
#   target_link_libraries(test_unsebu
#     unsebu
#     GTest::GTest
#     GTest::Main)

#   enable_testing()
#   add_test(NAME test_unsebu
#     WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
#     COMMAND test_unsebu)
# endif()

include(ExportAndInstallLibrary)

# EOF #
