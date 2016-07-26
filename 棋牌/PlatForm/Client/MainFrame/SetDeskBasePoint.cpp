// SetDeskBasePoint.cpp : 实现文件
//

#include "stdafx.h"
#include "SetDeskBasePoint.h"


// CSetDeskBasePoint 对话框

IMPLEMENT_DYNAMIC(CSetDeskBasePoint, CDialog)

CSetDeskBasePoint::CSetDeskBasePoint(CWnd* pParent /*=NULL*/)
	: CDialog(CSetDeskBasePoint::IDD, pParent),m_iBasePoint(0)
{
	m_bkBrush=NULL;
}

CSetDeskBasePoint::~CSetDeskBasePoint()
{
	DeleteObject(m_bkBrush);
}

void CSetDeskBasePoint::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_BtOK);
	DDX_Control(pDX, IDCANCEL, m_BtCancel);
}

BOOL CSetDeskBasePoint::OnInitDialog()
{
	__super::OnInitDialog();

//	int r,g,b;
	CString s=CBcfFile::GetAppPath ();//本地路径
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f( s + strSkin);
	TCHAR path[MAX_PATH];
	CString skinfolder;
	if ("skin0.bcf" == strSkin)
		skinfolder=f.GetKeyVal("skin0","skinfolder",m_skinmgr.GetSkinPath());
	else
		skinfolder=f.GetKeyVal("skin1","skinfolder",m_skinmgr.GetSkinPath());
	wsprintf(path,"%sdialog\\setbasepoint.bmp",skinfolder);
	m_bk.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	wsprintf(path,"%sdialog\\ok_bt.bmp",skinfolder);
	m_BtOK.LoadButtonBitmap(path,false);
	wsprintf(path,"%sdialog\\cancel_bt.bmp",skinfolder);
	m_BtCancel.LoadButtonBitmap(path,false);
	CheckRadioButton(IDC_RADIO_0,IDC_RADIO_10000,IDC_RADIO_0);

	m_bkBrush=CreateSolidBrush(m_bk.GetPixel(68,100));

	return TRUE;
}

void CSetDeskBasePoint::OnOK()
{
	m_iBasePoint=GetCheckedRadioButton(IDC_RADIO_0,IDC_RADIO_10000);
	EndDialog(IDOK);
}
int CSetDeskBasePoint::GetBasePoint()
{
	return m_iBasePoint;
}
LRESULT CSetDeskBasePoint::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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
		int id=::GetDlgCtrlID((HWND)lParam);
		if(id==IDC_BIND_USER||id==IDC_BIND_GAME||id==IDC_BIND_ROOM||id==IDC_STATIC2||id==IDC_STATIC3)
		{
			SetBkColor((HDC)wParam, m_bk.GetPixel(68,100));
			return (LRESULT)m_bkBrush;
		}
		SetBkColor((HDC)wParam, m_bk.GetPixel(68,100));
		return (LRESULT)m_bkBrush;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}
void CSetDeskBasePoint::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CRect ClientRect;
	GetClientRect(&ClientRect);
	//this->MapWindowPoints(this->GetParent(),&ClientRect);
	CGameImageHelper	help(&m_bk);
	//this->MoveWindow(ClientRect.left,ClientRect.top,help.GetWidth(),help.GetHeight());
	help.BitBlt(dc.GetSafeHdc(),0,0);
}
void CSetDeskBasePoint::OnLButtonDown(UINT nFlags, CPoint point)
{
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
	//CGameFaceGo::OnLButtonDown(nFlags, point);
}

BEGIN_MESSAGE_MAP(CSetDeskBasePoint, CDialog)
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_MESSAGE(WM_EXCHANGE_SKIN,OnExchangeSkin)
END_MESSAGE_MAP()

//换肤
LRESULT CSetDeskBasePoint::OnExchangeSkin(WPARAM wpara,LPARAM lpara)
{	
	//OnPaint();
	Invalidate();
	return LRESULT(0);
}


// CSetDeskBasePoint 消息处理程序
