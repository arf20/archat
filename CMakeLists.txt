cmake_minimum_required(VERSION 3.20)

project(arfchat)

file(GLOB COMMON_SRC "common/*.c")
file(GLOB CLIENT_SRC "arfchat/src/*.c")
file(GLOB DAEMON_SRC "arfchatd/src/*.c")

include_directories("common/")

add_executable(arfchat ${COMMON_SRC} ${CLIENT_SRC})
add_executable(arfchatd ${COMMON_SRC} ${DAEMON_SRC})
