# Copyright (c)  2012-2013  Mikael Leetmaa
# Copyright (c)  2016-2019  Shao Zhengjiang
#
# This file is part of the KMCLib project distributed under the terms of the
# GNU General Public License version 3, see <http://www.gnu.org/licenses/>.
#

# -----------------------------------------------------------------------------
# LOAD SWIG
# -----------------------------------------------------------------------------

# Make sure the swig package is loaded.
find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

# Make sure the Python include files and libraries are loaded.
find_package(PythonLibs REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories( ${KMCLib_SOURCE_DIR}/src )

# Read config file.
if (CONFIG)
    file(READ "${PROJECT_SOURCE_DIR}/config" config)
    string(REGEX MATCH "^PYINCLUDE = (.*)\nPYLIB = (.*)\n+$" path ${config})

    # Read PYINCLUDE.
    if (CMAKE_MATCH_1)
        set(PYINCLUDE ${CMAKE_MATCH_1})
    else()
        message(FATAL_ERROR "Missing PYINCLUDE in config file")
    endif()

    # Read PYLIB.
    if (CMAKE_MATCH_2)
        set(PYLIB ${CMAKE_MATCH_2})
    else()
        message(FATAL_ERROR "Missing PYLIB in config file")
    endif()
endif()

# Load PYTHON_INCLUDE_PATH & PYTHON_LIBRARY.
if (PYINCLUDE)
    # Python header file include.
    message( STATUS "Customized Python include path detected" )
    set(PYTHON_INCLUDE_PATH ${PYINCLUDE})
    include_directories(${PYTHON_INCLUDE_PATH})

    # Python library.
    if (NOT PYLIB)
        message(FATAL_ERROR "No Python library provided, use `cmake -DPYLIB=<python-library>`")
    endif()

    message( STATUS "Customized Python library detected" )
    set(PYTHON_LIBRARY ${PYLIB})
else()
    find_package(PythonLibs REQUIRED)
endif()

message(STATUS "Using Python header files in ${PYTHON_INCLUDE_PATH}")
message(STATUS "Using Python library ${PYTHON_LIBRARY}")

# Set the flags.
set(CMAKE_SWIG_FLAGS "")

# Set the properties for the interface file.
set_source_files_properties(backend.i PROPERTIES CPLUSPLUS ON)
set_source_files_properties(backend.i PROPERTIES SWIG_FLAGS "")


# -----------------------------------------------------------------------------
# SET COMPILER FLAGS TO IGNORE KNOWN SWIG GENERATED ERRORS
# -----------------------------------------------------------------------------
add_definitions( -Wno-uninitialized )

# For intel.
if (USING_INTEL)
   message( STATUS "Ignoring Intel SWIG warnings")
  add_definitions( -Wno-unused-but-set-variable )

elseif (CMAKE_COMPILER_IS_GNUCXX)
  message( STATUS "Ignoring g++ SWIG warnings")
  add_definitions( -Wno-unused-but-set-variable )
  # For older g++.
  add_definitions( -Wno-missing-field-initializers )
  add_definitions( -Wno-write-strings )
  add_definitions( -fno-strict-aliasing )

elseif (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
  message( STATUS "Ignoring Clang SWIG warnings")
  add_definitions( -Wno-unused-variable )
  add_definitions( -Wno-dynamic-class-memaccess )
  add_definitions( -Wno-self-assign )
  add_definitions( -Wno-deprecated )

  # ML: Do we need these here?
  add_definitions( -Wno-missing-field-initializers )
  add_definitions( -Wno-write-strings )
  add_definitions( -fno-strict-aliasing )


endif()


# Add the target.
swig_add_module( Backend python backend.i )


# -----------------------------------------------------------------------------
# LINK
# -----------------------------------------------------------------------------

message( STATUS "Creating makefiles for system: ${CMAKE_SYSTEM}")
# For Mac OS X
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

  # To force a Mac OSX with macports Python, use with -DMACPORT=TRUE
  if (MACPORT)
    set(CMAKE_LIBRARY_PATH "/opt/local/Library/Frameworks")
    message( STATUS "Looking for libraries in ${CMAKE_LIBRARY_PATH}" )
  endif()

  swig_link_libraries( Backend ${PYTHON_LIBRARY} src )

# For Linux (Ubuntu 12.04 LTS)
else()#${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  swig_link_libraries( Backend src )
endif()

