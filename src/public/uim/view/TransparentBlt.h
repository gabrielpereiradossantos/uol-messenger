#pragma once

BOOL G_TransparentBlt(HDC hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest,
					  HDC hdcSrc,  int nXOriginSrc,  int nYOriginSrc,  int nWidthSrc,  int nHeightSrc,
					  UINT crTransparent);