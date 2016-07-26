// Edit2.cpp : implementation file
//

#include "stdafx.h"
#include "DllForGame.h"
#include "Edit2.h"


// CEdit2

IMPLEMENT_DYNAMIC(CEdit2, CWnd)

CEdit2::CEdit2()
{

}

CEdit2::~CEdit2()
{
}


BEGIN_MESSAGE_MAP(CEdit2, CWnd)
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()



// CEdit2 message handlers



void CEdit2::OnKillFocus(CWnd* pNewWnd)
{
	CWnd::OnKillFocus(pNewWnd);

	// TODO: Add your message handler code here
	AfxMessageBox("wyl");
}
