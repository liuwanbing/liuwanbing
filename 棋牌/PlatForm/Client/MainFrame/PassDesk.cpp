// PassDesk.cpp : 实现文件
//

#include "stdafx.h"
#include "PassDesk.h"


// CPassDesk 对话框

IMPLEMENT_DYNAMIC(CPassDesk, CDialog)

CPassDesk::CPassDesk(CWnd* pParent /*=NULL*/)
	: CDialog(CPassDesk::IDD, pParent)
	, m_pass(_T(""))
{

}

CPassDesk::~CPassDesk()
{
}

void CPassDesk::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_pass);
	DDX_Control(pDX, IDOK, m_BtOk);
	DDX_Control(pDX, IDCANCEL, m_BtCancel);
	DDV_MaxChars(pDX, m_pass, 30);
}


BEGIN_MESSAGE_MAP(CPassDesk, CDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CPassDesk 消息处理程序

void CPassDesk::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CGameImageHelper	help(&m_bk);
	help.BitBlt(dc.GetSafeHdc(),0,0);
	// 不为绘图消息调用 CDialog::OnPaint()
}

BOOL CPassDesk::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString s=CBcfFile::GetAppPath ();/////本地路径
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f( s + strSkin);
	TCHAR path[MAX_PATH];
	CString skinfolder;
	if ("skin0.bcf" == strSkin)
		skinfolder=f.GetKeyVal("skin0","skinfolder",m_skinmgr.GetSkinPath());
	else
		skinfolder=f.GetKeyVal("skin1","skinfolder",m_skinmgr.GetSkinPath());

	wsprintf(path,"%slogon\\desk_psw.bmp",skinfolder);
	m_bk.SetLoadInfo(path,TRUE);
	CenterWindow();
	CGameImageHelper help(&m_bk);
	HRGN hRgn;
	hRgn = AFCBmpToRgn(help,m_bk.GetPixel(0,0),RGB(1,1,1));
	if(hRgn)
		SetWindowRgn(hRgn,TRUE);
	CGameImage		m_bt;
	wsprintf(path,"%slogon\\button.bmp",skinfolder);
	m_BtCancel.LoadButtonBitmap(path,false);
	m_bt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	CGameImageHelper bthelp(&m_bt);
	HRGN hRgn2;
	hRgn2=AFCBmpToRgn(bthelp,m_bt.GetPixel(0,0),RGB(1,1,1));
	if(hRgn2)
		m_BtCancel.SetWindowRgn(hRgn2,true);

	wsprintf(path,"%slogon\\button.bmp",skinfolder);
	m_BtOk.LoadButtonBitmap(path,false);
	m_bt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	hRgn2=AFCBmpToRgn(bthelp,m_bt.GetPixel(0,0),RGB(1,1,1));
	if(hRgn2)
		m_BtOk.SetWindowRgn(hRgn2,true);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CPassDesk::LoadSkin()
{
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f( s + strSkin);
	TCHAR path[MAX_PATH];
	CString skinfolder;
	if ("skin0.bcf" == strSkin)
		skinfolder=f.GetKeyVal("skin0","skinfolder",m_skinmgr.GetSkinPath());
	else
		skinfolder=f.GetKeyVal("skin1","skinfolder",m_skinmgr.GetSkinPath());

	wsprintf(path,"%slogon\\desk_psw.bmp",skinfolder);
	m_bk.SetLoadInfo(path,TRUE);
	CenterWindow();

	CGameImage		m_bt;
	wsprintf(path,"%slogon\\button.bmp",skinfolder);
	m_BtCancel.LoadButtonBitmap(path,false);
	m_bt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);

	wsprintf(path,"%slogon\\button.bmp",skinfolder);
	m_BtOk.LoadButtonBitmap(path,false);
	m_bt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);

}
//换肤
LRESULT CPassDesk::OnExchangeSkin(WPARAM wpara,LPARAM lpara)
{
	LoadSkin();
	//OnPaint();
	Invalidate();
	return LRESULT(0);
}
