include(FetchContent)

FetchContent_Declare(libzip
  GIT_REPOSITORY    git@github.com:avm-energo/libzip.git
  GIT_TAG           v1.11.5
)

FetchContent_MakeAvailable(libzip)
