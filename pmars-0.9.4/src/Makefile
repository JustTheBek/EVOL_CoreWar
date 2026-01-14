#---------------------------------------------------------------------------------------
#
# Updated Makefile allowing to build both the GUI and the Terminal version of pMars.
# Author: JustTheBek
# Date: 2026.01.14
#
#---------------------------------------------------------------------------------------

# generic UNIX makefile
CC = gcc			# req. for linux

# Configuration options:
#
# No.   Name            Incompatible with   Description
# (1)   -DSERVER        2                   disables cdb debugger (koth server 
#                                           version)
# (2)   -DGRAPHX        1                   enables platform specific core 
#                                           graphics
# (3)   -DKEYPRESS                          only for curses display on SysV:
#                                           enter cdb upon keypress (use if
#                                           Ctrl-C doesn't work)
# (4)   -DEXT94                             ICWS'94 + SEQ,SNE,NOP,*,{,}
# (5)   -DSMALLMEM                          16-bit addresses, less memory
# (6)   -DXWINGRAPHX    1                   X-Windows graphics (UNIX)
# (7)   -DPERMUTATE                         enables -P switch
# (9)   -DRWLIMIT                           enables read/write limits


# Base configuration options
CFLAGS = -O2 -Wall -Wextra -DPERMUTATE -DRWLIMIT -DEXT94

# Linker flags
LIB =
# LIB = -lcurses -ltermlib		# enable this one for curses display
# LIB = -lvgagl -lvga			# enable this one for Linux/SVGA


# Build directories
BUILD_DIR = build
GUI_DIR   = gui_build

# Ensure build directories exist
$(shell mkdir -p $(BUILD_DIR))
$(shell mkdir -p $(GUI_DIR))

.SUFFIXES: .o .c .c~ .man .doc .6
MAINFILE = $(BUILD_DIR)/pmars
GUIFILE  = $(GUI_DIR)/pmars_gui

HEADER = global.h config.h asm.h sim.h 

# Object files for normal build
OBJ1 = $(BUILD_DIR)/pmars.o $(BUILD_DIR)/asm.o $(BUILD_DIR)/eval.o $(BUILD_DIR)/disasm.o $(BUILD_DIR)/cdb.o $(BUILD_DIR)/sim.o $(BUILD_DIR)/pos.o
OBJ2 = $(BUILD_DIR)/clparse.o $(BUILD_DIR)/global.o $(BUILD_DIR)/token.o
OBJ3 = $(BUILD_DIR)/str_eng.o #$(BUILD_DIR)/sighandler.o

all: flags $(MAINFILE)

flags:
	@echo Making $(MAINFILE) with compiler flags $(CFLAGS)

$(MAINFILE): $(OBJ1) $(OBJ2) $(OBJ3)
	@echo Linking $(MAINFILE)
	@$(CC) -o $(MAINFILE) $(OBJ1) $(OBJ2) $(OBJ3) $(LIB)
	@strip $(MAINFILE)
	@echo done

# GUI target
gui: gui_flags $(GUIFILE)

gui_flags:
	@echo Building GUI version in $(GUI_DIR)
	$(eval CFLAGS := $(CFLAGS) -DGRAPHX -DXWINGRAPHX)
	$(eval LIB := -lX11)

# GUI object files (same sources, different build folder)
GUI_OBJ1 = $(GUI_DIR)/pmars.o $(GUI_DIR)/asm.o $(GUI_DIR)/eval.o $(GUI_DIR)/disasm.o $(GUI_DIR)/cdb.o $(GUI_DIR)/sim.o $(GUI_DIR)/pos.o
GUI_OBJ2 = $(GUI_DIR)/clparse.o $(GUI_DIR)/global.o $(GUI_DIR)/token.o
GUI_OBJ3 = $(GUI_DIR)/str_eng.o #$(BUILD_DIR)/sighandler.o

$(GUIFILE): $(GUI_OBJ1) $(GUI_OBJ2) $(GUI_OBJ3)
	@echo Linking GUI $(GUIFILE)
	@$(CC) -o $(GUIFILE) $(GUI_OBJ1) $(GUI_OBJ2) $(GUI_OBJ3) $(LIB)
	@strip $(GUIFILE)
	@echo done

# Compile .c -> .o into build/ or gui_build/
$(BUILD_DIR)/%.o: %.c
	@echo Compiling $< -> $@
	@$(CC) $(CFLAGS) -c $< -o $@

$(GUI_DIR)/%.o: %.c
	@echo Compiling GUI $< -> $@
	@$(CC) $(CFLAGS) -c $< -o $@

# Dependencies for object files
$(BUILD_DIR)/sighandler.o: sighandler.c
$(BUILD_DIR)/token.o $(BUILD_DIR)/asm.o $(BUILD_DIR)/disasm.o: asm.h
$(BUILD_DIR)/sim.o $(BUILD_DIR)/cdb.o $(BUILD_DIR)/pos.o $(BUILD_DIR)/disasm.o: sim.h

# Sources that have multiple .c dependencies
$(BUILD_DIR)/sim.o: curdisp.c uidisp.c lnxdisp.c xwindisp.c

# Header files for individual sources
$(BUILD_DIR)/xwindisp.o: xwindisp.h pmarsicn.h
$(BUILD_DIR)/lnxdisp.o: lnxdisp.h

# GUI dependencies
$(GUI_DIR)/xwindisp.o: xwindisp.h pmarsicn.h
$(GUI_DIR)/lnxdisp.o: lnxdisp.h

# General dependencies for all objects
$(OBJ1) $(OBJ2) $(OBJ3): Makefile config.h global.h
$(GUI_OBJ1) $(GUI_OBJ2) $(GUI_OBJ3): Makefile config.h global.h

clean:
	rm -rf $(BUILD_DIR) $(GUI_DIR)
