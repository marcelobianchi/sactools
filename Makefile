all:
	@echo ""
	@echo "You need cmake / ccmake and optionally pgplot to compile this tool."
	@echo ""
	mkdir -p build
	cd build && ccmake ../
	@echo ""
	@echo "If you see no errors just enter the folder 'build'"
	@echo "and type in 'make' to compile or 'ccmake .' to configure"
	@echo "note that you need X11 libraries, Pgplot libraries to compile"
	@echo "the x11tool and library."
	@echo "To install it just type in 'make install'"
