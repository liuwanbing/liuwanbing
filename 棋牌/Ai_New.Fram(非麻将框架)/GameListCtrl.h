#pragma once
#include "StdAfx.h"
//#include "GamePlace.h"
#include "comstruct.h"

//类说明
class CGameListCtrl;
struct MSG_GP_SR_OnLineStruct;
#include <vector>
using namespace std;

/*******************************************************************************************************/
//游戏列表子项
class CAFCGameListItem
{
	//数据变量
public:
	UINT										m_uDataType;					//数据类型
	LONG										m_uUpdateTime;					//更新时间
	CPtrArray									m_ItemPtrArray;					//子项数组
	CAFCGameListItem							* m_pParentItem;				//父项指针

	//视图数据
public:
	HTREEITEM									m_hUpdateItem;					//获取索引
	HTREEITEM									m_hCurrentItem;					//当前索引
	static CGameListCtrl						* m_pTreeCtrl;					//树控件指针

	//函数定义
public:
	//析构函数
	CAFCGameListItem();
	//析构函数
	virtual ~CAFCGameListItem();

	//功能函数
public:
	//初始化项
	bool Init(CAFCGameListItem * pParentItem);
	//加入游戏类型
	bool AddGameKind(ComKindInfo * pGameKind);
	//加入游戏名字
	bool AddGameName(ComNameInfo * pGameName);
	//加入游戏房间
	bool AddGameRoom(ComRoomInfo * pGameRoom);
	//加入比赛房间
	bool AddContestItem(ComRoomInfo * pGameRoom);
	//加入内部数据
	bool AddInsideData(GameInsideInfo * pInsideData, UINT uIconIndex);
	//展开数据
	bool ExpandItem(int iExpendCount);
	//是否需要更新
	bool IsNeedUpdate();
	//更新获取数据项
	bool UpdateGetListItem();
	//删除过时房间 
	bool DeleteOutTimeItem();

	//辅助函数
protected:
	//插入数据
	HTREEITEM AddTreeData(TCHAR * szText, UINT uIconIndex, LPARAM lParam);
};

/*******************************************************************************************************/

//游戏类型子项
class CAFCKindItem : public CAFCGameListItem
{
	//变量定义
public:
	ComKindInfo								m_KindInfo;						//类型数据

	//函数定义
public:
	//构造函数
	CAFCKindItem() 
	{ 
		m_uDataType=GLK_NO_DATA;
		memset(&m_KindInfo,0,sizeof(m_KindInfo)); 
		m_KindInfo.Head.uSize=sizeof(m_KindInfo);
		m_KindInfo.Head.bDataType=GLK_GAME_KIND;
	};
	//析构函数
	virtual ~CAFCKindItem() {};
};

/*******************************************************************************************************/

//游戏名字子项
class CAFCNameItem : public CAFCGameListItem
{
	//变量定义
public:
	///UINT										m_uOnLineCount;					//在线人数
	ComNameInfo								m_NameInfo;						//名字数据	
	//函数定义
public:
	//构造函数
	CAFCNameItem() 
	{ 
		m_hUpdateItem=NULL;
		m_uDataType=GLK_GAME_NAME;
		memset(&m_NameInfo,0,sizeof(m_NameInfo)); 
		m_NameInfo.Head.uSize=sizeof(m_NameInfo);
		m_NameInfo.Head.bDataType=GLK_GAME_NAME;
	};
	//析构函数
	virtual ~CAFCNameItem() {};
	//设置在线人数
	bool UpdateOnLineCount();
};

/*******************************************************************************************************/

//游戏房间子项
class CAFCRoomItem : public CAFCGameListItem
{
	//变量定义
public:
	ComRoomInfo								m_RoomInfo;						//房间数据
	int										m_iRoomIndex;		//比赛房间的当前索引，从服务器获得
	bool									m_bIsContest;		//是否比赛结点
	int										m_iContestID;		//比赛ID
	int										m_iContestPeopleCount; //比赛人数
	vector<ComRoomInfo>						m_RoomContest;		//比赛房间
	//函数定义
public:
	//构造函数
	CAFCRoomItem() 
	{ 
		m_uDataType=GLK_GAME_ROOM;
		memset(&m_RoomInfo,0,sizeof(m_RoomInfo)); 
		m_RoomInfo.Head.uSize=sizeof(m_RoomInfo);
		m_RoomInfo.Head.bDataType=GLK_GAME_ROOM;
		m_iRoomIndex = -1;
		m_bIsContest = false;
		m_iContestID = 0;
		m_iContestPeopleCount = 0;
	};
	//析构函数
	virtual ~CAFCRoomItem() {};
};

/*******************************************************************************************************/

//游戏内部子项
class CAFCInsideItem : public CAFCGameListItem//游戏列表
{
	//变量定义
public:
	GameInsideInfo								m_InsideInfo;					//内部数据

	//函数定义
public:
	//构造函数
	CAFCInsideItem()
	{
		m_uDataType=GLK_GAME_USE;
		memset(&m_InsideInfo,0,sizeof(m_InsideInfo)); 
		m_InsideInfo.Head.uSize=sizeof(m_InsideInfo);
		m_InsideInfo.Head.bDataType=GLK_GAME_USE;
	}
	//析构函数
	virtual ~CAFCInsideItem() {};
};

/*******************************************************************************************************/

//游戏列表控制类
class CGameListCtrl : public CTreeCtrl////////////重新派生一个新类
{
	//变量定义
protected:
	CPtrArray									m_InsideArray;					//游戏内部数组
	DECLARE_MESSAGE_MAP()
	//函数定义
public:
	///void UpdateAllOnLineData();
	

//处理数据
    bool HandleNameOnLineData(DL_GP_RoomListPeoCountStruct * pOnLineCountPtr, UINT uCount);
	// 查找游戏
	CAFCNameItem *FindNameItem(UINT uID);
	// 查找房间
	CAFCRoomItem *FindRoomItem(UINT uRoomID);

	void IEnterARoomOrLeave(UINT uRoomID,bool bEnter);
	///void IEnterARoomOrLeave(CAFCRoomItem * pGameRoomItem,bool bEnter);
	bool ExpandItemFirstRoom();
	bool HandleRoomOnLineData(DL_GP_RoomListPeoCountStruct * pOnLineCountPtr, UINT uCount);
	//析构函数
	CGameListCtrl();
	//析构函数
	virtual ~CGameListCtrl();
	
	//功能函数
public:
	//初始化函数
	bool Init();
	//展开数据
	bool ExpandItem(int iExpendCount);
	//插入根项
	bool AddRootItem(GameInsideInfo * pInsideData, UINT uIconIndex);
	//获取根项
	CAFCGameListItem * GetKindParentItem() { return (CAFCGameListItem *)m_InsideArray.GetAt(0); }
	//处理数据
	bool HandleIOnLineData(MSG_GP_SR_OnLineStruct * pOnLineCountPtr, UINT uCount);
	//处理数据
	bool HandleKindData(ComKindInfo * pKindInfoPtr, UINT uKindCount);
	//处理数据
	bool HandleNameData(ComNameInfo * pNameInfoPtr, UINT uNameCount);
	//处理数据
	bool HandleRoomData(ComRoomInfo * pRoomInfoPtr, UINT uRoomCount, UINT uKindID, UINT uNameID);
	//填充获取数据项
	bool FillGetListItem();
	//删除过时房间 
	bool DeleteOutTimeRoom(UINT uKindID, UINT uNameID);
	//更新总在线人数
	bool UpdateOnLineCount();
	//获取进程名字
	bool GetProcessName(CAFCRoomItem * pGameRoomItem, TCHAR * szProcessName, UINT uBufferSize);
	//获取游戏名字
	bool GetGameName(CAFCRoomItem * pGameRoomItem, TCHAR * szGameName, UINT uBufferSize);
	void UpdataItem(CAFCGameListItem* pItem);
	//辅助函数
//private:
	//查找类型子项
	CAFCKindItem * FindKindItem(UINT uKindID);
	//查找名字子项
	CAFCNameItem * FindNameItem(CAFCKindItem * pKindItem, UINT uNameID);
	//查找房间子项
	CAFCRoomItem * FindRoomItem(CAFCNameItem * pNameItem, UINT uRoomID);
	//查找房间子项
	CAFCRoomItem * FindRoomItemByContest(CAFCNameItem * pNameItem, ComRoomInfo* pRoomInfo);
	//查找比赛房间子项
	ComRoomInfo * FindContestRoomItem(CAFCRoomItem * pRoomItem, int iRoomID);
public:
	void UpdateAllIcon(void);
	void JoinAllItem(void);
public:
	bool ExpandAll(bool ExpName=true);
};

/*******************************************************************************************************/
