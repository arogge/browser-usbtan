cmake_minimum_required(VERSION 3.18)
project(browser-usbtan C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE
      "RelWithDebInfo"
      CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
                                               "MinSizeRel" "RelWithDebInfo")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
file(CREATE_LINK "${CMAKE_BINARY_DIR}/compile_commands.json"
     "${CMAKE_SOURCE_DIR}/compile_commands.json" SYMBOLIC)

add_subdirectory(deps EXCLUDE_FROM_ALL)

find_package(Gwenhywfar REQUIRED)

include(include-what-you-use)

add_compile_options(-Wall -Wextra -pedantic)

add_executable(browser-usbtan)
target_sources(
  browser-usbtan
  PRIVATE src/main.cc
          src/generate_tan.cc
          src/generate_tan.hh
          src/hhd_command.cc
          src/hhd_command.hh
          src/resolve_func.c
          src/resolve_func.h)
target_link_libraries(browser-usbtan PRIVATE Gwenhywfar::Gwenhywfar GSL
                                             nlohmann_json::nlohmann_json)

configure_file(usbtan.json.in ${CMAKE_BINARY_DIR}/usbtan.json @ONLY)

install(FILES ${CMAKE_BINARY_DIR}/usbtan.json
        DESTINATION $ENV{HOME}/.mozilla/native-messaging-hosts)
