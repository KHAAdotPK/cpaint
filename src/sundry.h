/*
    src/sundry.h
 */

#include <stdlib.h>

#include "graphics.h"
#include "png.h"

#ifndef CHARACTER_PAINTER_SUNDRY_H
#define CHARACTER_PAINTER_SUNDRY_H

#define EDGE_BOUND(a1, a2) ((a1 - a2)/2)

/*
#define     DEFAULT_CPAINT_BIT_DEPTH        4
#define     DEFAULT_CPAINT_WINDOW_HEIGHT    720
#define     DEFAULT_CPAINT_WINDOW_WIDTH     1424
 */

/*
#define EXPORT_IMPORT __declspec( dllexport )

//typedef struct Pixel { unsigned char b, g, r, a; } PIXEL;
*/
#ifdef __cplusplus
#undef EXPORT_IMPORT
#define EXPORT_IMPORT __declspec( dllimport )

extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif


/* SUNDRY */
//EXPORT_IMPORT void set_back_or_foreground_color(unsigned char*, unsigned long long, PIXEL);

/* GRAPHICS */
//EXPORT_IMPORT void draw_line(unsigned char*, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, PIXEL);

EXPORT_IMPORT void displayMessage(void);

#ifdef __cplusplus
}
#endif


EXPORT_IMPORT void foo(void);
//EXPORT_IMPORT void displayMessage(void);

#endif
