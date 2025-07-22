include(FetchContent)

FetchContent_Declare(libzip
  GIT_REPOSITORY    https://git.avmenergo.ru/avm-energo/libzip.git
  GIT_TAG           v1.11.4
)

FetchContent_MakeAvailable(libzip)
