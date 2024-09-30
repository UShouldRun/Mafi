CC = gcc
CFLAGS = -Wall -Werror -Wpedantic -fmessage-length=0

INCLUDE_MAFI = -I./include/ -I./algo/include/ -I./libs/logger/include/ -I./libs/tree/include/
SRC_MAFI = src/main.c src/core.c src/utils.c
LIBS_MAFI = -lalgo -L./algo/build/ -llogger -L./libs/logger/build/ -ltree -L./libs/tree/build/

INCLUDE_DISPLAY = -I./display/include/ -I./algo/include/ -I./libs/error/include/
SRC_DISPLAY = display/src/main.c display/src/screen.c display/src/data.c
LIBS_DISPLAY = -lncurses -lalgo -L./algo/build/ -lerror -L./libs/error/build/

PREFIX ?= .
DEBUG ?=

BUILDDIR = ./build/
BINDIR = $(PREFIX)/bin/

.PHONY: build install clean uninstall mafi display install_mafi install_display uninstall_mafi uninstall_display

build: mafi display
install: install_mafi install_display
uninstall: uninstall_mafi uninstall_display

clean:
	@rm -f build/mafi.o build/display.o
	@echo "Successfully cleaned ./build/"

builddir:
	@if [ ! -d $(BUILDDIR) ]; then \
		mkdir -p $(BUILDDIR); \
		echo "Successfully created $(BUILDDIR)"; \
	fi

bindir:
	@if [ ! -d $(BINDIR) ]; then \
		mkdir -p $(BINDIR); \
		echo "Successfully created $(BINDIR)"; \
	fi
	
mafi: src/main.c | builddir
	@$(CC) $(CFLAGS) $(DEBUG) -o $(BUILDDIR)/mafi.o $(SRC_MAFI) $(INCLUDE_MAFI) $(LIBS_MAFI)
	@echo "Successfully compiled mafi to $(BUILDDIR)"

display: display/src/main.c | builddir
	@$(CC) $(CFLAGS) $(DEBUG) -o $(BUILDDIR)/display.o $(SRC_DISPLAY) $(INCLUDE_DISPLAY) $(LIBS_DISPLAY)
	@echo "Successfully compiled display to $(BUILDDIR)"

install_mafi: mafi
	@mkdir -p $(BINDIR)
	@if [ -f $(BINDIR)/mafi ]; then \
		echo "'mafi' is already installed in $(BINDIR)"; \
	else \
		install -m 0755 build/mafi.o $(BINDIR)/mafi; \
		echo "Successfully installed mafi in $(BINDIR)"; \
	fi

install_display: display
	@mkdir -p $(BINDIR)
	@install -m 0755 build/display.o $(BINDIR)/display
	@echo "Successfully installed display in $(BINDIR)"

uninstall_mafi:
	@rm -f $(BINDIR)/mafi
	@echo "Successfully uninstalled mafi from $(BINDIR)"

uninstall_display:
	@rm -f $(BINDIR)/display
	@echo "Successfully uninstalled display from $(BINDIR)"
