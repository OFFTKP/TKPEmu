cmake_minimum_required(VERSION 3.19)
project(TKPLib)
set(FILES md5.cpp messagequeue.cxx)
add_library(TKPLib ${FILES})
target_include_directories(TKPLib PUBLIC ../)
