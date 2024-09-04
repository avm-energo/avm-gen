include(FetchContent)

FetchContent_Declare(LibLZMA
  GIT_REPOSITORY    https://git.avmenergo.ru/avm-energo/xz-utils.git
  GIT_TAG           master
)

FetchContent_MakeAvailable(LibLZMA)

