include_directories("../mblibsac")
include_directories(".")
### Compile this one
add_executable(sacrename diskwork.c filelist.c sacrename.c)
target_link_libraries(sacrename mblibsac)
install(TARGETS sacrename DESTINATION bin)
