#include "Stdafx.h"
#include "GameEndInfoDlg.h"
#include "ClientGameDlg.h"
#include "ClientView.h"


BEGIN_MESSAGE_MAP(GameEndInfoDlg, CGameFaceGo)
	ON_BN_CLICKED(IDC_NO_CONTINUE, OnBnClickedQuit)
END_MESSAGE_MAP()

//构造函数
GameEndInfoDlg::GameEndInfoDlg(CWnd *pParent) : CGameFaceGo(GameEndInfoDlg::IDD,pParent)
{
}

//析够函数
GameEndInfoDlg::~GameEndInfoDlg()
{
}

//DDX/DDV 支持
void GameEndInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CGameFaceGo::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_List);
	DDX_Control(pDX, IDCANCEL, m_btKnow);
	DDX_Control(pDX, IDC_NO_CONTINUE, m_btQuit);
}

//设置信息
BOOL GameEndInfoDlg::SetEndGameData(GameEndStruct * pGameEnd, UserItemStruct * pUserInfo[], int iPlayCardCount, CWnd * pParent)
{
	int iItem;
	TCHAR szBuffer[30];

	if (GetSafeHwnd()==NULL) Create(IDD_GAME_END,pParent);
	else m_List.DeleteAllItems();

	//填写信息
	for (int i=0;i<4;i++)
	{
		if (pUserInfo[i]!=NULL)
		{
			iItem=m_List.InsertItem(0,pUserInfo[i]->GameUserInfo.szName,0);
			if ((i!=pGameEnd->iUpGradeStation)&&((i+2)%4!=pGameEnd->iUpGradeStation))
			{
				sprintf(szBuffer,TEXT("%3d"),pGameEnd->iGamePoint);
				m_List.SetItem(iItem,1,1,szBuffer,0,0,0,0);
			}
			sprintf(szBuffer,TEXT("%2d"),pGameEnd->iTurePoint[i]);
			m_List.SetItem(iItem,2,1,szBuffer,0,0,0,0);
		}
	}
	m_BackCardView.SetCard(pGameEnd->iBackList,NULL,(iPlayCardCount==2)?8:6);
	
	//显示窗口
	ShowWindow(SW_SHOW);
	SetFocus();

	return TRUE;
}

//初始化函数
BOOL GameEndInfoDlg::OnInitDialog()
{
	CGameFaceGo::OnInitDialog();

	//设置列表
	m_List.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,LVS_EX_FULLROWSELECT,LVS_EX_FULLROWSELECT);
	m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_FLATSB);
	m_List.InsertColumn(0,"昵称",LVCFMT_LEFT,80);
	m_List.InsertColumn(1,"游戏分",LVCFMT_LEFT,80);
	m_List.InsertColumn(2,"得分",LVCFMT_LEFT,80);

	//建立扑克
	CRect ClientRect;
	GetClientRect(&ClientRect);
	BYTE iBackCard[8];
	::memset(iBackCard,0,sizeof(iBackCard));
	m_BackCardView.Create(NULL,NULL,WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,150,NULL);
	m_BackCardView.SetBasePoint(CPoint(ClientRect.Width()/2,ClientRect.Height()-156),CUpGradeGameCard::XShowMode::SWX_MODE_CENTER,CUpGradeGameCard::YShowMode::SWY_MODE_TOP);
	m_BackCardView.SetCard(iBackCard,NULL,8);
	m_BackCardView.SetShowCard(true);

	return TRUE;
}

//取消
void GameEndInfoDlg::OnCancel()
{
	DestroyWindow();
}

//退出按钮
void GameEndInfoDlg::OnBnClickedQuit()
{
	DestroyWindow();
	CClientPlayView* p=CONTAINING_RECORD(this,CClientPlayView,m_GameEndView);
	CClientGameDlg* pp=CONTAINING_RECORD(p,CClientGameDlg,m_PlayView);
	pp->PostMessage(WM_CLOSE,0,0);
}

