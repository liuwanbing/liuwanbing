#ifndef GDIPLUSIMAGE_HEADER
#define GDIPLUSIMAGE_HEADER

#pragma once
#include <gdiplus.h>
using namespace Gdiplus;  
#pragma comment(lib, "gdiplus.lib")

class CGDIPlus_Image
{
public:
	CGDIPlus_Image();
	virtual ~CGDIPlus_Image();

protected:
    Image                           * m_pImage;
    DWORD                           m_dwToken;
    int                             m_nWidth;
    int                             m_nHeight;
public:
    IStream * GetStreamFromRes(UINT uResourceID, HMODULE hModuleDll, LPCTSTR szResType = TEXT("PNG"));
    bool LoadFromStream(IStream * pStream);
    bool LoadFromFile(const WCHAR * wzFilePath);
public:
    BOOL DrawImage(CDC * pDC, POINT * pPoint = NULL, int srcx = NULL, int srcy = NULL, int srcwidth = NULL, int srcheight = NULL, float * pfClrAttr = NULL);
    BOOL DrawImage(CDC * pDC, int xDest, int yDest, int nDestWidth, int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, float fAlpha = -1.0);
    BOOL DrawImage(CDC * pDC, int xDest, int yDest, float fAlpha = -1.0);
public:
    bool IsNull();
    int GetWidth() { return m_nWidth; }
    int GetHeight() { return m_nHeight; }
};
























#endif