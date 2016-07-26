// AwardDlg.cpp : 实现文件


//////////////////////////////////////////////////////////////////////////
///奖励弹出窗口，让用户填写相应数据，然后通知主窗口
///2010.8.12 rongqiufen
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TCP.h"
#include "AwardDlg.h"



// CAwardDlg 对话框

IMPLEMENT_DYNAMIC(CAwardDlg, CDialog)

CAwardDlg::CAwardDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAwardDlg::IDD, pParent)
	, m_UserID(0)
	, m_UserNickName(_T(""))
	, m_UserLoginName(_T(""))
	, m_Reason(_T(""))
	, m_Details(_T(""))
{

}

CAwardDlg::~CAwardDlg()
{
}

void CAwardDlg::SetUserInfo(UserInfoStruct *userinfo, int iRoomID)
{
	m_UserID = userinfo->dwUserID;
	m_UserNickName = userinfo->nickName;
	m_UserLoginName = userinfo->szName;
	m_RoomID = iRoomID;
}

void CAwardDlg::SetUserInfo(GM_OperatedPlayerInfo_t *userinfo)
{
	m_UserID = userinfo->iUserID;
	m_UserNickName = userinfo->szNickName;
	m_UserLoginName = userinfo->szUserName;
	m_RoomID = userinfo->WID;
}

void CAwardDlg::SetAwardProjectList(map<int, GM_AwardType_t*> vAwardProjectList)
{
	m_AwardType.clear();
	map<int, GM_AwardType_t*>::iterator iter;
	for(iter = vAwardProjectList.begin(); iter != vAwardProjectList.end(); iter++)
	{
		m_AwardType.push_back(iter->second);
	}
}
void CAwardDlg::SetAwardList(map<int, GM_GetPropInfo_t*> vAwardList)
{
	m_PropList.clear();
	map<int, GM_GetPropInfo_t*>::iterator iter;
	for(iter = vAwardList.begin(); iter != vAwardList.end(); iter++)
	{
		m_PropList.insert(*iter);
	}
}

void CAwardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT4, m_UserID);
	DDX_Text(pDX, IDC_EDIT5, m_UserNickName);
	DDX_Text(pDX, IDC_EDIT6, m_UserLoginName);
	DDX_Control(pDX, IDC_COMBO1, m_AwardCmb);
	DDX_Text(pDX, IDC_EDIT1, m_Details);
	DDX_Text(pDX, IDC_EDIT2, m_Reason);
	DDV_MaxChars(pDX, m_Reason, 64);
	
}


BEGIN_MESSAGE_MAP(CAwardDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CAwardDlg::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CAwardDlg::OnCbnSelchangeCombo1)
END_MESSAGE_MAP()


// CAwardDlg 消息处理程序

void CAwardDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
}

void CAwardDlg::OnCbnSelchangeCombo1()
{
	// TODO: 在此添加控件通知处理程序代码

	int selectindex = m_AwardCmb.GetCurSel();
	if (selectindex >= 0)
	{
		CString s1, s2;
		s1.Format("金币:%d\r\n", m_AwardType[selectindex]->iGoldNum);
		for (int i = 0; i < 4; i++)
		{
			if (m_AwardType[selectindex]->iPropAward[i].iCount != 0)
			{
				s2.Format("%s:%d\r\n", m_PropList[m_AwardType[selectindex]->iPropAward[i].iPropID]->szPropName, m_AwardType[selectindex]->iPropAward[i].iCount);
				s1 += s2;
			}
		}
		m_Details = s1;
		UpdateData(false);
	}
}


void CAwardDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	UpdateData();
	if (m_Reason.GetLength() <= 0) 
	{
		AfxMessageBox("操作原因不能为空!");
		return;
	}

	int selectindex = m_AwardCmb.GetCurSel();
	if (0 <= selectindex && selectindex < m_AwardType.size())

	{
		ZeroMemory(&m_GMOperate, sizeof(m_GMOperate));
		m_GMOperate.iUserID = m_UserID;
		m_GMOperate.iWorkMinutes = m_AwardType[selectindex]->iSeriNo;
		memcpy(m_GMOperate.szNickName, m_UserNickName.GetBuffer(), m_UserNickName.GetLength());
		memcpy(m_GMOperate.szReason, m_Reason.GetBuffer(), m_Reason.GetLength());
				

		m_GMOperate.iOperation = GM_ACT_AWORD;
		///通知主窗口
		AfxGetMainWnd()->SendMessage(MSG_CMD_OPERATE, WPARAM(&m_GMOperate), LPARAM(&m_RoomID));

		m_Reason = "";

		UpdateData(false);

		CDialog::OnOK();
	}
}

BOOL CAwardDlg::OnInitDialog()
{
	CDialog::OnInitDialog();


	// TODO:  在此添加额外的初始化
	((CEdit*)GetDlgItem(IDC_EDIT2))->SetLimitText(32);
	CString s;
	for (int i = 0; i < m_AwardType.size(); i++)
	{
		s.Format("方案%d", m_AwardType[i]->iSeriNo);
		m_AwardCmb.AddString(s);
	}
	if (m_AwardType.size() > 0) m_AwardCmb.SetCurSel(0);
	OnCbnSelchangeCombo1();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
