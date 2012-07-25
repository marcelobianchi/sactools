include_directories("../mblibsac")
### Compile this one
add_executable(sacchhdr sacchhdr.c)
target_link_libraries(sacchhdr mblibsac)
install(TARGETS sacchhdr DESTINATION bin)
