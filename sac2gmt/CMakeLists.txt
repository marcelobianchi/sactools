include_directories("../mblibsac")
### Compile this one
add_executable(sac2gmt sac2gmt.c)
target_link_libraries(sac2gmt mblibsac)
install(TARGETS sac2gmt DESTINATION bin)
