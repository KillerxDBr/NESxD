SHELL=pwsh.exe
CC=ccache gcc -fdiagnostics-color=always
CFLAGS=-Wall -Wextra -Winvalid-pch -std=gnu11 -Og -g3 -ggdb -march=native -DKXD_DEBUG -D_UNICODE -DUNICODE -DPLATFORM_DESKTOP -DGRAPHICS_API_OPENGL_33
CFLAGSREL=-Wall -Wextra -std=gnu11 -O3 -D_UNICODE -DUNICODE -DPLATFORM_DESKTOP -DGRAPHICS_API_OPENGL_33 -mwindows
LIBS=./lib/libraylib.a -lgdi32 -lwinmm -lcomdlg32 -lole32
INC=-I$(OBJDIR) -I. -Iinclude -Isrc -Istyles -Iextern
SRCDIR=src/
SRC=main.c 6502.c input.c config.c gui.c
FULLSRC=$(addprefix $(SRCDIR), $(SRC))
OBJ=$(SRC:.c=.o) nob.o tinyfiledialogs.o resource.o
OBJDIR=build/
OUT=bin/nesxd.exe
FULLOBJ=$(addprefix $(OBJDIR), $(OBJ))


all: $(OUT)

# Program
$(OUT): $(FULLOBJ)
	$(CC) $(INC) -o $(OUT) $(FULLOBJ) $(CFLAGS) $(LIBS) -DKXD_DEBUG

rel: $(FULLOBJ)
	$(CC) $(INC) -o $(OUT) $(FULLOBJ) $(OBJDIR)tinyfiledialogs.o $(OBJDIR)resource.o $(CFLAGSREL) $(LIBS)

# Precompiled Headers
$(OBJDIR)mainprecomp.h.gch: $(SRCDIR)main.h
	$(CC) -xc-header $(INC) -o $(OBJDIR)mainprecomp.h.gch $(SRCDIR)main.h $(CFLAGS)

$(OBJDIR)6502precomp.h.gch: $(SRCDIR)6502.h
	$(CC) -xc-header $(INC) -o $(OBJDIR)6502precomp.h.gch $(SRCDIR)6502.h $(CFLAGS)

$(OBJDIR)inputprecomp.h.gch: $(SRCDIR)input.h
	$(CC) -xc-header $(INC) -o $(OBJDIR)inputprecomp.h.gch $(SRCDIR)input.h $(CFLAGS)

$(OBJDIR)configprecomp.h.gch: $(SRCDIR)config.h
	$(CC) -xc-header $(INC) -o $(OBJDIR)configprecomp.h.gch $(SRCDIR)config.h $(CFLAGS)

$(OBJDIR)guiprecomp.h.gch: $(SRCDIR)gui.h
	$(CC) -xc-header $(INC) -o $(OBJDIR)guiprecomp.h.gch $(SRCDIR)gui.h $(CFLAGS)
	

# Object files
$(OBJDIR)6502.o: $(SRCDIR)6502.c $(OBJDIR)6502precomp.h.gch
	$(CC) -include $(OBJDIR)6502precomp.h $(INC) -o $(OBJDIR)6502.o -c $(SRCDIR)6502.c $(CFLAGS) -DNES

$(OBJDIR)main.o: $(SRCDIR)main.c $(OBJDIR)mainprecomp.h.gch
	$(CC) -include $(OBJDIR)mainprecomp.h $(INC) -o $(OBJDIR)main.o -c $(SRCDIR)main.c $(CFLAGS) -H

$(OBJDIR)input.o: $(SRCDIR)input.c $(OBJDIR)inputprecomp.h.gch
	$(CC) -include $(OBJDIR)inputprecomp.h $(INC) -o $(OBJDIR)input.o -c $(SRCDIR)input.c $(CFLAGS)

$(OBJDIR)config.o: $(SRCDIR)config.c $(OBJDIR)configprecomp.h.gch
	$(CC) -include $(OBJDIR)configprecomp.h $(INC) -o $(OBJDIR)config.o -c $(SRCDIR)config.c $(CFLAGS)

$(OBJDIR)gui.o: $(SRCDIR)gui.c $(OBJDIR)guiprecomp.h.gch
	$(CC) -include $(OBJDIR)guiprecomp.h $(INC) -o $(OBJDIR)gui.o -c $(SRCDIR)gui.c $(CFLAGS)

$(OBJDIR)tinyfiledialogs.o: extern/tinyfiledialogs/tinyfiledialogs.c extern/tinyfiledialogs/tinyfiledialogs.h
	$(CC) $(INC) -o $(OBJDIR)tinyfiledialogs.o -c extern/tinyfiledialogs/tinyfiledialogs.c

$(OBJDIR)nob.o: ./include/nob.h
	$(CC) -xc -o $(OBJDIR)nob.o -c ./include/nob.h -DNOB_IMPLEMENTATION

# Resources
$(OBJDIR)resource.o: resource.rc manifest.xml
	windres -i resource.rc -o $(OBJDIR)resource.o

# Bundler
bundler: nob.c
	$(CC) -o bin/nob.exe bin/nob.c 

clean:
	remove-item -WhatIf * -Include *.exe, *.gch, *.log, *.o, *.ini -Recurse -Exclude *.c, *.h, *.py
