SHELL=pwsh.exe
CC=ccache gcc -fdiagnostics-color=always
CFLAGS=-Wall -Wextra -std=gnu11 -Og -g3 -ggdb -march=native -DKXD_DEBUG -D_UNICODE -DUNICODE -DPLATFORM_DESKTOP -DGRAPHICS_API_OPENGL_33
CFLAGSREL=-Wall -Wextra -std=gnu11 -O3 -D_UNICODE -DUNICODE -DPLATFORM_DESKTOP -DGRAPHICS_API_OPENGL_33 -mwindows
LIBS=./lib/libraylib.a -lgdi32 -lwinmm -lcomdlg32 -lole32
INC=-I. -Iinclude -Isrc -Istyles -Iextern
SRCDIR=src/
SRC=main.c 6502.c input.c config.c
FULLSRC=$(addprefix $(SRCDIR), $(SRC))
OBJ=$(SRC:.c=.o)
OBJDIR=build/
OUT=bin/nesxd.exe
FULLOBJ=$(addprefix $(OBJDIR), $(OBJ))

all: $(OUT)

# Program
$(OUT): $(FULLOBJ) $(OBJDIR)main.h.gch $(OBJDIR)6502.h.gch $(OBJDIR)input.h.gch $(OBJDIR)tinyfiledialogs.o $(OBJDIR)resource.o
	$(CC) $(INC) -o $(OUT) $(FULLOBJ) $(OBJDIR)tinyfiledialogs.o $(OBJDIR)resource.o $(CFLAGS) $(LIBS) -DKXD_DEBUG

rel: $(FULLOBJ) $(SRCDIR)main.h.gch
	$(CC) $(INC) -o $(OUT) $(FULLOBJ) $(CFLAGSREL) $(INC) $(LIBS)

# Precompiled Headers
$(OBJDIR)main.h.gch: $(SRCDIR)main.h
	$(CC) -xc-header $(INC) -o $(OBJDIR)main.h.gch $(SRCDIR)main.h

$(OBJDIR)6502.h.gch: $(SRCDIR)6502.h
	$(CC) -xc-header $(INC) -o $(OBJDIR)6502.h.gch $(SRCDIR)6502.h

$(OBJDIR)input.h.gch: $(SRCDIR)input.h
	$(CC) -xc-header $(INC) -o $(OBJDIR)input.h.gch $(SRCDIR)input.h

$(OBJDIR)config.h.gch: $(SRCDIR)config.h
	$(CC) -xc-header $(INC) -o $(OBJDIR)config.h.gch $(SRCDIR)config.h

# Object files
$(OBJDIR)6502.o: $(SRCDIR)6502.c $(OBJDIR)6502.h.gch
	$(CC) $(INC) -o $(OBJDIR)6502.o -c $(SRCDIR)6502.c $(CFLAGS) -DNES

$(OBJDIR)main.o: $(SRCDIR)main.c $(OBJDIR)main.h.gch
	$(CC) $(INC) -o $(OBJDIR)main.o -c $(SRCDIR)main.c $(CFLAGS)

$(OBJDIR)input.o: $(SRCDIR)input.c $(OBJDIR)input.h.gch
	$(CC) $(INC) -o $(OBJDIR)input.o -c $(SRCDIR)input.c $(CFLAGS)

$(OBJDIR)config.o: $(SRCDIR)config.c $(OBJDIR)config.h.gch
	$(CC) $(INC) -o $(OBJDIR)config.o -c $(SRCDIR)config.c $(CFLAGS)

$(OBJDIR)tinyfiledialogs.o: extern/tinyfiledialogs/tinyfiledialogs.c extern/tinyfiledialogs/tinyfiledialogs.h
	$(CC) $(INC) -o $(OBJDIR)tinyfiledialogs.o -c extern/tinyfiledialogs/tinyfiledialogs.c

# Resources
$(OBJDIR)resource.o: resource.rc manifest.xml
	windres -i resource.rc -o $(OBJDIR)resource.o

# Bundler
bundler: nob.c
	$(CC) -o bin/nob.exe bin/nob.c 

clean:
	remove-item * -Include *.exe, *.gch, *.log, *.o, *.ini -Recurse -Exclude *.c, *.h, *.py
