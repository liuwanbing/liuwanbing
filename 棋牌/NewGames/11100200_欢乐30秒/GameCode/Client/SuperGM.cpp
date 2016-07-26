// SuperGM.cpp : 实现文件
//

#include "stdafx.h"
#include "SuperGM.h"
#include "ClientGameDlg.h"

// CSuperGM 对话框

IMPLEMENT_DYNAMIC(CSuperGM, CDialog)

CSuperGM::CSuperGM(CWnd* pParent /*=NULL*/)
	: CDialog(CSuperGM::IDD, pParent)
{
	m_bShowRobotInfo = false;
}

CSuperGM::~CSuperGM()
{
}

void CSuperGM::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, 1071, m_ListCtrl);
}


BEGIN_MESSAGE_MAP(CSuperGM, CDialog)
	ON_NOTIFY(LVN_ITEMCHANGED, 1071, &CSuperGM::OnLvnItemchanged1071)
	ON_BN_CLICKED(IDCANCEL, &CSuperGM::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_RADIO_Z, &CSuperGM::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO_X, &CSuperGM::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO_H, &CSuperGM::OnBnClickedRadio3)
	ON_BN_CLICKED(IDC_RADIO_N, &CSuperGM::OnBnClickedRadio4)
	ON_BN_CLICKED(IDC_CHECK_ROBOT_INFO, &CSuperGM::OnBnClickedCheckRobotInfo)
END_MESSAGE_MAP()


// CSuperGM 消息处理程序

BOOL CSuperGM::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect rect;
	int iBlockLen = 0;
	m_ListCtrl.GetClientRect(&rect);
	iBlockLen = (rect.right - rect.left) / 10;
	
	m_ListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	int count=0;
	m_ListCtrl.InsertColumn(count++, "玩家名", LVCFMT_LEFT, iBlockLen);
	m_ListCtrl.InsertColumn(count++, "初始金币", LVCFMT_LEFT, iBlockLen);
	m_ListCtrl.InsertColumn(count++, "总金币", LVCFMT_LEFT, iBlockLen);
	m_ListCtrl.InsertColumn(count++, "庄", LVCFMT_LEFT, iBlockLen);
	m_ListCtrl.InsertColumn(count++, "庄天王", LVCFMT_LEFT, iBlockLen);
	m_ListCtrl.InsertColumn(count++, "庄对子", LVCFMT_LEFT, iBlockLen);
	m_ListCtrl.InsertColumn(count++, "闲", LVCFMT_LEFT, iBlockLen);
	m_ListCtrl.InsertColumn(count++, "闲天王", LVCFMT_LEFT, iBlockLen);
	m_ListCtrl.InsertColumn(count++, "闲对子", LVCFMT_LEFT, iBlockLen);
	m_ListCtrl.InsertColumn(count++, "和", LVCFMT_LEFT, iBlockLen);
	m_ListCtrl.InsertColumn(count++, "同点和", LVCFMT_LEFT, iBlockLen);

	m_SelectedArea = -1;
	UpdataCtrlText();
	return TRUE;
}


void CSuperGM::OnLvnItemchanged1071(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


void CSuperGM::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}

void CSuperGM::OnBnClickedRadio1()
{
	// TODO: 在此添加控件通知处理程序代码
	m_SelectedArea = 1;
	SendCtrlMsg();///发送控制消息

	CButton *pButton = NULL;
	pButton = (CButton *)GetDlgItem(IDC_RADIO_X);
	pButton->SetCheck(0);
	pButton = (CButton *)GetDlgItem(IDC_RADIO_H);
	pButton->SetCheck(0);
	pButton = (CButton *)GetDlgItem(IDC_RADIO_N);
	pButton->SetCheck(0);
}

void CSuperGM::OnBnClickedRadio2()
{
	// TODO: 在此添加控件通知处理程序代码
	m_SelectedArea = 2;
	SendCtrlMsg();///发送控制消息

	CButton *pButton = NULL;
	pButton	= (CButton *)GetDlgItem(IDC_RADIO_Z);
	pButton->SetCheck(0);

	pButton = (CButton *)GetDlgItem(IDC_RADIO_H);
	pButton->SetCheck(0);
	pButton = (CButton *)GetDlgItem(IDC_RADIO_N);
	pButton->SetCheck(0);
}

void CSuperGM::OnBnClickedRadio3()
{
	// TODO: 在此添加控件通知处理程序代码
	m_SelectedArea = 3;
	SendCtrlMsg();///发送控制消息

	CButton *pButton = NULL;
	pButton	= (CButton *)GetDlgItem(IDC_RADIO_Z);
	pButton->SetCheck(0);
	pButton = (CButton *)GetDlgItem(IDC_RADIO_X);
	pButton->SetCheck(0);

	pButton = (CButton *)GetDlgItem(IDC_RADIO_N);
	pButton->SetCheck(0);
}

void CSuperGM::OnBnClickedRadio4()
{
	// TODO: 在此添加控件通知处理程序代码
	m_SelectedArea = 0;
	SendCtrlMsg();///发送控制消息

	CButton *pButton = NULL;
	pButton	= (CButton *)GetDlgItem(IDC_RADIO_Z);
	pButton->SetCheck(0);
	pButton = (CButton *)GetDlgItem(IDC_RADIO_X);
	pButton->SetCheck(0);
	pButton = (CButton *)GetDlgItem(IDC_RADIO_H);
	pButton->SetCheck(0);
}

//是否显示机器人信息
void CSuperGM::OnBnClickedCheckRobotInfo()
{
	// TODO: Add your control notification handler code here

	CButton *pButton = NULL;

	pButton = (CButton *)GetDlgItem(IDC_CHECK_ROBOT_INFO);

	if(NULL != pButton)
	{
		m_bShowRobotInfo = pButton->GetCheck();
	}
}


void CSuperGM::OnSendDesktopXzInfo()
{
	if (m_bShowRobotInfo)
	{
		((CLoveSendClassInExe *)GetParent())->SendMessage(WM_SHOW_ROBOT_CHIP, 1, 0);
	}
	else
	{
		((CLoveSendClassInExe *)GetParent())->SendMessage(WM_SHOW_ROBOT_CHIP, 0, 0);
	}
}

//刷新显示数据
void CSuperGM::UpdateListData(SUPERDATA *pSuperData)
{
		CString str;
		//str.Format("shenzheng UpdateListData  insert allnote %d",pSuperData->sUserInfo[i].iAllNote);
		//OutputDebugString(str);

	////避免闪烁
	//m_ListCtrl.LockWindowUpdate();
	//m_ListCtrl.SetRedraw(TRUE);
	m_ListCtrl.DeleteAllItems();
	//m_ListCtrl.UnlockWindowUpdate();	

	int num=0;
	for(int i=0;i<PLAY_COUNT ;++i)
	{
		//没有下注 和 是机器人并不要求显示机器人信息的都不显示
		if(pSuperData->sUserInfo[i].iAllNote<=0 || (pSuperData->sUserInfo[i].bIsRobot && !m_bShowRobotInfo))
			continue;

		SUPERGM userinfo = pSuperData->sUserInfo[i];
		TCHAR buf[512];
		int count = 1;
		if (count > 50)
		{
			return;
		}
		//玩家姓名
		m_ListCtrl.InsertItem(num, userinfo.cUserName);

		//玩家初始金币
		memset(buf, 0, 512 * sizeof(TCHAR));
		wsprintf(buf, _T("%I64d"), pSuperData->sUserInfo[i].iOldMoney);
		m_ListCtrl.SetItemText(num, count++, buf);


		//玩家总下注金币
		memset(buf, 0, 512 * sizeof(TCHAR));
		wsprintf(buf, _T("%I64d"), pSuperData->sUserInfo[i].iAllNote);
		m_ListCtrl.SetItemText(num, count++, buf);

		//区域0下注金币
		memset(buf, 0, 512 * sizeof(TCHAR));
		wsprintf(buf, _T("%I64d"), pSuperData->sUserInfo[i].iUserNote[0]);
		m_ListCtrl.SetItemText(num, count++, buf);

		//区域1下注金币
		memset(buf, 0, 512 * sizeof(TCHAR));
		wsprintf(buf, _T("%I64d"), pSuperData->sUserInfo[i].iUserNote[1]);
		m_ListCtrl.SetItemText(num, count++, buf);

		//区域2下注金币
		memset(buf, 0, 512 * sizeof(TCHAR));
		wsprintf(buf, _T("%I64d"), pSuperData->sUserInfo[i].iUserNote[2]);
		m_ListCtrl.SetItemText(num, count++, buf);

		//区域3下注金币
		memset(buf, 0, 512 * sizeof(TCHAR));
		wsprintf(buf, _T("%I64d"), pSuperData->sUserInfo[i].iUserNote[3]);
		m_ListCtrl.SetItemText(num, count++, buf);

		//区域4下注金币
		memset(buf, 0, 512 * sizeof(TCHAR));
		wsprintf(buf, _T("%I64d"), pSuperData->sUserInfo[i].iUserNote[4]);
		m_ListCtrl.SetItemText(num, count++, buf);

		//区域5下注金币
		memset(buf, 0, 512 * sizeof(TCHAR));
		wsprintf(buf, _T("%I64d"), pSuperData->sUserInfo[i].iUserNote[5]);
		m_ListCtrl.SetItemText(num, count++, buf);

		//区域6下注金币
		memset(buf, 0, 512 * sizeof(TCHAR));
		wsprintf(buf, _T("%I64d"), pSuperData->sUserInfo[i].iUserNote[6]);
		m_ListCtrl.SetItemText(num, count++, buf);

		//区域7下注金币
		memset(buf, 0, 512 * sizeof(TCHAR));
		wsprintf(buf, _T("%I64d"), pSuperData->sUserInfo[i].iUserNote[7]);
		m_ListCtrl.SetItemText(num, count++, buf);

		num++;
		CString str;
		str.Format("shenzheng TTTTTTTTTTTTTTTT  insert dlg  -> %d", i);
		OutputDebugString(str);
	}
	m_ListCtrl.SetRedraw(TRUE);
}

///更新控制提示
void CSuperGM::UpdataCtrlText()
{
	CStatic *pStatic = (CStatic *)GetDlgItem(IDC_STATIC_INFO);
	switch(m_SelectedArea)
	{
	case 1://控制开庄
		{
			pStatic->SetWindowTextA("当前控制开庄");
		}break;
	case 2://控制开闲
		{
			pStatic->SetWindowTextA("当前控制开闲");
		}break;
	case 3://控制开和
		{
			pStatic->SetWindowTextA("当前控制开和");
		}break;
	default:
		{
			pStatic->SetWindowTextA("当前不做控制");
		}break;
	}
}
void CSuperGM::UpDataNoteRateInfo(__int64 iNtWin , __int64 iXianWin ,__int64 iHeWin) 
{
	CString strNoteInfo ; 

	CStatic *pStatic = (CStatic *)GetDlgItem(IDC_STATIC_ZHUANG_TOTLE);
	if(NULL != pStatic)
	{
		strNoteInfo.Format("%I64d" , iNtWin) ; 
		pStatic->SetWindowTextA(strNoteInfo.GetBuffer()) ;
	}
    
	pStatic = (CStatic *)GetDlgItem(IDC_STATIC_XIAN_TOTLE);
	if(NULL != pStatic)
	{
		strNoteInfo.Format("%I64d" , iXianWin) ; 
		pStatic->SetWindowTextA(strNoteInfo.GetBuffer()) ;
	}
	
	pStatic = (CStatic *)GetDlgItem(IDC_STATIC_HE_TOTLE);
	if(NULL != pStatic)
	{
		strNoteInfo.Format("%I64d" , iHeWin) ; 
		pStatic->SetWindowTextA(strNoteInfo.GetBuffer()) ;
	}
}
///显示窗口
void CSuperGM::ShowSuperWindow(bool show)
{
	ShowWindow(show);
	UpdataCtrlText();
}

///发送控制消息
void CSuperGM::SendCtrlMsg()
{
	if(g_pCClientGameDlg != NULL)
	{
		g_pCClientGameDlg->OnSuperCtr(m_SelectedArea,0);
	}
}

//设置控制结果
void CSuperGM::SetCtrlResult(BYTE result)
{
	m_SelectedArea = result;
	UpdataCtrlText();

	CButton *pButton = NULL;
	pButton	= (CButton *)GetDlgItem(IDC_RADIO_Z);
	pButton->SetCheck(0);
	pButton = (CButton *)GetDlgItem(IDC_RADIO_X);
	pButton->SetCheck(0);
	pButton = (CButton *)GetDlgItem(IDC_RADIO_H);
	pButton->SetCheck(0);
	pButton = (CButton *)GetDlgItem(IDC_RADIO_N);
	pButton->SetCheck(0);
	if(m_SelectedArea>=1 && m_SelectedArea<=3)
	{
		pButton	= (CButton *)GetDlgItem(IDC_RADIO_Z + m_SelectedArea -1);
		pButton->SetCheck(1);
	}
	else
	{
		pButton = (CButton *)GetDlgItem(IDC_RADIO_N);
		pButton->SetCheck(1);
	}
}
