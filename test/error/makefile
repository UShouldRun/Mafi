CFLAGS = -Wall -Werror -Wpedantic
INCLUDE = -I./include/

release: liberror_release
	@echo "Error Library compiled successfully in release mode"
debug: liberror_debug
	@echo "Error Library compiled successfully in debug mode"

liberror_release: error_out_release
	@echo "Producing Error Library in release mode"
	@ar rcs build/liberror.a build/error.o

liberror_debug: error_out_debug
	@echo "Producing Error Library in debug mode"
	@ar rcs build/liberror.a build/error.o

error_out_release: src/error.c
	@echo "Compiling src file"
	@gcc -c src/error.c $(CFLAGS) -o build/error.o $(INCLUDE)

error_out_debug: src/error.c
	@echo "Compiling src file"
	@gcc -c src/error.c $(CFLAGS) -g -o build/error.o $(INCLUDE)

clean:
	@rm build/*
	@echo "Cleaned build directory"
