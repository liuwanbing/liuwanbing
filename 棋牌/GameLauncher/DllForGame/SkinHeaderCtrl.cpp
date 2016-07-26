// SkinHeaderCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "SkinHeaderCtrl.h"
#include "memdc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSkinHeaderCtrl

#define SAFERELEASE(p) {if(!p){delete p;}}

CSkinHeaderCtrl::CSkinHeaderCtrl()
{
	LoadSkinImage();
}

CSkinHeaderCtrl::~CSkinHeaderCtrl()
{
	UnLoadSkinImage();
}


BEGIN_MESSAGE_MAP(CSkinHeaderCtrl, CHeaderCtrl)
	//{{AFX_MSG_MAP(CSkinHeaderCtrl)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSkinHeaderCtrl message handlers

void CSkinHeaderCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{

}

void CSkinHeaderCtrl::LoadSkinImage()
{
	CString strAppPath = CBcfFile::GetAppPath();
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f(strAppPath + "..\\" + strSkin);

	TCHAR path[MAX_PATH] = {0};
	CString skinfolder = strAppPath + "..\\";
	skinfolder += f.GetKeyVal(m_skinmgr.GetKeyVal(strSkin),"skinfolder",m_skinmgr.GetSkinPath());
	wsprintf(path,"%sdialog\\BankDlg\\head.bmp",skinfolder);

	m_BitHandle = (HBITMAP)LoadImage(NULL, path, IMAGE_BITMAP, 0,0, LR_LOADFROMFILE);
	m_BitHandle1 = (HBITMAP)LoadImage(NULL, path, IMAGE_BITMAP, 0,0, LR_LOADFROMFILE);
	m_BitHandle3 = (HBITMAP)LoadImage(NULL, path, IMAGE_BITMAP, 0,0, LR_LOADFROMFILE);
	wsprintf(path,"%sdialog\\BankDlg\\span.bmp",skinfolder);
	m_BitHandle2 = (HBITMAP)LoadImage(NULL, path, IMAGE_BITMAP, 0,0, LR_LOADFROMFILE);
	wsprintf(path,"%sdialog\\BankDlg\\end.bmp",skinfolder);
	m_BitHandleEnd = (HBITMAP)LoadImage(NULL, path, IMAGE_BITMAP, 0,0, LR_LOADFROMFILE);

	m_bitmap.Attach(m_BitHandle);
	m_bitmap2.Attach(m_BitHandle1);
	m_bitmap3.Attach(m_BitHandle2);
	m_bitmapSpan.Attach(m_BitHandle3);
	m_bitmapEnd.Attach(m_BitHandleEnd);
}

void CSkinHeaderCtrl::UnLoadSkinImage()
{
	m_bitmap.Detach();
	m_bitmap2.Detach();
	m_bitmap3.Detach();
	m_bitmapSpan.Detach();
	m_bitmapEnd.Detach();
}

void CSkinHeaderCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rect, rectItem;
	GetClientRect(&rect);
	CMemDC memDC(&dc, rect);
	CDC bitmapDC;
	bitmapDC.CreateCompatibleDC(&dc);
	
	memDC.FillSolidRect(&rect, RGB(76,85,118));




	BITMAP bm;
	GetObject(m_BitHandle1, sizeof(BITMAP), &bm);
	CBitmap* pOldBitmapSpan = bitmapDC.SelectObject(&m_bitmapSpan);
	
	for(int v=0; v<rect.Width(); v++)
	{
		memDC.BitBlt((rect.left+2)+(v*1), 0, 1,bm.bmHeight, &bitmapDC,0,0,SRCCOPY);
	}
	bitmapDC.SelectObject(pOldBitmapSpan);

	static int width = rect.Width();
	BITMAP bmEnd;
	GetObject(m_BitHandleEnd, sizeof(BITMAP), &bmEnd);
	pOldBitmapSpan = bitmapDC.SelectObject(&m_bitmapEnd);
	memDC.BitBlt(width-bmEnd.bmWidth, 0,rect.Width(),bm.bmHeight, &bitmapDC,0,0,SRCCOPY);
	bitmapDC.SelectObject(pOldBitmapSpan);
	
	int nItems = GetItemCount();

	for(int i = 0; i <nItems; i++)
	{	
		TCHAR buf1[256];
		HD_ITEM hditem1;
		
		hditem1.mask = HDI_TEXT | HDI_FORMAT | HDI_ORDER;
		hditem1.pszText = buf1;
		hditem1.cchTextMax = 255;
		GetItem( i, &hditem1 );
		
		GetItemRect(i, &rect);
		
		CBitmap* pOldBitmap = NULL;
		
		//make sure we draw the start piece
		//on the first item so it has a left border

		//For the following items we will just use the
		//right border of the previous items as the left
		//border
		
		if(hditem1.iOrder==0)
		{
			pOldBitmap = bitmapDC.SelectObject(&m_bitmap);
			memDC.BitBlt(rect.left,rect.top,2,bm.bmHeight,&bitmapDC,0,0,SRCCOPY);
		}
		else
		{
			memDC.BitBlt(rect.left-1,rect.top,2,bm.bmHeight,&bitmapDC,0,0,SRCCOPY);
			pOldBitmap = bitmapDC.SelectObject(&m_bitmap2);
			memDC.BitBlt(rect.left+1,rect.top,1,bm.bmHeight,&bitmapDC,0,0,SRCCOPY);
		}

		bitmapDC.SelectObject(pOldBitmap);
		
		//span the bitmap for the width of the column header item
		int nWidth = rect.Width() - 4;
		
		CBitmap* pOldBitmap2 = bitmapDC.SelectObject(&m_bitmap2);
		for(int v=0; v<nWidth; v++)
		{
			memDC.BitBlt((rect.left+2)+(v*1), 0, 1, bm.bmHeight, &bitmapDC,0,0,SRCCOPY);
		}

		bitmapDC.SelectObject(pOldBitmap2);
		
		
		//draw the end piece of the column header
		CBitmap* pOldBitmap3 = bitmapDC.SelectObject(&m_bitmap3);
		if (i != nItems-1)
		{
			memDC.BitBlt((rect.right-2), 2, 2, bm.bmHeight, &bitmapDC,0,0,SRCCOPY);
			bitmapDC.SelectObject(pOldBitmap3);
		}
		else
		{

		}
		
		//Get all the info for the current
		//item so we can draw the text to it
		//in the desired font and style
		DRAWITEMSTRUCT	DrawItemStruct;
		GetItemRect(i, &rectItem);
		
		DrawItemStruct.CtlType		= 100;
		DrawItemStruct.hDC			= dc.GetSafeHdc();
		DrawItemStruct.itemAction	= ODA_DRAWENTIRE; 
		DrawItemStruct.hwndItem 	= GetSafeHwnd(); 
		DrawItemStruct.rcItem	= rectItem;
		DrawItemStruct.itemID	= i;
		DrawItem(&DrawItemStruct);
		
		UINT uFormat = DT_SINGLELINE | DT_NOPREFIX | DT_TOP |DT_CENTER | DT_END_ELLIPSIS ;
		
		CFont font;
		LOGFONT lf;
		memset(&lf, 0, sizeof(LOGFONT));
		lf.lfHeight = 16;
		strcpy(lf.lfFaceName, "Sevenet 7");
		font.CreateFontIndirect(&lf);
		CFont* def_font = memDC.SelectObject(&font);
		
		memDC.SetBkMode(TRANSPARENT);
		rectItem.DeflateRect(2,2,2,2);
		
		TCHAR buf[256];
		HD_ITEM hditem;
		
		hditem.mask = HDI_TEXT | HDI_FORMAT | HDI_ORDER;
		hditem.pszText = buf;
		hditem.cchTextMax = 255;
		GetItem( DrawItemStruct.itemID, &hditem );

		memDC.DrawText(buf, &rectItem, uFormat);
		memDC.SelectObject(def_font);
		font.DeleteObject();
	}

	//static int width = 328;
	//GetClientRect(&rect);
	//BITMAP bmEnd;
	//GetObject(m_BitHandleEnd, sizeof(BITMAP), &bmEnd);
	//pOldBitmapSpan = bitmapDC.SelectObject(&m_bitmapEnd);
	//memDC.BitBlt(width-bmEnd.bmWidth, 0,rect.Width(),bm.bmHeight, &bitmapDC,0,0,SRCCOPY);
	//bitmapDC.SelectObject(pOldBitmapSpan);


}

BOOL CSkinHeaderCtrl::OnEraseBkgnd(CDC* pDC) 
{
	return false;	
}

void CSkinHeaderCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{

}