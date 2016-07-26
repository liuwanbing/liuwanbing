// TransparentStatic.cpp : 透明静态控件实现文件
//

#include "stdafx.h"
#include "TransparentStatic.h"


// CTransparentStatic

IMPLEMENT_DYNAMIC(CTransparentStatic, CStatic)

CTransparentStatic::CTransparentStatic()
{

}

CTransparentStatic::~CTransparentStatic()
{
}


BEGIN_MESSAGE_MAP(CTransparentStatic, CStatic)
	ON_WM_CTLCOLOR_REFLECT()
	ON_MESSAGE(WM_SETTEXT,OnSetText)
END_MESSAGE_MAP()



// CTransparentStatic 消息处理程序
HBRUSH CTransparentStatic::CtlColor(CDC* pDC, UINT uCtlColor)
{
	if (NULL != pDC)
	{
		pDC->SetBkMode(TRANSPARENT);
	}
	return (HBRUSH)GetStockObject(NULL_BRUSH);
}

LRESULT CTransparentStatic::OnSetText(WPARAM wParam,LPARAM lParam)
{
	LRESULT lResult = Default();
	CWnd *pWnd = GetParent();
	if (NULL != pWnd)
	{
		CRect Rect;
		GetWindowRect(&Rect);
		pWnd->ScreenToClient(&Rect);
		pWnd->InvalidateRect(&Rect);
	}
	return lResult;
}