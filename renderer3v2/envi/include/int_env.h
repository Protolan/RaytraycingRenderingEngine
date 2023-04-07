#if defined _WIN32
#include "int_win.h"
#elif defined __linux
#include "int_lin.h"
#else
#error Unsupported platform
#endif
