// ColorComboBox.cpp : 实现文件
//

#include "stdafx.h"
#include "ColorComboBox.h"


// CColorComboBox

IMPLEMENT_DYNAMIC(CColorComboBox, CComboBox)

CColorComboBox::CColorComboBox()
{

}

CColorComboBox::~CColorComboBox()
{
}


BEGIN_MESSAGE_MAP(CColorComboBox, CComboBox)
	ON_WM_DRAWITEM()
END_MESSAGE_MAP()



// CColorComboBox 消息处理程序




void CColorComboBox::DrawItem(LPDRAWITEMSTRUCT lp/*lpDrawItemStruct*/)
{

	CDC dc;
	dc.Attach(lp->hDC);
	CRect rc(lp->rcItem);
	CBrush brush;
	switch(lp->itemID)
	{
	case 0:	brush.CreateSolidBrush(RGB(0,0,0));				break;
	case 1:	brush.CreateSolidBrush(RGB(128,0,0));			break;
	case 2:	brush.CreateSolidBrush(RGB(0,128,0));			break;
	case 3:	brush.CreateSolidBrush(RGB(128,128,0));			break;
	case 4:	brush.CreateSolidBrush(RGB(0,0,128));			break;
	case 5:	brush.CreateSolidBrush(RGB(128,0,128));			break;
	case 6:	brush.CreateSolidBrush(RGB(0,128,128));			break;
	case 7:	brush.CreateSolidBrush(RGB(128,128,128));		break;
	case 8:	brush.CreateSolidBrush(RGB(255,0,0));			break;
	case 9:	brush.CreateSolidBrush(RGB(0,255,0));			break;
	case 10:	brush.CreateSolidBrush(RGB(255,255,0));		break;
	case 11:	brush.CreateSolidBrush(RGB(0,0,255));		break;
	case 12:	brush.CreateSolidBrush(RGB(255,0,255));		break;
	case 13:	brush.CreateSolidBrush(RGB(0,255,255));		break;
	case 14:	brush.CreateSolidBrush(RGB(255,255,255));	break;
	}
	rc.InflateRect(-2,-2);
	dc.FillRect(rc,&brush);
	if(lp->itemState && ODS_SELECTED)
		dc.DrawFocusRect(rc);

	/// {{Added by zxd 20100709 释放GDI资源
	brush.DeleteObject();
	/// Added by zxd 20100709 释放GDI资源}}

	dc.Detach();

	// TODO:  添加您的代码以绘制指定项
}
