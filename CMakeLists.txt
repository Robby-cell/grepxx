cmake_minimum_required(VERSION 3.15)
project(grepxx CXX)

add_executable(grepxx
    src/grepxx.cc
    src/grepxx.h

    src/main.cc
    
    src/options.cc
    src/options.h
    
    src/terminal.cc
    src/terminal.h)
set_target_properties(grepxx
    PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF)
