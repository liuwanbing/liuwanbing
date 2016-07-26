#pragma once
#ifndef USERLISTDLG_HEAD_H
#define USERLISTDLG_HEAD_H

#include "PublicDefine.h"
#include "AFCButton.h"
#include "AFCDialog.h"
#include "AFCListCtrl.h"
#include "../../../../SDK/include/NewGameFrame/Client/DirectUI/BzDui.h"
#include "../SkinMgr.h"

//名字空间声明
/*using namespace AFC;
using namespace AFC::AFCDialog;
using namespace AFC::AFCButton;
using namespace AFC::AFCListCtrl;*/

/*******************************************************************************************************/

//列表类
class EXT_CLASS CGameUserListCtrl  : public CHappyGoDlg
{
	//变量定义
private:
	static UINT					m_uItemKind[12];					//子项类型
	int							m_uComType;
	CString						m_iniFile;
	BOOL                        m_bMode;                            // 加载模式

    // PengJiLin, 2010-6-11, 用户列表的配置样式。鸥朴
    // 0为原先的样式，1为鸥朴样式，2...
    BYTE m_byUserListType;

	//函数定义
public:
	//构造函数
	CGameUserListCtrl();  
	//析构函数
	virtual ~CGameUserListCtrl();
	//初始化
	virtual void InitListCtrl(GetOrderName * pOrderFunction, UINT uComType);
	//是否允许查看 IP
	void EnableIPView(bool bLookIP);
	//设置旁观对象
	void SetWatchUser(UserItemStruct * pUserItem, TCHAR * szWatchName);

    // PengJiLin, 2010-6-11, 获取用户列表的配置样式。鸥朴
    inline BYTE GetUserListDefineType(){return m_byUserListType;}

	//消息映射
protected:
	DECLARE_MESSAGE_MAP()

	//重载函数
public:
	void SetIniFile(LPCTSTR path, BOOL bMode = FALSE);
	//增加用户
	virtual bool AddGameUser(UserItemStruct * pUserItem, bool bIsMySelf = false);
	//删除用户
	virtual bool DeleteGameUser(UserItemStruct * pUserItem);
	//更新状态
	virtual bool UpdateGameUser(UserItemStruct * pUserItem);
	//换皮肤
	virtual void LoadSkin(void);
private:
	/// 根据排序规则取得需要插入的位置
	int GetInsertPos(UserItemStruct * pUserItem);
	/// 获取玩家ID信息，是否散户，特别账号，指派账号
	/// @param UserID 玩家ID,NomalIDFrom散户ID开始，NomalIDEnd散户ID结束
	/// @return 0到8是指派账号后面的0的个数，-1为散户，-2为无效账号，-5为5连号，-6为6连号
	int GetIDInformation(long UserID,long NomalIDFrom,long NomalIDEnd);
	/// 加载列设置
	void LoadColumnConfig();
	/// 将字符串解析出 顺序号，列名，列宽
	void DecodeString(CString str,CString &strColumn,int &width);
	/// 设置每一行的值
	void PrepareDate(UserItemStruct * pUserItem);

private:
	CSkinMgr	m_skinmgr;

public:
	void InitIniFile();
};

//列表类(DirectUI版)
class EXT_CLASS CGameUserListCtrlDu
{

private:

	int							m_uComType;
	CString						m_iniFile;
	BOOL                        m_bMode;                            //加载模式
	GetOrderName				*m_pPointOrderFun;					//等级函数

public:
	UINT						m_uGameNameID;						//游戏ID

public:

	//构造函数
	CGameUserListCtrlDu(BzDui::CListUI* pListCtrl);

	//初始化
	virtual void InitListCtrl(GetOrderName* pOrderFunction, UINT uComType);

	//是否允许查看 IP
	void EnableIPView(bool bLookIP);

	//设置旁观对象
	void SetWatchUser(UserItemStruct* pUserItem, TCHAR* szWatchName);

	//获取用户状态 ID
	UINT GetStateImageIndex(UserItemStruct* pUserItem);

	//获取第n行的用户数据
	UserItemStruct* GetUser(int nRow);

//重载函数
public:

	//增加用户
	virtual bool AddGameUser(UserItemStruct* pUserItem,bool bIsMySelf = false);

	//更新状态
	virtual bool UpdateGameUser(UserItemStruct * pUserItem);

	//删除用户
	virtual bool DeleteGameUser(UserItemStruct * pUserItem);

	//删除所有
	virtual void DeleteAllItems();

//消息处理函数
public:

	//列表文本重画
	void OnGetItemText(UserItemStruct* pUserItem,int iIndex,int iSubItem,char* szout,bool bSecret = false, bool bshowname = true);
private:

	/// 加载列设置
	void LoadColumnConfig();

	/// 将字符串解析出 顺序号，列名，列宽
	void DecodeString(CString str,CString &strColumn,int &width);

	//设置每一行的值
	void PrepareDate(UserItemStruct* pUserItem,BzDui::CListTextElementUI* pElementItem);

	//查找玩家数据在列表的位置
	int FindItem(UserItemStruct* pUserItem,VtDuTable::iterator* _out_it_found = NULL);

	//查找玩家列表项的插入位置
	int GetInsertPos(UserItemStruct* pUserItem,VtDuTable::iterator* _out_it_found = NULL);

public:

	int FindItem(BzDui::CControlUI* pItemCtrl);

//控件数据
public:

	//列表控件
	BzDui::CListUI* m_pListCtrl;

//数据
protected:

	//保存列表item临时信息
	TDuItem m_TempItem;

	//列表所有item信息
	VtDuTable m_vTable;

	//列表头顺序
	std::vector<string> m_vtHeader;

	//自己的信息
	UserItemStruct* m_pMeUser;

};


/*******************************************************************************************************/

//用户列表对话框
class EXT_CLASS CPictureHelper/*C-UserListDlg*/ : public CGameFaceGo
{
	//控件变量
public:
	CGameUserListCtrl						m_UserList;						//用户列表 MFC版
	CGameUserListCtrlDu*					m_pUserListDu;					//用户列表 Dui版

	//函数定义
public:
	//构造函数 
	CPictureHelper();
	//析构函数
	virtual ~CPictureHelper();

	DECLARE_MESSAGE_MAP()

	//重载函数
protected:
	//倥件绑定函数
	virtual void DoDataExchange(CDataExchange * pDX);
	//确定函数
	virtual void OnOK() { return; }
	//取消函数
	virtual void OnCancel() { return; }

	//功能函数
public:
	//初始化
	inline void InitListCtrl(GetOrderName * pOrderFunction, UINT uComType);
	//增加用户
	inline bool AddGameUser(UserItemStruct * pUserItem, bool bIsMySelf = false);
	//删除用户
	inline bool DeleteGameUser(UserItemStruct * pUserItem);
	//更新状态
	inline bool UpdateGameUser(UserItemStruct * pUserItem);
	//是否允许查看 IP
	inline void EnableIPView(bool bLookIP);
	//删除所有
	inline void DeleteAllUser();
	//设置旁观对象
	inline void SetWatchUser(UserItemStruct * pUserItem, TCHAR * szWatchName);

	//消息函数 
public:
	//位置变化
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//鼠标右键用户列表消息
	afx_msg void OnNMRclickUserList(NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg void OnNMLclickUserList(NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg void OnNMDBclickUserList(NMHDR * pNMHDR, LRESULT * pResult);

private:
	CSkinMgr m_skinmgr;
};

#endif
/*******************************************************************************************************/
