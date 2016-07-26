#include "../stdafx.h"
#include "FlashViewDlg.h"

BEGIN_MESSAGE_MAP(CIEHelper,CGameFaceGo)
	ON_WM_SIZE()
END_MESSAGE_MAP()
/*******************************************************************************************************/
long CGoAnimationWnd::GetReadyState()
{
	long result;
	InvokeHelper(DISPID_READYSTATE,DISPATCH_PROPERTYGET,VT_I4,&result,NULL);
	return result;
}

long CGoAnimationWnd::GetTotalFrames()
{
	long result;
	InvokeHelper(0x7c,DISPATCH_PROPERTYGET,VT_I4,&result,NULL);
	return result;
}

BOOL CGoAnimationWnd::GetPlaying()
{
	BOOL result;
	InvokeHelper(0x7d,DISPATCH_PROPERTYGET,VT_BOOL,&result,NULL);
	return result;
}

void CGoAnimationWnd::SetPlaying(BOOL bNewValue)
{
	BYTE parms[]=VTS_BOOL;
	InvokeHelper(0x7d,DISPATCH_PROPERTYPUT,VT_EMPTY,NULL,parms,bNewValue);
}

long CGoAnimationWnd::GetQuality()
{
	long result;
	InvokeHelper(0x69,DISPATCH_PROPERTYGET,VT_I4,&result,NULL);
	return result;
}

void CGoAnimationWnd::SetQuality(long nNewValue)
{
	BYTE parms[]=VTS_I4;
	InvokeHelper(0x69,DISPATCH_PROPERTYPUT,VT_EMPTY,NULL,parms,nNewValue);
}

long CGoAnimationWnd::GetScaleMode()
{
	long result;
	InvokeHelper(0x78,DISPATCH_PROPERTYGET,VT_I4,&result,NULL);
	return result;
}

void CGoAnimationWnd::SetScaleMode(long nNewValue)
{
	BYTE parms[]=
		VTS_I4;
	InvokeHelper(0x78,DISPATCH_PROPERTYPUT,VT_EMPTY,NULL,parms,
		 nNewValue);
}

long CGoAnimationWnd::GetAlignMode()
{
	long result;
	InvokeHelper(0x79,DISPATCH_PROPERTYGET,VT_I4,&result,NULL);
	return result;
}

void CGoAnimationWnd::SetAlignMode(long nNewValue)
{
	BYTE parms[]=VTS_I4;
	InvokeHelper(0x79,DISPATCH_PROPERTYPUT,VT_EMPTY,NULL,parms,nNewValue);
}

long CGoAnimationWnd::GetBackgroundColor()
{
	long result;
	InvokeHelper(0x7b,DISPATCH_PROPERTYGET,VT_I4,&result,NULL);
	return result;
}

void CGoAnimationWnd::SetBackgroundColor(long nNewValue)
{
	BYTE parms[]=VTS_I4;
	InvokeHelper(0x7b,DISPATCH_PROPERTYPUT,VT_EMPTY,NULL,parms,nNewValue);
}

BOOL CGoAnimationWnd::GetLoop()
{
	BOOL result;
	InvokeHelper(0x6a,DISPATCH_PROPERTYGET,VT_BOOL,&result,NULL);
	return result;
}

void CGoAnimationWnd::SetLoop(BOOL bNewValue)
{
	BYTE parms[]=VTS_BOOL;
	InvokeHelper(0x6a,DISPATCH_PROPERTYPUT,VT_EMPTY,NULL,parms,bNewValue);
}

CString CGoAnimationWnd::GetMovie()
{
	CString result;
	InvokeHelper(0x66,DISPATCH_PROPERTYGET,VT_BSTR,&result,NULL);
	return result;
}

void CGoAnimationWnd::SetMovie(LPCTSTR lpszNewValue)
{
	BYTE parms[]=VTS_BSTR;
	InvokeHelper(0x66,DISPATCH_PROPERTYPUT,VT_EMPTY,NULL,parms,lpszNewValue);
}

long CGoAnimationWnd::GetFrameNum()
{
	long result;
	InvokeHelper(0x6b,DISPATCH_PROPERTYGET,VT_I4,&result,NULL);
	return result;
}

void CGoAnimationWnd::SetFrameNum(long nNewValue)
{
	BYTE parms[]=VTS_I4;
	InvokeHelper(0x6b,DISPATCH_PROPERTYPUT,VT_EMPTY,NULL,parms,nNewValue);
}

void CGoAnimationWnd::SetZoomRect(long left,long top,long right,long bottom)
{
	BYTE parms[]=VTS_I4 VTS_I4 VTS_I4 VTS_I4;
	InvokeHelper(0x6d,DISPATCH_METHOD,VT_EMPTY,NULL,parms,left,top,right,bottom);
}

void CGoAnimationWnd::Zoom(long factor)
{
	BYTE parms[]=VTS_I4;
	InvokeHelper(0x76,DISPATCH_METHOD,VT_EMPTY,NULL,parms,factor);
}

void CGoAnimationWnd::Pan(long x, long y, long mode)
{
	BYTE parms[]=VTS_I4 VTS_I4 VTS_I4;
	InvokeHelper(0x77,DISPATCH_METHOD,VT_EMPTY,NULL,parms,x,y,mode);
}

void CGoAnimationWnd::Play()
{
	InvokeHelper(0x70,DISPATCH_METHOD,VT_EMPTY,NULL,NULL);
}

void CGoAnimationWnd::Stop()
{
	InvokeHelper(0x71,DISPATCH_METHOD,VT_EMPTY,NULL,NULL);
}

void CGoAnimationWnd::Back()
{
	InvokeHelper(0x72,DISPATCH_METHOD,VT_EMPTY,NULL,NULL);
}

void CGoAnimationWnd::Forward()
{
	InvokeHelper(0x73,DISPATCH_METHOD,VT_EMPTY,NULL,NULL);
}

void CGoAnimationWnd::Rewind()
{
	InvokeHelper(0x74,DISPATCH_METHOD,VT_EMPTY,NULL,NULL);
}

void CGoAnimationWnd::StopPlay()
{
	InvokeHelper(0x7e,DISPATCH_METHOD,VT_EMPTY,NULL,NULL);
}

void CGoAnimationWnd::GotoFrame(long FrameNum)
{
	BYTE parms[]=VTS_I4;
	InvokeHelper(0x7f,DISPATCH_METHOD,VT_EMPTY,NULL,parms,FrameNum);
}

long CGoAnimationWnd::CurrentFrame()
{
	long result;
	InvokeHelper(0x80,DISPATCH_METHOD,VT_I4,&result,NULL);
	return result;
}

BOOL CGoAnimationWnd::IsPlaying()
{
	BOOL result;
	InvokeHelper(0x81,DISPATCH_METHOD,VT_BOOL,&result,NULL);
	return result;
}

long CGoAnimationWnd::PercentLoaded()
{
	long result;
	InvokeHelper(0x82,DISPATCH_METHOD,VT_I4,&result,NULL);
	return result;
}

BOOL CGoAnimationWnd::FrameLoaded(long FrameNum)
{
	BOOL result;
	BYTE parms[]=VTS_I4;
	InvokeHelper(0x83,DISPATCH_METHOD,VT_BOOL,&result,parms,FrameNum);
	return result;
}

long CGoAnimationWnd::FlashVersion()
{
	long result;
	InvokeHelper(0x84,DISPATCH_METHOD,VT_I4,&result,NULL);
	return result;
}

CString CGoAnimationWnd::GetWMode()
{
	CString result;
	InvokeHelper(0x85,DISPATCH_PROPERTYGET,VT_BSTR,&result,NULL);
	return result;
}

void CGoAnimationWnd::SetWMode(LPCTSTR lpszNewValue)
{
	BYTE parms[]=VTS_BSTR;
	InvokeHelper(0x85,DISPATCH_PROPERTYPUT,VT_EMPTY,NULL,parms,lpszNewValue);
}

CString CGoAnimationWnd::GetSAlign()
{
	CString result;
	InvokeHelper(0x86,DISPATCH_PROPERTYGET,VT_BSTR,&result,NULL);
	return result;
}

void CGoAnimationWnd::SetSAlign(LPCTSTR lpszNewValue)
{
	BYTE parms[]=VTS_BSTR;
	InvokeHelper(0x86,DISPATCH_PROPERTYPUT,VT_EMPTY,NULL,parms,lpszNewValue);
}

BOOL CGoAnimationWnd::GetMenu()
{
	BOOL result;
	InvokeHelper(0x87,DISPATCH_PROPERTYGET,VT_BOOL,&result,NULL);
	return result;
}

void CGoAnimationWnd::SetMenu(BOOL bNewValue)
{
	BYTE parms[]=VTS_BOOL;
	InvokeHelper(0x87,DISPATCH_PROPERTYPUT,VT_EMPTY,NULL,parms,bNewValue);
}

CString CGoAnimationWnd::GetBase()
{
	CString result;
	InvokeHelper(0x88,DISPATCH_PROPERTYGET,VT_BSTR,&result,NULL);
	return result;
}

void CGoAnimationWnd::SetBase(LPCTSTR lpszNewValue)
{
	BYTE parms[]=VTS_BSTR;
	InvokeHelper(0x88,DISPATCH_PROPERTYPUT,VT_EMPTY,NULL,parms,lpszNewValue);
}

CString CGoAnimationWnd::GetScale()
{
	CString result;
	InvokeHelper(0x89,DISPATCH_PROPERTYGET,VT_BSTR,&result,NULL);
	return result;
}

void CGoAnimationWnd::SetScale(LPCTSTR lpszNewValue)
{
	BYTE parms[]=VTS_BSTR;
	InvokeHelper(0x89,DISPATCH_PROPERTYPUT,VT_EMPTY,NULL,parms,lpszNewValue);
}

BOOL CGoAnimationWnd::GetDeviceFont()
{
	BOOL result;
	InvokeHelper(0x8a,DISPATCH_PROPERTYGET,VT_BOOL,&result,NULL);
	return result;
}

void CGoAnimationWnd::SetDeviceFont(BOOL bNewValue)
{
	BYTE parms[]=VTS_BOOL;
	InvokeHelper(0x8a,DISPATCH_PROPERTYPUT,VT_EMPTY,NULL,parms,bNewValue);
}

BOOL CGoAnimationWnd::GetEmbedMovie()
{
	BOOL result;
	InvokeHelper(0x8b,DISPATCH_PROPERTYGET,VT_BOOL,&result,NULL);
	return result;
}

void CGoAnimationWnd::SetEmbedMovie(BOOL bNewValue)
{
	BYTE parms[]=VTS_BOOL;
	InvokeHelper(0x8b,DISPATCH_PROPERTYPUT,VT_EMPTY,NULL,parms,bNewValue);
}

CString CGoAnimationWnd::GetBGColor()
{
	CString result;
	InvokeHelper(0x8c,DISPATCH_PROPERTYGET,VT_BSTR,&result,NULL);
	return result;
}

void CGoAnimationWnd::SetBGColor(LPCTSTR lpszNewValue)
{
	BYTE parms[]=VTS_BSTR;
	InvokeHelper(0x8c,DISPATCH_PROPERTYPUT,VT_EMPTY,NULL,parms,lpszNewValue);
}

CString CGoAnimationWnd::GetQuality2()
{
	CString result;
	InvokeHelper(0x8d,DISPATCH_PROPERTYGET,VT_BSTR,&result,NULL);
	return result;
}

void CGoAnimationWnd::SetQuality2(LPCTSTR lpszNewValue)
{
	BYTE parms[]=VTS_BSTR;
	InvokeHelper(0x8d,DISPATCH_PROPERTYPUT,VT_EMPTY,NULL,parms,lpszNewValue);
}

void CGoAnimationWnd::LoadMovie(long layer,LPCTSTR url)
{
	BYTE parms[]=VTS_I4 VTS_BSTR;
	InvokeHelper(0x8e,DISPATCH_METHOD,VT_EMPTY,NULL,parms,layer,url);
}

void CGoAnimationWnd::TGotoFrame(LPCTSTR target, long FrameNum)
{
	BYTE parms[]=VTS_BSTR VTS_I4;
	InvokeHelper(0x8f,DISPATCH_METHOD,VT_EMPTY,NULL,parms,target,FrameNum);
}

void CGoAnimationWnd::TGotoLabel(LPCTSTR target,LPCTSTR label)
{
	BYTE parms[]=VTS_BSTR VTS_BSTR;
	InvokeHelper(0x90,DISPATCH_METHOD,VT_EMPTY,NULL,parms,target,label);
}

long CGoAnimationWnd::TCurrentFrame(LPCTSTR target)
{
	long result;
	BYTE parms[]=VTS_BSTR;
	InvokeHelper(0x91,DISPATCH_METHOD,VT_I4,&result,parms,target);
	return result;
}

CString CGoAnimationWnd::TCurrentLabel(LPCTSTR target)
{
	CString result;
	BYTE parms[]=VTS_BSTR;
	InvokeHelper(0x92,DISPATCH_METHOD,VT_BSTR,&result,parms,target);
	return result;
}

void CGoAnimationWnd::TPlay(LPCTSTR target)
{
	BYTE parms[]=VTS_BSTR;
	InvokeHelper(0x93,DISPATCH_METHOD,VT_EMPTY,NULL,parms,target);
}

void CGoAnimationWnd::TStopPlay(LPCTSTR target)
{
	BYTE parms[]=VTS_BSTR;
	InvokeHelper(0x94,DISPATCH_METHOD,VT_EMPTY,NULL,parms,target);
}

void CGoAnimationWnd::SetVariable(LPCTSTR name,LPCTSTR value)
{
	BYTE parms[]=VTS_BSTR VTS_BSTR;
	InvokeHelper(0x97,DISPATCH_METHOD,VT_EMPTY,NULL,parms,name,value);
}

CString CGoAnimationWnd::GetVariable(LPCTSTR name)
{
	CString result;
	BYTE parms[]=VTS_BSTR;
	InvokeHelper(0x98,DISPATCH_METHOD,VT_BSTR,&result,parms,name);
	return result;
}

void CGoAnimationWnd::TSetProperty(LPCTSTR target,long property,LPCTSTR value)
{
	BYTE parms[]=VTS_BSTR VTS_I4 VTS_BSTR;
	InvokeHelper(0x99,DISPATCH_METHOD,VT_EMPTY,NULL,parms,target,property,value);
}

CString CGoAnimationWnd::TGetProperty(LPCTSTR target,long property)
{
	CString result;
	BYTE parms[]=VTS_BSTR VTS_I4;
	InvokeHelper(0x9a,DISPATCH_METHOD,VT_BSTR,&result,parms,target,property);
	return result;
}

void CGoAnimationWnd::TCallFrame(LPCTSTR target,long FrameNum)
{
	BYTE parms[]=VTS_BSTR VTS_I4;
	InvokeHelper(0x9b,DISPATCH_METHOD,VT_EMPTY,NULL,parms,target,FrameNum);
}

void CGoAnimationWnd::TCallLabel(LPCTSTR target,LPCTSTR label)
{
	BYTE parms[]=VTS_BSTR VTS_BSTR;
	InvokeHelper(0x9c,DISPATCH_METHOD,VT_EMPTY,NULL,parms,target,label);
}

void CGoAnimationWnd::TSetPropertyNum(LPCTSTR target,long property,double value)
{
	BYTE parms[]=VTS_BSTR VTS_I4 VTS_R8;
	InvokeHelper(0x9d,DISPATCH_METHOD,VT_EMPTY,NULL,parms,target,property,value);
}

double CGoAnimationWnd::TGetPropertyNum(LPCTSTR target,long property)
{
	double result;
	BYTE parms[]=VTS_BSTR VTS_I4;
	InvokeHelper(0x9e,DISPATCH_METHOD,VT_R8,&result,parms,target,property);
	return result;
}

double CGoAnimationWnd::TGetPropertyAsNumber(LPCTSTR target,long property)
{
	double result;
	BYTE parms[]=VTS_BSTR VTS_I4;
	InvokeHelper(0xac,DISPATCH_METHOD,VT_R8,&result,parms,target,property);
	return result;
}

CString CGoAnimationWnd::GetSWRemote()
{
	CString result;
	InvokeHelper(0x9f,DISPATCH_PROPERTYGET,VT_BSTR,&result,NULL);
	return result;
}

void CGoAnimationWnd::SetSWRemote(LPCTSTR lpszNewValue)
{
	BYTE parms[]=VTS_BSTR;
	InvokeHelper(0x9f,DISPATCH_PROPERTYPUT,VT_EMPTY,NULL,parms,lpszNewValue);
}

CString CGoAnimationWnd::GetFlashVars()
{
	CString result;
	InvokeHelper(0xaa,DISPATCH_PROPERTYGET,VT_BSTR,&result,NULL);
	return result;
}

void CGoAnimationWnd::SetFlashVars(LPCTSTR lpszNewValue)
{
	BYTE parms[]=VTS_BSTR;
	InvokeHelper(0xaa,DISPATCH_PROPERTYPUT,VT_EMPTY,NULL,parms,lpszNewValue);
}

CString CGoAnimationWnd::GetAllowScriptAccess()
{
	CString result;
	InvokeHelper(0xab,DISPATCH_PROPERTYGET,VT_BSTR,&result,NULL);
	return result;
}

void CGoAnimationWnd::SetAllowScriptAccess(LPCTSTR lpszNewValue)
{
	BYTE parms[]=VTS_BSTR;
	InvokeHelper(0xab,DISPATCH_PROPERTYPUT,VT_EMPTY,NULL,parms,lpszNewValue);
}

/*******************************************************************************************************/

//构造函数
CIEHelper::CIEHelper(CWnd * pParent) : CGameFaceGo(IDD_FLASH_DLG,pParent)
{
}

//析构函数
CIEHelper::~CIEHelper()
{
}

//倥件绑定函数
void CIEHelper::DoDataExchange(CDataExchange * pDX)
{
	CGameFaceGo::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_FALSH,m_FlashView);
}

//建立函数
bool CIEHelper::CreateFalsh(CWnd * pParent)
{
	AfxSetResourceHandle(GetModuleHandle(FACE_DLL_NAME));
	Create(IDD_FLASH_DLG,pParent);
	AfxSetResourceHandle(GetModuleHandle(NULL));
	return true;
}

//位置函数
void CIEHelper::OnSize(UINT nType, int cx, int cy)
{
	CGameFaceGo::OnSize(nType,cx,cy);

	//移动位置
	int iWidth=cx,iHeight=cy;
	SafeMoveWindow(&m_FlashView,0,0,iWidth,iHeight);

	return;
}

/*******************************************************************************************************/
