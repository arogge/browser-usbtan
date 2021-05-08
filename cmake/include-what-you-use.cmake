find_program(IWYU_EXECUTABLE iwyu)

if(IWYU_EXECUTABLE)
  message(STATUS "Enabling include-what-you-use")
  set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE "${IWYU_EXECUTABLE}")
  set(CMAKE_C_INCLUDE_WHAT_YOU_USE "${IWYU_EXECUTABLE}")
endif()
