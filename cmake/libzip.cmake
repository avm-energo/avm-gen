include(FetchContent)

FetchContent_Declare(libzip
  GIT_REPOSITORY    ssh://git@git.avmenergo.ru:5507/avm-energo/libzip.git
  GIT_TAG           v1.11.4
)

FetchContent_MakeAvailable(libzip)
