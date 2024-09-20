#define WIN32_LEAN_AND_MEAN
#define NOATOM
#define NOGDI
#define NOGDICAPMASKS
#define NOMETAFILE
#define NOMINMAX
#define NOMSG
#define NOOPENFILE
#define NORASTEROPS
#define NOSCROLL
#define NOSOUND
#define NOSYSMETRICS
#define NOTEXTMETRIC
#define NOWH
#define NOCOMM
#define NOKANJI
#define NOCRYPT
#define NOMCX
#define NOSERVICE
#define NOIME

#include <windows.h>
#include "WindowsHeader.h"

bool Win_SetConsoleOutputCP(uint32_t wCodePageID){
    return SetConsoleOutputCP(wCodePageID);
}
