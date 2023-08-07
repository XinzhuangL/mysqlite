cmake_minimum_required(VERSION 3.0)
project(mysqlite C)

set(CMAKE_C_STANDARD 11)

add_library(external_file
        input.h
        input.c
        command.c
        command.h
        statement.h
        statement.c
#        table.c
#        table.h
        table.h
        table.c
        tree_node.h
        tree_node.c)

add_executable(mysqlite main.c)

target_link_libraries(mysqlite external_file)
