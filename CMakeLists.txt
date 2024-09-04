cmake_minimum_required(VERSION 3.15)

project(gen
  DESCRIPTION "gen"
  VERSION 1.5.0)

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
include(SetProjectTargetName)

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

# Setup build type if it's not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
  message(STATUS "CMAKE_BUILD_TYPE is not specified. Using default: ${CMAKE_BUILD_TYPE}")
  # Possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

find_package(QT NAMES Qt6)
find_package(Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Network REQUIRED
)

find_package(LibLZMA QUIET)
if (${LibLZMA_FOUND})
    message(STATUS "Using installed liblzma")
else()
    message(STATUS "Using external xz-utils")
    include(lzma)
endif()

#------------------------------------------------------------------------------#
############################## Target definition ###############################
#------------------------------------------------------------------------------#
# Define target sources
file(GLOB_RECURSE cpp_sources ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE public_headers ${PROJECT_SOURCE_DIR}/include/*.h)

# Library building settings
if(BUILD_SHARED_LIBS)
    message(STATUS "GEN SHARED LIBRARY")
    add_library(${PROJECT_NAME} SHARED ${public_headers} ${cpp_sources})
    target_compile_definitions(${PROJECT_NAME} PRIVATE -DGEN_EXPORTS)
    target_compile_definitions(${PROJECT_NAME} INTERFACE -DGEN_IMPORTS)
else()
    message(STATUS "GEN STATIC LIBRARY")
    add_library(${PROJECT_NAME} STATIC ${public_headers} ${cpp_sources})
    target_compile_definitions(${PROJECT_NAME} PUBLIC -DHAVE_STATIC_BUILD)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

message(STATUS "LibLZMA_HEADERS_APIDIR: ${liblzma_SOURCE_DIR}/src/liblzma/api")
message(STATUS "LibLZMA_BINARY_DIR: ${liblzma_BINARY_DIR}")
target_include_directories(${PROJECT_NAME} PRIVATE ${liblzma_INCLUDE_DIRS} ${liblzma_SOURCE_DIR}/src/liblzma/api)

# Linking Qt core libraries for using in gen library
target_link_libraries(
  ${PROJECT_NAME} PUBLIC
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Network
  liblzma
  # ${liblzma_BINARY_DIR}/lzma.lib
)
target_compile_definitions(${PROJECT_NAME} PUBLIC -DCMAKE_CONFIG)

# add_dependencies(${PROJECT_NAME} LibLZMA)

# Using compile warnings to detect bugs and mistakes
if(SYSTEM_NAME STREQUAL "windows")
  target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else() # # GCC or Clang
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Using sanitizers for library in debug build type
if(GEN_USING_SANITIZERS)
  if(SYSTEM_NAME STREQUAL "windows")
    message(STATUS "MSVC version: ${MSVC_VERSION}, toolset version: ${MSVC_TOOLSET_VERSION}")
  else() # GCC or Clang
    target_compile_options(${PROJECT_NAME} PRIVATE -fno-omit-frame-pointer -fsanitize=address,undefined -fstack-protector)
    target_link_options(${PROJECT_NAME}    PRIVATE -fno-omit-frame-pointer -fsanitize=address,undefined -fstack-protector)
  endif()
endif()

# Set project version as gen target property
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})

message(STATUS "CMAKE_BUILD_TYPE is: ${CMAKE_BUILD_TYPE}")

# get_target_property(liblzma_lib LibLZMA IMPORTED_LOCATION_${CMAKE_BUILD_TYPE})
# message(STATUS "liblzma is: ${liblzma_lib}")
# add_library(liblzma ${liblzma_lib})


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
