#include "StdAfx.h"
#include "PlaceResource.h"

//变量定义 
CImageList											CPlaceResource::m_GameListImage;		//游戏图标
CArray<IconIDInfoStruct,IconIDInfoStruct>			CPlaceResource::m_IconIndex;			//索引

//DLL 接口函数定义
typedef HICON (GetGameIconFunc)(UINT,bool);
// 用来输出调试信息
extern void DebugPrintf(const char *p, ...);


//初始化函数
void CPlaceResource::Init(COLORREF color)
{
	//初始化游戏列表图标
	UINT uIconID[]=
	{
		//固定图标
		IDI_LIST_KIND,
		IDI_LIST_ROOM,
		IDI_LIST_NONAME,
		IDI_LIST_NOINSTALL,
		IDI_LIST_ROOM,
		IDI_LIST_ROOM,

		//大厅部分
		IDI_LIST_ROOT,
		IDI_LIST_ROOM_LOCK,

		// duanxiaohui 20111107 add 大厅游戏列表二级目录图标(因二级目录Icon下标位置为13, 但8、9、10、11、12 之前下标位没有Icon, 暂时全部填充一样的)
		IDI_LIST_KIND2,
		IDI_LIST_KIND2,
		IDI_LIST_KIND2,
		IDI_LIST_KIND2,
		IDI_LIST_KIND2,
		IDI_LIST_KIND2
		// end duanxiaohui
	};
	m_GameListImage.DeleteImageList();
	m_GameListImage.Create(16,16,ILC_COLOR16|ILC_MASK,sizeof(uIconID)/sizeof(uIconID[0]),sizeof(uIconID)/sizeof(uIconID[0]));
	m_GameListImage.SetBkColor(color);
	for (UINT i=0;i<sizeof(uIconID)/sizeof(uIconID[0]);i++)
	{
		HICON hIcon=AfxGetApp()->LoadIcon(uIconID[i]);
		m_GameListImage.Add(hIcon);
		DestroyIcon(hIcon);
	}
	return;
}

void CPlaceResource::SetColor(COLORREF color)
{
	m_GameListImage.SetBkColor(color);
}
//获取图标索引
UINT CPlaceResource::GetListIconIndex(ComNameInfo * pNameItem)
{
	//寻找图标
	for (INT_PTR i=0;i<m_IconIndex.GetCount();i++)
	{
		IconIDInfoStruct * pIconInfo=&m_IconIndex[i];
		if (pIconInfo->uNameID==pNameItem->uNameID)	return pIconInfo->uIconIndex;
	}

	CString gamename=pNameItem->szGameProcess;
	gamename.MakeLower();
	if(gamename.Right(4)==".ico")
	{//原有方式
		//获取DLL中的图标
		HINSTANCE hDllHandle=::LoadLibrary(pNameItem->szGameProcess);
		if (hDllHandle!=NULL)
		{
			//获取图标
			GetGameIconFunc * pGetIcon=(GetGameIconFunc *)GetProcAddress(hDllHandle,TEXT("BeginInitGameIcon"));
			if (pGetIcon!=NULL)
			{
				IconIDInfoStruct IconInfo;
				memset(&IconInfo,0,sizeof(IconInfo));
				IconInfo.uNameID=pNameItem->uNameID;
				HICON hListIcon=pGetIcon(pNameItem->uNameID,true);
				if (hListIcon!=NULL) 
				{
					IconInfo.uIconIndex=m_GameListImage.Add(hListIcon);
					DestroyIcon(hListIcon);
				}
				else IconInfo.uIconIndex=IND_LIST_NO_ICON;
				::FreeLibrary(hDllHandle);
				return IconInfo.uIconIndex;
			}
			::FreeLibrary(hDllHandle);
			return IND_LIST_NO_ICON;///无图标，使用默认图标
		}
		return IND_LIST_NO_INSTALL;//没安装
	}
	else
	{//分离方式
		//获取DLL中的图标
		//{{Modified by zxd 正确判断游戏程序文件名
		TCHAR szIcoName[MAX_PATH];
		IconIDInfoStruct IconInfo;
		memset(&IconInfo,0,sizeof(IconInfo));
		IconInfo.uNameID=pNameItem->uNameID;

		wsprintf(szIcoName,"%d\\GameIcon.ico",pNameItem->uNameID);
		HICON hListIcon=(HICON)LoadImage(NULL,szIcoName,IMAGE_ICON,0,0,LR_LOADFROMFILE);//LoadImage()  LoadIcon(AfxGetInstanceHandle(),szIcoName);
		if(hListIcon)
		{
			IconInfo.uIconIndex=m_GameListImage.Add(hListIcon);
			DestroyIcon(hListIcon);
		}
		else
			IconInfo.uIconIndex=IND_LIST_NO_INSTALL;
		return IconInfo.uIconIndex;
	}



/*

//没安装*/
}

//获取房间图标
HICON CPlaceResource::GetRoomIcon(CAFCRoomItem * pRoomItem, TCHAR * szProcessName)
{
	HICON hRoomIcon=NULL;
	//{{Modify By ZXD为的是正确装载游戏图标，从服务器传来的是exe扩展名
	TCHAR szIcoName[MAX_PATH];
	int nLen = _tcslen(szProcessName);
	if ((szProcessName[nLen-3] == TCHAR('e') || szProcessName[nLen-3] == TCHAR('E'))
		&&(szProcessName[nLen-2] == TCHAR('x') || szProcessName[nLen-2] == TCHAR('X'))
		&&(szProcessName[nLen-1] == TCHAR('e') || szProcessName[nLen-1] == TCHAR('E'))
		)
	{
		wsprintf(szIcoName,"%d\\GameIcon.ico",pRoomItem->m_RoomInfo.uNameID);
		hRoomIcon=LoadIcon(AfxGetInstanceHandle(),szIcoName);
		if (hRoomIcon==NULL) hRoomIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_LOGON));
		return hRoomIcon;
	}
	else
	{
		HINSTANCE hDllHandle=::LoadLibrary(szProcessName);
		if (hDllHandle!=NULL)
		{
			//获取图标
			GetGameIconFunc * pGetIcon=(GetGameIconFunc *)GetProcAddress(hDllHandle,TEXT("BeginInitGameIcon"));
			if (pGetIcon!=NULL) hRoomIcon=pGetIcon(pRoomItem->m_RoomInfo.uNameID,false);
			::FreeLibrary(hDllHandle);
		}

		//获取默认图标
		if (hRoomIcon==NULL) hRoomIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_LOGON));
		return hRoomIcon;
	}
}

UINT CPlaceResource::LoadExtendIcon(CString szIconPath)
{
	HICON hExtendIcon=NULL;
	CString spath;
	spath.Format("%s\\image\\client\\%s",CBcfFile::GetAppPath(),szIconPath);
	hExtendIcon=(HICON)LoadImage(NULL,spath,IMAGE_ICON,0,0,LR_LOADFROMFILE);
	if(hExtendIcon)
	{
		UINT uIconIndex=m_GameListImage.Add(hExtendIcon);
		DestroyIcon(hExtendIcon);
		return uIconIndex;
	}
	return 0;
}
