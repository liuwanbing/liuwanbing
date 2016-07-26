// LeaveMessage.cpp : 实现文件
//

#include "stdafx.h"
#include "LeaveMessage.h"
#include "ClientGameDlg.h"

// CLeaveMessage 对话框

IMPLEMENT_DYNAMIC(CLeaveMessage, CGameFaceGo)

CLeaveMessage::CLeaveMessage(CWnd* pParent /*=NULL*/)
	: CGameFaceGo(CLeaveMessage::IDD)
{
	m_pParentWnd = pParent;
}

CLeaveMessage::~CLeaveMessage()
{
}

void CLeaveMessage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HAVETHING_NICKNAME, m_NickName);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
}


BEGIN_MESSAGE_MAP(CLeaveMessage, CGameFaceGo)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// CLeaveMessage 消息处理程序

BOOL CLeaveMessage::OnInitDialog()
{
	CGameFaceGo::OnInitDialog();

	TCHAR szPath[MAX_PATH]={0};
	_stprintf_s(szPath, sizeof(szPath), TEXT("image\\thingmsg.bmp"), SKIN_FOLDER);
	m_imgBackground.SetLoadInfo(szPath,false);

	CGameImageHelper imgHelper(&m_imgBackground);
	HRGN hRgn=AFCBmpToRgn(imgHelper,m_imgBackground.GetPixel(0,0),RGB(1,1,1));
	if (hRgn!=NULL)
	{
		SetWindowRgn(hRgn,TRUE);
	}

	SetWindowPos(&wndTopMost,0,0,imgHelper.GetWidth(),imgHelper.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);
	CenterWindow(GetParent());

	_stprintf_s(szPath, sizeof(szPath), TEXT("image\\agree_bt.bmp"),SKIN_FOLDER);
	m_btnOK.LoadButtonBitmap(szPath,false);
	_stprintf_s(szPath, sizeof(szPath), TEXT("image\\disagree_bt.bmp"),SKIN_FOLDER);
	m_btnCancel.LoadButtonBitmap(szPath,false);

	if(NULL != m_btnOK.GetSafeHwnd())
	{
		CRect rectClient;
		CRect rect;
		GetClientRect(&rectClient);
		m_btnOK.GetClientRect(&rect);
		int x = rectClient.Width()/2-rect.Width()-20;   //
		int y = rectClient.Height()-40;
		m_btnOK.MoveWindow(x,y,rect.Width(),rect.Height(),false);
		x =  rectClient.Width()/2+20;
		m_btnCancel.MoveWindow(x,y,rect.Width(),rect.Height(),false);
		CString s = CBcfFile::GetAppPath ();/////本地路径
		CBcfFile f( s + SKIN_FOLDER + _T(".bcf"));
		CString strKey = TEXT("LeaveMessageDlg");
		CPoint ptPoint = CPoint(0,0);
		TCHAR szValueName[_MAX_PATH] = {_T("LeaveNickName")};
		_stprintf_s(szValueName, sizeof(szValueName), _T("LeaveNickNamex"));
		ptPoint.x = f.GetKeyVal(strKey, szValueName, 19);
		_stprintf_s(szValueName, sizeof(szValueName), _T("LeaveNickNamey"));
		ptPoint.y = f.GetKeyVal(strKey, szValueName, 47);
		_stprintf_s(szValueName, sizeof(szValueName), _T("LeaveNickNameWidth"));
		int iWidth = f.GetKeyVal(strKey, szValueName, 100);
		_stprintf_s(szValueName, sizeof(szValueName), _T("LeaveNickNameHeight"));
		int iHeight = f.GetKeyVal(strKey, szValueName, 12);
		GetDlgItem(IDC_HAVETHING_NICKNAME)->MoveWindow(ptPoint.x,ptPoint.y,iWidth,iHeight,false);

		ptPoint = CPoint(0,0);
		_stprintf_s(szValueName, sizeof(szValueName), _T("LeaveMsgx"));
		ptPoint.x = f.GetKeyVal(strKey, szValueName, 15);
		_stprintf_s(szValueName, sizeof(szValueName), _T("LeaveMsgy"));
		ptPoint.y = f.GetKeyVal(strKey, szValueName, 70);
		_stprintf_s(szValueName, sizeof(szValueName), _T("LeaveMsgWidth"));
		iWidth = f.GetKeyVal(strKey, szValueName, 290);
		_stprintf_s(szValueName, sizeof(szValueName), _T("LeaveMsgHeight"));
		iHeight = f.GetKeyVal(strKey, szValueName, 65);
		GetDlgItem(IDC_HAVETHING_MSG)->MoveWindow(ptPoint.x,ptPoint.y,iWidth,iHeight,false);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CLeaveMessage::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CGameImageHelper	help(&m_imgBackground);
	help.BitBlt(dc.GetSafeHdc(),0,0);
}

//取消函数
void CLeaveMessage::OnCancel()
{
	if (NULL != m_pParentWnd)
	{
		// 不同意
		((CClientGameDlg *)m_pParentWnd)->SendMessage(WM_USER_AGREE_LEAVE, USER_DISAGREE_LEAVE,USER_DISAGREE_LEAVE);
	}
	CGameFaceGo::OnCancel();
}

//确定函数
void CLeaveMessage::OnOK()
{
	if (NULL != m_pParentWnd)
	{
		// 不同意
		((CClientGameDlg *)m_pParentWnd)->SendMessage(WM_USER_AGREE_LEAVE, USER_AGREE_LEAVE, USER_AGREE_LEAVE);
	}
	CGameFaceGo::OnOK();
}

//设置信息
void CLeaveMessage::SetLeaveMessage(TCHAR * szMessage, TCHAR * szUserName)
{
	// 玩家昵称
	CWnd *pWnd = GetDlgItem(IDC_HAVETHING_NICKNAME);
	if (NULL != pWnd)
	{
		pWnd->SetWindowText(szUserName);
	}
	
	// 有事离开的原因
	pWnd = GetDlgItem(IDC_HAVETHING_MSG);
	if (NULL != pWnd)
	{
		pWnd->SetWindowText(szMessage);
	}
	return;
}

void CLeaveMessage::OnLButtonDown(UINT nFlags,CPoint point)
{
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
}


BOOL CLeaveMessage::Create(UINT nIDTemplate, CWnd* pParentWnd )
{
	// TODO: 在此添加专用代码和/或调用基类
	m_pParentWnd = pParentWnd;

	return CGameFaceGo::Create(nIDTemplate, pParentWnd);
}
