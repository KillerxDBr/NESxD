SHELL=pwsh.exe
CC=ccache gcc -fdiagnostics-color=always
CFLAGS=-Wall -Wextra -std=gnu11 -Og -g3 -ggdb -march=native -DKXD_DEBUG -D_UNICODE -DUNICODE -DPLATFORM_DESKTOP -DGRAPHICS_API_OPENGL_33
CFLAGSREL=-Wall -Wextra -std=gnu11 -O3 -D_UNICODE -DUNICODE -DPLATFORM_DESKTOP -DGRAPHICS_API_OPENGL_33
LIBS=./lib/libraylib.a -lgdi32 -lwinmm
INC=-I. -I./include
SRCDIR=src/
SRC=main.c 6502.c
FULLSRC=$(addprefix $(SRCDIR), $(SRC))
OBJ=$(SRC:.c=.o)
OBJDIR=build/
OUT=bin/nesxd.exe
FULLOBJ=$(addprefix $(OBJDIR), $(OBJ))

all: $(OUT)

$(OUT): $(FULLOBJ) $(SRCDIR)main.h.gch
	$(CC) -o $(OUT) $(FULLOBJ) $(CFLAGS) $(INC) $(LIBS) -DKXD_DEBUG

$(SRCDIR)main.h.gch: $(SRCDIR)main.h
	$(CC) -xc-header $(SRCDIR)main.h

rel: $(FULLOBJ) $(SRCDIR)main.h.gch
	$(CC) -o $(OUT) $(FULLOBJ) $(CFLAGSREL) $(INC) $(LIBS)

$(OBJDIR)6502.o: $(SRCDIR)6502.c $(SRCDIR)6502.h
	$(CC) -o $(OBJDIR)6502.o -c $(SRCDIR)6502.c $(CFLAGS)

$(OBJDIR)main.o: $(SRCDIR)main.c $(SRCDIR)main.h
	$(CC) -o $(OBJDIR)main.o -c $(SRCDIR)main.c $(CFLAGS)

bundler: nob.c
	$(CC) -o bin/nob.exe bin/nob.c 

clean:
	remove-item * -Include *.exe, *.gch, *.log -Recurse -Exclude *.c, *.h
