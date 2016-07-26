#ifndef GAME_FRAME_VIEW_HEAD_FILE
#define GAME_FRAME_VIEW_HEAD_FILE

#pragma once

#include "GameFrameHead.h"
#include "DlgInsureGame.h"
#include "DlgMatchAwards.h"

//////////////////////////////////////////////////////////////////////////////////

//游戏视图
class GAME_FRAME_CLASS CGameFrameView : public CWnd, public IGameFrameView
{
	//位置变量
protected:
	CPoint							m_ptClock[MAX_CHAIR];				//时间位置
	CPoint							m_ptReady[MAX_CHAIR];				//准备位置
	CPoint							m_ptAvatar[MAX_CHAIR];				//头像位置
	CPoint							m_ptNickName[MAX_CHAIR];			//昵称位置

	//用户变量
protected:
	WORD							m_wUserClock[MAX_CHAIR];			//用户时钟
	IClientUserItem *				m_pIClientUserItem[MAX_CHAIR];		//用户接口

	//组件接口
protected:
	IStringMessage *				m_pIStringMessage;					//信息接口
	IGameLevelParser *				m_pIGameLevelParser;				//等级解释

	//滚动文字
public:
	CString							m_strRollText;						//滚动文字
	vector<CString>					m_vecText;							//滚动文字
	WORD							m_wRollTextCount;					//滚动次数
	CRect							m_rcText;							//刷新区域
	WORD							m_wTextLeft;						//剩余文字
	CMD_GR_Match_Info				*m_pMatchInfo;						//比赛信息
	CMD_GR_Match_Wait_Tip			*m_pMatchWaitTip;					//等待提示
	CMD_GR_MatchResult				*m_pMatchResult;					//比赛结果

	//组件变量
public:
	CDlgInsureGame                  m_DlgInsureGame;                    //银行接口
	CDlgMatchAwards					*m_pDlgMatchAwards;					//奖状窗口

	//静态变量
protected:
	static CGameFrameView *			m_pGameFrameView;					//游戏视图

protected:
	CPngImageEx						m_ImageUserFace;					//用户头像

	//函数定义
public:
	//构造函数
	CGameFrameView();
	//析构函数
	virtual ~CGameFrameView();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//窗口接口
public:
	//窗口句柄
	virtual HWND GetGameViewHwnd();
	//创建窗口
	virtual bool CreateGameViewWnd(CWnd * pParentWnd, UINT nID);

	//控制接口
public:
	//切换状态
	virtual VOID SwitchToReadyStatus();

	//时钟管理
public:
	//获取时钟
	virtual WORD GetUserClock(WORD wChairID);
	//设置时钟
	virtual VOID SetUserClock(WORD wChairID, WORD wUserClock);

	//用户管理
public:
	//获取用户
	virtual IClientUserItem * GetClientUserItem(WORD wChairID);
	//设置用户
	virtual VOID SetClientUserItem(WORD wChairID, IClientUserItem * pIClientUserItem);

	//重载函数
public:
	//重置界面
	virtual VOID ResetGameView()=NULL;
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight)=NULL;
	//界面更新
	virtual VOID InvalidGameView(INT nXPos, INT nYPos, INT nWidth, INT nHeight)=NULL;
	//桌面消息
	virtual void AddGameTableMessage(LPCTSTR pszMessage,WORD wLen,WORD wType){};
	//比赛信息
	void SetMatchInfo(CMD_GR_Match_Info *pMatchInfo);
	//等待提示
	void SetMatchWaitTip(CMD_GR_Match_Wait_Tip *pMatchWaitTip);
	//比赛结果
	void SetMatchResult(CMD_GR_MatchResult *pMatchResult);

	//声音函数
public:
	//播放声音
	bool PlayGameSound(LPCTSTR pszSoundName);
	//播放声音
	bool PlayGameSound(HINSTANCE hInstance, LPCTSTR pszSoundName);
	
	//信息函数
public:
	//获取等级
	LPCTSTR GetLevelDescribe(IClientUserItem * pIClientUserItem);	

	//发送消息
public:
	//发送消息
	LRESULT SendEngineMessage(UINT uMessage, WPARAM wParam, LPARAM lParam);
	//投递消息
	LRESULT PostEngineMessage(UINT uMessage, WPARAM wParam, LPARAM lParam);

	//消息映射
protected:
	//位置消息
	VOID OnSize(UINT nType, INT cx, INT cy);
	//创建消息
	INT OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	VOID OnRButtonUp(UINT nFlags, CPoint Point);
	//鼠标消息
	VOID OnLButtonDown(UINT nFlags, CPoint Point);
	
	//功能函数
public:
	//获取实例
	static CGameFrameView * GetInstance() { return m_pGameFrameView; }

	DECLARE_MESSAGE_MAP()

	//银行函数
public:
	//银行消息
	virtual bool OnEventInsureMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//设置属性
	void SetInsureOption(bool blCanGetOnlyFree, bool blCanStore);
	//银行存
	void ShowInsureSave(IClientKernel *	pIClientKernel,CPoint pos);
	//银行取
	void ShowInsureGet(IClientKernel * pIClientKernel,CPoint pos);
};

//////////////////////////////////////////////////////////////////////////////////

#endif