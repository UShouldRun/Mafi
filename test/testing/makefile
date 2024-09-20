CFLAGS = -Wall -Werror -Wpedantic
INCLUDE = -I./include/

release: libtesting_release
	@echo "Testing Library compiled sucessfully in release mode"
debug: libtesting_debug
	@echo "Testing Library compiled sucessfully in debug mode"

libtesting_release: testing_out_release
	@echo "Producing testing library in release mode"
	@ar rcs build/libtesting.a build/testing.o

libtesting_debug: testing_out_debug
	@echo "Producing testing library in debug mode"
	@ar rcs build/libtesting.a build/testing.o

testing_out_release: src/testing.c
	@echo "Compiling into output file"
	@gcc -c src/testing.c $(CFLAGS) -o build/testing.o $(INCLUDE)

testing_out_debug: src/testing.c
	@echo "Compiling into output file"
	@gcc -c src/testing.c $(CFLAGS) -g -o build/testing.o $(INCLUDE)

clean:
	@rm build/libtesting.a build/testing.o
	@echo "Cleaned build directory"
