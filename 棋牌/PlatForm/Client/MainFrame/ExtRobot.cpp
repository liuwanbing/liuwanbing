// ExtRobot.cpp : 实现文件
//

#include "stdafx.h"
#include "ExtRobot.h"


// CExtRobot 对话框

IMPLEMENT_DYNAMIC(CExtRobot, CDialog)

CExtRobot::CExtRobot(CWnd* pParent /*=NULL*/)
	: CDialog(CExtRobot::IDD, pParent)
	, m_extRobot(0)
{

}

CExtRobot::~CExtRobot()
{
}

void CExtRobot::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_extRobot);
}


BEGIN_MESSAGE_MAP(CExtRobot, CDialog)
	ON_BN_CLICKED(IDOK, &CExtRobot::OnBnClickedOk)
END_MESSAGE_MAP()


// CExtRobot 消息处理程序

void CExtRobot::OnBnClickedOk()
{
	UpdateData();
	OnOK();
}
