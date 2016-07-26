// IMSetUser.cpp : 实现文件
//

#include "stdafx.h"
#include "DllForGame.h"
#include "IMSetUser.h"


// CIMSetUser 对话框

IMPLEMENT_DYNAMIC(CIMSetUser, CDialog)

CIMSetUser::CIMSetUser(CWnd* pParent /*=NULL*/)
	: CDialog(CIMSetUser::IDD, pParent)
{

}

CIMSetUser::~CIMSetUser()
{
}

void CIMSetUser::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_ctlWords);
	DDX_Control(pDX, IDC_STATE, ctlTip);
}


BEGIN_MESSAGE_MAP(CIMSetUser, CDialog)
	ON_BN_CLICKED(IDOK, &CIMSetUser::OnBnClickedOk)
END_MESSAGE_MAP()


// CIMSetUser 消息处理程序

void CIMSetUser::OnBnClickedOk()
{
	m_ctlWords.GetWindowText(imWords);
	imWords.Trim();
	if(imWords.GetLength()>100)
		imWords=imWords.Left(100);//不能超长
	OnOK();
}

BOOL CIMSetUser::OnInitDialog()
{
	CDialog::OnInitDialog();

	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	CString str = fMsg.GetKeyVal("MainRoom","MakeFriendLeaveMsg","您向用户[%s] 请求加为好友，请输入请求留言：");
	CString s;
	s.Format(str,szName);
	ctlTip.SetWindowText(s);
	imWords="";


	SetWindowPos(&wndTopMost,   0,   0,   0,   0,   SWP_NOMOVE|SWP_NOSIZE|WS_EX_TOPMOST); 

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}



// CIMSetUser 消息处理程序
