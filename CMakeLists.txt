cmake_minimum_required(VERSION 3.1)
project(sqlogger)
set(CMAKE_CXX_STANDARD 11)

set(SQLogger_SRC src/sqlogger.cpp src/sqlite/sqlite3.c)

option(ENABLE_TESTING "Enables unit tests. They are built using Google Testing Framework." true)
option(BUILD_EXAMPLES "Enables build of example programs supplied in source code." true)

include_directories(include)
include_directories(src)

add_library(${PROJECT_NAME} STATIC ${SQLogger_SRC})
target_link_libraries(${PROJECT_NAME} dl pthread)

add_library(${PROJECT_NAME}_so SHARED ${SQLogger_SRC})
target_link_libraries(${PROJECT_NAME}_so dl pthread)

if(BUILD_EXAMPLES)
  add_executable(example1 src/example1.cpp)
  link_directories(${CMAKE_BINARY_DIR})
  target_link_libraries(example1 ${PROJECT_NAME})
endif(BUILD_EXAMPLES)

if(ENABLE_TESTING)
  add_subdirectory(tests)
endif(ENABLE_TESTING)

