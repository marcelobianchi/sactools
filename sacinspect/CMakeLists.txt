include_directories("../mblibsac")
include_directories("../mblibgr")
include_directories(".")
### Compile this one
add_executable(sacinspect EX.c PK.c aux.c edit_tf.c io.c sacinspect.c collector.c globers.c HS.c MAP.c)
target_link_libraries(sacinspect mblibsac mblibgr)
install(TARGETS sacinspect DESTINATION bin)

