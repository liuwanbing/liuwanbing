// HaveThing.cpp : 实现文件
//

#include "stdafx.h"
#include "HaveThing.h"
#include "ClientGameDlg.h"

// CHaveThing 对话框

IMPLEMENT_DYNAMIC(CHaveThing, CGameFaceGo)

CHaveThing::CHaveThing(CWnd* pParent /*=NULL*/)
	: CGameFaceGo(CHaveThing::IDD)
{
	m_pParentWnd = pParent;
	m_bIsClick = false;
}

CHaveThing::~CHaveThing()
{
}

void CHaveThing::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
}


BEGIN_MESSAGE_MAP(CHaveThing, CGameFaceGo)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_EN_SETFOCUS(IDC_THING,&CHaveThing::OnEnSetfocusEdit)
END_MESSAGE_MAP()


// CHaveThing 消息处理程序

void CHaveThing::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	char szBuffer[HAVE_THING_LENGHT]={0};
	int iCount=GetDlgItem(IDC_THING)->GetWindowText(szBuffer,sizeof(szBuffer));
	if (iCount==0||!m_bIsClick)
	{
		AFCMessageBox(TEXT("请在输入框中输入离开的理因！"), _T("提示"));
		GetDlgItem(IDC_THING)->SetFocus();
		return;
	}
	szBuffer[HAVE_THING_LENGHT-1]=0;   // 保持结束符
	if(NULL != m_pParentWnd)
	{
		((CClientGameDlg *)m_pParentWnd)->SendMessage(WM_USER_SEND_THING,(WPARAM)szBuffer,(LPARAM)szBuffer);
	}
	CGameFaceGo::OnOK();
}

void CHaveThing::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	CGameFaceGo::OnCancel();
}

BOOL CHaveThing::OnInitDialog()
{
	CGameFaceGo::OnInitDialog();

	TCHAR szPath[MAX_PATH]={0};
	_stprintf_s(szPath, sizeof(szPath), TEXT("image\\havething.bmp"), SKIN_FOLDER);
	m_imgBackground.SetLoadInfo(szPath,true);

	((CEdit *)GetDlgItem(IDC_THING))->LimitText(HAVE_THING_LENGHT-1);
	
	CGameImageHelper imgHelper(&m_imgBackground);
	HRGN hRgn=AFCBmpToRgn(imgHelper,m_imgBackground.GetPixel(0,0),RGB(1,1,1));
	if (hRgn!=NULL)
	{
		SetWindowRgn(hRgn,TRUE);
	}

	SetWindowPos(&wndBottom,0,0,imgHelper.GetWidth(),imgHelper.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);
	CenterWindow(GetParent());

	_stprintf_s(szPath, sizeof(szPath), TEXT("image\\send.bmp"),SKIN_FOLDER);
	m_btnOK.LoadButtonBitmap(szPath,false);
	_stprintf_s(szPath, sizeof(szPath), TEXT("image\\cancel.bmp"),SKIN_FOLDER);
	m_btnCancel.LoadButtonBitmap(szPath,false);
	if(m_btnOK.GetSafeHwnd())
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
		CString strKey = TEXT("HaveThingDlg");
		CPoint ptPoint = CPoint(0,0);
		TCHAR szValueName[_MAX_PATH] = {_T("ThingMsgx")};
		_stprintf_s(szValueName, sizeof(szValueName), _T("ThingMsgx"));
		ptPoint.x = f.GetKeyVal(strKey, szValueName, 15);
		_stprintf_s(szValueName, sizeof(szValueName), _T("ThingMsgy"));
		ptPoint.y = f.GetKeyVal(strKey, szValueName, 70);
		_stprintf_s(szValueName, sizeof(szValueName), _T("ThingMsgWidth"));
		int iWidth = f.GetKeyVal(strKey, szValueName, 290);
		_stprintf_s(szValueName, sizeof(szValueName), _T("ThingMsgHeight"));
		int iHeight = f.GetKeyVal(strKey, szValueName, 45);
		GetDlgItem(IDC_THING)->MoveWindow(ptPoint.x,ptPoint.y,iWidth,iHeight,false);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CHaveThing::OnPaint()
{	
	CPaintDC dc(this);
	CGameImageHelper	help(&m_imgBackground);
	help.BitBlt(dc.GetSafeHdc(),0,0);
}

void CHaveThing::OnLButtonDown(UINT nFlags, CPoint point)
{
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
}

void CHaveThing::OnEnSetfocusEdit()
{
	if(m_bIsClick==false)
	{
		m_bIsClick=true;
		((CEdit *)GetDlgItem(IDC_THING))->SetWindowText("");
	}
}

BOOL CHaveThing::Create(UINT nIDTemplate, CWnd* pParentWnd )
{
	// TODO: 在此添加专用代码和/或调用基类
	m_pParentWnd = pParentWnd;

	return CGameFaceGo::Create(nIDTemplate, pParentWnd);
}
