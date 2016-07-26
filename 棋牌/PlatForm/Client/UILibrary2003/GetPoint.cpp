// GetPoint.cpp : 实现文件
//

#include "stdafx.h"
#include "Resource.h"
#include "GetPoint.h"
#include "GameFrameDlg.h"

// CGetPoint 对话框

IMPLEMENT_DYNAMIC(CGetPoint, CDialog)

CGetPoint::CGetPoint(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_GETPOINT, pParent)
{
	m_pSend = NULL;
	m_SerPoint = 0;
	m_iInWhere = 0;
	m_bkBrush = NULL;
}

CGetPoint::~CGetPoint()
{
	DeleteObject (m_bkBrush);
}

void CGetPoint::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDCANCEL, m_btcancel);
	DDX_Control(pDX, IDOK, m_btok);
}


BEGIN_MESSAGE_MAP(CGetPoint, CDialog)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDOK, &CGetPoint::OnBnClickedOk)
END_MESSAGE_MAP()


// CGetPoint 消息处理程序

LRESULT CGetPoint::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	//case WM_CTLCOLORSCROLLBAR :
	//case WM_CTLCOLORBTN:
	//case WM_CTLCOLORMSGBOX:
	//case WM_CTLCOLOREDIT:
	//case WM_CTLCOLORLISTBOX:
	//case WM_CTLCOLORDLG:
	case WM_CTLCOLORSTATIC :
		SetTextColor((HDC)wParam, RGB(0,0,0)) ;
		SetBkColor((HDC)wParam, m_bkimage.GetPixel(145,35));
		return (LRESULT)m_bkBrush;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

void CGetPoint::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CGameImageHelper	handle(&m_bkimage);
	handle.BitBlt(dc.GetSafeHdc(),0,0);
}

BOOL CGetPoint::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString s=CBcfFile::GetAppPath ();/////本地路径
	CINIFile f( s + Glb().m_skin);

	CString key=TEXT("config");
	TCHAR path[MAX_PATH];
	CString skinfolder;
	skinfolder=f.GetKeyVal(key,"skinfolder","image\\game\\");
	skinfolder=s+"\\"+skinfolder;
	wsprintf(path,"%sgame\\point\\getpoint_bk.bmp",skinfolder);


	CGameImage	m_bt;
	m_bkimage.SetLoadInfo(path,CGameImageLink::m_bAutoLock);

	CGameImageHelper ImageHandle(&m_bkimage);
	int cx,cy;
	HRGN hRgn=AFCBmpToRgn(ImageHandle,RGB(255, 0, 255),RGB(1,1,1));
	if (hRgn!=NULL)
	{
		SetWindowRgn(hRgn,TRUE);
		///{{ Added by zxd 20100709 释放区域对象
		DeleteObject(hRgn);
		///Added by zxd 20100709 释放区域对象}}
	}
	cx=ImageHandle.GetWidth();
	cy=ImageHandle.GetHeight();
	SetWindowPos(NULL,0,0,cx,cy,SWP_NOMOVE|SWP_NOZORDER);

	wsprintf(path,"%sgame\\point\\ok_bt.bmp",skinfolder);
	m_btok.LoadButtonBitmap(path,false);

	wsprintf(path,"%sgame\\point\\cancel_bt.bmp",skinfolder);
	m_btcancel.LoadButtonBitmap(path,false);


	m_bkBrush = CreateSolidBrush(m_bkimage.GetPixel(145,35));
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CGetPoint::OnLButtonDown(UINT nFlags, CPoint point)
{
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
	//CDialog::OnLButtonDown(nFlags, point);
}

void CGetPoint::OnBnClickedOk()
{
	DWORD point = GetDlgItemInt(IDC_POINT);
	if(point <= 0 || point > m_SerPoint)
	{
		//AFCMessageBox("请正确输入积分值!", "提示");
		DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示","请正确输入积分值!");
		return ;
	}
	if(point > 1000000)
	{
		//AFCMessageBox("一次最多只能取100万!", "提示");
		DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示","一次最多只能取100万!");
		return ;
	}
	//GetParent()->
	m_pSend->PostMessage(IDM_GETPOINT,point,m_iInWhere);

	ShowWindow(false);
}

void CGetPoint::SetPoint(DWORD dwPoint)
{
	TCHAR sz[100];
	m_SerPoint = dwPoint;
	wsprintf(sz, TEXT("你的积分盒里有 %d 积分."), dwPoint);
	SetDlgItemText(IDC_SERPOINT, sz);
}
