cmake_minimum_required(VERSION 2.8)
project(sactools)

## Options
option(X11Tools "Compile graphical utilities -- Depends on pgplot")


## Libraries
add_subdirectory(mblibsac)

if (X11Tools)
	add_subdirectory(mblibgr)
endif(X11Tools)

## Programs
add_subdirectory(sac2gmt)
add_subdirectory(sachinfo)
add_subdirectory(sacchhdr)
add_subdirectory(sacrename)
add_subdirectory(sacrgap)
add_subdirectory(sacrms)
add_subdirectory(sacpickmax)
if (X11Tools)
	add_subdirectory(sacinspect)
endif(X11Tools)
