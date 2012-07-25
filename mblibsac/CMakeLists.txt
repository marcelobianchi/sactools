cmake_minimum_required(VERSION 2.8)
include_directories(".")
add_library(mblibsac STATIC  head.c headutils.c iir.c sacio.c sactimeu.c yutils.c)
target_link_libraries(mblibsac m)
