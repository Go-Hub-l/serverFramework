cmake_minimum_required(VERSION 3.0)
project(sylar)

set(CMAKE_VERBOSE_MAKEFILE ON)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic -g -std=c++11 -Wall")
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 \
-ggdb -std=c++11 -Wall -Wno-deprecated -Werror \
-Wno-unused-function -lpthread -ldl \
-lssl -lcrypto -lmysqlclient -lmyJson \
-lhiredis -lfdfsclient -lfastcommon")
# SET(CMAKE_CXX_COMPILER "/usr/bin/g++")


link_directories(/usr/local/lib64)
link_directories(/usr/lib64/mysql)

find_library(YAMLCPP yaml-cpp)
find_library(PTHREAD pthread)
message("***", ${YAMLCPP})

set(LIB_SRC
    sylar/address.cc
    sylar/application.cc
    sylar/bytearray.cc
    sylar/config.cc
    sylar/common/base64.cc
    sylar/common/cfg.cc
    sylar/common/cJSON.cc
    sylar/common/deal_mysql.cc
    sylar/common/des.cc
    sylar/common/make_log.cc
    sylar/common/md5.cc
    sylar/common/redis_op.cc
    sylar/common/util_cgi.cc
    sylar/daemon.cc
    sylar/db/db.cc
    sylar/db/myMysql.cc
    sylar/db/user.cc
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
    sylar/http/ws_server.cc
    sylar/http/ws_servlet.cc
    sylar/http/ws_session.cc
    sylar/iomanager.cc
    sylar/jsoncpp.cpp
    sylar/library.cc
    sylar/log.cc
    sylar/mutex.cc
    sylar/module.cc
    sylar/module/chat_servlet.cc
    sylar/module/dealfile_servlet.cc
    sylar/module/dealsharefile_servlet.cc
    sylar/module/login_servlet.cc
    sylar/module/sharefiles_servlet.cc
    sylar/module/myfiles_servlet.cc
    sylar/module/reg_servlet.cc
    sylar/module/my_module.cc
    sylar/module/md5_servlet.cc
    sylar/module/protocol.cc
    sylar/module/resource_servlet.cc
    sylar/module/upload_servlet.cc
    sylar/protocol.cc
    sylar/scheduler.cc
    sylar/socket.cc
    sylar/stream.cc
    sylar/streams/socket_stream.cc
    sylar/tcp_server.cc
    sylar/timer.cc
    sylar/thread.cc
    sylar/uri.cc
    sylar/util.cc
    sylar/util/hash_util.cc
    sylar/util/json_util.cc
    sylar/worker.cc
    sylar/hook.cc
    )

include_directories(sylar/)
include_directories(/usr/include/c++/8/)
# include_directories(sylar/common)
include_directories(/usr/local/include)

add_library(sylar SHARED ${LIB_SRC})
#add_library(sylar_static STATIC ${LIB_SRC})
#SET_TARGET_PROPERTIES (sylar_static PROPERTIES OUTPUT_NAME "sylar")

add_executable(test1 tests/test.cc)
add_dependencies(test1 sylar)
target_link_libraries(test1 sylar ${YAMLCPP})

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

add_executable(mainDFS sylar/main.cc)
add_dependencies(mainDFS sylar)
target_link_libraries(mainDFS sylar ${PTHREAD} ${YAMLCPP})

add_executable(test_mysql tests/test_mysql.cc)
add_dependencies(test_mysql sylar)
target_link_libraries(test_mysql sylar ${PTHREAD} ${YAMLCPP})

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)


