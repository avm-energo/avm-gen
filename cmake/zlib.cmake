include(FetchContent)

FetchContent_Declare(zlib
  GIT_REPOSITORY    https://github.com/madler/zlib.git
  GIT_TAG           master
)

set(ZLIB_LIBRARIES zlib)
FetchContent_MakeAvailable(zlib)
