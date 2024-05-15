cmake_minimum_required(VERSION 3.15)

project(gen
  DESCRIPTION "gen"
  VERSION 1.3.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(GEN_STATIC "Build gen as static library" OFF)
option(GEN_USING_SANITIZERS "Using ASan and UBSan for gen and its tests" ON)
option(GEN_BUILD_TESTS "Build gen library tests" ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_BINARY_DIR}")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(GNUInstallDirs)
include(DetermineTargetArchitecture)
include(SetProjectTargetName)

# Setup build type if it's not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
  message(STATUS "CMAKE_BUILD_TYPE is not specified. Using default: ${CMAKE_BUILD_TYPE}")
  # Possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

find_package(QT NAMES Qt6)
find_package(Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Network Xml REQUIRED
)

if(NOT DEFINED PROJECT_ARCHITECTURE)
  target_architecture(PROJECT_ARCHITECTURE)
  set_project_target_name(${PROJECT_ARCHITECTURE} PROJECT_TARGET_NAME USER_DIRECTORY)
endif()

if(PROJECT_TARGET_NAME MATCHES "win")
  include(ConanInstallDependencies)
endif()

#------------------------------------------------------------------------------#
############################## Target definition ###############################
#------------------------------------------------------------------------------#
# Define target sources
file(GLOB_RECURSE cpp_sources ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE public_headers ${PROJECT_SOURCE_DIR}/include/*.h)

# Library building settings
if(GEN_STATIC)
  message(STATUS "GEN STATIC LIBRARY")
  add_library(${PROJECT_NAME} STATIC ${public_headers} ${cpp_sources})
  target_compile_definitions(${PROJECT_NAME} PUBLIC -DHAVE_STATIC_BUILD)
else()
  message(STATUS "GEN SHARED LIBRARY")
  add_library(${PROJECT_NAME} SHARED ${public_headers} ${cpp_sources})
  target_compile_definitions(${PROJECT_NAME} PRIVATE -DGEN_EXPORTS)
  target_compile_definitions(${PROJECT_NAME} INTERFACE -DGEN_IMPORTS)
endif()
target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
target_compile_definitions(${PROJECT_NAME} PUBLIC -DCMAKE_CONFIG)

# Using compile warnings to detect bugs and mistakes
if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else() # # GCC or Clang
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Using sanitizers for library in debug build type
if(GEN_USING_SANITIZERS)
  if(MSVC)
    message(STATUS "MSVC version: ${MSVC_VERSION}, toolset version: ${MSVC_TOOLSET_VERSION}")
  else() # GCC or Clang
    target_compile_options(${PROJECT_NAME} PRIVATE -fno-omit-frame-pointer -fsanitize=address,undefined -fstack-protector)
    target_link_options(${PROJECT_NAME}    PRIVATE -fno-omit-frame-pointer -fsanitize=address,undefined -fstack-protector)
  endif()
endif()

# Set project version as gen target property
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})

# Linking Qt core libraries for using in gen library
target_link_libraries(
  ${PROJECT_NAME} PUBLIC
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Xml
  Qt${QT_VERSION_MAJOR}::Network
)

# Using Qt6: linking Qt5 Compatibility library
#if (${QT_VERSION_MAJOR} EQUAL 6)
#  find_package(Qt${QT_VERSION_MAJOR}
#    COMPONENTS Core5Compat REQUIRED)
#  target_link_libraries(
##    ${PROJECT_NAME} PUBLIC
 #   Qt${QT_VERSION_MAJOR}::Core5Compat)
#endif()

# Linking liblzma
if(PROJECT_TARGET_NAME MATCHES "win")
  # Getting liblzma from conan xz_utils packet
  find_package(LibLZMA)
  if(LibLZMA_FOUND)
    include_directories(${LibLZMA_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE LibLZMA::LibLZMA)
  else()
    message(FATAL_ERROR "liblzma not found, please install it manually!")
  endif()
else()
  # Getting liblzma from APT: apt-get install liblzma-dev
  target_link_libraries(${PROJECT_NAME} PUBLIC lzma)
endif()

# Building tests
if (GEN_BUILD_TESTS)
  add_subdirectory(tests)
endif()

# --- Install libs and binaries ---
install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

# --- Install include headers ---
install(FILES
  ${public_headers}
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}")
