#pragma once
#include "../Server/ServerManage.h"


/// 重载游戏服务器类
class CServerGameDeskEx : public CServerGameDesk, virtual public IGameDesk
{
public:
    CServerGameDeskEx();
    ~CServerGameDeskEx();

    //重载函数
public:
    //-------------------------------------------------------------------------
    // 从IGameDesk重载
    //-------------------------------------------------------------------------
    /// 处理游戏内部消息
    virtual int HandleNotifyMessage(int nDeskStation, UINT code, BYTE* pBuffer, int nLen);
    /// 获取游戏状态，并发送到客户端
    virtual int OnGetGameStation(int nDeskStation, UserInfoForGame_t& userInfo);
    /// 玩家重入
    virtual int UserReCome(int nDeskStation, UserInfoForGame_t& userInfo);
    /// 定时器时间到
    virtual int OnGameTimer(int nTimerID);
    /// 玩家断线
    virtual int UserNetCut(int nDeskStation, UserInfoForGame_t& userInfo);
    /// 玩家入坐到某位置
    virtual int UserSitDesk(int nDeskStation, UserInfoForGame_t& userInfo);
    /// 玩家离桌
    virtual int UserLeftDesk(int nDeskStation, UserInfoForGame_t& userInfo);
    /// 玩家同意游戏
    virtual int UserAgreeGame(int nDeskStation, UserInfoForGame_t& userInfo);
    /// 游戏是否在进行中
    virtual bool IsPlayingByGameStation(void);
    /// 某桌号玩家是否在游戏中
    virtual bool IsPlayGame(int bDeskStation);
    /// 初始化桌子
    virtual int InitialDesk(int nDeskNo, int nMaxPeople, IGameMainManager* pIMainManager);
    /// 获取当前游戏状态
    virtual int GetCurGameStation(){return m_bGameStation;}
    ///单人游戏用户游戏中强退
    virtual int ForceQuit(){return 0;}
    //-------------------------------------------------------------------------    
	// 游戏结束
	virtual bool GameFinish(BYTE bDeskStation, BYTE bCloseFlag);


	///掷色子看宝事件
	void HandleSeziLookBao();

public:



};
