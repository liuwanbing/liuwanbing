#include "DlgRoomPW.h"
#include "DlgRoomPW.h"
#include "DlgRoomPW.h"
// DlgRoomPW.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgRoomPW.h"


// CDlgRoomPW 对话框

IMPLEMENT_DYNAMIC(CDlgRoomPW, CDialog)

CDlgRoomPW::CDlgRoomPW(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRoomPW::IDD, pParent)
{
	m_pBtmBackGround = NULL;
	LoadSkin();
	m_bkBrush = NULL; 
}

CDlgRoomPW::~CDlgRoomPW()
{
	if (NULL != m_pBtmBackGround)
		delete m_pBtmBackGround;
	DeleteObject(m_bkBrush);
}

void CDlgRoomPW::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_BtnOK);
	DDX_Control(pDX, IDCANCEL, m_BtnCancel);
	DDX_Control(pDX, IDC_BUTTON_QUIT, m_BtnQuit);
}

BEGIN_MESSAGE_MAP(CDlgRoomPW, CDialog)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_QUIT,OnBnClickedQuit)
	ON_MESSAGE(WM_EXCHANGE_SKIN,OnExchangeSkin)
END_MESSAGE_MAP()

// CDlgRoomPW 消息处理程序

BOOL CDlgRoomPW::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	((CEdit*)GetDlgItem(IDC_PW))->LimitText(12);
	((CEdit*)GetDlgItem(IDC_PW_CONFIRM))->LimitText(12);

	if (m_pBtmBackGround)
		MoveWindow(0,0,m_pBtmBackGround->GetWidth(),m_pBtmBackGround->GetHeight());

	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	CString str = fMsg.GetKeyVal("DlgRoomPW","ModifyPW","如果你现在点击确定修改密码后，会将此房间中其他玩家踢出，你确定修改吗？");
	
	SetDlgItemText(IDC_STATIC_TXT,str);
	InitBtnSkin();


	Color cl;
	m_pBtmBackGround->GetPixel(116,136,&cl);
	m_bkBrush = CreateSolidBrush(RGB(cl.GetR(),cl.GetG(),cl.GetB()));

	return TRUE;  /// return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

/// 画图
/// @param void
///@return  void
void CDlgRoomPW::OnPaint()
{
	CPaintDC dc(this);
	Graphics pG(dc.GetSafeHdc());
	pG.DrawImage(m_pBtmBackGround,Rect(0,0,m_pBtmBackGround->GetWidth(),m_pBtmBackGround->GetHeight()),
		0,0,m_pBtmBackGround->GetWidth(),m_pBtmBackGround->GetHeight(),UnitPixel);
	pG.ReleaseHDC(dc.GetSafeHdc());
}

/// 加载皮肤
/// @param void
///@return  void
void CDlgRoomPW::LoadSkin()
{
	TCHAR AppPath[MAX_PATH] = {0};
	TCHAR FilePath[MAX_PATH] = {0};
	wchar_t strWideChar[MAX_PATH];

    // PengJiLin, 2010-6-7, GetCurrentDirectory()函数全部替代
    //::GetCurrentDirectory(MAX_PATH,AppPath);
    lstrcpy(AppPath, CBcfFile::GetAppPath());

	_tcscpy(FilePath,AppPath);
	
	_tcscat(FilePath,"\\" + m_skinmgr.GetSkinPath() + "dialog\\set_room_pw.bmp");
	
	MultiByteToWideChar(CP_ACP,0,FilePath,-1,strWideChar,MAX_PATH);
	
    #ifndef BZ_ZLIB  //< add by wxx 
    m_pBtmBackGround = new Bitmap(strWideChar);
    #else
    CUnZipRes unZipRes;
    IStream * pStream=NULL;
    pStream=unZipRes.LoadFileResFromZip(FilePath);

    m_pBtmBackGround = new Bitmap(pStream);
    if(pStream!=NULL)
    {
        pStream->Release();
        pStream=NULL;
    }
    #endif
}
LRESULT CDlgRoomPW::OnExchangeSkin(WPARAM wpara,LPARAM lpara)
{
	InitBtnSkin();
	LoadSkin();
	//OnPaint();
	Invalidate();
	return LRESULT(0);
}
/// 初始化按键皮肤
/// @param void
///@return  void
void CDlgRoomPW::InitBtnSkin()
{
	CString s = CBcfFile::GetAppPath();/////本地路径
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f( s + strSkin);
	TCHAR path[MAX_PATH];
	CString skinfolder=f.GetKeyVal(m_skinmgr.GetKeyVal(strSkin),"skinfolder",m_skinmgr.GetSkinPath());
	
	CGameImage	m_bt;

	wsprintf(path,"%sdialog\\cancel_bt.bmp",skinfolder);
	m_bt.SetLoadInfo(path,true);
	m_BtnCancel.LoadButtonBitmap(path,false);
	m_BtnCancel.SetWindowPos(NULL,0,0,m_bt.GetWidth()/4,m_bt.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

	wsprintf(path,"%sdialog\\ok_bt.bmp",skinfolder);
	m_bt.SetLoadInfo(path,true);
	m_BtnOK.LoadButtonBitmap(path,false);
	m_BtnOK.SetWindowPos(NULL,0,0,m_bt.GetWidth()/4,m_bt.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);

	wsprintf(path,"%sdialog\\log_exit_bt.bmp",skinfolder);//game_close_bt.bmp
	m_bt.SetLoadInfo(path,true);
	m_BtnQuit.LoadButtonBitmap(path,false);
	m_BtnQuit.SetWindowPos(NULL,0,0,m_bt.GetWidth()/4,m_bt.GetHeight(),SWP_NOMOVE|SWP_NOZORDER);
}

/// 确定按钮
/// @param void
///@return  void
void CDlgRoomPW::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	GetDlgItemText(IDC_PW,m_szPassWord,12);
	GetDlgItemText(IDC_PW_CONFIRM,m_szPassWordConfirm,12);

	CString str;
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");

	if (0 == _tcslen(m_szPassWord) || 0 == _tcslen(m_szPassWordConfirm))
	{
		str = fMsg.GetKeyVal("DlgRoomPW","EnterPW","请输入密码！");
		//AFCMessageBox(str);
	    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",str);
	}
	else if (0 != _tcscmp(m_szPassWord,m_szPassWordConfirm))
	{
		str = fMsg.GetKeyVal("DlgRoomPW","SamePW","两次输入的密码不一样，请重新输入！");
		//AFCMessageBox(str);
	    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",str);
	}
	else
	{
		CDialog::OnOK();
	}
}

void CDlgRoomPW::OnBnClickedQuit()
{
	CDialog::OnCancel();
}

LRESULT CDlgRoomPW::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	switch(message)
	{
	case WM_CTLCOLORSTATIC :
		Color cl;
		m_pBtmBackGround->GetPixel(116,136,&cl);
		SetBkColor((HDC)wParam, RGB(cl.GetR(),cl.GetG(),cl.GetB()));
		return (LRESULT)m_bkBrush;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}
