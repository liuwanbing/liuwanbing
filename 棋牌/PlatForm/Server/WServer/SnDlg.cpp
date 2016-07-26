// SnDlg.cpp : 实现文件
//

#include "stdafx.h"
//#include "游戏登陆.2003.h"
#include "SnDlg.h"
#include ".\sndlg.h"
#include "SetDlg.h"


// CSnDlg 对话框

IMPLEMENT_DYNAMIC(CSnDlg, CDialog)
CSnDlg::CSnDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSnDlg::IDD, pParent)
{
	m_Sn=0;
	m_bOK=true;
}

CSnDlg::~CSnDlg()
{
}

void CSnDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSnDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, OnBnClickedButton7)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CSnDlg 消息处理程序

void CSnDlg::OnBnClickedButton1()
{
	m_Sn+=1;
}

void CSnDlg::OnBnClickedButton2()
{
	m_Sn+=2;
}

void CSnDlg::OnBnClickedButton3()
{
	m_Sn+=4;
	m_bOK=false;
	CSetDlg dlg;
	dlg.DoModal();
}

void CSnDlg::OnBnClickedButton4()
{
	m_Sn+=8;
	m_bOK=false;
}

void CSnDlg::OnBnClickedButton5()
{
	m_Sn+=16;
}

void CSnDlg::OnBnClickedButton6()
{
	m_Sn+=32;
}

void CSnDlg::OnBnClickedButton7()
{
	m_Sn+=64;
}

//2100141
void CSnDlg::OnBnClickedOk()
{
	if(m_Sn==212 && m_bOK)
		OnOK();
	else
		AfxMessageBox("请首先配制服务器");
}
