#include "stdafx.h"
#include "TransparentStatic.h"


BEGIN_MESSAGE_MAP(CTransparentStaticT, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/**********************************************************************************************************/

/**
 * 构造函数
 */
CTransparentStaticT::CTransparentStaticT()
: m_StartX(0),m_StartY(0),m_isRgn(true),FontWidth(0),m_IsMulitList(false)
{
	m_clrFontColor = RGB(0,0,0);
	m_bDCStored = false;
	m_hSnapType = DT_CENTER;

	m_Font.CreateFont(-12,0,0,0,400,0,0,0,134,3,2,1,2,TEXT("新宋体"));
}

/**
 * 析构函数
 */
CTransparentStaticT::~CTransparentStaticT()
{
	m_Font.DeleteObject();
	m_memDC.DeleteDC();
}

/**
 * 设置字体显示颜色
 *
 * @param clrFont 要设置的颜色值
 */
void CTransparentStaticT::SetTextColor(COLORREF clrFont)
{
	m_clrFontColor = clrFont;
}

/**
 * 清空背景图
 */
void CTransparentStaticT::ClearBackground()
{
	m_memDC.DeleteDC();
    m_bDCStored = false;

	CRect m_Rect;
	GetClientRect(&m_Rect);
    
	InvalidateRect(m_Rect);
}

/**
 * 设置标签显示文字
 *
 * @param pText 要显示的文字
 */
void CTransparentStaticT::SetMyText(std::string pText) 
{ 
	m_Text = pText; 

	Invalidate();

	//if(GetParent()) {
	//	CRect m_Rect;
	//	GetClientRect(&m_Rect);
	//	GetParent()->InvalidateRect(CRect(m_StartX,m_StartY,m_StartX+m_Rect.right,m_StartY+m_Rect.bottom));
	//}

	// 拆分多行
	if(m_IsMulitList)
		SplitStringToMulitList();

	m_isRgn = true;
}

/**
 * 设置标签显示数字
 *
 * @param pNum 要显示的数字
 */
void CTransparentStaticT::SetMyNumber(int pNum)
{
	char str[64];
	sprintf(str,"%d",pNum);

	m_Text = str;

	Invalidate();

	//if(GetParent()) {
	//	CRect m_Rect;
	//	GetClientRect(&m_Rect);
	//	GetParent()->InvalidateRect(CRect(m_StartX,m_StartY,m_StartX+m_Rect.right,m_StartY+m_Rect.bottom));
	//}

	m_isRgn = true;
}

/** 
 * 拆分字符串以便于多行显示
 */
void CTransparentStaticT::SplitStringToMulitList(void)
{
	if(m_Text.empty() || !m_IsMulitList) return;

	m_TextList.clear();

	CRect	rect;
	GetClientRect(&rect);

	if(m_Font.GetSafeHandle())
	{
		LOGFONT myLogFont;
		m_Font.GetLogFont(&myLogFont);
		FontWidth = abs(myLogFont.lfHeight);
	}

	int fontcount = rect.Width()/FontWidth;
	std::string myText = m_Text;

	while(!myText.empty() && fontcount >= 4)
	{
		if((int)myText.length()/2 >= fontcount) 
		{
			std::string temp = myText.substr(0,fontcount*2-2);
	
			int Count = GetChineseCharCount(temp,(int)temp.length()-1);

			if(Count % 2 != 0)
			{
				myText = myText.substr(temp.length()-1,myText.length());
				temp = temp.substr(0,temp.length()-1);	
			}
			else
			{
				myText = myText.substr(temp.length(),myText.length());
			}

			m_TextList.push_back(temp);
		}
		else 
		{
			m_TextList.push_back(myText);
			myText.clear();
		}
	}
}

/**
 * 标签的绘制
 */
void CTransparentStaticT::OnPaint()
{
	CPaintDC dc(this); 

	CRect	rect;
	GetClientRect(&rect);

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);

	CBitmap Screen;
	Screen.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	memDC.SelectObject(&Screen);
	Screen.DeleteObject();

	if(m_Font.GetSafeHandle())
	{
		memDC.SelectObject(m_Font);

		LOGFONT myLogFont;
		m_Font.GetLogFont(&myLogFont);
		FontWidth = abs(myLogFont.lfHeight);
	}

	memDC.SetTextColor(m_clrFontColor);

	//画背景 
	memDC.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &m_memDC, 0, 0, SRCCOPY);

	memDC.SetBkMode(TRANSPARENT);

	if(!m_Text.empty()) 
	{
		int fontcount = rect.Width()/FontWidth;

		if((int)m_Text.length()/2 >= fontcount) 
		{
			// 单行显示
			if(!m_IsMulitList)
			{
				std::string temp = m_Text.substr(0,fontcount*2-6);

				int Count = GetChineseCharCount(temp,(int)temp.length()-1);

				if(Count % 2 != 0)
					temp = temp.substr(0,temp.length()-1);

				temp += "...";
				memDC.DrawText(temp.c_str(), rect,m_hSnapType);
			}
			else                         // 多行显示
			{
				if(!m_TextList.empty())
				{
					for(int i=0;i<(int)m_TextList.size();i++)
					{
						CRect myRect = CRect(rect.left,rect.top+i*FontWidth,rect.right,rect.top+i*FontWidth+FontWidth);

						memDC.DrawText(m_TextList[i].c_str(),myRect,0);
					}
				}
			}
		}
		else 
		{
			memDC.DrawText(m_Text.c_str(), rect,m_hSnapType);
		}
	}

	//创建不规则窗体
	if(m_isRgn) 
	{
		HRGN hRgn = CreateRgnFromFile(*(memDC.GetCurrentBitmap()),RGB(255,0,255));
		SetWindowRgn( hRgn, TRUE );
		DeleteObject(hRgn);
		m_isRgn = false;
	}

	dc.TransparentBlt(rect.left, rect.top, rect.Width(), rect.Height(), &memDC, rect.left,rect.top,rect.Width(),rect.Height(),RGB(255,0,255));
	//dc.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);

	memDC.DeleteDC();
}

/**
 * 计算从指定位置开始的中文字符的个数
 *
 * @param str 要计算的字符串
 * @param startpos 开始计算的位置
 *
 * @return 返回中文字符的个数
 */
int CTransparentStaticT::GetChineseCharCount(std::string str,int startpos)
{
	if(str.empty() || startpos > (int)str.length()) return 0;
	
	int count = 0;

	for(int i=startpos;i>=0;i--)
	{
		if(!(str.at(i) & 0x80)) break;
		else 
		{
			count += 1;
		}
	}

	return count;
}

BOOL CTransparentStaticT::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(!m_bDCStored)
	{
    	CRect  rect;
	    GetClientRect(&rect);

		m_memDC.DeleteDC();
		m_memDC.CreateCompatibleDC (pDC);
		CBitmap	btScreen;
		btScreen.CreateCompatibleBitmap (pDC,rect.Width(),rect.Height());
		m_memDC.SelectObject (&btScreen);

		m_memDC.FillSolidRect(&rect,RGB(255,0,255));

		//if(m_Text.empty())
		//{
		//	m_memDC.BitBlt (0,0,rect.Width(),rect.Height(),pDC,0,0,SRCCOPY);

		//	CRgn rgn;
		//	rgn.CreateRectRgn (0, 0, rect.Width(), rect.Height());
		//	SetWindowRgn (rgn, TRUE);
		//}

		m_bDCStored=true;

		btScreen.DeleteObject();
	}

	return TRUE;// CButton::OnEraseBkgnd(pDC);//
}


HRGN CTransparentStaticT::CreateRgnFromFile( HBITMAP hBmp, COLORREF color )
{
	// get image properties
	BITMAP bmp = { 0 };
	GetObject( hBmp, sizeof(BITMAP), &bmp );
	// allocate memory for extended image information
	LPBITMAPINFO bi = (LPBITMAPINFO) new BYTE[ sizeof(BITMAPINFO) + 8 ];
	memset( bi, 0, sizeof(BITMAPINFO) + 8 );
	bi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	// set window size
	//m_dwWidth	= bmp.bmWidth;		// bitmap width
	//m_dwHeight	= bmp.bmHeight;		// bitmap height
	// create temporary dc
	HDC dc = CreateIC( "DISPLAY",NULL,NULL,NULL );
	// get extended information about image (length, compression, length of color table if exist, ...)
	DWORD res = GetDIBits( dc, hBmp, 0, bmp.bmHeight, 0, bi, DIB_RGB_COLORS );
	// allocate memory for image data (colors)
	LPBYTE pBits = new BYTE[ bi->bmiHeader.biSizeImage + 4 ];
	// allocate memory for color table
	if ( bi->bmiHeader.biBitCount == 8 )
	{
		// actually color table should be appended to this header(BITMAPINFO),
		// so we have to reallocate and copy it
		LPBITMAPINFO old_bi = bi;
		// 255 - because there is one in BITMAPINFOHEADER
		bi = (LPBITMAPINFO)new char[ sizeof(BITMAPINFO) + 255 * sizeof(RGBQUAD) ];
		memcpy( bi, old_bi, sizeof(BITMAPINFO) );
		// release old header
		delete old_bi;
	}
	// get bitmap info header
	BITMAPINFOHEADER& bih = bi->bmiHeader;
	// get color table (for 256 color mode contains 256 entries of RGBQUAD(=DWORD))
	LPDWORD clr_tbl = (LPDWORD)&bi->bmiColors;
	// fill bits buffer
	res = GetDIBits( dc, hBmp, 0, bih.biHeight, pBits, bi, DIB_RGB_COLORS );
	DeleteDC( dc );

	BITMAP bm;
	GetObject( hBmp, sizeof(BITMAP), &bm );
	// shift bits and byte per pixel (for comparing colors)
	LPBYTE pClr = (LPBYTE)&color;
	// swap red and blue components
	BYTE tmp = pClr[0]; pClr[0] = pClr[2]; pClr[2] = tmp;
	// convert color if curent DC is 16-bit (5:6:5) or 15-bit (5:5:5)
	if ( bih.biBitCount == 16 )
	{
		// for 16 bit
		color = ((DWORD)(pClr[0] & 0xf8) >> 3) |
				((DWORD)(pClr[1] & 0xfc) << 3) |
				((DWORD)(pClr[2] & 0xf8) << 8);
		// for 15 bit
//		color = ((DWORD)(pClr[0] & 0xf8) >> 3) |
//				((DWORD)(pClr[1] & 0xf8) << 2) |
//				((DWORD)(pClr[2] & 0xf8) << 7);
	}

	const DWORD RGNDATAHEADER_SIZE	= sizeof(RGNDATAHEADER);
	const DWORD ADD_RECTS_COUNT		= 40;			// number of rects to be appended
													// to region data buffer

	// BitPerPixel
	BYTE	Bpp = bih.biBitCount >> 3;				// bytes per pixel
	// bytes per line in pBits is DWORD aligned and bmp.bmWidthBytes is WORD aligned
	// so, both of them not
	DWORD m_dwAlignedWidthBytes = (bmp.bmWidthBytes & ~0x3) + (!!(bmp.bmWidthBytes & 0x3) << 2);
	// DIB image is flipped that's why we scan it from the last line
	LPBYTE	pColor = pBits + (bih.biHeight - 1) * m_dwAlignedWidthBytes;
	DWORD	dwLineBackLen = m_dwAlignedWidthBytes + bih.biWidth * Bpp;	// offset of previous scan line
													// (after processing of current)
	DWORD	dwRectsCount = bih.biHeight;			// number of rects in allocated buffer
	INT		i, j;									// current position in mask image
	INT		first = 0;								// left position of current scan line
													// where mask was found
	bool	wasfirst = false;						// set when mask has been found in current scan line
	bool	ismask;									// set when current color is mask color

	// allocate memory for region data
	// region data here is set of regions that are rectangles with height 1 pixel (scan line)
	// that's why first allocation is <bm.biHeight> RECTs - number of scan lines in image
	RGNDATAHEADER* pRgnData = 
		(RGNDATAHEADER*)new BYTE[ RGNDATAHEADER_SIZE + dwRectsCount * sizeof(RECT) ];
	// get pointer to RECT table
	LPRECT pRects = (LPRECT)((LPBYTE)pRgnData + RGNDATAHEADER_SIZE);
	// zero region data header memory (header  part only)
	memset( pRgnData, 0, RGNDATAHEADER_SIZE + dwRectsCount * sizeof(RECT) );
	// fill it by default
	pRgnData->dwSize	= RGNDATAHEADER_SIZE;
	pRgnData->iType		= RDH_RECTANGLES;

	for ( i = 0; i < bih.biHeight; i++ )
	{
		for ( j = 0; j < bih.biWidth; j++ )
		{
			// get color
			switch ( bih.biBitCount )
			{
			case 8:
				ismask = (clr_tbl[ *pColor ] != color);
				break;
			case 16:
				ismask = (*(LPWORD)pColor != (WORD)color);
				break;
			case 24:
				ismask = ((*(LPDWORD)pColor & 0x00ffffff) != color);
				break;
			case 32:
				ismask = (*(LPDWORD)pColor != color);
			}
			// shift pointer to next color
			pColor += Bpp;
			// place part of scan line as RECT region if transparent color found after mask color or
			// mask color found at the end of mask image
			if ( wasfirst )
			{
				if ( !ismask )
				{
					// save current RECT
					pRects[ pRgnData->nCount++ ] = CRect( first, i, j, i + 1 );
					// if buffer full reallocate it with more room
					if ( pRgnData->nCount >= dwRectsCount )
					{
						dwRectsCount += ADD_RECTS_COUNT;
						// allocate new buffer
						LPBYTE pRgnDataNew = new BYTE[ RGNDATAHEADER_SIZE + dwRectsCount * sizeof(RECT) ];
						// copy current region data to it
						memcpy( pRgnDataNew, pRgnData, RGNDATAHEADER_SIZE + pRgnData->nCount * sizeof(RECT) );
						// delte old region data buffer
						delete pRgnData;
						// set pointer to new regiondata buffer to current
						pRgnData = (RGNDATAHEADER*)pRgnDataNew;
						// correct pointer to RECT table
						pRects = (LPRECT)((LPBYTE)pRgnData + RGNDATAHEADER_SIZE);
					}
					wasfirst = false;
				}
			}
			else if ( ismask )		// set wasfirst when mask is found
			{
				first = j;
				wasfirst = true;
			}
		}

		if ( wasfirst && ismask )
		{
			// save current RECT
			pRects[ pRgnData->nCount++ ] = CRect( first, i, j, i + 1 );
			// if buffer full reallocate it with more room
			if ( pRgnData->nCount >= dwRectsCount )
			{
				dwRectsCount += ADD_RECTS_COUNT;
				// allocate new buffer
				LPBYTE pRgnDataNew = new BYTE[ RGNDATAHEADER_SIZE + dwRectsCount * sizeof(RECT) ];
				// copy current region data to it
				memcpy( pRgnDataNew, pRgnData, RGNDATAHEADER_SIZE + pRgnData->nCount * sizeof(RECT) );
				// delte old region data buffer
				delete pRgnData;
				// set pointer to new regiondata buffer to current
				pRgnData = (RGNDATAHEADER*)pRgnDataNew;
				// correct pointer to RECT table
				pRects = (LPRECT)((LPBYTE)pRgnData + RGNDATAHEADER_SIZE);
			}
			wasfirst = false;
		}

		pColor -= dwLineBackLen;
	}
	// release image data
	delete pBits;
	delete bi;

	// create region
	HRGN hRgn = ExtCreateRegion( NULL, RGNDATAHEADER_SIZE + pRgnData->nCount * sizeof(RECT), (LPRGNDATA)pRgnData );
	// release region data
	delete pRgnData;

	return hRgn;
}

