cmake_minimum_required(VERSION 3.20)

project(arfchat)

file(GLOB SRC "src/*.c")

add_executable(arfchat ${SRC})
