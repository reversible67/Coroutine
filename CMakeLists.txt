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
    duan_src/thread.cc
    duan_src/fiber.cc
    duan_src/scheduler.cc
    duan_src/timer.cc
    duan_src/iomanager.cc
    )

add_library(duan SHARED ${LIB_SRC})
#add_library(duan_static STATIC &{LIB_SRC})
#SET_TARGET_PROPERTIES (duan_static PROPERTIES OUTPUT_NAME "duan")

set(LIB_LIB
    duan
    pthread
    ${YAMLCPP}
    )

add_executable(test_log tests/test_log.cc)
add_dependencies(test_log duan)
#target_link_libraries(test_log duan -L/usr/local/lib64 -lyaml-cpp)
target_link_libraries(test_log ${LIB_LIB} -L/usr/local/lib64 -lyaml-cpp)

add_executable(test_config tests/test_config.cc)
add_dependencies(test_config duan)
#target_link_libraries(test_config duan -L/usr/local/lib64 -lyaml-cpp) 
target_link_libraries(test_config duan ${LIB_LIB} -L/usr/local/lib64 -lyaml-cpp)

add_executable(test_thread tests/test_thread.cc)
add_dependencies(test_thread duan)
#target_link_libraries(test_config duan -L/usr/local/lib64 -lyaml-cpp) 
target_link_libraries(test_thread duan ${LIB_LIB} -L/usr/local/lib64 -lyaml-cpp)

add_executable(test_util tests/test_util.cc)
add_dependencies(test_util duan)
#target_link_libraries(test_config duan -L/usr/local/lib64 -lyaml-cpp) 
target_link_libraries(test_util duan ${LIB_LIB} -L/usr/local/lib64 -lyaml-cpp)

add_executable(test_fiber tests/test_fiber.cc)
add_dependencies(test_fiber duan)
#target_link_libraries(test_config duan -L/usr/local/lib64 -lyaml-cpp) 
target_link_libraries(test_fiber duan ${LIB_LIB} -L/usr/local/lib64 -lyaml-cpp)

add_executable(test_scheduler tests/test_scheduler.cc)
add_dependencies(test_scheduler duan)
#target_link_libraries(test_config duan -L/usr/local/lib64 -lyaml-cpp) 
target_link_libraries(test_scheduler duan ${LIB_LIB} -L/usr/local/lib64 -lyaml-cpp)

add_executable(test_iomanager tests/test_iomanager.cc)
add_dependencies(test_iomanager duan)
#target_link_libraries(test_config duan -L/usr/local/lib64 -lyaml-cpp) 
target_link_libraries(test_iomanager duan ${LIB_LIB} -L/usr/local/lib64 -lyaml-cpp)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)