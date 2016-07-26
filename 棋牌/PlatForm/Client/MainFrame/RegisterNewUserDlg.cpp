// RegisterNewUserDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "RegisterNewUserDlg.h"
#include "comutil.h"

// CRegisterNewUserDlg 对话框

CRegisterNewUserDlg::CRegisterNewUserDlg(CWnd* pParent /*=NULL*/)
	: CLoadFileBmp(CRegisterNewUserDlg::IDD, pParent)
{
	m_bkBrush=NULL;
}

CRegisterNewUserDlg::~CRegisterNewUserDlg()
{
	DeleteObject(m_bkBrush);
}

void CRegisterNewUserDlg::DoDataExchange(CDataExchange* pDX)
{//都变成透明
	//静态控件
	//DDX_Control(pDX, IDC_STATIC_1234567, m_static_CopyRight);
	
	DDX_Control(pDX, IDC_EXPLORER1, m_ie);
	DDX_Control(pDX,IDCANCEL,m_btQuit);
	__super::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRegisterNewUserDlg, CLoadFileBmp)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()
END_MESSAGE_MAP()


		//const TCHAR * szLimitName[]={TEXT("快乐"),TEXT("江泽民"),TEXT("陈龙水"),TEXT("毛泽东"),TEXT("系统"),TEXT("管理"),
		//	TEXT("阴道"),TEXT("日"),TEXT("你妈"),TEXT("叼"),TEXT("我日"),TEXT("FUCK"),TEXT("妈B"),TEXT("操"),TEXT("靠"),
		//	TEXT("外管"),TEXT("内管"),TEXT("系统"),TEXT("金币"),TEXT("游戏中心"),TEXT("消息"),
		//	TEXT("中奖"),TEXT("信息"),TEXT("客服"),TEXT("腾奥"),TEXT("腾澳"),TEXT("腾澳"),TEXT("管理员"),TEXT("官方"),TEXT("game0437"),TEXT("客服中心"),TEXT("客服服务")
		//	TEXT("GM"),TEXT(" "),TEXT("名字"),TEXT("昵称"),TEXT("名称")


BOOL CRegisterNewUserDlg::OnInitDialog()
{
	CLoadFileBmp::OnInitDialog();
//	SetWindowTitle("注册新用户");

	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	SetWindowTitle(fMsg.GetKeyVal("LogonDlg","RegNewUser","注册新用户"));
	LoadSkin();

	//初始化游戏
	CenterWindow();


	CRect ClientRect;
	GetClientRect(&ClientRect);
	this->MapWindowPoints(this->GetParent(),&ClientRect);
	CGameImageHelper	help(&m_bk);
	this->SetWindowPos(this->GetParent(),0,0,help.GetWidth(),help.GetHeight(),SWP_NOMOVE);

	CString s=CBcfFile::GetAppPath ();/////本地路径

	CBcfFile fskin( s + "skin0.bcf");
	CString key=TEXT("RegisterUser");
	CString RegPageColor=fskin.GetKeyVal(key,"regpagebgcolor","");
	CBcfFile f( s + "bzgame.bcf");
	key="BZW";
	CString m_regurl=f.GetKeyVal (Glb().m_key,"AppRegUrl","http://localhost/app/regist.asp");
	CString m_ReCommend=f.GetKeyVal ("CustomSet","ReCommend","");
	m_regurl+="?bgcolor="+RegPageColor+"&recommend="+m_ReCommend;
	m_ie.Navigate(m_regurl,0,0,0,0);

	m_regname="";

	CenterWindow(AfxGetApp()->m_pMainWnd);

	
	return false;  // return TRUE unless you set the focus to a control
}


void CRegisterNewUserDlg::OnBnClickedCancel()
{
	OnCancel();
}

LRESULT CRegisterNewUserDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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
		SetBkColor((HDC)wParam, m_bkcolor);
		if (m_bkBrush == NULL) m_bkBrush = CreateSolidBrush(m_bkcolor);
		return (LRESULT)m_bkBrush;
	}
	return CLoadFileBmp::WindowProc(message, wParam, lParam);
}

void CRegisterNewUserDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CRect ClientRect;
	GetClientRect(&ClientRect);
	//this->MapWindowPoints(this->GetParent(),&ClientRect);
	CGameImageHelper	help(&m_bk);
	//this->SetWindowPos(this->GetParent(),0,0,help.GetWidth(),help.GetHeight(),SWP_NOMOVE);
	help.BitBlt(dc.GetSafeHdc(),0,0);
	
}



void CRegisterNewUserDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(point.y<=30)
		PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
	//CLoadFileBmp::OnLButtonDown(nFlags, point);
}

void CRegisterNewUserDlg::LoadSkin(void)
{
	CRect rect;
//	int r,g,b;
	int cx,cy;
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f( s + strSkin);
	CString key=TEXT("RegisterUser");
	TCHAR path[MAX_PATH];
	CString skinfolder;
	if ("skin0.bcf" == strSkin)
		skinfolder=f.GetKeyVal("skin0","skinfolder",m_skinmgr.GetSkinPath());
	else
		skinfolder=f.GetKeyVal("skin1","skinfolder",m_skinmgr.GetSkinPath());

	wsprintf(path,"%slogon\\reg_bk.bmp",skinfolder);
	m_bk.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	bool needsetrgn=f.GetKeyVal(key,"regrgn",0);
	CGameImageHelper ImageHandle(&m_bk);
	if(needsetrgn)
	{
		HRGN hRgn=AFCBmpToRgn(ImageHandle,m_bk.GetPixel(0,0),RGB(1,1,1));
		if (hRgn!=NULL)
		{
			SetWindowRgn(hRgn,TRUE);
			///{{ Added by zxd 20100709 释放区域对象
			DeleteObject(hRgn);
			///Added by zxd 20100709 释放区域对象}}
		}
	}
	cx=ImageHandle.GetWidth();
	cy=ImageHandle.GetHeight();
	SetWindowPos(NULL,0,0,cx,cy,SWP_NOMOVE|SWP_NOZORDER);
	//移动IE
	//m_ie.MoveWindow(15,45,cx-30,cy-65,false);
	m_ie.MoveWindow(6,28,cx-12,cy-33,false);//JMod-调整用户注册窗口-20090506
	////下面几行是关闭注册窗口用的
	wsprintf(path,"%slogon\\log_exit_bt.bmp",skinfolder);
	m_bt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	m_btQuit.LoadButtonBitmap(path,false);
	m_btQuit.MoveWindow(cx-40,-2,m_bt.GetWidth()/4,m_bt.GetHeight(),false);
	m_btQuit.ShowWindow(SW_SHOW);

	return;

}

int CRegisterNewUserDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CLoadFileBmp::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}


BEGIN_EVENTSINK_MAP(CRegisterNewUserDlg, CLoadFileBmp)
	ON_EVENT(CRegisterNewUserDlg, IDC_EXPLORER1, 250, CRegisterNewUserDlg::BeforeNavigate2Explorer1, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
	ON_EVENT(CRegisterNewUserDlg, IDC_EXPLORER1, 271, CRegisterNewUserDlg::NavigateErrorExplorer1, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
END_EVENTSINK_MAP()

void CRegisterNewUserDlg::BeforeNavigate2Explorer1(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers, BOOL* Cancel)
{
	// TODO: 在此处添加消息处理程序代码
	CString surl=(char*)_bstr_t(URL->bstrVal);
	if(surl.Find("bzwp://regsucceed")==0)
	{
		int uindex=surl.Find("username=");
		int uindex1 = surl.Find("userid=");
		m_regname=surl.Mid(uindex+9, uindex1 - uindex - 9 - 1);
		m_regID = surl.Mid(uindex1+7);
		CDialog::OnOK();
		*Cancel=true;
	}
}

void CRegisterNewUserDlg::NavigateErrorExplorer1(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Frame, VARIANT* StatusCode, BOOL* Cancel)
{
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	CString strTip = fMsg.GetKeyVal("LogonDlg","ErrorTip","错误提示");
	CString strMessage = fMsg.GetKeyVal("LogonDlg","RegLinkError","连接注册服务器失败");

	m_ie.Navigate("about:blank",0,0,0,0);
	
	//AFCMessageBox(strMessage,strTip);
	DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,strTip,strMessage);
	CDialog::OnCancel();
	*Cancel=true;
}
