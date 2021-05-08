#[=======================================================================[.rst:
FindGwenhywfar
-----------

Find Gwenhywfar headers and libraries.
`
IMPORTED Targets
^^^^^^^^^^^^^^^^

The following :prop_tgt:`IMPORTED` targets may be defined:

``Gwenhywfar::Gwenhywfar``
  Gwenhywfar library.

Result variables
^^^^^^^^^^^^^^^^

This module will set the following variables in your project:

``GWENHYWFAR_FOUND``
  True if Gwenhywfar found.
``GWENHYWFAR_INCLUDE_DIRS``
  Where to find gwenhywfar/version.h.
``GWENHYWFAR_LIBRARIES``
  List of libraries when using Gwenhywfar.
``GWENHYWFAR_VERSION_STRING``
  The version of Gwenhywfar found.

#]=======================================================================]

find_package(PkgConfig QUIET)
pkg_check_modules(PC_GWENHYWFAR QUIET gwenhywfar)

find_path(
  GWENHYWFAR_INCLUDE_DIR
  NAMES gwenhywfar/version.h
  HINTS ${PC_GWENHYWFAR_INCLUDEDIR} ${PC_GWENHYWFAR_INCLUDE_DIRS}
)

find_library(
  GWENHYWFAR_LIBRARY
  NAMES gwenhywfar libgwenhywfar
  HINTS ${PC_GWENHYWFAR_LIBDIR} ${PC_GWENHYWFAR_LIBRARY_DIRS}
)

if(PC_GWENHYWFAR_VERSION)
  set(GWENHYWFAR_VERSION_STRING ${PC_GWENHYWFAR_VERSION})
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  Gwenhywfar
  REQUIRED_VARS GWENHYWFAR_LIBRARY GWENHYWFAR_INCLUDE_DIR
  VERSION_VAR GWENHYWFAR_VERSION_STRING
)

if(GWENHYWFAR_FOUND)
  set(GWENHYWFAR_LIBRARIES ${GWENHYWFAR_LIBRARY})
  set(GWENHYWFAR_INCLUDE_DIRS ${GWENHYWFAR_INCLUDE_DIR})
endif()

mark_as_advanced(GWENHYWFAR_INCLUDE_DIR GWENHYWFAR_LIBRARY)

if(NOT TARGET Gwenhywfar::Gwenhywfar)
  add_library(Gwenhywfar::Gwenhywfar UNKNOWN IMPORTED)
  set_target_properties(
    Gwenhywfar::Gwenhywfar PROPERTIES INTERFACE_INCLUDE_DIRECTORIES
    "${GWENHYWFAR_INCLUDE_DIRS}"
  )
  set_target_properties(
    Gwenhywfar::Gwenhywfar PROPERTIES INTERFACE_COMPILE_OPTIONS
    "${GWENHYWFAR_DEFINITIONS}"
  )
  set_property(
    TARGET Gwenhywfar::Gwenhywfar
    APPEND
    PROPERTY IMPORTED_LOCATION "${GWENHYWFAR_LIBRARY}"
  )
endif()
