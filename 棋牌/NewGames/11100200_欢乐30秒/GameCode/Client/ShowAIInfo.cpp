// ShowAIInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "ShowAIInfo.h"
#include "ClientGameDlg.h"
#define  BaseX  50
#define  BaseY  50
// CShowAIInfo 对话框
///机器人输赢控制，信息显示类
IMPLEMENT_DYNAMIC(CShowAIInfo, CDialog)

CShowAIInfo::CShowAIInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CShowAIInfo::IDD, pParent)
	, m_GoalMoney(0)
	, m_iAveMoney(0)
{
	bIsClearAIWinMoney	= false;		//已赢钱清零
	bIsSetAICtrl	= false;			//修改设置
	bAIWinAndLostAutoCtrl	= false;	//开启控制
}

CShowAIInfo::~CShowAIInfo()
{
}

void CShowAIInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_GOAL_MONEY, m_GoalMoney);
	DDX_Text(pDX, IDC_EDIT_AVE_MONEY, m_iAveMoney);
}


BEGIN_MESSAGE_MAP(CShowAIInfo, CDialog)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUT_SEND_CHANGE, &CShowAIInfo::OnBnClickedButSendChange)
	ON_BN_CLICKED(IDC_CH_CHANGE, &CShowAIInfo::OnBnClickedChChange)
	ON_BN_CLICKED(IDC_CH_CLEAR_WIN, &CShowAIInfo::OnBnClickedChClearWin)
	ON_BN_CLICKED(IDC_CH_CHANGE_SET, &CShowAIInfo::OnBnClickedChChangeSet)
	ON_EN_CHANGE(IDC_EDIT_GOAL_MONEY, &CShowAIInfo::OnEnChangeEditGoalMoney)
	ON_EN_CHANGE(IDC_EDIT_AVE_MONEY, &CShowAIInfo::OnEnChangeEditAveMoney)
END_MESSAGE_MAP()


// CShowAIInfo 消息处理程序

BOOL CShowAIInfo::OnInitDialog() 
{
	CDialog::OnInitDialog();
	this->CenterWindow(this);

	((CButton *)GetDlgItem(IDC_CH_SET_CTRL))->SetCheck(BST_CHECKED);	///启动控制

	m_GoalMoney = m_AIInfo.iAIWantWinMoney;
	m_iAveMoney = m_AIInfo.iAIMaxWinInOne;

	CString str="";
	str.Format("%d",m_GoalMoney);
	((CEdit *)GetDlgItem(IDC_EDIT_GOAL_MONEY))->SetWindowTextA(str);
	str.Format("%d",m_iAveMoney);
	((CEdit *)GetDlgItem(IDC_EDIT_AVE_MONEY))->SetWindowTextA(str);

	return TRUE;
}


void CShowAIInfo::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()
	CFont font, *pOldFont;
	font.CreateFont(-13,0,0,0,400,NULL,NULL,NULL,134,3,2,1,2,TEXT("宋体"));
	pOldFont = (CFont*)dc.SelectObject(&font);
	SetTextColor(dc.GetSafeHdc(), RGB(0,0,0));

	dc.SetBkMode(TRANSPARENT);
	CString str="当前机器人输赢控制如下";
	dc.TextOutA(BaseX,BaseY,str);

	if(m_AIInfo.bAIWinAndLostAutoCtrl)
	{
		str="当前机器人输赢控制处于开启状态";
	}
	else
	{
		str="当前机器人输赢控制处于关闭状态";
	}
	dc.TextOutA(BaseX,BaseY+40,str);

	str.Format("机器人赢钱目标：%d",m_AIInfo.iAIWantWinMoney);
	dc.TextOutA(BaseX,BaseY+60,str);

	str.Format("机器人已经赢了：%d",m_AIInfo.iAIHaveWinMoney);
	dc.TextOutA(BaseX,BaseY+80,str);

	str.Format("机器人平均每把赢钱上限为：%d",m_AIInfo.iAIMaxWinInOne);
	dc.TextOutA(BaseX,BaseY+100,str);

	if(m_AIInfo.iAICtrlCount<=0)
		m_AIInfo.iAICtrlCount=1;
	str.Format("当前机器人赢钱平均数：%d",m_AIInfo.iAIHaveWinMoney/m_AIInfo.iAICtrlCount);
	dc.TextOutA(BaseX,BaseY+120,str);

	str.Format("机器人已经进行自动控制的把数：%d",m_AIInfo.iAICtrlCount);
	dc.TextOutA(BaseX,BaseY+140,str);
	dc.SelectObject(pOldFont);
	font.DeleteObject();

}
//显示对话框
void CShowAIInfo::ShowAIInfoWindow()
{

	((CButton *)GetDlgItem(IDC_CH_CLEAR_WIN))->EnableWindow(false);		///当前清零
	((CButton *)GetDlgItem(IDC_CH_CHANGE_SET))->EnableWindow(false);	///修改配置
	((CButton *)GetDlgItem(IDC_BUT_SEND_CHANGE))->EnableWindow(false);	///发送消息按钮

	((CButton *)GetDlgItem(IDC_CH_SET_CTRL))->EnableWindow(false);		///开启控制
	((CEdit *)GetDlgItem(IDC_EDIT_GOAL_MONEY))->EnableWindow(false);	///赢钱目标
	((CEdit *)GetDlgItem(IDC_EDIT_AVE_MONEY))->EnableWindow(false);		///赢钱平均值

	((CButton *)GetDlgItem(IDC_CH_CLEAR_WIN))->SetCheck(BST_UNCHECKED);
	((CButton *)GetDlgItem(IDC_CH_CHANGE_SET))->SetCheck(BST_UNCHECKED);
	((CButton *)GetDlgItem(IDC_CH_CHANGE))->SetCheck(BST_UNCHECKED);	

	m_GoalMoney = m_AIInfo.iAIWantWinMoney;


	CString str="";
	str.Format("%d",m_GoalMoney);
	((CEdit *)GetDlgItem(IDC_EDIT_GOAL_MONEY))->SetWindowTextA(str);

	str="";
	m_iAveMoney = m_AIInfo.iAIMaxWinInOne;
	str.Format("%d",m_iAveMoney);
	((CEdit *)GetDlgItem(IDC_EDIT_AVE_MONEY))->SetWindowTextA(str);

	((CEdit *)GetDlgItem(IDC_EDIT_GOAL_MONEY))->UpdateData(false);
	((CEdit *)GetDlgItem(IDC_EDIT_GOAL_MONEY))->UpdateWindow();

	((CEdit *)GetDlgItem(IDC_EDIT_AVE_MONEY))->UpdateData(false);
	((CEdit *)GetDlgItem(IDC_EDIT_AVE_MONEY))->UpdateWindow();

	this->ShowWindow(true);
}
///点击发送按钮
void CShowAIInfo::OnBnClickedButSendChange()
{
	// TODO: 在此添加控件通知处理程序代码
	if(((CButton *)GetDlgItem(IDC_CH_CHANGE))->GetCheck() == BST_CHECKED)///
	{
		this->UpdateWindow();
		if(((CButton *)GetDlgItem(IDC_CH_CLEAR_WIN))->GetCheck() == BST_CHECKED)//清零
		{
			bIsClearAIWinMoney = true;
		}
		if(((CButton *)GetDlgItem(IDC_CH_CHANGE_SET))->GetCheck() == BST_CHECKED)///修改配置
		{
			bAIWinAndLostAutoCtrl = false;
			bIsSetAICtrl = true;
			if(((CButton *)GetDlgItem(IDC_CH_SET_CTRL))->GetCheck() == BST_CHECKED)///启动控制
			{
				bAIWinAndLostAutoCtrl = true;
			}
		}
		CString str="";
		str.Format("控制信息：目标%d",m_GoalMoney);
		OutputDebugString(str);
		///发送修改消息
		this->GetParent()->SendMessage(IDM_SEND_AI_CTRL,0,0);
	}

}
//启动需要发送消息选项
void CShowAIInfo::OnBnClickedChChange()
{
	// TODO: 在此添加控件通知处理程序代码
	if(((CButton *)GetDlgItem(IDC_CH_CHANGE))->GetCheck() == BST_CHECKED)///
	{
		((CButton *)GetDlgItem(IDC_CH_CLEAR_WIN))->EnableWindow(true);	///当前清零
		((CButton *)GetDlgItem(IDC_CH_CHANGE_SET))->EnableWindow(true);	///修改配置
		((CButton *)GetDlgItem(IDC_BUT_SEND_CHANGE))->EnableWindow(true);	///发送消息按钮
	}
	else
	{
		((CButton *)GetDlgItem(IDC_CH_CLEAR_WIN))->EnableWindow(false);	///当前清零
		((CButton *)GetDlgItem(IDC_CH_CHANGE_SET))->EnableWindow(false);	///修改配置
		((CButton *)GetDlgItem(IDC_BUT_SEND_CHANGE))->EnableWindow(false);	///发送消息按钮

		((CButton *)GetDlgItem(IDC_CH_SET_CTRL))->EnableWindow(false);	///开启控制
		((CEdit *)GetDlgItem(IDC_EDIT_GOAL_MONEY))->EnableWindow(false);	///赢钱目标
		((CEdit *)GetDlgItem(IDC_EDIT_AVE_MONEY))->EnableWindow(false);	///赢钱平均值

		((CButton *)GetDlgItem(IDC_CH_CLEAR_WIN))->SetCheck(BST_UNCHECKED);
		((CButton *)GetDlgItem(IDC_CH_CHANGE_SET))->SetCheck(BST_UNCHECKED);
	}

}
///是否清零赢钱
void CShowAIInfo::OnBnClickedChClearWin()
{
	// TODO: 在此添加控件通知处理程序代码
}
//是否修改当前配置
void CShowAIInfo::OnBnClickedChChangeSet()
{
	// TODO: 在此添加控件通知处理程序代码
	if(((CButton *)GetDlgItem(IDC_CH_CHANGE_SET))->GetCheck() == BST_CHECKED)///
	{
		((CButton *)GetDlgItem(IDC_CH_SET_CTRL))->EnableWindow(true);	///开启控制
		((CEdit *)GetDlgItem(IDC_EDIT_GOAL_MONEY))->EnableWindow(true);	///赢钱目标
		((CEdit *)GetDlgItem(IDC_EDIT_AVE_MONEY))->EnableWindow(true);	///赢钱平均值
	}
	else 
	{
		((CButton *)GetDlgItem(IDC_CH_SET_CTRL))->EnableWindow(false);	///开启控制
		((CEdit *)GetDlgItem(IDC_EDIT_GOAL_MONEY))->EnableWindow(false);	///赢钱目标
		((CEdit *)GetDlgItem(IDC_EDIT_AVE_MONEY))->EnableWindow(false);	///赢钱平均值
	}

}

void CShowAIInfo::OnEnChangeEditGoalMoney()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	this->UpdateData(true);
}

void CShowAIInfo::OnEnChangeEditAveMoney()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	this->UpdateData(true);
}
