SHELL=pwsh.exe
CC=ccache gcc
CFLAGS=-Wall -Wextra -std=gnu11 -Og -g3 -ggdb -march=native
CFLAGSREL=-Wall -Wextra -std=gnu11 -O3
LIBS=./lib/libraylib.a -lgdi32 -lwinmm
INC=-I. -I./include
SRCDIR=src/
SRC=main.c
FULLSRC=$(addprefix $(SRCDIR), $(SRC))
OUT=bin/nesxd.exe

all: $(OUT)

$(OUT): $(FULLSRC) $(SRCDIR)main.h.gch
	$(CC) -o $(OUT) $(FULLSRC) $(CFLAGS) $(INC) $(LIBS)

debug: $(FULLSRC) $(SRCDIR)main.h.gch
	$(CC) -o $(OUT) $(FULLSRC) $(CFLAGS) $(INC) $(LIBS) -DKXD_DEBUG

$(SRCDIR)main.h.gch: $(SRCDIR)main.h
	$(CC) -xc-header $(SRCDIR)main.h

rel: $(FULLSRC) $(SRCDIR)main.h.gch
	$(CC) -o $(OUT) $(FULLSRC) $(CFLAGSREL) $(INC) $(LIBS)

bundler: nob.c
	$(CC) -o bin/nob.exe bin/nob.c 

clean:
	remove-item * -Include *.exe, *.gch, *.log -Recurse -Exclude *.c, *.h
