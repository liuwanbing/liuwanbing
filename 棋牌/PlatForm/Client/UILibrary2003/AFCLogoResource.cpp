#include "StdAfx.h"
#include "AFCFunction.h"
#include "AFCLogoResource.h"
#include "..\\Resource2003\\Resource.h"

BEGIN_MESSAGE_MAP(CGameClientTranStatic, CStatic)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

//using namespace AFC;

//静态变量
CImage				CGameLogoRes::m_MatchPic;								//比赛头像
CImage				CGameLogoRes::m_LostPic;								//断线头像
CImage				CGameLogoRes::m_LogoPicArray[LOGO_COUNT];				//头像图片数组
CImage				CGameLogoRes::SitBoyArray[ZZ_COUNT];				//男人坐姿图片数组
CImage				CGameLogoRes::SitGirlArray[ZZ_COUNT];				//女人坐姿图片数组
COLORREF			CGameLogoRes::m_crBackColor=RGB(0,0,0);					//透明颜色

/*******************************************************************************************************/

//构造函数
CGameLogoRes::CGameLogoRes(void)
{
	Init();
}

//析构函数
CGameLogoRes::~CGameLogoRes(void)
{
}

//获取比赛图片
CImage * CGameLogoRes::GetMatchImage()
{
	return &m_MatchPic;
}

//初始化函数
bool CGameLogoRes::Init()
{
	HMODULE hResDllHandle=AfxLoadLibrary(TEXT("HN_SYSR.dll"));
	if (hResDllHandle!=NULL)
	{
		AfxSetResourceHandle(hResDllHandle);
		for (int i=0;i<LOGO_COUNT;i++)	
		{
			TCHAR chLogName[150];
			wsprintf(chLogName,".\\image\\log\\gamelog%d.png",i%6);
			m_LogoPicArray[i].Load(chLogName);
		}
/*		for (UINT i=0;i<ZZ_COUNT;i++)	
		{
			SitBoyArray[i].LoadFromResource(hResDllHandle,IDB_boy01+i);///////////读男人坐姿图片树组
			SitGirlArray[i].LoadFromResource(hResDllHandle,IDB_Girl01+i);///////////读女人坐姿图片树组
		}*/
		m_LostPic.LoadFromResource(hResDllHandle,IDB_LOST_LOGO);
		m_MatchPic.LoadFromResource(hResDllHandle,IDB_MATCH_LOGO);
		m_crBackColor=m_LogoPicArray[1].GetPixel(0,0);
		AfxSetResourceHandle(GetModuleHandle(NULL));
		FreeLibrary(hResDllHandle);
		return true;
	}
	return false;

}

//获取 LOGO 图片
CImage * CGameLogoRes::GetLogoImage(UINT uIndex, bool bLost)
{
	if (bLost==false) return &m_LogoPicArray[uIndex%LOGO_COUNT];
	return &m_LostPic;
}


//获取坐姿 图片，坐上的是女的
CImage * CGameLogoRes::GetSitGirl(UINT uIndex, bool bLost)
{
	if (bLost==false) return &SitGirlArray[uIndex%LOGO_COUNT];
	return &m_LostPic;
}
//获取坐姿 图片，坐上的是男的
CImage * CGameLogoRes::GetSitBoy(UINT uIndex, bool bLost)
{
	if (bLost==false) return &SitBoyArray[uIndex%LOGO_COUNT];
	return &m_LostPic;
}

/*******************************************************************************************************/

//构造函数
CGameClientTranStatic::CGameClientTranStatic(void)
{
	m_pLogoImage=NULL;
}

//析构函数
CGameClientTranStatic::~CGameClientTranStatic(void)
{
}

//设置图标
bool CGameClientTranStatic::SetLogoImage(CImage * pLogoImage)
{
	m_pLogoImage=pLogoImage;
	Invalidate(FALSE);
	return true;
}

//重画函数
void CGameClientTranStatic::OnPaint()
{
	CPaintDC dc(this);

	//设置底图
	CRect ClientRect;
	GetClientRect(&ClientRect);
	dc.FillSolidRect(&ClientRect,RGB(250,250,250));
	if ((m_pLogoImage!=NULL)&&(m_pLogoImage->IsNull()==false)) 
		AFCStretchImage(&dc,0,0,LOGO_WIDTH,LOGO_HEIGHT,*m_pLogoImage,0,0,LOGO_WIDTH,LOGO_HEIGHT,CGameLogoRes::GetBackColor());
	
	return;
}

//鼠标单击
void CGameClientTranStatic::OnLButtonDown(UINT nFlags, CPoint point)
{
	GetParent()->PostMessage(WM_HIT_LOGO,0,0);
	__super::OnLButtonDown(nFlags, point);
}

//鼠标双击击
void CGameClientTranStatic::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	GetParent()->PostMessage(WM_HIT_LOGO,0,0);
	__super::OnLButtonDblClk(nFlags, point);
}

/*******************************************************************************************************/

