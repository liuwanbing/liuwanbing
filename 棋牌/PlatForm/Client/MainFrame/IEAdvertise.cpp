// IEAdvertise.cpp : 实现文件
//
#include "Stdafx.h"
#include "IEAdvertise.h"
#include "resource.h"
#include "MainRoomEx.h"

// CIEAdvertise 对话框

BEGIN_MESSAGE_MAP(CIEAdvertise, CLoadFileBmp)
	ON_WM_TIMER()
END_MESSAGE_MAP()


CIEAdvertise::CIEAdvertise():CLoadFileBmp(IDD_IEAD)
{

}

CIEAdvertise::~CIEAdvertise()
{
}

BOOL CIEAdvertise::OnInitDialog()
{
	CLoadFileBmp::OnInitDialog();
	CGameImage btn;
	// TODO:  在此添加额外的初始化
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f( s + "skin0.bcf");
	TCHAR path[MAX_PATH];

	int  nX, nY, nWidth, nHeight;
	CString strUrl;
	int timeCount;

	CString skinfolder;
	skinfolder=f.GetKeyVal("skin0","skinfolder","image\\client\\logon\\skin0");
	wsprintf(path,"%slogon\\log_exit_bt.bmp",skinfolder);	

	CBcfFile fInit( s + "bzgame.bcf");
	CString key= "IEAdvertise";

	nX = nY = 0;
	nWidth = fInit.GetKeyVal(key, "IEWidth", 500);
	nHeight = fInit.GetKeyVal(key, "IEHeight", 500);
	strUrl = fInit.GetKeyVal(key, "URL", _T("www.baidu.com"));
	timeCount =  fInit.GetKeyVal(key, "Time", 5);
	
	//设置背景
	SetWindowPos(NULL,0,0,nWidth,nHeight,SWP_NOMOVE|SWP_NOZORDER);
	//MoveWindow(0,0,nWidth,nHeight);

	//设置关闭按钮
	m_btClose.LoadButtonBitmap(path,false);
	btn.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	m_btClose.MoveWindow(nWidth - btn.GetWidth()/4, 0,btn.GetWidth()/4,btn.GetHeight());
	
	//设置IE
	m_ie.MoveWindow(nX,nY,nWidth,nHeight);
	m_ie.Navigate(strUrl,0,0,0,0);

	if(timeCount > 30)//设置广告时间最大值。
		timeCount = 30;
	SetTimer(1,timeCount * 1000,NULL);//启动定时器
	return TRUE;  
}

void CIEAdvertise::DoDataExchange(CDataExchange* pDX)
{
	CLoadFileBmp::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXPLORER1, m_ie);
	DDX_Control(pDX, IDCANCEL, m_btClose);
}

void CIEAdvertise::OnTimer(UINT nIDEvent) 
{
	::SendMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(),WM_CLOSE,0,0);
	//GetParent()->GetParent()->DestroyWindow();
	CLoadFileBmp::OnOK();
}

void CIEAdvertise::OnCancel()
{
	::SendMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(),WM_CLOSE,0,0);
	return CDialog::OnCancel();
}