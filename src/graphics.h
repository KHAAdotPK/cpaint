/*
    graphics.h
 */

#ifndef CHARACTER_PAINTER_GRAPHICS_H
#define CHARACTER_PAINTER_GRAPHICS_H

/*
    DETAILS RELATED TO DEFAULT_CPAINT_BIT_DEPTH
    -------------------------------------------
    In WM_PAINT message, Ihave field of struct BITMAPINFO by the name of bmiHeader.biBitCount, the following detail is important
    from its point of view

    Common values for biBitCount are 1 (monochrome), 
    4 (16 colors), 8 (256 colors), 
    24 (true color), and 32 (true color with alpha channel). 
    The minimum valid value for biBitCount is 1, and the maximum value depends on the capabilities of the graphics device.
 */
/*#define     DEFAULT_CPAINT_BIT_DEPTH        4*/
#define     DEFAULT_CPAINT_BIT_DEPTH        3
#define     DEFAULT_CPAINT_WINDOW_HEIGHT    720
#define     DEFAULT_CPAINT_WINDOW_WIDTH     1424

#ifdef EXPORT_IMPORT
#undef EXPORT_IMPORT
#endif

#define EXPORT_IMPORT __declspec( dllexport )

//void draw_line(unsigned char*, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, PIXEL);

typedef struct Pixel { unsigned char b, g, r/*, a*/; } PIXEL;

#ifdef __cplusplus
#undef EXPORT_IMPORT
#define EXPORT_IMPORT __declspec( dllimport )

extern "C" {  // only need to export C interface if
              // used by C++ source code

EXPORT_IMPORT void draw_line(unsigned char*, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, PIXEL);
EXPORT_IMPORT void set_back_or_foreground_color(unsigned char*, unsigned long long, PIXEL);              

#ifdef __cplusplus
}
#endif             

#endif

#endif