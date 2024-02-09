/*
    src/png.h
 */

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>

#include "graphics.h"
#include "zlib.h"

#ifndef CHARACTER_PAINTER_PNG_H
#define CHARACTER_PAINTER_PNG_H

#define PNG_SIGNATURE_SIZE  8
#define PNG_CHUNK_IDHR_LENGTH 0x0D

typedef struct deflated_data
{
    unsigned long size;
    unsigned char* data;

}DEFLATED_DATA;

typedef struct deflated_data* DEFLATED_DATA_PTR;

typedef struct chunk 
{
    unsigned char length[4];
    unsigned char type[4];
    unsigned char* data;
    unsigned char crc[4];

    struct chunk* prev;
    struct chunk* next;
}PNG_CHUNK;
typedef PNG_CHUNK* PNG_CHUNK_PTR;

typedef struct png
{    
    unsigned char signature[PNG_SIGNATURE_SIZE];
    unsigned int n; // Number of chunks
    PNG_CHUNK_PTR ptr; // Pointer to first chunk    
}PNG;
typedef PNG* PNG_PTR;

#ifdef EXPORT_IMPORT
#undef EXPORT_IMPORT
#endif

#define EXPORT_IMPORT __declspec( dllexport )

#ifdef __cplusplus
#undef EXPORT_IMPORT
#define EXPORT_IMPORT __declspec( dllimport )

extern "C" {  // only need to export C interface if
              // used by C++ source code

EXPORT_IMPORT void png_init(const unsigned char*, size_t, size_t, unsigned char, wchar_t*);       

#ifdef __cplusplus
}
#endif             

#endif

#define REAL_COLOR_BLACK 0x000000
#define REAL_COLOR_WHITE 0xFFFFFF
#define REAL_COLOR_RED   0xFF0000
#define REAL_COLOT_GREEN 0x00FF00
#define REAL_COLOR_BLUE  0x0000FF

#define PLTE_BLACK_INDEX 0x00;
#define PLTE_WHITE_INDEX 0x01;
#define PLTE_RED_INDEX   0x02;
#define PLTE_GREEN_INDEX 0x03;
#define PLTE_BLUE_INDEX  0x04;

#endif