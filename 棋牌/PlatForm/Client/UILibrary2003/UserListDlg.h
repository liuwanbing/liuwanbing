#pragma once
#ifndef USERLISTDLG_HEAD_H
#define USERLISTDLG_HEAD_H

#include "PublicDefine.h"
#include "AFCButton.h"
#include "AFCDialog.h"
#include "AFCListCtrl.h"

//名字空间声明
/*using namespace AFC;
using namespace AFC::AFCDialog;
using namespace AFC::AFCButton;
using namespace AFC::AFCListCtrl;*/

/*******************************************************************************************************/

//列表类
class CGameUserListCtrl  : public CHappyGoDlg
{
	//变量定义
private:
	static UINT					m_uItemKind[15];					//子项类型
	int							m_uComType;
	CString						m_iniFile;
	CBitmap						m_NewUserStateBmp;
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

	//消息映射
protected:
	DECLARE_MESSAGE_MAP()

	//重载函数
public:
	void SetIniFile(LPCTSTR path);
	//增加用户
	virtual bool AddGameUser(UserItemStruct * pUserItem);
	//删除用户
	virtual bool DeleteGameUser(UserItemStruct * pUserItem);
	//更新状态
	virtual bool UpdateGameUser(UserItemStruct * pUserItem);
	//换皮肤
	virtual void LoadSkin(void);
};

/*******************************************************************************************************/

//用户列表对话框
class EXT_CLASS CPictureHelper/*C-UserListDlg*/ : public CGameFaceGo
{
	//控件变量
public:
	CGameUserListCtrl						m_UserList;						//用户列表

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
	inline void InitListCtrl(GetOrderName * pOrderFunction, UINT uComType) { return m_UserList.InitListCtrl(pOrderFunction,uComType); }
	//增加用户
	inline bool AddGameUser(UserItemStruct * pUserItem) { return m_UserList.AddGameUser(pUserItem); };
	//删除用户
	inline bool DeleteGameUser(UserItemStruct * pUserItem) { return m_UserList.DeleteGameUser(pUserItem); };
	//更新状态
	inline bool UpdateGameUser(UserItemStruct * pUserItem) { return m_UserList.UpdateGameUser(pUserItem); };
	//是否允许查看 IP
	inline void EnableIPView(bool bLookIP) { m_UserList.EnableIPView(bLookIP); };
	//删除所有
	inline void DeleteAllUser() { m_UserList.DeleteAllItems(); };
	//设置旁观对象
	inline void SetWatchUser(UserItemStruct * pUserItem, TCHAR * szWatchName) { m_UserList.SetWatchUser(pUserItem,szWatchName); }

	//消息函数 
public:
	//位置变化
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//鼠标右键用户列表消息
	afx_msg void OnNMRclickUserList(NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg void OnNMLclickUserList(NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg void OnNMDBclickUserList(NMHDR * pNMHDR, LRESULT * pResult);
};

#endif
/*******************************************************************************************************/
