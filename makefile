### This makefile is managed by pdantler.py
### Any changes made directly will be overwritten

PYTHON_VENV=~/projects/project_venv/bin/activate
PYTHON3_BIN=/usr/bin/python3
PDANTLER_SCRIPT=~/projects/pdantler/pdantler.py

export PLAYDATE_SDK_PATH = /home/mchristensen/src/PlaydateSDK-2.2.0

HEAP_SIZE      = 8388208
STACK_SIZE     = 61800

PRODUCT = Pecklers.pdx

# Locate the SDK
SDK = ${PLAYDATE_SDK_PATH}
ifeq ($(SDK),)
	SDK = $(shell egrep '^\s*SDKRoot' ~/.Playdate/config | head -n 1 | cut -c9-)
endif

ifeq ($(SDK),)
$(error SDK path not found; set ENV value PLAYDATE_SDK_PATH)
endif

######
# IMPORTANT: You must add your source folders to VPATH for make to find them
# ex: VPATH += src1:src2
######

VPATH += src

# List C source files here
SRC = src/pdantler/main.c src/pdantler/lifecycle.c src/pdantler/utils.c src/pdantler/pdlogger.c src/pdantler/printf.c src/pdantler/mooselib.c src/scenes/scene_splash.c src/scenes/scene_credits.c src/scenes/scene_game.c

# List all user directories here
UINCDIR = 

# List user asm files
UASRC = 

# List all user C define here, like -D_DEBUG=1
UDEFS = 

# Define ASM defines here
UADEFS = 

# List the user directory to look for the libraries here
ULIBDIR =

# List all user libraries here
ULIBS =

include $(SDK)/C_API/buildsupport/common.mk

### Code generatoin
pdantler: pdantler.json
	@echo "Running pdantler"
	bash -c "source $(PYTHON_VENV); python3 $(PDANTLER_SCRIPT) pdantler.json"

pdantler-force: FORCE
	@echo "Running pdantler"
	bash -c "source $(PYTHON_VENV); python3 $(PDANTLER_SCRIPT) pdantler.json"


### Testing

build-tests: pdantler tests/all_tests.c
# -g is to add debug info so valgrind can use it
	@gcc -g  tests/munit.c tests/all_tests.c  -lm -o tests/all_tests.o

test:	build-tests
	@tests/all_tests.o --color never --log-visible debug

test-with-valgrind: build-tests
	valgrind --track-origins=yes tests/all_tests.o

### Custom build steps
format: FORCE
	@echo "Running clang-format"
	find src/ -iname *.h -o -iname *.c | xargs clang-format -i

generate-build-number: FORCE
	@echo "Generating build number"	
	$(PYTHON3_BIN) tools/generate_build_number.py src/build.h

build: pdantler format generate-build-number
	@echo "Building game"	
	make clean; make
 
force-build: pdantler-force format generate-build-number
	@echo "Building game"	
	make clean; make
FORCE:
