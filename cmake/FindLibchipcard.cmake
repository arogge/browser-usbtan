#[=======================================================================[.rst:
FindLibchipcard
-----------

Find Libchipcard headers and libraries.
`
IMPORTED Targets
^^^^^^^^^^^^^^^^

The following :prop_tgt:`IMPORTED` targets may be defined:

``Libchipcard::Libchipcard``
Libchipcard library.

Result variables
^^^^^^^^^^^^^^^^

This module will set the following variables in your project:

``LIBCHIPCARD_FOUND``
True if Libchipcard found.
``LIBCHIPCARD_INCLUDE_DIRS``
  Where to find chipcard/version.h.
  ``LIBCHIPCARD_LIBRARIES``
List of libraries when using Libchipcard.
``LIBCHIPCARD_VERSION_STRING``
The version of Libchipcard found.

#]=======================================================================]

find_package(PkgConfig QUIET)
pkg_check_modules(PC_LIBCHIPCARD QUIET libchipcard-server)

find_path(
  LIBCHIPCARD_INCLUDE_DIR
  NAMES chipcard/version.h
  HINTS ${PC_LIBCHIPCARD_INCLUDEDIR} ${PC_LIBCHIPCARD_INCLUDE_DIRS}
)

find_library(
  LIBCHIPCARD_LIBRARY
  NAMES chipcard libchipcard
  HINTS ${PC_LIBCHIPCARD_LIBDIR} ${PC_LIBCHIPCARD_LIBRARY_DIRS}
)

if(PC_LIBCHIPCARD_VERSION)
  set(LIBCHIPCARD_VERSION_STRING ${PC_LIBCHIPCARD_VERSION})
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  Libchipcard
  REQUIRED_VARS LIBCHIPCARD_LIBRARY LIBCHIPCARD_INCLUDE_DIR
  VERSION_VAR LIBCHIPCARD_VERSION_STRING
)

if(LIBCHIPCARD_FOUND)
  set(LIBCHIPCARD_LIBRARIES ${LIBCHIPCARD_LIBRARY})
  set(LIBCHIPCARD_INCLUDE_DIRS ${LIBCHIPCARD_INCLUDE_DIR})
endif()

mark_as_advanced(LIBCHIPCARD_INCLUDE_DIR LIBCHIPCARD_LIBRARY)

if(NOT TARGET Libchipcard::Libchipcard)
  add_library(Libchipcard::Libchipcard UNKNOWN IMPORTED)
  set_target_properties(
    Libchipcard::Libchipcard PROPERTIES INTERFACE_INCLUDE_DIRECTORIES
    "${LIBCHIPCARD_INCLUDE_DIRS}"
  )
  set_target_properties(
    Libchipcard::Libchipcard PROPERTIES INTERFACE_COMPILE_OPTIONS
    "${LIBCHIPCARD_DEFINITIONS}"
  )
  set_property(
    TARGET Libchipcard::Libchipcard
    APPEND
    PROPERTY IMPORTED_LOCATION "${LIBCHIPCARD_LIBRARY}"
  )
endif()
