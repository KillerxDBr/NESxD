#pragma once

#ifdef _WIN32

#define strcasecmp _stricmp
#define strdup     _strdup

#endif // _WIN32
