#include "Stdafx.h"
#include "UserProp.h"
//#include "ClientGameDlg.h"


BEGIN_MESSAGE_MAP(CUserProp, CDialog)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOVE()
END_MESSAGE_MAP()

//构造函数
CUserProp::CUserProp(CWnd * pParent) : CDialog(CUserProp::IDD,pParent)
{
	::memset(m_szName,0,sizeof(m_szName));
	m_iCurCount = 0;
	m_iCurDest = -1;
}

//析构函数
CUserProp::~CUserProp()
{
}

//DDX/DDV 支持
void CUserProp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDCANCEL, m_Cancel);
	DDX_Control(pDX, IDOK, m_Ok);
}

//确定按钮
void CUserProp::OnOK()
{
	m_iCurDest = ((CComboBox*)GetDlgItem(IDC_PROP_DEST))->GetCurSel();

	CDialog::OnOK();
	//PostMessage(IDM_PROP,0,0);
}

//初始化对话框
BOOL CUserProp::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString s=CINIFile::GetAppPath ();/////本地路径
	CINIFile f( s + m_iniFile);

	CString key=TEXT("config");
	TCHAR path[MAX_PATH];
	CString skinfolder;
	skinfolder=f.GetKeyVal(key,"skinfolder","image\\client\\game\\");
	skinfolder=s+"\\"+skinfolder;
	wsprintf(path,"%sgame\\button.bmp",skinfolder);



	TCHAR path[MAX_PATH];
	m_Ok.LoadButtonBitmap(path,false);
	m_Cancel.LoadButtonBitmap(path,false);
	wsprintf(path,"%sgame\\prop.bmp",skinfolder);
	//wsprintf(path,TEXT("image\\client\\game\\prop.bmp"));
	m_bk.SetLoadInfo(path,true);

	CGameImageHelper ImageHandle(&m_bk);
	HRGN hRgn=AFCBmpToRgn(ImageHandle,RGB(255,0,255),RGB(1,1,1));

	if (hRgn!=NULL)
	{
		SetWindowPos(NULL,0,0,ImageHandle.GetWidth(),ImageHandle.GetHeight(),SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOMOVE);
		SetWindowRgn(hRgn,TRUE);
	}

	MoveWindow(0,0,m_bk.GetWidth(),m_bk.GetHeight());
	CenterWindow(GetParent());
	
	InitUserList();
	AfxSetResourceHandle(GetModuleHandle(NULL));
	return TRUE; 
}

LRESULT CUserProp::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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
		SetTextColor((HDC)wParam, RGB(0,255,0)) ;
		//	SetBkMode(h
		SetBkColor((HDC)wParam,m_bk.GetPixel(10,10));//RGB(187,104,2));
		return (LRESULT)CreateSolidBrush(m_bk.GetPixel(10,10));//RGB(187,104,2));
	}
	//InvalidateRect(NULL);	
	return CDialog::WindowProc(message, wParam, lParam);
}

void CUserProp::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CGameImageHelper	help(&m_bk);	
	help.BitBlt(dc.GetSafeHdc(),0,0);	
	
	dc.SetBkMode(TRANSPARENT);
	dc.SelectObject(CreatePen(PS_SOLID,2,RGB(0,255,0)));
	//dc.SelectObject((CBrush*)GetStockObject(NULL_BRUSH));//变态啊．居然不能够透明
	//dc.Rectangle(&m_iRect[m_iCurIndex]);
	//没有办法才用画线处理
	dc.MoveTo(m_iRect[m_iCurIndex].left,m_iRect[m_iCurIndex].top);
	dc.LineTo(m_iRect[m_iCurIndex].left,m_iRect[m_iCurIndex].bottom);

	dc.MoveTo(m_iRect[m_iCurIndex].left,m_iRect[m_iCurIndex].bottom);
	dc.LineTo(m_iRect[m_iCurIndex].right,m_iRect[m_iCurIndex].bottom);

	dc.MoveTo(m_iRect[m_iCurIndex].right,m_iRect[m_iCurIndex].bottom);
	dc.LineTo(m_iRect[m_iCurIndex].right,m_iRect[m_iCurIndex].top);
	
	dc.MoveTo(m_iRect[m_iCurIndex].right,m_iRect[m_iCurIndex].top);
	dc.LineTo(m_iRect[m_iCurIndex].left,m_iRect[m_iCurIndex].top);
}


void CUserProp::AddUser(TCHAR * szUserName)
{
	lstrcpy(m_szName[m_iCurCount++],szUserName);
	//((CComboBox*)GetDlgItem(IDC_PROP_DEST))->AddString(szUserName);
	//((CComboBox*)GetDlgItem(IDC_PROP_DEST))->SetCurSel(0);
}

void CUserProp::OnLButtonDown(UINT nFlags, CPoint point)
{
	for(int i = 0; i < 2; i ++)
	{
		if(PtInRect(&(m_iRect[i]),point))
		{
			m_iCurIndex = i;
		}
	}
	Invalidate(true);
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
	//CDialog::OnLButtonDown(nFlags, point);
}

void CUserProp::InitUserList()
{
	for(int i = 0; i < m_iCurCount; i ++)
	{
		((CComboBox*)GetDlgItem(IDC_PROP_DEST))->AddString(m_szName[i]);
	}
	((CComboBox*)GetDlgItem(IDC_PROP_DEST))->SetCurSel(0);
	
	m_iCurIndex = 0;
	CRect rect;
	GetClientRect(&rect);
	int x1,y1,x2,y2;
	x1 = rect.left + 55 ;
	y1 = rect.top + 43  ;
	x2 = x1 + 91  ;
	y2 = y1 + 91 ;
	m_iRect[0].SetRect(x1,y1,x2,y2);
	x1 = x2 + 22 ;
	y1 = rect.top + 43  ;
	x2 = x1 + 91  ;
	y2 = y1 + 90 ;
	m_iRect[1].SetRect(x1,y1,x2,y2);
}


void CUserProp::OnMove(int x, int y)
{
	for(int i = 0; i < 2; i ++)
	{
		if(PtInRect(&(m_iRect[i]),CPoint(x,y)))
		{
			m_iCurIndex = i;
		}
	}
	
	Invalidate(true);
	CDialog::OnMove(x, y);
	// TODO: 在此处添加消息处理程序代码
}
