cmake_minimum_required(VERSION 2.8)
project(duan)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -g -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")

set(LIB_SRC
    duan_src/log.cc
    duan_src/util.cc
    )

add_library(duan SHARED ${LIB_SRC})
#add_library(duan_static STATIC &{LIB_SRC})
#SET_TARGET_PROPERTIES (duan_static PROPERTIES OUTPUT_NAME "duan")

add_executable(test tests/test.cc)
add_dependencies(test duan)
target_link_libraries(test duan)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)