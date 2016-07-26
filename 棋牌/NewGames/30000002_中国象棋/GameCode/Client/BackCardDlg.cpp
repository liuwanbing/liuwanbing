#include "Stdafx.h"
#include "Resource.h"
#include "BackCardDlg.h"


BEGIN_MESSAGE_MAP(CBackCardDlg, CGameFaceGo)
END_MESSAGE_MAP()

/**
 * 构造函数
 */
CBackCardDlg::CBackCardDlg(CWnd * pParent) : CGameFaceGo(CBackCardDlg::IDD)
{
	m_iBackCount=0;
}

/**
 * 析构函数
 */
CBackCardDlg::~CBackCardDlg()
{
}

/**
 * DDX/DDV 支持
 */
void CBackCardDlg::DoDataExchange(CDataExchange* pDX)
{
	CGameFaceGo::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btOk);
}

/**
 * 设置底牌
 */
void CBackCardDlg::SetBackCard(BYTE iCardList[], int iCardCount)
{
	m_iBackCount=iCardCount;
	::CopyMemory(m_iBackCard,iCardList,sizeof(BYTE)*iCardCount);
	if (m_BackCard.GetSafeHwnd()) m_BackCard.SetCard(m_iBackCard,NULL,m_iBackCount);
	return;
}

/**
 * 初始化函数
 */
BOOL CBackCardDlg::OnInitDialog()
{
	CGameFaceGo::OnInitDialog();

	RECT Rect;
	m_BackCard.Create(NULL,NULL,WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS,CRect(0,0,0,0),this,20,NULL);
	m_BackCard.SetCard(m_iBackCard,NULL,m_iBackCount);
	m_BackCard.SetShowCard(true);
	GetClientRect(&Rect);
	m_BackCard.SetBasePoint(CPoint(Rect.right/2,-10),CUpGradeGameCard::XShowMode::SWX_MODE_CENTER,CUpGradeGameCard::YShowMode::SWY_MODE_TOP);

	return TRUE;
}

/**
 * 取消函数
 */
void CBackCardDlg::OnCancel()
{
	__super::OnCancel();
//	CGameFaceGo::OnClose();
}

/**
 * 确定函数
 */
void CBackCardDlg::OnOK()
{
	__super::OnOK();
//	CGameFaceGo::OnClose();
}
