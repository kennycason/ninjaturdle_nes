#ifndef NESLIB_WRAPPER_H
#define NESLIB_WRAPPER_H

// Save the original NULL definition
#ifdef NULL
#define ORIGINAL_NULL NULL
#undef NULL
#endif

// Include neslib.h
#include "../neslib/neslib.h"

// Restore the original NULL definition if it existed
#ifdef ORIGINAL_NULL
#undef NULL
#define NULL ORIGINAL_NULL
#endif

#endif // NESLIB_WRAPPER_H 