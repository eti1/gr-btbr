# Copyright 2011,2012,2016 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Setup library
########################################################################
include(GrPlatform) #define LIB_SUFFIX
find_package(BTBB REQUIRED)

include_directories(${Boost_INCLUDE_DIR} ${LIBBTBB_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

list(APPEND btbr_sources
    btdecode_impl.cc
)

set(btbr_sources "${btbr_sources}" PARENT_SCOPE)
if(NOT btbr_sources)
	MESSAGE(STATUS "No C++ sources... skipping lib/")
	return()
endif(NOT btbr_sources)

add_library(gnuradio-btbr SHARED ${btbr_sources})
target_link_libraries(gnuradio-btbr ${Boost_LIBRARIES} ${GNURADIO_ALL_LIBRARIES} ${LIBBTBB_LIBRARIES})
set_target_properties(gnuradio-btbr PROPERTIES DEFINE_SYMBOL "gnuradio_btbr_EXPORTS")

if(APPLE)
    set_target_properties(gnuradio-btbr PROPERTIES
        INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
    )
endif(APPLE)

########################################################################
# Install built library files
########################################################################
include(GrMiscUtils)
GR_LIBRARY_FOO(gnuradio-btbr RUNTIME_COMPONENT "btbr_runtime" DEVEL_COMPONENT "btbr_devel")

########################################################################
# Build and register unit test
########################################################################
include(GrTest)

include_directories(${CPPUNIT_INCLUDE_DIRS})

list(APPEND test_btbr_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/test_btbr.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_btbr.cc
)

add_executable(test-btbr ${test_btbr_sources})

target_link_libraries(
  test-btbr
  ${GNURADIO_RUNTIME_LIBRARIES}
  ${Boost_LIBRARIES}
  ${CPPUNIT_LIBRARIES}
  gnuradio-btbr
)

GR_ADD_TEST(test_btbr test-btbr)

########################################################################
# Print summary
########################################################################
message(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Building for version: ${VERSION} / ${LIBVER}")

