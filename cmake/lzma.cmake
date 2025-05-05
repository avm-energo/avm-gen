include(FetchContent)

find_package(Git REQUIRED)

set(XZ_DOC OFF)
set(XZ_NLS OFF)
set(XZ_TOOL_LZMADEC OFF)
set(XZ_TOOL_LZMAINFO OFF)
set(XZ_TOOL_XZ OFF)
set(XZ_TOOL_XZDEC OFF)

FetchContent_Declare(LibLZMA
  GIT_REPOSITORY    https://git.avmenergo.ru/avm-energo/xz-utils.git
  GIT_TAG           master
)

FetchContent_MakeAvailable(LibLZMA)

