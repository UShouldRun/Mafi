CFLAGS = -Wall -Werror -Wpedantic
INCLUDE = -I./include/

release: liblogger_release
	@echo "Logger Library compiled successfully in release mode"
debug: liblogger_debug
	@echo "Logger Library compiled successfully in debug mode"

liblogger_release: logger_out_release
	@echo "Producing logger library in release mode"
	@ar rcs build/liblogger.a build/logger.o

liblogger_debug: logger_out_debug
	@echo "Producing logger library in debug mode"
	@ar rcs build/liblogger.a build/logger.o

logger_out_release: src/logger.c
	@echo "Compiling into the output file"
	@gcc -c src/logger.c $(CFLAGS) -o build/logger.o $(INCLUDE)

logger_out_debug: src/logger.c
	@echo "Compiling into the output file"
	@gcc -c src/logger.c $(CFLAGS) -g -o build/logger.o $(INCLUDE)

clean:
	@rm build/logger.o build/liblogger.a
	@echo "Cleaned build directory"
