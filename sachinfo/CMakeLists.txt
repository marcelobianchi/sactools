include_directories("../mblibsac")
### Compile this one
add_executable(sachinfo sachinfo.c)
target_link_libraries(sachinfo mblibsac)
install(TARGETS sachinfo DESTINATION bin)
