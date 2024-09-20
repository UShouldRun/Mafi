CFLAGS = -Wall -Werror -Wpedantic
INCLUDE_ALGO = -I./include -I./../libs/logger/include/ -I./../libs/tree/include/
INCLUDE_TYPE = -I./include/
LIBS = -llogger -L./../libs/logger/build/ -ltree -L./../libs/tree/build/

release: libalgo_release
	@echo "Algo Library compiled successfully in release mode"
debug: libalgo_debug
	@echo "Algo Library compiled successfully in debug mode"

libalgo_release: algo_out_release type_out_release
	@echo "Producing algo library in release mode"
	@ar rcs build/libalgo.a build/algo.o build/type.o	

libalgo_debug: algo_out_debug type_out_debug
	@echo "Producing algo library in debug mode"
	@ar rcs build/libalgo.a build/algo.o build/type.o	

algo_out_release: src/algo.c 
	@echo "Compiling src/algo.c into the output file"
	@gcc -c src/algo.c $(CFLAGS) -o build/algo.o $(INCLUDE_ALGO) $(LIBS)

type_out_release: src/type.c
	@echo "Compiling src/type.c into the output file"
	@gcc -c src/type.c $(CFLAGS) -o build/type.o $(INCLUDE_TYPE)

algo_out_debug: src/algo.c 
	@echo "Compiling src/algo.c into the output file"
	@gcc -c src/algo.c $(CFLAGS) -g -o build/algo.o $(INCLUDE_ALGO) $(LIBS)

type_out_debug: src/type.c
	@echo "Compiling src/type.c into the output file"
	@gcc -c src/type.c $(CFLAGS) -g -o build/type.o $(INCLUDE_TYPE)

clean:
	@echo "Cleaning build directory"
	@rm build/libalgo.a build/algo.o build/type.o
