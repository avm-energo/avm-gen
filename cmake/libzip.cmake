include(FetchContent)

FetchContent_Declare(libzip
  GIT_REPOSITORY    https://github.com/avm-energo/libzip.git
  GIT_TAG           main
)

FetchContent_MakeAvailable(libzip)
