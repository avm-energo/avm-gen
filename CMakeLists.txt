cmake_minimum_required(VERSION 3.18)

project(avm-gen
  DESCRIPTION "General library for AVM-Energo C++ projects"
  VERSION 1.10.6)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_BINARY_DIR}")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Getting CMake system name
string(TOLOWER "${CMAKE_SYSTEM_NAME}" SYSTEM_NAME)

if(SYSTEM_NAME STREQUAL "linux")
    message(STATUS "Compiling for linux")
    add_compile_definitions(SYSTEM_TYPE=0)
elseif(SYSTEM_NAME STREQUAL "windows")
    message(STATUS "Compiling for windows")
    add_compile_definitions(SYSTEM_TYPE=1)
else()
    message(STATUS "Compiling for unknown")
    add_compile_definitions(SYSTEM_TYPE=2)
endif()

include(libzip)

# Setup build type if it's not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
  message(STATUS "CMAKE_BUILD_TYPE is not specified. Using default: ${CMAKE_BUILD_TYPE}")
  # Possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

find_package(QT NAMES Qt5 Qt6)
find_package(Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Network Xml REQUIRED
)

#------------------------------------------------------------------------------#
############################## Target definition ###############################
#------------------------------------------------------------------------------#
# Define target sources
file(GLOB_RECURSE cpp_sources ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE h_sources ${PROJECT_SOURCE_DIR}/include/*.h)
file(GLOB_RECURSE public_headers RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/include LIST_DIRECTORIES true ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/*.h)

# Library building settings
if(BUILD_SHARED_LIBS)
    message(STATUS "GEN SHARED LIBRARY")
    add_library(${PROJECT_NAME} SHARED ${h_sources} ${cpp_sources})
    target_compile_definitions(${PROJECT_NAME} PRIVATE -DGEN_EXPORTS)
    target_compile_definitions(${PROJECT_NAME} INTERFACE -DGEN_IMPORTS)
else()
    message(STATUS "GEN STATIC LIBRARY")
    add_library(${PROJECT_NAME} STATIC ${h_sources} ${cpp_sources})
    target_compile_definitions(${PROJECT_NAME} PUBLIC -DHAVE_STATIC_BUILD)
endif()

# add synonim for gen
add_library(avm-energo::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# Set project version as gen target property
set_target_properties(${PROJECT_NAME} PROPERTIES
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
  PUBLIC_HEADER "${public_headers}"
)

# if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
#     option(GEN_USING_SANITIZERS "Using ASan and UBSan for gen and its tests" OFF)
#     if (BUILD_TESTING)
#       add_subdirectory(tests)
#     endif()
#     if(GEN_USING_SANITIZERS)
#       if(SYSTEM_NAME STREQUAL "windows")
#         message(STATUS "MSVC version: ${MSVC_VERSION}, toolset version: ${MSVC_TOOLSET_VERSION}")
#       else() # GCC or Clang
#         target_compile_options(${PROJECT_NAME} PRIVATE -fno-omit-frame-pointer -fsanitize=address,undefined -fstack-protector)
#         target_link_options(${PROJECT_NAME}    PRIVATE -fno-omit-frame-pointer -fsanitize=address,undefined -fstack-protector)
#       endif()
#     endif()
# endif()

target_link_libraries(
    ${PROJECT_NAME} PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Xml
)

target_link_libraries(
  ${PROJECT_NAME} PUBLIC
  avm-libzip
)

target_compile_definitions(${PROJECT_NAME} PUBLIC -DCMAKE_CONFIG)

# Using compile warnings to detect bugs and mistakes
if(SYSTEM_NAME STREQUAL "windows")
  target_compile_options(${PROJECT_NAME} PRIVATE)
else() # # GCC or Clang
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Wno-nullability-extension)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

# --- Install libs and binaries ---
set(PROJECT_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    CACHE STRING "Path to ${PROJECT_NAME} CMake files")

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}_Targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# create version file for cmake project
write_basic_package_version_file(${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION
    ${PROJECT_INSTALL_CMAKEDIR})

install(EXPORT ${PROJECT_NAME}_Targets
        FILE ${PROJECT_NAME}Targets.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${PROJECT_INSTALL_CMAKEDIR})

install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION ${PROJECT_INSTALL_CMAKEDIR})

set(VERSION_IN ${CMAKE_CURRENT_SOURCE_DIR}/VERSION.in)
set(VERSION_OUT ${CMAKE_CURRENT_SOURCE_DIR}/package/src/VERSION)
configure_file(${VERSION_IN} ${VERSION_OUT} @ONLY)
