cmake_minimum_required(VERSION 2.8)
find_package(X11)

include_directories(".")
add_library(mblibgr STATIC  basic.c ctl.c interaction.c cfg.c)
target_link_libraries(mblibgr ${X11_LIBRARIES} cpgplot)
