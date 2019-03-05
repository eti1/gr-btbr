INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_BTBR btbr)

FIND_PATH(
    BTBR_INCLUDE_DIRS
    NAMES btbr/api.h
    HINTS $ENV{BTBR_DIR}/include
        ${PC_BTBR_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    BTBR_LIBRARIES
    NAMES gnuradio-btbr
    HINTS $ENV{BTBR_DIR}/lib
        ${PC_BTBR_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(BTBR DEFAULT_MSG BTBR_LIBRARIES BTBR_INCLUDE_DIRS)
MARK_AS_ADVANCED(BTBR_LIBRARIES BTBR_INCLUDE_DIRS)

