#pragma once

//////////////////////////////////////////////////////////////////////////
///大厅主界面
///2010.8.12 rongqiufen
//////////////////////////////////////////////////////////////////////////

#include "basemessage.h"
#include "../../../SDK/include/socket/TCPClientSocket.h"
#include "comstruct.h"
#include "afxcmn.h"
#include "gameplacemessage.h"
#include <vector>
#include "CommandMessage.h"
#include "GameUserManage.h"
#include "gameroommessage.h"
#include "OperateDlg.h"
#include "GM_MessageDefine.h"
#include "ChatDlg.h"
#include "WClientSocket.h"
#include "AwardDlg.h"
#include "FindUserDlg.h"
#include "BroadcastDlg.h"
#include "BackWSocket.h"
#include "LockDeskDlg.h"

#include <cmath>
#include "afxwin.h"
using namespace std;

// CGameManageDlg 对话框



#define GLK_GAME_NONE			0
#define GLK_GAME_KIND			1									///游戏类型
#define GLK_GAME_NAME			2									///游戏名字
#define GLK_GAME_ROOM			3									///游戏房间


struct CGameRoomItem;			///
struct CGameNameItem;			///
struct CGameKindItem;			///
struct CGameRootItem;			///

class CNodeType
{
public:
	UINT m_uDataType;
};

class CGameRoomItem: public CNodeType
{
public:
	ComRoomInfo		*m_RoomInfo;
	HTREEITEM		m_hCurrentItem;
	CGameNameItem   *m_GameName;
	CGameRoomItem()
	{
		m_uDataType = GLK_GAME_ROOM; 
	}
};
class CGameNameItem: public CNodeType
{
public:
	ComNameInfo		*m_NameInfo;
	HTREEITEM		m_hCurrentItem;
	CGameKindItem   *m_GameKind;
	vector<CGameRoomItem*> m_GameRoomList;
	CGameRoomItem* GetGameRoomItem(UINT uRoomID)
	{
		for (int i = 0; i < m_GameRoomList.size(); i++)
		{
			if (uRoomID == m_GameRoomList[i]->m_RoomInfo->uRoomID)
				return m_GameRoomList[i];
		}
		return NULL;
	}
	CGameNameItem()
	{
		m_uDataType = GLK_GAME_NAME; 
	}
};
class CGameKindItem: public CNodeType 
{
public:
	ComKindInfo		*m_KindInfo;
	HTREEITEM		m_hCurrentItem;
	CGameRootItem	*m_GameRoot;
	vector<CGameNameItem*> m_GameNameList;
	CGameNameItem* GetGameNameItem(UINT uNameID)
	{
		for (int i = 0; i < m_GameNameList.size(); i++)
		{
			if (uNameID == m_GameNameList[i]->m_NameInfo->uNameID)
				return m_GameNameList[i];
		}
		return NULL;
	}
	CGameKindItem()
	{
		m_uDataType = GLK_GAME_KIND; 
	}
};
class CGameRootItem: public CNodeType
{
public:
	HTREEITEM		m_hCurrentItem;
	vector<CGameKindItem*> m_GameKindList;
	CGameKindItem* GetGameKindItem(UINT uKindID)
	{
		for (int i = 0; i < m_GameKindList.size(); i++)
		{
			if (uKindID == m_GameKindList[i]->m_KindInfo->uKindID)
				return m_GameKindList[i];
		}
		return NULL;
	}
	CGameRootItem()
	{
		m_uDataType = GLK_GAME_NONE; 
	}
};

class CGameManageDlg : public CDialog
{
	DECLARE_DYNAMIC(CGameManageDlg)

public:
	CGameManageDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGameManageDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnCancel();


public:
	bool OnGetGameKindList(vector<ComKindInfo*> list);
	bool OnGetGameNameList(vector<ComNameInfo*> list);
	bool OnGetGameRoomList(vector<ComRoomInfo*> list);
	bool UpdatePeoCount();

	bool OnGMTalkMsg(GM_Broadcast_t *pMsg, bool issystem = false);
	void AddMsg(CString msg);
	void OnGetUser(GM_OperatedPlayerInfo_t* user);

	void SendOperaToW(GM_OperateByID_t* opera, int iRoomID);

	bool ShowGameDesk();
	bool ShowGameUeser();
	bool ShowGameDeskStation();
public:
	CWClientSocket m_wSocket;
	CBackWSocket m_BwSocket;

	GM_OperateByID_t*	m_BackOpera;
	int					m_BackRoomID;
	

	CTreeCtrl m_GameListTreeCtrl;

	///游戏列表根结点
	CGameRootItem m_GameRootItem;

	//用户登陆信息
	GM_MSG_O_LogonResult_t*		m_pLogonInfo;
	///奖励方案表
	(map<int, GM_AwardType_t*>)* m_AwardType;
	///道具表
	(map<int, GM_GetPropInfo_t*>)* m_PropList;

	CGameRoomItem* m_pCurrentRoom; 


	COperateDlg		m_OperateDlg;
	vector<CChatDlg*>		m_ChatDlgs;
	CAwardDlg		m_AWardDlg;
	CFindUserDlg	m_FindUser;
	CBroadcastDlg	m_BroadDlg;
	CLockDeskDlg    m_LockDeskDlg;

private:
	int m_ListIndex1;
	int m_ListIndex2;
	int m_OperaState;
	HBRUSH m_hBrush;

	CString StateToString(int state);
	
public:
	afx_msg void OnTvnItemexpandingTree1(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult);
public:
	CListCtrl m_GameDiskListCtrl;
public:
	CListCtrl m_GameUeserListCtrl;
public:
	afx_msg void OnNMRclickList4(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedButton1();
public:
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedOk();
public:
	afx_msg void OnNMRclickList1(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnBnClickedButton2();
public:
	afx_msg void OnBnClickedButton3();
public:
	afx_msg void OnBnClickedButton4();
public:
	afx_msg void OnBnClickedButton5();
public:
	afx_msg void OnBnClickedCancel();
public:
	afx_msg void OnBnClickedButton6();
public:
	afx_msg void OnBnClickedButton7();
public:
	afx_msg void OnBnClickedPrintLog();
public:
	CString m_TalkMsg;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
public:
	afx_msg void OnNMRclickTree1(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
public:
	CEdit m_MsgEdit;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
//	afx_msg void OnNMRdblclkTree1(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
};
