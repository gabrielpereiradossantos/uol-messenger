#include "stdafx.h"
#include "TransparentBlt.h"
#include ".\ddbmask.h"

BOOL G_TransparentBlt(HDC hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest,
					  HDC hdcSrc,  int nXOriginSrc,  int nYOriginSrc,  int nWidthSrc,  int nHeightSrc,
					  UINT crTransparent)
{
	CDDBMask mask;

	mask.Create(hdcSrc, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc, crTransparent);

	return mask.TransBlt(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest,
		hdcSrc, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc);
}
