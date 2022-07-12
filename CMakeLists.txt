cmake_minimum_required(VERSION 3.0)
project(sylar)

set(CMAKE_VERBOSE_MAKEFILE ON)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic -g -std=c++11 -Wall")
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -lpthread -ldl -lssl -lcrypto")

link_directories(/usr/local/lib64)

find_library(YAMLCPP yaml-cpp)
find_library(PTHREAD pthread)
message("***", ${YAMLCPP})

set(LIB_SRC
    sylar/address.cc
    sylar/bytearray.cc
    sylar/config.cc
    sylar/daemon.cc
    sylar/env.cc
    sylar/fiber.cc
    sylar/fd_manager.cc
    sylar/http/http.cc
    sylar/http/http11_parser.rl.cc
    sylar/http/httpclient_parser.rl.cc
    sylar/http/http_connection.cc
    sylar/http/http_parser.cc
    sylar/http/http_session.cc
    sylar/http/http_server.cc
    sylar/http/servlet.cc
    sylar/iomanager.cc
    sylar/log.cc
    sylar/mutex.cc
    sylar/scheduler.cc
    sylar/socket.cc
    sylar/stream.cc
    sylar/streams/socket_stream.cc
    sylar/tcp_server.cc
    sylar/timer.cc
    sylar/thread.cc
    sylar/uri.cc
    sylar/util.cc
    sylar/hook.cc
    )

include_directories(sylar/)
include_directories(/usr/local/include)

add_library(sylar SHARED ${LIB_SRC})
#add_library(sylar_static STATIC ${LIB_SRC})
#SET_TARGET_PROPERTIES (sylar_static PROPERTIES OUTPUT_NAME "sylar")

add_executable(test tests/test.cc)
add_dependencies(test sylar)
target_link_libraries(test sylar ${YAMLCPP})

add_executable(test_config tests/test_config.cc)
add_dependencies(test_config sylar)
target_link_libraries(test_config sylar ${YAMLCPP})

add_executable(test_thread tests/test_thread.cc)
add_dependencies(test_thread sylar)
target_link_libraries(test_thread sylar ${PTHREAD} ${YAMLCPP})

add_executable(test_fiber tests/test_fiber.cc)
add_dependencies(test_fiber sylar)
target_link_libraries(test_fiber sylar ${PTHREAD} ${YAMLCPP})

add_executable(test_scheduler tests/test_scheduler.cc)
add_dependencies(test_scheduler sylar)
target_link_libraries(test_scheduler sylar ${PTHREAD} ${YAMLCPP})

add_executable(test_iomanager tests/test_iomanager.cc)
add_dependencies(test_iomanager sylar)
target_link_libraries(test_iomanager sylar ${PTHREAD} ${YAMLCPP})

add_executable(test_hook tests/test_hook.cc)
add_dependencies(test_hook sylar)
target_link_libraries(test_hook sylar ${PTHREAD} ${YAMLCPP})

add_executable(test_scheduler2 tests/test_scheduler2.cc)
add_dependencies(test_scheduler2 sylar)
target_link_libraries(test_scheduler2 sylar ${PTHREAD} ${YAMLCPP})

add_executable(test_address tests/test_address.cc)
add_dependencies(test_address sylar)
target_link_libraries(test_address sylar ${PTHREAD} ${YAMLCPP})

add_executable(test_socket tests/test_socket.cc)
add_dependencies(test_socket sylar)
target_link_libraries(test_socket sylar ${PTHREAD} ${YAMLCPP})

add_executable(test_bytearray tests/test_bytearray.cc)
add_dependencies(test_bytearray sylar)
target_link_libraries(test_bytearray sylar ${PTHREAD} ${YAMLCPP})

add_executable(test_http tests/test_http.cc)
add_dependencies(test_http sylar)
target_link_libraries(test_http sylar ${PTHREAD} ${YAMLCPP})

add_executable(test_http_parser tests/test_http_parser.cc)
add_dependencies(test_http_parser sylar)
target_link_libraries(test_http_parser sylar ${PTHREAD} ${YAMLCPP})

add_executable(test_tcp_server tests/test_tcp_server.cc)
add_dependencies(test_tcp_server sylar)
target_link_libraries(test_tcp_server sylar ${PTHREAD} ${YAMLCPP})

add_executable(echo_server examples/echo_server.cc)
add_dependencies(echo_server sylar)
target_link_libraries(echo_server sylar ${PTHREAD} ${YAMLCPP})

add_executable(test_http_server tests/test_http_server.cc)
add_dependencies(test_http_server sylar)
target_link_libraries(test_http_server sylar ${PTHREAD} ${YAMLCPP})

add_executable(test_http_connection tests/test_http_connection.cc)
add_dependencies(test_http_connection sylar)
target_link_libraries(test_http_connection sylar ${PTHREAD} ${YAMLCPP})

add_executable(test_uri tests/test_uri.cc)
add_dependencies(test_uri sylar)
target_link_libraries(test_uri sylar ${PTHREAD} ${YAMLCPP})

add_executable(my_http_server samples/my_http_server.cc)
add_dependencies(my_http_server sylar)
target_link_libraries(my_http_server sylar ${PTHREAD} ${YAMLCPP})

add_executable(test_daemon tests/test_daemon.cc)
add_dependencies(test_daemon sylar)
target_link_libraries(test_daemon sylar ${PTHREAD} ${YAMLCPP})

add_executable(test_env tests/test_env.cc)
add_dependencies(test_env sylar)
target_link_libraries(test_env sylar ${PTHREAD} ${YAMLCPP})

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)


