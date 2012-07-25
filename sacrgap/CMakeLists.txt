include_directories("../mblibsac")
### Compile this one
add_executable(sacrgap sacrgap.c)
target_link_libraries(sacrgap mblibsac)
install(TARGETS sacrgap DESTINATION bin)
