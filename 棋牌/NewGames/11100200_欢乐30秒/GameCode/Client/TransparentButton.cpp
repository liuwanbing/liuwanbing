// TransparentButton.cpp : 实现文件
//

#include "stdafx.h"
#include "TransparentButton.h"


// CTransparentButton

IMPLEMENT_DYNAMIC(CTransparentButton, CButton)

CTransparentButton::CTransparentButton()
{

}

CTransparentButton::~CTransparentButton()
{
}


BEGIN_MESSAGE_MAP(CTransparentButton, CButton)
	ON_WM_CTLCOLOR_REFLECT()
	ON_MESSAGE(WM_SETTEXT,OnSetText)
END_MESSAGE_MAP()



// CTransparentButton 消息处理程序



HBRUSH CTransparentButton::CtlColor(CDC* pDC, UINT nCtlColor)
{
	// TODO:  在此更改 DC 的任何属性

	// TODO:  如果不应调用父级的处理程序，则返回非空画笔
	if (NULL != pDC)
	{
		pDC->SetBkMode(TRANSPARENT);
	}
	return (HBRUSH)GetStockObject(NULL_BRUSH);
	//return NULL;
}

LRESULT CTransparentButton::OnSetText(WPARAM wParam,LPARAM lParam)
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