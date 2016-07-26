#include "StdAfx.h"
#include "Resource.h" 
#include "AFCResource.h"
#include "AFCLogoResource.h"

//名字空间
//using namespace AFC;
//using namespace AFC::AFCResource;

//全局使用变量
bool					CGameImageLink::m_bAutoLock=true;				//是否加载
CFont					CGameImageLink::m_VFont;						//显示字体
CFont					CGameImageLink::m_HFont;						//显示字体
HCURSOR					CGameImageLink::m_hHandCursor=NULL;				//鼠标光标
CAFCTabResource			CGameImageLink::m_TabResource;					//属性页资源
CAFCTitleResource		CGameImageLink::m_TitleResource;				//标题资源
CAFCButtonResource		CGameImageLink::m_ButtonResource;				//按钮资源
CAFCListResource		CGameImageLink::m_ListResource;					//列表资源
CAFCDialogResource		CGameImageLink::m_DialogResource;				//对话框资源
CAFCEditResource		CGameImageLink::m_EditResource;					//编辑控件资源
CAFCSplitResource		CGameImageLink::m_SplitResource;				//拆分条资源
CAFCColorResource		CGameImageLink::m_ColorResource;				//颜色资源

/*******************************************************************************************************/

//初始化资源
bool CGameImageLink::InitResource(TCHAR * szSinkFileName)
{
	//资源句柄
	HINSTANCE hDllHinstance=::GetModuleHandle(FACE_DLL_NAME);
	//初始化头像资源
	CGameLogoRes::Init();

	//初始化全局资源
	m_VFont.CreateFont(-12,0,900,900,400,0,0,0,134,0,0,0,0,TEXT("新宋体"));
	m_HFont.CreateFont(-12,0,0,0,400,0,0,0,134,3,2,1,2,TEXT("新宋体"));
	m_hHandCursor=::LoadCursor(hDllHinstance,MAKEINTRESOURCE(IDC_HAND));
	
	//初始化属性页资源
	m_TabResource.m_crFrameHeight=RGB(255,255,255);
	m_TabResource.m_crFrameShadow=RGB(0,0,0);
	m_TabResource.m_crFrameBKColor=RGB(125,125,125);
	m_TabResource.m_crNormalTXColor=RGB(0,0,0);
	//m_TabResource.m_crSelectTXColor=RGB(255,0,0);//红色
	m_TabResource.m_crSelectTXColor=RGB(186,218,254);//淡蓝色
	//m_TabResource.m_ActivePic.SetLoadInfo(hDllHinstance,IDB_TAB_ACTIVE,m_bAutoLock);//主选项卡按下
	//m_TabResource.m_NormalPic.SetLoadInfo(hDllHinstance,IDB_TAB_NORMAL,m_bAutoLock);//主选项卡突起
	//m_TabResource.m_BackPic.SetLoadInfo(hDllHinstance,IDB_TAB_BACK,m_bAutoLock);///最下层的背景图片
	//m_TabResource.m_ActivePic.SetLoadInfo("tab_sel.bmp",m_bAutoLock);//主选项卡按下
	//m_TabResource.m_NormalPic.SetLoadInfo("tab_normal.bmp",m_bAutoLock);//主选项卡突起
	//m_TabResource.m_BackPic.SetLoadInfo("tab_bottom.bmp",m_bAutoLock);///最后面的背景图片

	////甄亮添加
	//m_TabResource.m_BackBottom.SetLoadInfo(hDllHinstance,IDB_ROOM_BOTTOM,m_bAutoLock);//主框架底部中间平铺图片
	//m_TabResource.m_BackBottomLeft.SetLoadInfo(hDllHinstance,IDB_ROOM_BOTTOM_LEFT,m_bAutoLock);//////////主框架底部左侧图片
	//m_TabResource.m_BackBottomRight.SetLoadInfo(hDllHinstance,IDB_ROOM_BOTTOM_RIGHT,m_bAutoLock);/////主框架底部右侧图片
	//m_TabResource.m_BackBottom.SetLoadInfo("tab_bottom.bmp",m_bAutoLock);//主框架底部中间平铺图片
	//m_TabResource.m_BackBottomLeft.SetLoadInfo("bottom1.jpg",m_bAutoLock);//////////主框架底部左侧图片
	//m_TabResource.m_BackBottomRight.SetLoadInfo("bottom3.jpg",m_bAutoLock);/////主框架底部右侧图片
//	m_TabResource.m_BackPicNew.SetLoadInfo(hDllHinstance,IDB_TAB_BACK_NEW,m_bAutoLock);

	//初始化 TAB 控件资源
	m_TitleResource.m_crTextColor=RGB(255,255,255);
//	m_TitleResource.m_BackPic.SetLoadInfo(hDllHinstance,IDB_TITLE_NOR,m_bAutoLock);

	//初始化按钮资源
	m_ButtonResource.m_bSound=true;
	m_ButtonResource.m_hMouneCursor=NULL;

	m_ButtonResource.m_ActiveColor=RGB(255,120,80);		
	m_ButtonResource.m_ActiveTextColor=RGB(0,0,255);	
	m_ButtonResource.m_NormalTextColor=RGB(0,0,0);		
	m_ButtonResource.m_SelectTextColor=RGB(0,0,255);	
	m_ButtonResource.m_FrameColor=RGB(0,64,128);		
	m_ButtonResource.m_FrameHeight=RGB(230,230,230);	
	m_ButtonResource.m_FrameShadow=RGB(128,128,128);	
	m_ButtonResource.m_NormalColorTop=RGB(255,255,255);
	m_ButtonResource.m_NormalColorBottom=RGB(0,128,255);
//	m_ButtonResource.m_BackPic.SetLoadInfo(hDllHinstance,IDB_BUTTON_BACK,m_bAutoLock);

	//初始化列表资源
//	m_ListResource.m_crBkColor=RGB(107,142,198);
	//甄亮更改
	m_ListResource.m_crBkColor=RGB(233,233,233);
	m_ListResource.m_crFousBkColor=RGB(125,125,125);
	m_ListResource.m_crFousTxColor=RGB(0,0,255);
	m_ListResource.m_crEmemyTxColor=RGB(0,0,255);
	m_ListResource.m_crNormalTxColor=RGB(0,0,0);
	m_ListResource.m_crNorMemTxColor=RGB(0,0,255);
	m_ListResource.m_crHighMemTxColor=RGB(0,0,255);
	m_ListResource.m_crFriendTxColor=RGB(0,0,255);
	m_ListResource.m_crMasterTxColor=RGB(0,0,255);
	m_ListResource.m_crMeTxColor=RGB(0,0,255);
	m_ListResource.m_crListColor=RGB(255,255,255);
//	m_ListResource.m_TitlePic.SetLoadInfo(hDllHinstance,IDB_LIST_TITLE,m_bAutoLock);

	//初始化对话框资源
	m_DialogResource.m_rcTopFrameColor=RGB(0,80,255);	
	m_DialogResource.m_rcButFrameColor=RGB(0,52,214);	
	m_DialogResource.m_rcTitleTextColor=RGB(255,255,255);
	m_DialogResource.m_crTextColor=RGB(0,0,0);
	m_DialogResource.m_crBackColor=RGB(240,240,240);
	m_DialogResource.m_BackBrush.CreateSolidBrush(RGB(240,240,240));		
//	m_DialogResource.m_Title.SetLoadInfo(hDllHinstance,IDB_TITLE_NOR,m_bAutoLock);
//	m_DialogResource.m_CloseNor.SetLoadInfo(hDllHinstance,IDB_CLOSE_NOR,m_bAutoLock);
//	m_DialogResource.m_CloseFoc.SetLoadInfo(hDllHinstance,IDB_CLOSE_FOC,m_bAutoLock);
//	m_DialogResource.m_CloseDis.SetLoadInfo(hDllHinstance,IDB_CLOSE_DIS,m_bAutoLock);
//	m_DialogResource.m_MaxNor.SetLoadInfo(hDllHinstance,IDB_MAX_NOR,m_bAutoLock);
//	m_DialogResource.m_MaxFoc.SetLoadInfo(hDllHinstance,IDB_MAX_FOC,m_bAutoLock);
//	m_DialogResource.m_MaxDis.SetLoadInfo(hDllHinstance,IDB_MAX_DIS,m_bAutoLock);
//	m_DialogResource.m_ResNor.SetLoadInfo(hDllHinstance,IDB_RES_NOR,m_bAutoLock);
//	m_DialogResource.m_ResFoc.SetLoadInfo(hDllHinstance,IDB_RES_FOC,m_bAutoLock);
//	m_DialogResource.m_ResDis.SetLoadInfo(hDllHinstance,IDB_RES_DIS,m_bAutoLock);
//	m_DialogResource.m_MinNor.SetLoadInfo(hDllHinstance,IDB_MIN_NOR,m_bAutoLock);
//	m_DialogResource.m_MinFoc.SetLoadInfo(hDllHinstance,IDB_MIN_FOC,m_bAutoLock);
//	m_DialogResource.m_MinDis.SetLoadInfo(hDllHinstance,IDB_MIN_DIS,m_bAutoLock);
//	m_DialogResource.m_BackPic.SetLoadInfo(hDllHinstance,IDB_DIALOG_BACK,m_bAutoLock);

	//初始化编辑框资源
	m_EditResource.m_crFocusTXRGB=RGB(200,0,0);
	m_EditResource.m_crNoFocusTXRGB=RGB(0,0,0);
	m_EditResource.m_crDisFocusTXRGB=RGB(255,255,255);
	m_EditResource.m_crFocusBKRGB=RGB(255,255,0);
	m_EditResource.m_crNoFocusBKRGB=RGB(255,255,255);
	m_EditResource.m_crDisFocusBKRGB=RGB(125,125,125);
	m_EditResource.m_brFouns.CreateSolidBrush(RGB(255,255,0));
	m_EditResource.m_brNoFocus.CreateSolidBrush(RGB(255,255,255));
	m_EditResource.m_brDiable.CreateSolidBrush(RGB(125,125,125));

	//初始化拆分条资源
	m_SplitResource.m_hHorCur=LoadCursor(hDllHinstance,MAKEINTRESOURCE(IDC_HCUR));
	m_SplitResource.m_hVorCur=LoadCursor(hDllHinstance,MAKEINTRESOURCE(IDC_VCUR));
	m_SplitResource.m_hDisableCur=LoadCursor(hDllHinstance,MAKEINTRESOURCE(IDC_DIS));

	//初始化颜色资源
	m_ColorResource.m_crListTxColor=RGB(0,0,0);		
	m_ColorResource.m_crListBkColor=RGB(240,240,250);//列表背景颜色，以前是RGB(242,235,219);现在改成兰色
	m_ColorResource.m_crDeskTxColor=RGB(240,240,240);		
	m_ColorResource.m_crSysHeadTxColor=RGB(255,0,0);
	m_ColorResource.m_crSystemTxColor=RGB(255,0,0);
	m_ColorResource.m_crNewsTxColor=RGB(255,0,0);
	m_ColorResource.m_crNormalTxColor=RGB(0,0,200);
//	m_ColorResource.m_crTalkNameColor=RGB(255,255,255);
	m_ColorResource.m_crTalkNameColor=RGB(0,0,0);
	m_ColorResource.m_crTalkTxColor=RGB(0,0,0);
//	m_ColorResource.m_crTalkBkColor=RGB(107,142,198);
	m_ColorResource.m_crTalkBkColor=RGB(233,233,233);

	return true;
}
/*******************************************************************************************************/