set(<third_lib>_FOUND TRUE)
set(<third_lib>_ROOT_DIR ${CMAKE_SOURCE_DIR}/../../third/<third_lib>)

find_path(<third_lib>_INCLUDE_DIR NAMES <third_lib>.hpp PATHS ${<third_lib>_ROOT_DIR}/include)

set(<third_lib>_INCLUDE_DIR ${<third_lib>_INCLUDE_DIR})