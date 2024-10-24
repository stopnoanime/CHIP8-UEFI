#ifndef COMMON
#define COMMON

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define NULL ((void *)0)
#define output(s) SystemTable->ConOut->OutputString(SystemTable->ConOut, s);

#endif