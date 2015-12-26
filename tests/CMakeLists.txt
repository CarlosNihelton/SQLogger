cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)
#add_subdirectory(/home/cnihelton/Development/PC/googletest/googletest)

set(Core_SRC ../src/sqlogger.cpp ../src/sqlite/sqlite3.c)
set(teste1_SRC  test1.cpp)

include_directories(/home/cnihelton/Development/PC/googletest/googletest/include)
link_directories(/home/cnihelton/Development/PC/googletest/googletest/build)
include_directories(../include)
include_directories(../src)

add_executable(teste1 ${teste1_SRC} ${Core_SRC})
target_link_libraries(teste1 gtest pthread dl)
target_compile_features(teste1 PRIVATE cxx_range_for)
