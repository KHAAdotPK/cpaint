/*
    src/header.h
 */

#ifndef UNICODE
#define UNICODE
#endif

#include <iostream>
#include <windows.h>

#include "mousetrackevents.hh"
#include "resource\resource.h"
#include "sundry.h"

//#include "mousetrackevents.hh"

#ifndef CHARACTER_PAINTER_MAIN_H
#define CHARACTER_PAINTER_MAIN_H

/*
#define     DEFAULT_CPAINT_BIT_DEPTH        4
#define     DEFAULT_CPAINT_WINDOW_HEIGHT    720
#define     DEFAULT_CPAINT_WINDOW_WIDTH     1424
 */

/*
#include "resource\resource.h"
#include "sundry.h"

#include "mousetrackevents.hh"
*/

PIXEL defaultForegroundPixelColor = {0xFF, 0xFF, 0xFF/*, 0x00*/};

#define CPAINT_DEFAULT_FILE_NAME ".\\assets\\image.png"   

#define CalcStride(w, bpp) ( ((((w) * (bpp)) + 31) & ~31) >> 3 )

//struct Pixel { unsigned char b, g, r, a; };

#ifdef __cplusplus
extern "C" {
#endif    

    //void rust_function(void);
    
#ifdef __cplusplus    
}
#endif

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif
