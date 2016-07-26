#include "StdAfx.h"
#include "Resource.h"
#include "GameEndView.h"
#include ".\gameendview.h"

//using namespace AFC;
//using namespace AFC::AFCResource;

BEGIN_MESSAGE_MAP(CPalatesNeck, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

/*******************************************************************************************************/

//构造函数
CPalatesNeck::CPalatesNeck(BYTE bGamePeople, bool bShowTax) : m_bGamePeople(bGamePeople), m_bShowTax(bShowTax)
{
	m_dwGameTax=0;
	BYTE bLoadCount=bGamePeople;
	if (bShowTax==true) bLoadCount++;
	switch (bLoadCount)
	{
	case 2:
		{
			//m_BackBmp.SetLoadInfo(GetModuleHandle(FACE_DLL_NAME),IDB_GAME_END_2,CGameImageLink::m_bAutoLock);
			break;
		}
	case 3:
		{
			//m_BackBmp.SetLoadInfo(GetModuleHandle(FACE_DLL_NAME),IDB_GAME_END_2,CGameImageLink::m_bAutoLock);
			break;
		}
	case 4:
		{
			//m_BackBmp.SetLoadInfo(GetModuleHandle(FACE_DLL_NAME),IDB_GAME_END_2,CGameImageLink::m_bAutoLock);
			break;
		}
	case 5:
		{
			//m_BackBmp.SetLoadInfo(GetModuleHandle(FACE_DLL_NAME),IDB_GAME_END_2,CGameImageLink::m_bAutoLock);
			break;
		}
	case 6:
		{
			//m_BackBmp.SetLoadInfo(GetModuleHandle(FACE_DLL_NAME),IDB_GAME_END_2,CGameImageLink::m_bAutoLock);
			break;
		}
	case 7:
		{
			//m_BackBmp.SetLoadInfo(GetModuleHandle(FACE_DLL_NAME),IDB_GAME_END_2,CGameImageLink::m_bAutoLock);
			break;
		}
	case 8:
		{
			//m_BackBmp.SetLoadInfo(GetModuleHandle(FACE_DLL_NAME),IDB_GAME_END_2,CGameImageLink::m_bAutoLock);
			break;
		}
	case 9:
		{
			//m_BackBmp.SetLoadInfo(GetModuleHandle(FACE_DLL_NAME),IDB_GAME_END_2,CGameImageLink::m_bAutoLock);
			break;
		}
	}
}

//析构函数
CPalatesNeck::~CPalatesNeck()
{
	m_Font.DeleteObject();
}

//建立消息
int CPalatesNeck::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct)==-1) return -1;

	//设置数据
	m_dwGameTax=0;
	memset(m_dwPoint,0,sizeof(m_dwPoint));
	for (BYTE i=0;i<m_bGamePeople;i++) m_strName[i].Empty();

	//设置大小
	CGameImageHelper ImageHandle(&m_BackBmp);
	m_WndSize.cx=ImageHandle.GetWidth();
	m_WndSize.cy=ImageHandle.GetHeight();

	//设置界面
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);

	if(!m_Font.m_hObject)
		m_Font.CreateFont(-16,0,0,0,400,0,0,0,134,3,2,1,2,TEXT("新宋体"));
	return 0;
}

//重画函数
void CPalatesNeck::OnPaint() 
{
	CPaintDC dc(this); 
	CFont *oldFont = dc.SelectObject(&m_Font);
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(255,255,255));
//	dc.SelectObject(&CGameImageLink::m_HFont);

	CGameImageHelper ImageHandle(&m_BackBmp);
	ImageHandle.BitBlt(dc,0,0,ImageHandle.GetWidth(),ImageHandle.GetHeight(),0,0);

	dc.SetTextColor(RGB(0,0,0));
	dc.TextOut(81,120,"用户名",lstrlen("用户名"));
	dc.TextOut(80,121,"用户名",lstrlen("用户名"));
	dc.SetTextColor(RGB(255,255,255));
	dc.TextOut(80,120,"用户名",lstrlen("用户名"));

	dc.SetTextColor(RGB(0,0,0));
	dc.TextOut(221,120,"成绩",lstrlen("成绩"));
	dc.TextOut(220,121,"成绩",lstrlen("成绩"));
	dc.SetTextColor(RGB(255,255,255));
	dc.TextOut(220,120,"成绩",lstrlen("成绩"));

	//显示经验值
	int dJianGe=160/m_bGamePeople;
	int iStation=0;
	TCHAR szBuffer[30];
	memset(szBuffer,0,sizeof(szBuffer));
	for (BYTE i=0;i<m_bGamePeople;i++)
	{
		if (m_strName[i].IsEmpty()==false)
		{
			dc.SetTextColor(RGB(0,0,0));
			dc.TextOut(81,(int)(iStation*dJianGe+140),m_strName[i]);
			dc.TextOut(80,(int)(iStation*dJianGe+141),m_strName[i]);
			dc.SetTextColor(RGB(255,255,255));
			dc.TextOut(80,(int)(iStation*dJianGe+140),m_strName[i]);
			wsprintf(szBuffer,TEXT("%ld"),m_dwPoint[i]);

			dc.SetTextColor(RGB(0,0,0));
			dc.TextOut(221,(int)(iStation*dJianGe+140),szBuffer,lstrlen(szBuffer));
			dc.TextOut(220,(int)(iStation*dJianGe+141),szBuffer,lstrlen(szBuffer));
			dc.SetTextColor(RGB(255,255,255));
			dc.TextOut(220,(int)(iStation*dJianGe+140),szBuffer,lstrlen(szBuffer));
			iStation++;
		}
	}

	//显示税收
	if ((m_bShowTax==true)&&(iStation>0))
	{
		dc.SetTextColor(RGB(0,0,0));
		dc.TextOut(81,(int)(iStation*dJianGe+140),TEXT("交易税"),lstrlen(TEXT("交易税")));
		dc.TextOut(80,(int)(iStation*dJianGe+141),TEXT("交易税"),lstrlen(TEXT("交易税")));
		dc.SetTextColor(RGB(255,255,255));
		dc.TextOut(80,(int)(iStation*dJianGe+140),TEXT("交易税"),lstrlen(TEXT("交易税")));
		wsprintf(szBuffer,"%ld",m_dwGameTax);

		dc.SetTextColor(RGB(0,0,0));
		dc.TextOut(221,(int)(iStation*dJianGe+140),szBuffer,lstrlen(szBuffer));
		dc.TextOut(220,(int)(iStation*dJianGe+141),szBuffer,lstrlen(szBuffer));
		dc.SetTextColor(RGB(255,255,255));
		dc.TextOut(220,(int)(iStation*dJianGe+140),szBuffer,lstrlen(szBuffer));
	}
	dc.SelectObject(oldFont);
	return;
}

//获取大小函数
void CPalatesNeck::GetWndSize(SIZE & Size)
{
	Size=m_WndSize;
	return;
}

//设置信息
void CPalatesNeck::SetGamePoint(BYTE bDeskStation, TCHAR * szUserName, long int dwPoint)
{
	if (bDeskStation<m_bGamePeople)
	{
		m_strName[bDeskStation]=szUserName;
		m_dwPoint[bDeskStation]=dwPoint;
		Invalidate(FALSE);
	}
	return;
}

//设置税收
void CPalatesNeck::SetTax(long int dwGameTax)
{
	m_dwGameTax=dwGameTax;
	Invalidate(FALSE);
	return;
}

/*******************************************************************************************************/

void CPalatesNeck::OnLButtonDown(UINT nFlags, CPoint point)
{
	ShowWindow(SW_HIDE);

	CWnd::OnLButtonDown(nFlags, point);
}
