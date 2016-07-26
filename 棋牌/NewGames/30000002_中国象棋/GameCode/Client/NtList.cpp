#include "Stdafx.h"
#include "NtList.h"
#include "ClientGameDlg.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CNtList, CDialog)
	ON_BN_CLICKED(IDC_1, On1)
	ON_BN_CLICKED(IDC_2, On2)
	ON_BN_CLICKED(IDC_3, On3)
	ON_BN_CLICKED(IDC_4, On4)
END_MESSAGE_MAP()

/**
 * 构造函数
 */
CNtList::CNtList(CWnd* pParent /*=NULL*/): CDialog(CNtList::IDD, pParent)
{
}

/**
 * DDX 支持
 */
void CNtList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_4, m_Bm4);
	DDX_Control(pDX, IDC_3, m_Bm3);
	DDX_Control(pDX, IDC_2, m_Bm2);
	DDX_Control(pDX, IDC_1, m_Bm1);
}

/**
 * 确定函数
 */
void CNtList::OnOK() 
{
	return;
}

/**
 * 取消函数
 */
void CNtList::OnCancel() 
{
	return;
}

/**
 * 初始化函数
 */
BOOL CNtList::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//加载位图;
	//AfxSetResourceHandle(GetModuleHandle(CLIENT_DLL_NAME));
	m_Bm1.LoadBitmaps(TEXT("FL1U"),TEXT("FL1D"),TEXT("FL1U"),TEXT("FL1X"));
	m_Bm2.LoadBitmaps(TEXT("FL2U"),TEXT("FL2D"),TEXT("FL2U"),TEXT("FL2X"));
	m_Bm3.LoadBitmaps(TEXT("FL3U"),TEXT("FL3D"),TEXT("FL3U"),TEXT("FL3X"));
	m_Bm4.LoadBitmaps(TEXT("FL4U"),TEXT("FL4D"),TEXT("FL4U"),TEXT("FL4X"));
	m_Bm1.SizeToContent();
	m_Bm2.SizeToContent();
	m_Bm3.SizeToContent();
	m_Bm4.SizeToContent();

	int iHide=0,iWide=0;
	CRect rect;
	m_Bm1.GetWindowRect(&rect);
	iWide=rect.Width()*4;
	iHide=rect.Height();
	m_Bm1.MoveWindow(0,0,rect.Width(),rect.Height());
	m_Bm2.MoveWindow(rect.Width(),0,rect.Width(),rect.Height());
	m_Bm3.MoveWindow(rect.Width()*2,0,rect.Width(),rect.Height());
	m_Bm4.MoveWindow(rect.Width()*3,0,rect.Width(),rect.Height());
	GetWindowRect(&rect);
	MoveWindow(rect.left,rect.top,iWide,iHide);

	//AfxSetResourceHandle(GetModuleHandle(NULL));
	return TRUE;
}

/**
 * 更新按钮状态
 */
void CNtList::UpdateNtList(BOOL b1,BOOL b2,BOOL b3,BOOL b4)
{
	m_Bm1.EnableWindow(b1);
	m_Bm2.EnableWindow(b2);
	m_Bm3.EnableWindow(b3);
	m_Bm4.EnableWindow(b4);
	return;
}

/**
 * 按钮消息
 */
void CNtList::On1()   
{	
	//CClientPlayView *p=CONTAINING_RECORD(this,CClientPlayView,m_NtList);
	//CClientGameDlg *pp=CONTAINING_RECORD(p,CClientGameDlg,m_PlayView);
	//pp->SendMessage(IDM_USER_NT,UG_HEI_TAO,UG_HEI_TAO); 
}

void CNtList::On2()   
{	
	//CClientPlayView *p=CONTAINING_RECORD(this,CClientPlayView,m_NtList);
	//CClientGameDlg *pp=CONTAINING_RECORD(p,CClientGameDlg,m_PlayView);
	//pp->SendMessage(IDM_USER_NT,UG_HONG_TAO,UG_HONG_TAO); 
}

void CNtList::On3()   
{
	//CClientPlayView *p=CONTAINING_RECORD(this,CClientPlayView,m_NtList);
	//CClientGameDlg *pp=CONTAINING_RECORD(p,CClientGameDlg,m_PlayView);
	//pp->SendMessage(IDM_USER_NT,UG_MEI_HUA,UG_MEI_HUA); 
}

void CNtList::On4()  
{	
	//CClientPlayView *p=CONTAINING_RECORD(this,CClientPlayView,m_NtList);
	//CClientGameDlg *pp=CONTAINING_RECORD(p,CClientGameDlg,m_PlayView);
	//pp->SendMessage(IDM_USER_NT,UG_FANG_KUAI,UG_FANG_KUAI); 
}
