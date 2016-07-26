#pragma once
#ifndef GAMELISTCTRL_HEAD_H
#define GAMELISTCTRL_HEAD_H

#include "StdAfx.h"
#include "GamePlace.h"
#include "SkinMgr.h"

//类说明
class CGameListCtrl;
class CAFCNameItem;
struct MSG_GP_SR_OnLineStruct;

typedef vector<MSG_GP_R_SellGameListResult> SELLLISTINFO;

class CAFCGameListItemBase
{
public:
	UINT								m_uDataType;					//数据类型
	int									m_iGameHotDegree;				//游戏热度(此数值直接在客户端配置文件中读取)
	int									m_iGameHotType;					//游戏热度类型(1-新 2-表示热)

};

enum ItemGrade
{
	TOP_GRADE = 0,
	SEC_GRADE,
	THR_GRADE,
	FOU_GRADE
};

#define IMG_GAME_NAME_COUNT 5	//游戏名背景图片轮换图片数

// class CAFCGameUrlItem: public CAFCGameListItemBase
// {
// public:
// 	HTREEITEM									m_hCurrentItem;					//当前索引
// 	CString										m_stName;
// 	CString										m_stUrl;
// 	CAFCGameUrlItem()
// 	{
// 		m_uDataType = 0;
// 		m_hCurrentItem = NULL;
// 		m_stName = "";
// 		m_stUrl = "";
// 	}
// 	CAFCGameUrlItem(UINT uDataType, HTREEITEM hCurrentItem, CString stName, CString stUrl)
// 	{
// 		m_uDataType = uDataType;
// 		m_hCurrentItem = hCurrentItem;
// 		m_stName = stName;
// 		m_stUrl = stUrl;
// 	}
// };

/*******************************************************************************************************/
//游戏列表子项
class CAFCGameListItem: public CAFCGameListItemBase
{
	//数据变量
public:
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
	bool AddGameKind(ComKindInfo * pGameKind,int initType = GLK_GAME_KIND);
	//加入游戏名字
	CAFCNameItem* AddGameName(ComNameInfo * pGameName,int initType = GLK_GAME_NAME);
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
	//删除不显示的房间
	bool DeleteHideRoom();

	//设置列表游戏热度	ADD BY DWJ 20140318
	bool SetGameDegreeAndType(UINT	uGameNameID, int *iDegree, int *iType);


	//辅助函数
protected:
	//插入数据
	HTREEITEM AddTreeData(TCHAR * szText, UINT uIconIndex, LPARAM lParam, bool bNeedIco = true);

private:
	CSkinMgr m_skinmgr;
};

/*******************************************************************************************************/
class CAFCGameUrlItem: public CAFCGameListItem
{
public:
	HTREEITEM									m_hCurrentItem;					//当前索引
	CString										m_stName;
	CString										m_stUrl;
	CAFCGameUrlItem()
	{
		m_uDataType = 0;
		m_hCurrentItem = NULL;
		m_stName = "";
		m_stUrl = "";
	}
	CAFCGameUrlItem(UINT uDataType, HTREEITEM hCurrentItem, CString stName, CString stUrl)
	{
		m_uDataType = uDataType;
		m_hCurrentItem = hCurrentItem;
		m_stName = stName;
		m_stUrl = stUrl;
	}
};
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
	CString									m_sRoomState;		//游戏状态
	vector<ComRoomInfo>						m_RoomContest;		//比赛房间

	//函数定义
public:
	//构造函数
	CAFCRoomItem() 
	{ 
		m_uDataType=GLK_GAME_ROOM;
		m_bIsContest = false;
		m_iContestID = -1;
		m_iRoomIndex = -1;
		m_iContestPeopleCount = 0;
		m_RoomContest.clear();
		m_sRoomState = "";
		memset(&m_RoomInfo,0,sizeof(m_RoomInfo)); 
		m_RoomInfo.Head.uSize=sizeof(m_RoomInfo);
		m_RoomInfo.Head.bDataType=GLK_GAME_ROOM;
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
class CMyGameImageEx
{
public:
	CMyGameImageEx(UINT Index, UINT uNID, CGameImageEx *img)
	{
		uIndex = Index;
		uNameID = uNID;
		pimg_GameEx = img;
	}
	UINT uIndex;
	UINT uNameID;
	CGameImageEx *pimg_GameEx;
};

class GameImageExEQ
{
public: GameImageExEQ(const UINT& nameid):uNameID(nameid){}
		bool operator() (const CMyGameImageEx& c) const
		{
			return c.uNameID == uNameID;
		}
private:
	UINT uNameID;
};

//游戏列表控制类

class CGameListCtrl : public CTreeCtrl////////////重新派生一个新类
{
	//变量定义
protected:
	CPtrArray									m_InsideArray;					//游戏内部数组
	CPtrArray									m_GameUrl;

	vector<TLoginGameInfo>						m_LoginGameInfo;
	
public:
	void SetItemFont(HTREEITEM,   LOGFONT&); 
	void SetItemBold(HTREEITEM,   BOOL); 
	void SetItemColor(HTREEITEM,   COLORREF);
	void SetItemStatusColor(HTREEITEM hItem,UINT roomID,const COLORREF& color,const COLORREF& statusColor,const CString& strStatusText);
	BOOL GetItemFont(HTREEITEM,   LOGFONT   *); 
	BOOL GetItemBold(HTREEITEM); 
	COLORREF GetItemColor(HTREEITEM); 

protected: 
	struct   Color_Font
	{
		COLORREF	color; 
		LOGFONT		logfont;
		COLORREF	statuscolor;
		LOGFONT		statusfont;
		Color_Font()
		{
			logfont.lfFaceName[0] = '\0';
			statusfont.lfFaceName[0] = '\0';
			color = RGB(0,0,0);
			statuscolor = RGB(0,0,0);
		}
	};
	CMap<void*,void*,Color_Font,Color_Font&>	m_mapColorFont; 
	static std::map<UINT,Color_Font> m_mapStatusColor;  //房间ID和房间状态颜色
	static std::map<void*,UINT> m_mapHitemRoomID;		//游戏房间项句柄和房间ID映射
	COLORREF m_crGridLine;
	COLORREF m_crGridHotBk1;
	COLORREF m_crGridHotBk2;
	COLORREF m_crGridHotTxt1;
	COLORREF m_crGridHotTxt2;
	COLORREF m_crGridBk;

	CGameImage m_imgLogo;			//列表根节点logo
	CGameImageEx m_imgFold;			//列表折叠logo
	CGameImageEx m_imgUnFold;		//列表反折叠logo
	CGameImageEx m_imgUnderLine;	//列表下划线
	CGameImageEx m_imgHotDegree;	//游戏热度
	CGameImageEx m_imgHotLogo[3];	//游戏热度logo
	CGameImageEx m_imgHotGame;		//热门游戏
	CGameImageEx m_imgNewGame;		//新游戏
	CGameImageEx m_imgBgColor;		//游戏类型底色框
	CGameImageEx m_imgGameKindMg;   //游戏类型提示图标(最多999种)

	CGameImage m_imgGameKind;
	CGameImage m_imgGameRoom;
	CGameImage m_ArrimgGameName[IMG_GAME_NAME_COUNT];
	vector<CMyGameImageEx> m_vecGameImage;


	HTREEITEM  m_hLastMMItem;	//最后鼠标移入的Item    

	CGameImage m_imgAD;			//房间广告比如比赛房的报名和比赛已经开始已经结束

	CNormalBitmapButtonNew	m_btRule;	 //查看规则按钮


	HCURSOR m_curHand;		//鼠标手势1
	HCURSOR m_curArrow;		//鼠标手势2

protected: 
	//{{AFX_MSG(CTreeCtrlEx) 
	afx_msg void OnPaint(); 
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
 	//afx_msg void OnTvnItemexpanding(NMHDR *pNMHDR, LRESULT *pResult);
// 	afx_msg void OnTvnItemexpanded(NMHDR *pNMHDR, LRESULT *pResult);
 	afx_msg BOOL OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnNMDlbClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg LRESULT OnLayout(WPARAM wParam,LPARAM lParam);
	//}}AFX_MSG cga

	DECLARE_MESSAGE_MAP()
	//函数定义

	void OnClickReadRule();

public:
	///void UpdateAllOnLineData();
    bool                                        m_bISInit;                      //滚动条添加变量
    int                                         m_iCount;                       //调用次数
	SELLLISTINFO								m_SellListVec;					//销售列表

//处理数据
    bool HandleNameOnLineData(DL_GP_RoomListPeoCountStruct * pOnLineCountPtr, UINT uCount);
	// 查找游戏
	CAFCNameItem *FindNameItem(UINT uID, UINT uKind = -1);
	// 查找房间
	CAFCRoomItem *FindRoomItem(UINT uRoomID);

	// 查找房间,用于比赛房间
	CAFCRoomItem *FindRoomItemEx(UINT uRoomID, UINT uKind = -1);

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
	//
	bool Init2();
	//展开数据
	bool ExpandItem(int iExpendCount);
	bool ExpandItem(CAFCGameListItem *pListItem);
	//插入根项
	bool AddRootItem(GameInsideInfo * pInsideData, UINT uIconIndex);
	//获取根项
	//////////////////////////////////
	///Kylin 20090107  添加最近游戏列表
	CAFCGameListItem * GetKindParentItem() 
	{ 
		if(Glb().m_baFavor.GetCount()) 
			return (CAFCGameListItem *)m_InsideArray.GetAt(1);
		else return (CAFCGameListItem *)m_InsideArray.GetAt(0); 
	} 
	CAFCGameListItem * GetSellListParentItem() 
	{ 
		return (CAFCGameListItem *)m_InsideArray.GetAt(1); 
	} 
	//////////////////////////////////
	//处理数据
	bool HandleIOnLineData(MSG_GP_SR_OnLineStruct * pOnLineCountPtr, UINT uCount);
	//处理数据
	bool HandleKindData(ComKindInfo * pKindInfoPtr, UINT uKindCount);

	//处理数据
	bool HandleUrlData(CAFCGameUrlItem * pUrlInfoPtr, UINT uDatatype);

	bool HandleLoginGame(int iGameID);
	bool HandleLoginGameName(ComNameInfo * pNameInfoPtr);
	void ClearLoginGameName();

	//处理数据
	bool HandleNameData(ComNameInfo * pNameInfoPtr, UINT uNameCount);
	//处理数据
	bool HandleRoomData(ComRoomInfo * pRoomInfoPtr, UINT uRoomCount, UINT uKindID, UINT uNameID);
	//填充获取数据项
	bool FillGetListItem();
	//删除过时房间 
	bool DeleteOutTimeRoom(UINT uKindID, UINT uNameID);
	//删除不显示的房间
	bool DeleteHideRoom();
	//更新总在线人数
	bool UpdateOnLineCount();
	//获取进程名字
	bool GetProcessName(CAFCRoomItem * pGameRoomItem, TCHAR * szProcessName, UINT uBufferSize);
	//获取游戏名字
	bool GetGameName(CAFCRoomItem * pGameRoomItem, TCHAR * szGameName, UINT uBufferSize);
	void UpdataItem(CAFCGameListItem* pItem);
	//获取Item的层次值
	int GetItemLevel(HTREEITEM hItem);

    // PengJiLin, 2011-7-20, 游戏树形列表，支持显示在线人数数字显示、说明显示(空闲、繁忙)
    static bool m_bUseNumShow;      // 使用数字还是说明
    static void KindOnLineShow(TCHAR* pchDest, TCHAR* pchName, UINT uNum);
    static bool RoomOnLineShow(UINT uOldPeopleCount,TCHAR* pchDest, ComRoomInfo* pRoomInfo,COLORREF& statusColor,CString& statusText);
    static void RootOnLineShow(TCHAR* pchDest, TCHAR* pchFormat, UINT uNum);
    void CheckIfUseNumShow();       // 检测并确定显示方式

	//辅助函数
private:
	//查找类型子项
	CAFCKindItem * FindKindItem(UINT uKindID);
	//查找名字子项
	CAFCNameItem * FindNameItem(CAFCKindItem * pKindItem, UINT uNameID);
	CAFCNameItem * FindNameItem(CAFCGameListItem * pKindItem, UINT uNameID);
	//查找房间子项
	CAFCRoomItem * FindRoomItem(CAFCNameItem * pNameItem, UINT uRoomID, ComRoomInfo* pRoomInfo = NULL);	
	//查找房间子项
	CAFCRoomItem * FindRoomItemByContest(CAFCNameItem * pNameItem, ComRoomInfo* pRoomInfo);
	//查找比赛房间子项
	ComRoomInfo * FindContestRoomItem(CAFCRoomItem * pRoomItem, int iRoomID);
public:
	//查找房间子项
	CAFCRoomItem * FindRoomItem(UINT uKindID, UINT uNameID, CAFCNameItem* &pNameItem);
	void UpdateAllIcon(void);
	void JoinAllItem(void);
	CAFCKindItem * FindSellKindItem(UINT uKindID);
public:
	bool ExpandAll(bool ExpName=true);

private:
	CSkinMgr m_skinmgr;

public:
	void LoadSkin();
public:
	LRESULT OnExchangeSkin(WPARAM wpara,LPARAM lpara);
};

/*******************************************************************************************************/

//功能列表控制类
class CFunListCtrl : public CTreeCtrl////////////重新派生一个新类
{
	//变量定义
protected:
	CPtrArray									m_InsideArray;					//游戏内部数组
	DECLARE_MESSAGE_MAP()
	//函数定义
public:

	//析构函数
	CFunListCtrl();
	//析构函数
	virtual ~CFunListCtrl();
	
	//功能函数
public:
	//初始化函数
	bool Init();
	CStringArray m_saUrl;

private:
	CSkinMgr m_skinmgr;
public:
	void LoadSkin();
};
#endif
/*******************************************************************************************************/
