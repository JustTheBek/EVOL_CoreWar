# generic UNIX makefile
CC = gcc			# req. for linux
#CC = cc				# if you don't have gcc
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

#CFLAGS = -O -DGRAPHX -DXWINGRAPHX -DPERMUTATE -DRWLIMIT -DEXT94 \
#         -I/opt/homebrew/include -I/opt/homebrew/include/X11/
CFLAGS = -O -DPERMUTATE -DRWLIMIT -DEXT94 \
         -I/opt/homebrew/include -I/opt/homebrew/include/X11/
LFLAGS = -x
INCDIR = /opt/homebrew/include/X11/
# LIB = -lcurses -ltermlib		# enable this one for curses display
# LIB = -lvgagl -lvga			# enable this one for Linux/SVGA
LIB = -L/opt/homebrew/lib -lX11		# enable this one for X11

# Build directory
BUILD_DIR = build

# Ensure build directory exists
$(shell mkdir -p $(BUILD_DIR))

.SUFFIXES: .o .c .c~ .man .doc .6
MAINFILE = $(BUILD_DIR)/pmars

HEADER = global.h config.h asm.h sim.h 

# Object files go into build/
OBJ1 = $(BUILD_DIR)/pmars.o $(BUILD_DIR)/asm.o $(BUILD_DIR)/eval.o $(BUILD_DIR)/disasm.o $(BUILD_DIR)/cdb.o $(BUILD_DIR)/sim.o $(BUILD_DIR)/pos.o
OBJ2 = $(BUILD_DIR)/clparse.o $(BUILD_DIR)/global.o $(BUILD_DIR)/token.o
OBJ3 = $(BUILD_DIR)/str_eng.o $(BUILD_DIR)/sighandler.o

all: flags $(MAINFILE)

flags:
	@echo Making $(MAINFILE) with compiler flags $(CFLAGS)

$(MAINFILE): $(OBJ1) $(OBJ2) $(OBJ3)
	@echo Linking $(MAINFILE)
	@$(CC) -o $(MAINFILE) $(OBJ1) $(OBJ2) $(OBJ3) $(LIB)
	@strip $(MAINFILE)
	@echo done


# Dependencies for object files
$(BUILD_DIR)/sighandler.o: sighandler.c
$(BUILD_DIR)/token.o $(BUILD_DIR)/asm.o $(BUILD_DIR)/disasm.o: asm.h
$(BUILD_DIR)/sim.o $(BUILD_DIR)/cdb.o $(BUILD_DIR)/pos.o $(BUILD_DIR)/disasm.o: sim.h

# Sources that have multiple .c dependencies
$(BUILD_DIR)/sim.o: curdisp.c uidisp.c lnxdisp.c xwindisp.c

# Header files for individual sources
$(BUILD_DIR)/xwindisp.o: xwindisp.h pmarsicn.h
$(BUILD_DIR)/lnxdisp.o: lnxdisp.h

# General dependencies for all objects
$(OBJ1) $(OBJ2) $(OBJ3): Makefile config.h global.h

# Compile .c -> .o into build/
$(BUILD_DIR)/%.o: %.c
	@echo Compiling $< -> $@
	@$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -rf $(BUILD_DIR)

