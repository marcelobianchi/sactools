cmake_minimum_required(VERSION 2.8)
project(sactools)

## Options
option(GraphicTools "Compile graphical utilities (azplot, sacinspect + mblibgr) -- Depends on pgplot" ON)

## Libraries
add_subdirectory(mblibsac)

if (GraphicTools)
	add_subdirectory(mblibgr)
endif(GraphicTools)

## Programs
add_subdirectory(sac2gmt)
add_subdirectory(sachinfo)
add_subdirectory(sacchhdr)
add_subdirectory(sacrename)
add_subdirectory(sacrgap)
add_subdirectory(sacrms)
add_subdirectory(sacpickmax)

if (GraphicTools)
	add_subdirectory(azplot)
	add_subdirectory(sacinspect)
endif(GraphicTools)
