#pragma once

/*******************************************************************************************************/

//游戏列表子项
class CAFCGameListItem
{
	//数据变量
public:
	CPtrArray									m_ItemPtrArray;					//子项数组

	//函数定义
public:
	//析构函数
	CAFCGameListItem();
	CAFCGameListItem(CAFCGameListItem&);
	CAFCGameListItem& operator=(CAFCGameListItem&); 

	//析构函数
	virtual ~CAFCGameListItem();

	//功能函数
public:
	//加入游戏类型
	bool AddGameKind(ComKindInfo * pGameKind);
	//加入游戏名字
	bool AddGameName(ComNameInfo * pGameName);
	//加入游戏房间
	bool AddGameRoom(ComRoomInfo * pGameRoom);
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
//		m_uOnLineCount=0L;
		memset(&m_NameInfo,0,sizeof(m_NameInfo)); 
		m_NameInfo.Head.uSize=sizeof(m_NameInfo);
		m_NameInfo.Head.bDataType=GLK_GAME_NAME;
	};
	//析构函数
	virtual ~CAFCNameItem() {};
};

/*******************************************************************************************************/

//游戏房间子项
class CAFCRoomItem : public CAFCGameListItem
{
	//变量定义
public:
	ComRoomInfo								m_RoomInfo;						//房间数据

	//函数定义
public:
	//构造函数
	CAFCRoomItem() 
	{ 
		memset(&m_RoomInfo,0,sizeof(m_RoomInfo)); 
		m_RoomInfo.Head.uSize=sizeof(m_RoomInfo);
		m_RoomInfo.Head.bDataType=GLK_GAME_ROOM;
	};
	CAFCRoomItem(CAFCRoomItem&);
	CAFCRoomItem& operator = (CAFCRoomItem &);
	//析构函数
	virtual ~CAFCRoomItem() {};
};

/*******************************************************************************************************/

//游戏内部子项
class CAFCInsideItem : public CAFCGameListItem
{
	//变量定义
public:
	GameInsideInfo								m_InsideInfo;					//内部数据

	//函数定义
public:
	//构造函数
	CAFCInsideItem()
	{
		memset(&m_InsideInfo,0,sizeof(m_InsideInfo)); 
		m_InsideInfo.Head.uSize=sizeof(m_InsideInfo);
		m_InsideInfo.Head.bDataType=GLK_GAME_USE;
	}
	//析构函数
	virtual ~CAFCInsideItem() {};
};

/*******************************************************************************************************/

//游戏列表控制类
class CServerGameListManage0
{
	//变量定义
protected:
	//数据库线程使用
	DWORD										m_dwRoomCount;					//房间数目
	DWORD										m_dwNameCount;					//名字数目
	DWORD										m_dwKindCount;					//类型数目
	DWORD										m_dwRoomBufCount;				//房间数目
	DWORD										m_dwNameBufCount;				//名字数目
	DWORD										m_dwKindBufCount;				//类型数目
	ComRoomInfo								* m_pRoomPtr;					//房间指针
	ComNameInfo								* m_pNamePtr;					//名字指针
	ComKindInfo								* m_pKindPtr;					//类型指针
	
	//数据管理线程使用
	CPtrArray									m_NameArray;					//游戏名字
	CPtrArray									m_KindArray;					//游戏类型

	//函数定义
public:
	//构造函数
	CServerGameListManage0();
	CServerGameListManage0(CServerGameListManage0&);
	CServerGameListManage0 & operator =(CServerGameListManage0&);

	//析构函数
	virtual ~CServerGameListManage0();

	//功能函数
public:
	//获取游戏类型数目
	INT_PTR GetGameKindCount() { return m_KindArray.GetCount(); }
	//获取在线人数 
	INT_PTR FillOnLineCount(char * pOnLineBuffer, INT_PTR uBufferSize, INT_PTR uBeginPos, bool & bFinish);
	//获取游戏类型
	INT_PTR FillGameKind(char * pKindBuffer, INT_PTR uBufferSize, INT_PTR uBeginPos, bool & bFinish);
	//获取游戏名字
	INT_PTR FillGameName(char * pNamdBuffer, INT_PTR uBufferSize, INT_PTR uBeginPos, bool & bFinish);
	//获取房间名字
	INT_PTR FillGameRoom(char * pRoomBuffer, INT_PTR uBufferSize, UINT uKindID, UINT uNameID, INT_PTR uBeginPos, bool & bFinish);

	//更新函数
public:
	//更新游戏列表（数据管理线程调用）
	bool UpdateGameList();
	//更新游戏列表（数据库线程调用）
	bool UpdateGameListBuffer(HANDLE hDatabase);

	//辅助函数
public:
	//更新名字列表
	bool UpdateGameName(CAFCKindItem * pKindItem);
	//更新房间列表
	bool UpdateGameRoom(CAFCNameItem * pNameItem);
	//寻找类型
	CAFCKindItem * FindKindItem(UINT uKindID);
	//寻找名字
	CAFCNameItem * FindNameItem(CAFCKindItem * pKindItem, UINT uNameID);
	//寻找房间
	CAFCRoomItem * FindRoomItem(CAFCNameItem * pNameItem, UINT uRoomID);
};

/*******************************************************************************************************/
