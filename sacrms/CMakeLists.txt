include_directories("../mblibsac")
### Compile this one
add_executable(sacrms sacrms.c)
target_link_libraries(sacrms mblibsac)
install(TARGETS sacrms DESTINATION bin)
