# Copyright (c)  2012-2015  Mikael Leetmaa
#
# This file is part of the KMCLib project distributed under the terms of the
# GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
#

cmake_minimum_required(VERSION 2.8)
project(KMCLibUnittest)

if (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
  set( CPPUNIT ${KMCLibUnittest_SOURCE_DIR}/../externals/lib/cppunit/lib/libcppunit.dylib )

else()
  # Use the custom search path for the cppunit test library if given.
  message( STATUS "Custom search path for -lcppunit: ${CPPUNIT_DIR}")
  find_library( CPPUNIT NAMES cppunit PATHS
                ${CPPUNIT_DIR}
                ${KMCLibUnittest_SOURCE_DIR}/../externals/lib/cppunit/lib )
endif()

message( STATUS "Found cppunit:  ${CPPUNIT}")

# Find all test files.
file( GLOB CppSources test_* )

# Needed for clang.
add_definitions( -Wno-unused-parameter )
add_definitions( -Wno-unused-value )

# Compiler flag needed for using CPPUNIT.
add_definitions( -Wno-deprecated )

# Includsion from the source.
include_directories( ${KMCLibUnittest_SOURCE_DIR}/../src )
include_directories( ${KMCLibUnittest_SOURCE_DIR}/../externals/include )
link_directories( ${KMCLibUnittest_BINARY_DIR} )

# Compile the the unittest source.
add_library( unittest EXCLUDE_FROM_ALL ${CppSources} )

# Build and link the test runner.
add_executable( test.x EXCLUDE_FROM_ALL testRunner )

# Define the libraries to link the test.x executable against.
target_link_libraries( test.x ${CPPUNIT} unittest src )

# Test all functions by executing test.x
add_test( kmctest ${KMCLibUnittest_BINARY_DIR}/test.x )
enable_testing()
