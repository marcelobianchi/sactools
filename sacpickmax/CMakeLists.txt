include_directories("../mblibsac")
### Compile this one
add_executable(sacpickmax sacpickmax.c)
target_link_libraries(sacpickmax mblibsac)
install(TARGETS sacpickmax DESTINATION bin)
