#pragma once

#ifndef XRightFrame_HEAD_H
#define XRightFrame_HEAD_H

#include "StdAfx.h"
#include <deque>
#include "ExpressionWnd.h"

class CTipWnd;
class CGameUserListCtrlDu;
class CUseBoardCastWnd;

#define MAX_PROP_NUMS	20

enum EBUBLEPOS
{
	BUBLE_ON_PROP_POS,
	BUBLE_ON_BOARDCAST_POS
};

class XRightFrame : 
		public BzDui::CWindowWnd,
		public BzDui::INotifyUI,
		public BzDui::IMessageFilterUI,
		public BzDui::IListCallbackUI
{

	friend class XLogic;

public:

	XRightFrame(XLogic* pParent);
	virtual ~XRightFrame(void);

	LPCTSTR GetWindowClassName() const;

	UINT GetClassStyle() const;

	void OnFinalMessage(HWND hWnd);

	void Init();

	void Notify(BzDui::TNotifyUI& msg);

	void CenterWindow();

public:

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnCmdUserListMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

	bool OnNetMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize);

	virtual LPCTSTR GetItemText(BzDui::CControlUI* pControl, int iIndex, int iSubItem);

	//窗口显隐
	void ShowWindow(bool bShow = true, bool bTakeFocus = true);

	//窗口显隐,无渐变
	void HideWindow();

	//点击道具按钮
	void OnBnProp(BzDui::CButtonUI* pButton);

	//点击选择表情
	void OnBnExpression();

	//打开用户列表的菜单
	void OnPopupUserListMenu(POINT pt);

	//向聊天Box插入消息内容
	void InsertTalkMessage(char* szMsgBuf);

	//显示所有缓冲消息
	void FlushTalkMessage();

	//聊天框信息已准备好
	void OnTalkBufferReady(BzDui::CStdString strBuf,int iTextHeight);

public:

	//加载常规控件皮肤
	void LoadNormalCtrlSkin();

	//列表选中的用户消息，显示在上方板块
	void ShowUserInfo(UserItemStruct* pUser);
	void ShowUserInfo();

	//获得列表选中的用户
	UserItemStruct* GetShowUser();

	//获取玩家头像的logo文件路径
	void GetUserLogoFile(UserItemStruct* pUser,char* sz_out_file);

	//设置道具按钮图片
	void OnSetPropImages();	

	//重读talkIngame.ini
	void OnReLoadTalkINI();

	//获得聊天框输入的内容
	int GetTalkInputMessage(char* szMsg);

	//设置焦点于聊天输入框
	void SetFocusToTalkInput();

	//清空聊天输入框
	void ClearTalkInput();

	//添加表情到聊天输入框
	void AddExpressionToTalkInput(const CString& strExpression);


	//显示提示信息
	void ShowMessageTip(const BzDui::CButtonUI* pButton,const CString& strMsg,EBUBLEPOS pos = BUBLE_ON_PROP_POS);
private:

	//加载Combol的列表项
	void LoadComboItems(BzDui::CComboUI* pCtrl,LPCTSTR items[],int nums,int nSelect = 0);

	//根据文本设置选择列表项
	bool SelectComboItem(BzDui::CComboUI* pCtrl,LPCTSTR szText);

	//初始化道具栏
	void InitProp();

	//道具栏UI位置匹配
	void FixPropUI(int cx,int cy);

	//显示道具页
	void ShowPropPage(UINT uPage);

public:

	BzDui::CPaintManagerUI m_pm;

//控件定义
public:

	//玩家列表控件
	CGameUserListCtrlDu* m_pUsersList;

	//聊天框
	BzDui::CListUI* m_pTalkBoxList;

	//开聊天框缓冲文本
	BzDui::CTextUI* m_pTalkTempText;
		

private:

	//父窗口指针
	XLogic* m_pXlogic;

	//是否百家乐游戏
	bool m_bBJLGame;

	//是否能隐藏该窗口
	bool m_bCanHide;

	//玩家是否用踢人道具
	bool m_bUseKickUserProp;

	//当前顶部被显示信息的用户
	UserItemStruct* m_pShowUser;

	//道具按钮群
	BzDui::CButtonUI* m_pPropBtn[MAX_PROP_NUMS];

	//道具每页包含按钮的数量
	UINT m_uPropPageSize;

	//当前显示道具的页
	UINT m_uCurrPropPage;

	//道具页数
	UINT m_uPropPageNums;

	//窗口透明度
	int m_iTransparent;

	//表情窗口
	CGameTalkText m_ExpressWnd;	

	//喇叭窗口
	CUseBoardCastWnd* m_pBoardCastWnd;

	//缓冲的聊天消息
	std::deque<CString> m_dqTalkBuff;

	//表情框
	CExpressionWnd *m_expressUIWnd;


public:

	//窗口的最大宽度
	int m_uMaxWidth;

	//窗口的最小宽度
	int m_uMinWidth;

	//旧版可边框的宽度
	int m_uOldWidth;

	//显示状态
	UINT m_uShowState;

private:
	//标签窗口
	CTipWnd* m_pTipWnd;

	//皮肤
	CSkinMgr m_skinmgr;
};





#endif
/*******************************************************************************************************/