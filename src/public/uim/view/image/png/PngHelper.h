//------------------------------------------
//  PNGFILE.H -- Header File for pngfile.c
//------------------------------------------

// Copyright 2000, Willem van Schaik.  For conditions of distribution and
// use, see the copyright/license/disclaimer notice in png.h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include <lpng1216/png.h>

#ifdef __cplusplus
extern "C" {
#endif

BOOL PngLoadImage (LPBYTE pBuffer, int nSize, png_byte **ppbImageData, 
                   int *piWidth, int *piHeight, int *piChannels, png_color *pBkgColor);

BOOL PngFreeImage();
//
//BOOL PngSaveImage (PTSTR pstrFileName, png_byte *pDiData,
//                   int iWidth, int iHeight, png_color BkgColor);

#ifdef __cplusplus
 }
#endif


static void png_read_data(png_structp png_ptr, png_bytep data, png_size_t length);
//static void png_write_data(png_structp png_ptr, png_bytep data, png_size_t length);
static void png_flush(png_structp png_ptr);


