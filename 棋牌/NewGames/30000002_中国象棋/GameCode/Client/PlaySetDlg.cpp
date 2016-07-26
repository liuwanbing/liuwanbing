#include "Stdafx.h"
#include "PlaySetDlg.h"
#include "ClientGameDlg.h"
#include "Resource.h"


BEGIN_MESSAGE_MAP(CPlaySetDlg, CCoolView)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

/**
 * 构造函数
 */
CPlaySetDlg::CPlaySetDlg(CWnd * pParent) : CCoolView(CPlaySetDlg::IDD,pParent)
{	
	m_pPP=pParent;
	m_GameSetInfo.iJuTimeMinute = 10;
	m_GameSetInfo.iJuTimeSecond = 30;
	m_GameSetInfo.iBuTimeMinute = 1;
	m_GameSetInfo.iBuTimeSecond = 30;
	m_GameSetInfo.iDuMiaoMinute = 1;
	m_GameSetInfo.iDuMiaoSecond = 30;
	m_bRequireCoachMoeny = 0;
	TCHAR path[MAX_PATH];

	wsprintf(path,TEXT(".\\image\\set_bk.bmp"));
	m_bk.SetLoadInfo(path,true);
}

/**
 * 析够函数
 */
CPlaySetDlg::~CPlaySetDlg()
{
}

/**
 * DDX/DDV 支持
 */
void CPlaySetDlg::DoDataExchange(CDataExchange* pDX)
{
	CCoolView::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btOk);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	//DDX_Control(pDX, IDC_SPIN, m_SpinCtrl);
}

void CPlaySetDlg::AutoSize()
{
	MoveWindow(0,0,m_bk.GetWidth(),m_bk.GetHeight());	
}

/**
 * 初始化函数
 */
BOOL CPlaySetDlg::OnInitDialog()
{
	CCoolView::OnInitDialog();

	//AfxSetResourceHandle(GetModuleHandle(CLIENT_DLL_NAME));
	TCHAR path[MAX_PATH];	

	wsprintf(path,TEXT(".\\image\\set_bk.bmp"));
	LoadDialogBitmap(path,RGB(255,0,255),RGB(1,1,1));
	//AfxSetResourceHandle(GetModuleHandle(NULL));

	wsprintf(path,TEXT(".\\image\\ok.bmp"));
	m_btOk.LoadButtonBitmap(path,false);

	wsprintf(path,TEXT(".\\image\\cancel.bmp"));
	m_btCancel.LoadButtonBitmap(path,false);

	wsprintf(path,TEXT(".\\image\\set_bk.bmp"));
	m_bk.SetLoadInfo(path,true);

	m_GameSetInfo.iJuTimeMinute = 10;
	m_GameSetInfo.iJuTimeSecond = 30;
	m_GameSetInfo.iBuTimeMinute = 1;
	m_GameSetInfo.iBuTimeSecond = 30;
	m_GameSetInfo.iDuMiaoMinute = 1;
	m_GameSetInfo.iDuMiaoSecond = 30;
	m_GameSetInfo.iGameMoney = 0;

	//设置控件
	//设置控件初始化
	SetDlgItemInt(IDC_EDIT_JU_TIME_MINUTE,m_GameSetInfo.iJuTimeMinute);
	SetDlgItemInt(IDC_EDIT_JU_TIME_SECOND,m_GameSetInfo.iJuTimeSecond);
	SetDlgItemInt(IDC_EDIT_BU_TIME_MINUTE,m_GameSetInfo.iBuTimeMinute);
	SetDlgItemInt(IDC_EDIT_BU_TIME_SECOND,m_GameSetInfo.iBuTimeSecond);
	SetDlgItemInt(IDC_EDIT_DU_MIAO_MINUTE,m_GameSetInfo.iDuMiaoMinute);
	SetDlgItemInt(IDC_EDIT_DU_MIAO_SECOND,m_GameSetInfo.iDuMiaoSecond);
	SetDlgItemInt(IDC_EDIT_MONEY,m_GameSetInfo.iGameMoney);

	//CStatic *pStatic = (CStatic *)GetDlgItem(IDC_STATIC_1);
	//if(NULL != pStatic)
	//{
	//	//pStatic->set
	//}

	CEdit * pEdit = (CEdit *)GetDlgItem(IDC_EDIT_JU_TIME_MINUTE);
	if (pEdit!=NULL)
		pEdit->SetLimitText(2);
	
	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_JU_TIME_SECOND);
	if (pEdit!=NULL)
		pEdit->SetLimitText(2);
	
	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_BU_TIME_MINUTE);
	if (pEdit!=NULL)
		pEdit->SetLimitText(2);
	
	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_BU_TIME_SECOND);
	if (pEdit!=NULL)
		pEdit->SetLimitText(2);
	
	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_DU_MIAO_MINUTE);
	if (pEdit!=NULL)
		pEdit->SetLimitText(2);
	
	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_DU_MIAO_SECOND);
	if (pEdit!=NULL)
		pEdit->SetLimitText(2);

	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_MONEY);
	if (pEdit!=NULL)
		pEdit->SetLimitText(10);

	if (false == m_bRequireCoachMoeny)
	{
		pEdit = (CEdit *)GetDlgItem(IDC_EDIT_MONEY);
		if (pEdit)
		{
			pEdit->ShowWindow(SW_HIDE);
		}
		CTransparentStc *pStatic = (CTransparentStc *)GetDlgItem(IDC_STATIC_TYPE_NAME);	
		if (pStatic)
		{
			pStatic->ShowWindow(SW_HIDE);
		}
		pStatic = (CTransparentStc *)GetDlgItem(IDC_STATIC_MONEY_TYPE);
		if (pStatic)
		{
			pStatic->ShowWindow(SW_HIDE);
		}
	}	
	return TRUE;
}

/**
 * 取消按钮
 */
void CPlaySetDlg::OnCancel()
{
	CClientPlayView *p=CONTAINING_RECORD(this,CClientPlayView,m_GameSet);
	CClientGameDlg *pp=CONTAINING_RECORD(p,CClientGameDlg,m_PlayView);
	CCoolView::OnClose();
	::PostMessage(pp->GetSafeHwnd(),WM_CLOSE,0,0); 
}

/**
 * 确定按钮
 */
void CPlaySetDlg::OnOK()
{
	//读取数据
	BOOL bJTMTrancelate,bJTSTrancelate,bBTMTrancelate,bBTSTrancelate,bDMMTrancelate,bDMSTrancelate, bZDFTrancelate;
	bJTMTrancelate = bJTSTrancelate = bBTMTrancelate = bBTSTrancelate = bDMMTrancelate = bDMSTrancelate = bZDFTrancelate = FALSE;

	m_GameSetInfo.iJuTimeMinute = GetDlgItemInt(IDC_EDIT_JU_TIME_MINUTE,&bJTMTrancelate);
	m_GameSetInfo.iJuTimeSecond = GetDlgItemInt(IDC_EDIT_JU_TIME_SECOND,&bJTSTrancelate);
	m_GameSetInfo.iBuTimeMinute = GetDlgItemInt(IDC_EDIT_BU_TIME_MINUTE,&bBTMTrancelate);
	m_GameSetInfo.iBuTimeSecond = GetDlgItemInt(IDC_EDIT_BU_TIME_SECOND,&bBTSTrancelate);
	m_GameSetInfo.iDuMiaoMinute = GetDlgItemInt(IDC_EDIT_DU_MIAO_MINUTE,&bDMMTrancelate);
	m_GameSetInfo.iDuMiaoSecond = GetDlgItemInt(IDC_EDIT_DU_MIAO_SECOND,&bDMSTrancelate);
	m_GameSetInfo.iGameMoney    = GetDlgItemInt(IDC_EDIT_MONEY,&bZDFTrancelate);

	//m_GameSetInfo.iBeginStation=((CComboBox *)GetDlgItem(IDC_BEGIN_STATION))->GetCurSel()+2;
	//m_GameSetInfo.iEndStation=((CComboBox *)GetDlgItem(IDC_END_STATION))->GetCurSel()+2;
	//m_GameSetInfo.iCardCount=(((CButton *)GetDlgItem(IDC_TWO))->GetCheck()==BST_CHECKED)?1:2;
	//m_GameSetInfo.iPlayCount=GetDlgItemInt(IDC_COUNT,&bTrancelate);

	//效验数据
	if (bJTMTrancelate==FALSE || bJTSTrancelate == FALSE || (m_GameSetInfo.iJuTimeMinute == 0 && m_GameSetInfo.iJuTimeSecond == 0))
	{
		MessageBox(TEXT(" 请设置游戏的局用时时间！"),"提示",MB_ICONQUESTION);
		GetDlgItem(IDC_EDIT_JU_TIME_SECOND)->SetFocus();
		return;
	}
	if (bBTMTrancelate==FALSE || bBTSTrancelate == FALSE || (m_GameSetInfo.iBuTimeMinute == 0 && m_GameSetInfo.iBuTimeSecond == 0))
	{
		MessageBox(TEXT("请设置游戏的每步用时时间！"),"提示",MB_ICONQUESTION);
		GetDlgItem(IDC_EDIT_BU_TIME_SECOND)->SetFocus();
		return;
	}
	if (bBTMTrancelate==FALSE || bBTSTrancelate == FALSE || (m_GameSetInfo.iDuMiaoMinute == 0 && m_GameSetInfo.iDuMiaoSecond == 0))
	{
		MessageBox(TEXT("请设置游戏的每步用时时间！"),"提示",MB_ICONQUESTION);
		GetDlgItem(IDC_EDIT_DU_MIAO_MINUTE)->SetFocus();
		return;
	}
	if(m_GameSetInfo.iJuTimeMinute<0||m_GameSetInfo.iJuTimeMinute>999)
	{
		MessageBox("请设置合适的时间","提示",0);
		return ;
	}
	if(m_GameSetInfo.iJuTimeSecond<0||m_GameSetInfo.iJuTimeSecond>59)
	{
		MessageBox("请正确设置时间","提示",0);
		return ;
	}
	//局时设置至少5分钟
	if(m_GameSetInfo.iJuTimeMinute*60+m_GameSetInfo.iJuTimeSecond < 630)
	{
		MessageBox("请设置合适的时间(一局游戏至少10分30秒)","提示",0);
		GetDlgItem(IDC_EDIT_JU_TIME_SECOND)->SetFocus();
		return ;
	}
	if(m_GameSetInfo.iBuTimeMinute<0||m_GameSetInfo.iBuTimeMinute>999)
	{
		MessageBox("请设置合适的时间","提示",0);
		return ;
	}
	if(m_GameSetInfo.iBuTimeSecond<0||m_GameSetInfo.iBuTimeSecond>59)
	{
		MessageBox("请正确设置时间","提示",0);
		return ;
	}
	//每步时间至少10秒
	if(m_GameSetInfo.iBuTimeMinute*60+m_GameSetInfo.iBuTimeSecond < 90)
	{
		MessageBox("请设置合适的时间(一步至少1分30秒)","提示",0);
		GetDlgItem(IDC_EDIT_BU_TIME_SECOND)->SetFocus();
		return ;
	}	
	if(m_GameSetInfo.iDuMiaoMinute<0||m_GameSetInfo.iDuMiaoMinute>999)
	{
		MessageBox("请设置合适的时间","提示",0);
		return ;
	}
	if(m_GameSetInfo.iDuMiaoSecond<0||m_GameSetInfo.iDuMiaoSecond>59)
	{
		MessageBox("请正确设置时间","提示",0);
		return ;
	}
	//每步时间至少10秒
	if(m_GameSetInfo.iDuMiaoMinute*60+m_GameSetInfo.iDuMiaoSecond < 90)
	{
		MessageBox("请设置合适的时间(读秒至少1分30秒)","提示",0);
		GetDlgItem(IDC_EDIT_DU_MIAO_MINUTE)->SetFocus();
		return ;
	}	

	//本局指导费
	if(m_GameSetInfo.iGameMoney < 0 || m_GameSetInfo.iGameMoney > 100000000)
	{
		MessageBox("请设置合适的指导费(从0到100000000)","提示",0);
		GetDlgItem(IDC_EDIT_MONEY)->SetFocus();
		return ;
	}
	//发送消息
	CClientPlayView *p=CONTAINING_RECORD(this,CClientPlayView,m_GameSet);
	CClientGameDlg *pp=CONTAINING_RECORD(p,CClientGameDlg,m_PlayView);
	pp->PostMessage(IDM_BEGIN,0,0);
	CCoolView::OnClose();
	return;
}

/**
 * 设置游戏设置
 */
void CPlaySetDlg::SetGameSetInfo(int iCardCount)
{
	//设置变量
	///m_GameSetInfo.iCardCount=iCardCount;
	/*if (GetSafeHwnd()==NULL) 
		Create(IDD_SET_PLAY,m_pPP);*/	
	return;
}

/**
 * 获取游戏设置
 */
tagGameSetInfo * CPlaySetDlg::GetGameSetInfo()
{
//	if (GetSafeHwnd()) return NULL;
	return &m_GameSetInfo;
}

LRESULT CPlaySetDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	//case WM_CTLCOLORSCROLLBAR :
	//case WM_CTLCOLORBTN:
	//case WM_CTLCOLORMSGBOX:
	//case WM_CTLCOLOREDIT:
	//case WM_CTLCOLORLISTBOX:
	//case WM_CTLCOLORDLG:
	//case WM_CTLCOLORSTATIC :
	//	SetBkMode((HDC)wParam,TRANSPARENT);
	//	HBRUSH brush = (HBRUSH)::GetStockObject(NULL_BRUSH);
	//	return (LRESULT)brush;
	}
	return CCoolView::WindowProc(message, wParam, lParam);
}

HBRUSH CPlaySetDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch(pWnd->GetDlgCtrlID())
	{
	case IDC_STATIC_0:
	case IDC_STATIC_1:
	case IDC_STATIC_2:
	case IDC_STATIC_3:
	case IDC_STATIC_4:
	case IDC_STATIC_5:
	case IDC_STATIC_6:
	case IDC_STATIC_7:
	case IDC_STATIC_8:
	case IDC_STATIC_TYPE_NAME:
	case IDC_STATIC_MONEY_TYPE:
		{
			pDC-> SetTextColor(RGB(255,255,255));
			pDC->SetBkMode(TRANSPARENT); 
			HBRUSH hbr = (HBRUSH)::GetStockObject(NULL_BRUSH);
			return  hbr; 
		}
	default: break;
	}

	return CWnd::OnCtlColor(pDC,pWnd,nCtlColor);
}
