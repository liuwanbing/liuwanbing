
//
//事件：游戏代码6602转6603，使原来6602的游戏可以在6603上使用
//作者：王少江
//时间：2012.02  情人节快到了
//Q Q ：349126394
//QQ群：101494119
//邮箱：shaojiang216@163.com
//博客：http://shaojiang216.blog.163.com/ 
//
#ifndef GAME_FRAME_ENGINE_HEAD_FILE
#define GAME_FRAME_ENGINE_HEAD_FILE

#pragma once

#include "GameFrameHead.h"

//////////////////////////////////////////////////////////////////////////////////

//驱动引擎
class GAME_FRAME_CLASS CGameFrameEngine : public CWnd, public IGameFrameEngine
{
	//友元定义
	friend class CGameFrameService;

	//组件接口
protected:
	IClientKernel *					m_pIClientKernel;					//内核接口
	IStringMessage *				m_pIStringMessage;					//信息接口

	//静态变量
protected:
	static CGameFrameEngine *		m_pGameFrameEngine;					//引擎接口

	//函数定义
public:
	//构造函数
	CGameFrameEngine();
	//析构函数
	virtual ~CGameFrameEngine();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//系统事件
public:
	//创建函数
	virtual bool OnInitGameEngine()=NULL;
	//重置函数
	virtual bool OnResetGameEngine()=NULL;

	//时钟事件
public:
	//时钟删除
	virtual bool OnEventGameClockKill(WORD wChairID)=NULL;
	//时钟信息
	virtual bool OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD wClockID)=NULL;

	//游戏事件
public:
	//旁观消息
	virtual bool OnEventLookonMode(VOID * pData, WORD wDataSize)=NULL;
	//游戏消息
	virtual bool OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
	//场景消息
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonUser, VOID * pData, WORD wDataSize)=NULL;
	//银行消息
	virtual bool OnEventInsureMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//用户事件
public:
	//用户进入
	virtual VOID OnEventUserEnter(IClientUserItem * pIClientUserItem, bool bLookonUser){}
	//用户离开
	virtual VOID OnEventUserLeave(IClientUserItem * pIClientUserItem, bool bLookonUser){}
	//用户积分
	virtual VOID OnEventUserScore(IClientUserItem * pIClientUserItem, bool bLookonUser){}
	//用户状态
	virtual VOID OnEventUserStatus(IClientUserItem * pIClientUserItem, bool bLookonUser){}

	//模式函数
public:
	//单机模式
	bool IsSingleMode();
	//旁观用户
	bool IsLookonMode();
	//允许旁观
	bool IsAllowLookon();

	//状态函数
public:
	//获取状态
	BYTE GetGameStatus();
	//设置状态
	VOID SetGameStatus(BYTE cbGameStatus);

	//功能函数
public:
	//获取自己
	WORD GetMeChairID();
	//获取自己
	IClientUserItem * GetMeUserItem();
	//游戏用户
	IClientUserItem * GetTableUserItem(WORD wChariID);
	//游戏用户
	IClientUserItem * SearchTableUserItem(DWORD dwUserID);

	//银行接口
public:
	//存入金币
	bool PerformSaveScore(SCORE lScore);
	//提取金币
	bool PerformTakeScore(SCORE lScore, TCHAR *pszInsurePass);

	//发送函数
public:
	//发送函数
	bool SendSocketData(WORD wSubCmdID);
	//发送函数
	bool SendSocketData(WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//功能接口
public:
	//发送准备
	bool SendUserReady(VOID * pData, WORD wDataSize);
	//发送表情
	bool SendUserExpression(DWORD dwTargetUserID, WORD wItemIndex);
	//发送聊天
	bool SendUserChatMessage(DWORD dwTargetUserID, LPCTSTR pszChatMessage, COLORREF crFontColor);

	//时间函数
public:
	//时钟标识
	UINT GetClockID();
	//时钟位置
	WORD GetClockChairID();
	//删除时钟
	VOID KillGameClock(WORD wClockID);
	//设置时钟
	VOID SetGameClock(WORD wChairID, WORD wClockID, UINT nElapse);

	//辅助函数
public:
	//激活框架
	VOID ActiveGameFrame();
	//切换椅子
	WORD SwitchViewChairID(WORD wChairID);

	//声音函数
public:
	//播放声音
	bool PlayGameSound(LPCTSTR pszSoundName);
	//播放声音
	bool PlayGameSound(HINSTANCE hInstance, LPCTSTR pszSoundName);
	//播放声音
	bool PlayBackGroundSound(LPCTSTR pszSoundName);
	//播放声音
	bool PlayBackGroundSound(HINSTANCE hInstance, LPCTSTR pszSoundName);
	//停止声音
	bool StopSound();

	//内部函数
private:
	//重置引擎
	bool ResetGameFrameEngine();
	//创建引擎
	bool CreateGameFrameEngine();

	//功能函数
public:
	//显示消息
	INT ShowMessageBox(LPCTSTR pszString, UINT uType=MB_OK, UINT nElapse=0);
	//显示消息
	INT ShowMessageBox(LPCTSTR pszCaption, LPCTSTR pszString, UINT uType=MB_OK, UINT nElapse=0);

	//静态函数
public:
	//获取实例
	static CGameFrameEngine * GetInstance() { return m_pGameFrameEngine; }

	DECLARE_MESSAGE_MAP()

	//设置事件
public:
	//设置事件
	virtual bool OnGameOptionChange();
	//声音控制
	virtual bool AllowBackGroundSound(bool bAllowSound){return true;}
};

//////////////////////////////////////////////////////////////////////////////////

#endif