#pragma once
#ifndef PLACERESOURCE_HEAD_H
#define PLACERESOURCE_HEAD_H
#include "StdAfx.h"
#include "Resource.h"
#include "AfxTempl.h"
#include "GameListCtrl.h"

//固定图标
#define IND_KIND_ICON				0							//游戏类型
#define IND_ROOM_ICON				1							//游戏房间
#define IND_LIST_NO_ICON			2							//没有图标
#define IND_LIST_NO_INSTALL			3							//没有安装
#define IND_ROOM_NO_ICON			4							//没有图标
#define IND_NO_ROOM					5							//没有房间

//扩展图标
#define IND_LIST_ROOT				6							//游戏列表
#define IND_LIST_BANK				7							//在线银行
#define IND_LIST_DOWN				8							//游戏下载
#define IND_LIST_FORUM				9							//游戏论坛
#define IND_LIST_LINE				10							//游戏排行
#define IND_LIST_PROTECT			11							//密码保护
#define IND_LIST_CLASS				12							//游戏社团

// duanxiaohui 20111107 add 大厅二级目录图标
#define IND_KIND2_ICON              13                          //游戏类型
// end duanxiaohui

//图标索引
struct IconIDInfoStruct
{
	UINT						uNameID;						//名字 ID
	UINT						uIconIndex;						//图标索引
};

//游戏大厅资源类
class CPlaceResource
{
	//变量定义
protected:
	static CArray<IconIDInfoStruct,IconIDInfoStruct>			m_IconIndex;			//索引
	

	//函数定义
public:
	static CImageList											m_GameListImage;		//图标
	//初始化函数
	static void Init(COLORREF color);
	//变色
	static void SetColor(COLORREF color);
	//获取图标列
	static CImageList * GetGameListIcon() { return & m_GameListImage; }
	//获取图标索引
	static UINT GetListIconIndex(ComNameInfo * pNameItem);
	//获取房间图标
	static HICON GetRoomIcon(CAFCRoomItem * pRoomItem, TCHAR * szProcessName);
public:
	static UINT LoadExtendIcon(CString szIconPath);
};
#endif