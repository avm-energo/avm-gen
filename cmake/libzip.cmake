include(FetchContent)

FetchContent_Declare(libzip
  GIT_REPOSITORY    https://github.com/nih-at/libzip.git
  GIT_TAG           main
)

set(LIBZIP_LIBRARIES libzip)
FetchContent_MakeAvailable(libzip)
