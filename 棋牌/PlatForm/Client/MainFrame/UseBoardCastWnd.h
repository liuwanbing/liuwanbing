
#ifndef	__LOUDSPEAKERMSGWND__H__
#define	__LOUDSPEAKERMSGWND__H__

#include "StdAfx.h"
#include "IPC/ChannelService.h"

class CUseBoardCastWnd : 
	public BzDui::CWindowWnd,
	public BzDui::INotifyUI,
	public BzDui::IMessageFilterUI
{
public:
	CUseBoardCastWnd();
	virtual ~CUseBoardCastWnd();

public:

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
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnSendBuyProp(WPARAM wParam, LPARAM lParam);

	LRESULT OnExpress(WPARAM wParam, LPARAM lParam);


	void LoadNormalCtrlSkin();
public:
	//确定
	void OnOK();

	//设置道具ID
	void SetPropID(int iPropID);

	int GetPropPrice(int iPropID);

	_TAG_USERPROP* GetProp(int iPropID);

	//设置父窗体指针
	void SetParentWnd(CWnd * pParWnd);

	void OnClickEXpress();


	_TAG_BOARDCAST	m_BoardCast;
	void UseProp_Buy(int nPropIndex);

private:
	//绘图
	BzDui::CPaintManagerUI	m_pm;
	//父窗口指针
	CWnd					*m_pParentWnd;

	//喇叭类型
	int						m_iBroadcastStyle;


	//表情
	BzDui::CButtonUI		*m_btExpress;
	//喇叭编辑框消息
	BzDui::CRichEditUI*		m_pREdtCtrl;
	//用户待发送的喇叭消息
	BzDui::CStdString		m_userInfo;
	//用户消息长度
	UINT					m_uiUserInfoLen;
	//道具ID号
	int						m_nPropID;
	//颜色
	COLORREF				m_RGB;
	//信道
	CChannelService						*m_ChannelService;

	//道具信息提示
	BzDui::CTextUI*		m_TextTip;

	BzDui::CLabelUI*	m_LabCaption;


	CGameTalkText		m_ExpressWnd;						//表情窗口
};



#endif