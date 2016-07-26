#include "stdafx.h"
#include "GDIPlus_Image.h"

CGDIPlus_Image::CGDIPlus_Image()
{
    m_pImage = NULL;
    /// --Init Gdiplus
    GdiplusStartupInput GdiplusStart;
    GdiplusStartup(&m_dwToken, &GdiplusStart, NULL );
}

CGDIPlus_Image::~CGDIPlus_Image()
{
    delete m_pImage;
    GdiplusShutdown(m_dwToken);
}

IStream * CGDIPlus_Image::GetStreamFromRes(UINT uResourceID, HMODULE hModuleDll, LPCTSTR szResType)
{
    AfxSetResourceHandle(hModuleDll);
    HRSRC hSource = FindResource(hModuleDll, MAKEINTRESOURCE(uResourceID), szResType);
    ASSERT(hSource);
    HGLOBAL hGlobal = LoadResource(hModuleDll, hSource);
    ASSERT(hGlobal);
    LPVOID lpVoid = LockResource(hGlobal);
    ASSERT(lpVoid);

    int nSize =(UINT)SizeofResource(hModuleDll, hSource);
    HGLOBAL hGlobal2 = GlobalAlloc(GMEM_MOVEABLE, nSize);

    void* pData = GlobalLock(hGlobal2);
    memcpy(pData, (void *)hGlobal, nSize);
    GlobalUnlock(hGlobal2);

    IStream* pStream = NULL;

    if(S_OK == CreateStreamOnHGlobal(hGlobal2, TRUE, &pStream))
    {
        FreeResource(hGlobal2);
        AfxSetResourceHandle(GetModuleHandle(NULL));
        return pStream;
    }
    ASSERT(FALSE);
    return NULL;
}

bool CGDIPlus_Image::LoadFromStream(IStream * pStream)
{
    if (!IsNull())
    {
        delete m_pImage;
        m_pImage = NULL;
    }
    m_pImage = Gdiplus::Image::FromStream(pStream);
    if (!IsNull())
    {
        m_nWidth = m_pImage->GetWidth();
        m_nHeight = m_pImage->GetHeight();
        return true;
    }
    return false;
}

bool CGDIPlus_Image::LoadFromFile(const WCHAR * pwzFilePath)
{
    if (!IsNull())
    {
        delete m_pImage;
        m_pImage = NULL;
    }
    m_pImage = Gdiplus::Image::FromFile(pwzFilePath);
    if (!IsNull())
    {
        m_nWidth = m_pImage->GetWidth();
        m_nHeight = m_pImage->GetHeight();
        return true;
    }
    return false;
}

bool CGDIPlus_Image::IsNull()
{
    if (NULL == m_pImage) return true;
    if (Ok != m_pImage->GetLastStatus())
    {
        ///ASSERT(FALSE);
        delete m_pImage;
        m_pImage = NULL;
        return true;
    }
    return false;
}

BOOL CGDIPlus_Image::DrawImage(CDC * pDC, POINT * pPoint, int srcx, int srcy, int srcwidth, int srcheight, float * pfClrAttr)
{
    ASSERT(NULL != m_pImage);
    if (NULL == m_pImage) return 1;

    if (IsNull()) return 2;

    Graphics graph(pDC->m_hDC);
    Gdiplus::Point points[3];

    memset(points, 0, sizeof (points));
    if (NULL == pPoint)
    {
        points[1].X = m_nWidth;
        points[2].Y = m_nHeight;
    }
    else
    {
        for (BYTE i = 0;i < 3;i++)
        {
            points[i].X = pPoint[i].x;
            points[i].Y = pPoint[i].y;
        }
    }
    if ((0 == srcwidth)||(0 == srcheight))
    {
        srcwidth = m_nWidth;
        srcheight = m_nHeight;
    }
    if (NULL == pfClrAttr)
    {
        graph.DrawImage(m_pImage, points, 3, srcx, srcy, srcwidth, srcheight, UnitPixel);
    }
    else
    {
#ifdef _DEBUG
        for (BYTE i = 0;i < 25;i++)
        {
            ASSERT(0 <= pfClrAttr[i]);
        }
#endif
        ColorMatrix colorMatrix;
        memcpy(&colorMatrix, pfClrAttr, sizeof (colorMatrix));

        ImageAttributes imageAttr;
        imageAttr.SetColorMatrix(&colorMatrix);

        graph.DrawImage(m_pImage, points, 3, srcx, srcy, srcwidth, srcheight, UnitPixel, &imageAttr);
    }

    return TRUE;
}

BOOL CGDIPlus_Image::DrawImage(CDC * pDC, int xDest, int yDest, int nDestWidth, int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, float fAlpha)
{
    if ((0 == m_nWidth)||(0 == m_nHeight)||(0 == nDestWidth)||(0 == nDestHeight)||(0 == nSrcWidth)||(0 == nSrcHeight))
    {
        ASSERT(FALSE);
        return FALSE;
    }

    POINT ptShow[3];
    ptShow[0].x = xDest;
    ptShow[0].y = yDest;
    ptShow[1].x = nDestWidth + xDest;
    ptShow[1].y = yDest;
    ptShow[2].x = xDest;
    ptShow[2].y = nDestHeight + yDest;
    if (-1 == fAlpha)
    {
        return DrawImage(pDC, ptShow, xSrc, ySrc, nSrcWidth, nSrcHeight);
    }
    else
    {
        ASSERT(0 <=fAlpha);
        float fColorArray[25] =
        {
            1, 0, 0, 0, 0,
            0, 1, 0, 0, 0,
            0, 0, 1, 0, 0,
            0, 0, 0, fAlpha, 0,
            0, 0, 0, 0, 1
        };
        return DrawImage(pDC, ptShow, xSrc, ySrc, nSrcWidth, nSrcHeight, fColorArray);
    }
}

BOOL CGDIPlus_Image::DrawImage(CDC * pDC, int xDest, int yDest, float fAlpha)
{
    return DrawImage(pDC, xDest, yDest, m_nWidth, m_nHeight, 0, 0, m_nWidth, m_nHeight, fAlpha);
}













