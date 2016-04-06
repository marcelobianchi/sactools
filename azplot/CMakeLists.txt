include_directories("../mblibsac")
include_directories("../mblibgr")
include_directories(".")

### Compile this one
add_executable(azplot main.c)
target_link_libraries(azplot mblibsac mblibgr)
install(TARGETS azplot DESTINATION bin)
