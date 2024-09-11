project(gen_tests)

find_package(Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Test REQUIRED)
include(CTest)

# Collecting sources
set(GEN_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/gen_test_class.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gen_test_class.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/binary_file_tests.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/stdfunc_tests.cpp
)

# Adding executable file for gen tests
add_executable(${PROJECT_NAME} ${GEN_TEST_SOURCES})
# Adding test in ctest
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

# Using compile warnings to detect bugs and mistakes
if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else() # # GCC or Clang
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Using sanitizers for tests if gen using sanitizers
if(GEN_USING_SANITIZERS)
  if(MSVC)
    message(STATUS "MSVC version: ${MSVC_VERSION}, toolset version: ${MSVC_TOOLSET_VERSION}")
  else() # GCC or Clang
    target_compile_options(${PROJECT_NAME} PRIVATE -fno-omit-frame-pointer -fsanitize=address,undefined -fstack-protector)
    target_link_options(${PROJECT_NAME}    PRIVATE -fno-omit-frame-pointer -fsanitize=address,undefined -fstack-protector)
  endif()
endif()

# Linking all necessary libraries
target_link_libraries(${PROJECT_NAME}
  avm-gen
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Test)
