// ShowHistory.cpp : 实现文件
//

#include "stdafx.h"
#include "Auto_AI.h"
#include "ShowHistory.h"


// CShowHistory 对话框

IMPLEMENT_DYNAMIC(CShowHistory, CDialog)

CShowHistory::CShowHistory(CWnd* pParent /*=NULL*/)
	: CDialog(CShowHistory::IDD, pParent)
{
	m_iX = 30;
	m_iY = 150;
	//strcpy(m_szMsg,"版本说明");
}

CShowHistory::~CShowHistory()
{
}

void CShowHistory::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CShowHistory, CDialog)
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CShowHistory 消息处理程序

void CShowHistory::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDialog::OnOK();
	CDialog::OnLButtonUp(nFlags, point);
}

void CShowHistory::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch(nIDEvent)
	{
	case 0:
	default:

		m_iY -= 10;
		if(m_iY < 30)
			m_iStartLine ++;
		return ;
	}
	CDialog::OnTimer(nIDEvent);
}

BOOL CShowHistory::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetTimer(1,300,NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CShowHistory::OnPaint()
{
	//CPaintDC dc(this); // device context for painting
	//dc.SetBkMode(TRANSPARENT);
	//dc.SetTextColor(RGB(255,0,255));
	//dc.TextOutA(m_iX,m_iY,m_szMsg,strlen(m_szMsg));
}
