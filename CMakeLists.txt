cmake_minimum_required(VERSION 2.8)
project(duan)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -g -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")

include_directories(.)
include_directories(/usr/local/include/yaml-cpp)
include_directories(/usr/local/lib64)

find_library(YAMLCPP libyaml-cpp.a)

set(LIB_SRC
    duan_src/log.cc
    duan_src/util.cc
    duan_src/config.cc
    )

add_library(duan SHARED ${LIB_SRC})
#add_library(duan_static STATIC &{LIB_SRC})
#SET_TARGET_PROPERTIES (duan_static PROPERTIES OUTPUT_NAME "duan")

add_executable(test_log tests/test_log.cc)
add_dependencies(test_log duan)
target_link_libraries(test_log duan)

add_executable(test_config tests/test_config.cc)
add_dependencies(test_config duan)
target_link_libraries(test_config duan -L/usr/local/lib64 -lyaml-cpp) 

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)