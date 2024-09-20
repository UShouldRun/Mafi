CFLAGS = -Wall -Werror -Wpedantic
INCLUDE = -I./include

release: libtree_release
	@echo "Tree Library compiled successfully in release mode"
debug: libtree_debug
	@echo "Tree Library compiled successfully in debug mode"

libtree_release: tree_out_release
	@echo "Producing tree library in release mode"
	@ar rcs build/libtree.a build/tree.o

libtree_debug: tree_out_debug
	@echo "Producing tree library in debug mode"
	@ar rcs build/libtree.a build/tree.o

tree_out_release: src/tree.c
	@echo "Compiling tree into output file"
	@gcc -c src/tree.c $(CFLAGS) -o build/tree.o $(INCLUDE)

tree_out_debug: src/tree.c
	@echo "Compiling tree into output file"
	@gcc -c src/tree.c $(CFLAGS) -g -o build/tree.o $(INCLUDE)

clean:
	@rm build/libtree.a build/tree.o
	@echo "Cleaned build directory"
