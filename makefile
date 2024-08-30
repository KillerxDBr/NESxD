CC=ccache gcc
CFLAGS=-Wall -Wextra -std=gnu11 -Og -g3 -ggdb -march=native
CFLAGSREL=-Wall -Wextra -std=gnu11 -O3
LIBS=./raylib-5.0_win64_mingw-w64/lib/libraylib.a -lgdi32 -lwinmm
INC=-I. -I./raylib-5.0_win64_mingw-w64/include
SRCDIR=src/
SRC=main.c
FULLSRC=$(addprefix $(SRCDIR), $(SRC))


all: $(FULLSRC) $(SRCDIR)main.h.gch
	$(CC) -o nesxd.exe $(FULLSRC) $(CFLAGS) $(INC) $(LIBS)

debug: $(FULLSRC) $(SRCDIR)main.h.gch
	$(CC) -o nesxd.exe $(FULLSRC) $(CFLAGS) $(INC) $(LIBS) -DKXD_DEBUG

$(SRCDIR)main.h.gch: $(SRCDIR)main.h
	$(CC) -xc-header $(SRCDIR)main.h

rel: $(FULLSRC) $(SRCDIR)main.h.gch
	$(CC) -o nesxd.exe $(FULLSRC) $(CFLAGSREL) $(INC) $(LIBS)

bundler: nob.c
	$(CC) -o nob.exe nob.c 

clean:
	del *.exe .\src\*.gch .\src\*.exe
