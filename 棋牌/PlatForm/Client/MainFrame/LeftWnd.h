#pragma once
#include "GameListUI.h"
#include "GameListCtrl.h"

class CLeftWnd : 
	public BzDui::CWindowWnd,
	public BzDui::INotifyUI,
	public BzDui::IMessageFilterUI
{
public:
	CLeftWnd(void);
	~CLeftWnd(void);
	
	LPCTSTR GetWindowClassName() const;

	UINT GetClassStyle() const;

	void OnFinalMessage(HWND hWnd);

	void Init();
	
	void Notify(BzDui::TNotifyUI& msg);
	
public:

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	
public:

	void HandleKindData(ComKindInfo* pKindInfoPtr, int iCount);

	void HandleNameData(ComNameInfo* pNameInfoPtr, int iCount);

	void HandleRoomData(ComRoomInfo * pRoomInfoPtr, int iCount, UINT uKindID, UINT uNameID);

	//获取进程名字
	bool GetProcessName(CAFCRoomItem * pGameRoomItem, TCHAR * szProcessName, UINT uBufferSize);
	//获取游戏名字
	bool GetGameName(CAFCRoomItem * pGameRoomItem, TCHAR * szGameName, UINT uBufferSize);

	bool HandleNameOnLineData(DL_GP_RoomListPeoCountStruct * pOnLineCountPtr, UINT uCount);
	
	bool HandleRoomOnLineData(DL_GP_RoomListPeoCountStruct * pOnLineCountPtr, UINT uCount);

	ComRoomInfo* FindContestRoomItem(CAFCRoomItem * pRoomItem, int iRoomID);



	void UpdatePeopleCount();

	void UnInit();
	
	map<UINT, ComKindInfo*> m_mapKindInfo;
	
	map<UINT, Node*> m_mapNameNode;

	map<UINT, Node*> m_mapRoomNode;

	vector<UINT> m_vecKindID;
protected:

	BzDui::CPaintManagerUI m_pm;

	CGameListUI* m_pGameList;
	
	BzDui::CHorizontalLayoutUI* m_pHorOpt;

	BzDui::CTextUI* m_pTxtOnlineCount;
};

