// SkinListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "SkinListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSkinListCtrl

CSkinListCtrl::CSkinListCtrl()
{
}

CSkinListCtrl::~CSkinListCtrl()
{
}


BEGIN_MESSAGE_MAP(CSkinListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CSkinListCtrl)
	ON_WM_NCCALCSIZE()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT ( NM_CUSTOMDRAW, OnCustomDrawList )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSkinListCtrl message handlers

void CSkinListCtrl::PreSubclassWindow() 
{
	//use our custom CHeaderCtrl
	m_SkinHeaderCtrl.SubclassWindow(GetHeaderCtrl()->m_hWnd);

	CListCtrl::PreSubclassWindow();
}


void CSkinListCtrl::OnCustomDrawList ( NMHDR* pNMHDR, LRESULT* pResult )
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );
	static bool bHighlighted = false;
	

    // Take the default processing unless we set this to something else below.
    *pResult = CDRF_DODEFAULT;
	
    // First thing - check the draw stage. If it's the control's prepaint
    // stage, then tell Windows we want messages for every item.
	
    if ( CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage )
	{
        *pResult = CDRF_NOTIFYITEMDRAW;
	}
    else if ( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage )
	{
        int iRow = (int)pLVCD->nmcd.dwItemSpec;
		
		bHighlighted = IsRowHighlighted(m_hWnd, iRow);
		if (bHighlighted)
		{
			pLVCD->clrText   = m_Bcolor-160;
			pLVCD->clrTextBk = m_Bcolor+20;
			
			// Turn off listview highlight otherwise it uses the system colors!
			EnableHighlighting(m_hWnd, iRow, false);
		}
		
		// We want item post-paint notifications, so...
		*pResult = CDRF_DODEFAULT | CDRF_NOTIFYPOSTPAINT;
		
	}
	else if(CDDS_ITEMPOSTPAINT == pLVCD->nmcd.dwDrawStage)
	{
	if (bHighlighted)
      {
        int  iRow = (int)pLVCD->nmcd.dwItemSpec;

        // Turn listview control's highlighting back on now that we have
        // drawn the row in the colors we want.
        EnableHighlighting(m_hWnd, iRow, true);
      }

      *pResult = CDRF_DODEFAULT;

	}
}

void CSkinListCtrl::EnableHighlighting(HWND hWnd, int row, bool bHighlight)
{
  ListView_SetItemState(hWnd, row, bHighlight? 0xff: 0, LVIS_SELECTED);
}

bool CSkinListCtrl::IsRowSelected(HWND hWnd, int row)
{
  return ListView_GetItemState(hWnd, row, LVIS_SELECTED) != 0;
}

bool CSkinListCtrl::IsRowHighlighted(HWND hWnd, int row)
{
  return IsRowSelected(hWnd, row) /*&& (::GetFocus() == hWnd)*/;
}

void CSkinListCtrl::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	UpdateWindow();
	CListCtrl::OnNcCalcSize(bCalcValidRects, lpncsp);
}

BOOL CSkinListCtrl::OnEraseBkgnd(CDC* pDC) 
{
	return FALSE;
	//return CListCtrl::OnEraseBkgnd(pDC);
}

void CSkinListCtrl::OnPaint() 
{
	CPaintDC dc(this);

	CRect rect;
	GetClientRect(&rect);
	CMemDC memDC(&dc, rect);
	
	CRect headerRect;
	GetDlgItem(0)->GetWindowRect(&headerRect);
	ScreenToClient(&headerRect);
	dc.ExcludeClipRect(&headerRect);	   
	
	CFont font;
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = 16;
	strcpy(lf.lfFaceName, "Sevenet 7");
	font.CreateFontIndirect(&lf);

	CRect clip;
	memDC.GetClipBox(&clip);
	memDC.FillSolidRect(clip,m_Bcolor);
	SetTextBkColor(m_Bcolor);
	CFont* def_font = memDC.SelectObject(&font);

	DefWindowProc(WM_PAINT, (WPARAM)memDC->m_hDC, (LPARAM)0);
	memDC.SelectObject(def_font);
	font.DeleteObject();
}

void CSkinListCtrl::SetColor(DWORD Bcolor)
{
	m_Bcolor = Bcolor;
}