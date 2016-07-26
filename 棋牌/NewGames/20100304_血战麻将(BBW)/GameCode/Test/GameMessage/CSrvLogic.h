
#pragma once

#include "..\ChangeFile\CGameDataEx.h"
#include "..\ChangeFile\CUserDataEx.h"

#include "..\ChangeFile\CCheckCPGActionEx.h"
#include "..\ChangeFile\CCheckHuPaiEx.h"
#include "..\ChangeFile\CCountFenEx.h"

class CTestDlg;

#define TIME_CHECK_GAME_MSG         30              // 检测游戏消息数据包，超时没有收到数据包就认为是卡住了，系统自动处理
#define TIME_WAIT_MSG				31              // 超过事件无响应，是超时自动处理，系统自动处理
#define TIME_DEFAULT_HANDLE         60000			// 默认时间60秒

//服务器流程逻辑类

class CSrvLogic
{
public:
	CSrvLogic(void);
	~CSrvLogic(void);
	
public:	
	CTestDlg *pDesk;//父类（桌子）


public:
	//拷贝父类指针
	void SetDeskPoint(CTestDlg *desk);

///////接收数据处理//////////////////////////////////////////////////////

	//断线重连事件
	virtual bool GetGameStaton(BYTE bDeskStation, bool bWatchUser,BYTE GameStation);
	// 游戏数据包处理函数
	virtual int HandleNotifyMessage(int bDeskStation, UINT code, BYTE* pData, int uSize, bool bWatchUser=false);
	// 收到用户色子动画结束消息
	virtual int ReceiveSeziFinish(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser);
	// 收到托管消息
	virtual int ReceiveUserTuoGuan(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser);
	// 用户出牌
	virtual int ReceiveUserOutCard(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser);
	// 收到用户pass 消息
	virtual int ReceiveUserPassCheck(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser);
	//收到用户同意游戏消息
	virtual int ReceiveUserAgreeGame(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser);
	//有事请求离开
	virtual int ReceiveUserHaveThing(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser);
	//同意用户离开
	virtual int ReceiveArgeeUserLeft(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser);
	// 用户吃牌消息
	virtual int ReceiveUserChiCard(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser);
	// 用户碰牌消息
	virtual int ReceiveUserPengCard(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser);
	// 用户杠牌消息
	virtual int ReceiveUserGangCard(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser);
	// 用户听牌消息
	virtual int ReceiveUserTingCard(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser);
	// 用户糊牌消息
	virtual int ReceiveUserHuCard(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser);
	///收到超级客户端设置牌消息
	virtual void ReceiveSuperChangePai(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser);

	///描述：检测下一步动作
	virtual void CheckNextAction();
	///检查是否可以进行下一步处理
	virtual bool IsCanHandle(BYTE &mx);

////申请事件处理///////////////////////////////////////////////////////////////////////////////

	//请求事件,type要申请的事件
	virtual void ApplyAction(int type,int time);
	//游戏事件请求处理中心，确定下一个事件是什么，返回事件值,type申请的事件类型
	virtual void GameCPU(int type);
	///开始事件处理
	virtual void ExecuteGameBegin(BYTE &nextID,BYTE &time);
	///以第一个玩家为庄家
	virtual void ExecuteDongNt(BYTE &nextID,BYTE &time);
	///掷2颗色子确定庄家（点数和）
	virtual void ExecuteTwoSeziNt(BYTE &nextID,BYTE &time);	
	///掷2颗色子确定庄家和起牌顿数（点数和庄家，大点顿数）
	virtual void ExecuteTwoSeziNtAndGp(BYTE &nextID,BYTE &time);		
	///掷2颗色子确定起牌方向（点数和确定方向）
	virtual void ExecuteTwoSeziDir(BYTE &nextID,BYTE &time);	
	///掷2颗色子确定起牌方向和起牌敦数（点数和方向，小点敦数）
	virtual void ExecuteTwoSeziDirGp(BYTE &nextID,BYTE &time);	
	///掷2颗色子确定起牌敦数（点数和敦数）
	virtual void ExecuteTwoSeziGp(BYTE &nextID,BYTE &time);	
	///掷2颗色子确定庄家+起牌方向+起牌敦数（点数和庄家，大点方向，小点敦数）
	virtual void ExecuteTwoSeziNtDirGp(BYTE &nextID,BYTE &time);	
	///发牌
	virtual void ExecuteSendPai(BYTE &nextID,BYTE &time);	
	///掷2颗色子定精事件
	virtual void ExecuteTwoSeziMakeJing(BYTE &nextID,BYTE &time);	
	///所有玩家补花事件
	virtual void ExecuteAllBuHua(BYTE &nextID,BYTE &time);	
	///单个玩家补花事件
	virtual void ExecuteOneBuHua(BYTE &nextID,BYTE &time);	
	///开始出牌通知事件
	virtual void ExecuteBeginOut(BYTE &nextID,BYTE &time);	
	///抓牌事件
	virtual void ExecuteZhuaPai(BYTE &nextID,BYTE &time);	
	//吃牌事件
	virtual void ExecuteChiPai(BYTE &nextID,BYTE &time);
	//碰牌事件
	virtual void ExecutePengPai(BYTE &nextID,BYTE &time);
	//杠牌事件
	virtual void ExecuteGangPai(BYTE &nextID,BYTE &time);
	//糊牌事件
	virtual void ExecuteHuPai(BYTE &nextID,BYTE &time);
	///算分事件
	virtual void ExecuteCountFen(BYTE &nextID,BYTE &time);
	///正常游戏结束事件
	virtual void ExecuteNormalEnd(BYTE &nextID,BYTE &time);
	///异常游戏结束事件事件
	virtual void ExecuteUnNormalEnd(BYTE &nextID,BYTE &time);

		//发牌函数
	virtual void DisPatchCard();
	///描述：转换门牌数据,将总门牌数组中的数据转换到每个玩家的门牌数组中
	virtual void ChangeMenPai();
	///描述：抓一张牌
	virtual BYTE GetPai(BYTE &index,int station,bool head,BYTE type=0);
	///检测玩家station当前能做哪些动作
	virtual bool CheckAction(int station,BYTE pai,BYTE type);
	//放卡桌自动处理
	virtual void AutoHandle(bool bNetCut=false);
	///自动处理出牌或者抓牌
	virtual void AutoOutOrGet();

////////////////////其他处理///////////////////////////////////////////////

    /// 玩家断线
    virtual void UserNetCut(BYTE bDeskStation);
    /// 玩家离桌
    virtual void UserLeftDesk(BYTE bDeskStation);
	/// 玩家坐桌
	virtual void UserSitDesk(BYTE bDeskStation);
    /// 玩家重入
    virtual void UserReCome(BYTE bDeskStation);
	/// 处理计时器消息
	virtual void OnGameTimer(UINT uTimerID);
	/// 游戏结束要删除所有计时器
	virtual void KillAllTimer();

	/// 设置当前正在发生的事件
	virtual void SetDoingThing(BYTE thingId,BYTE st1=255,BYTE st2=255,BYTE pai=255,BYTE data=255);
	/// 超时处理
	virtual void OutTimeHandle();
	/// 设置游戏状态
	virtual void SetGameState(BYTE state);

	//事件记录：只记录最近10个事件
	void RecordThing(BYTE thingId,BYTE st1=255,BYTE st2=255,BYTE pai=255,BYTE data=255);


	//给单个玩家发送游戏消息
	void SendGameStation(BYTE bDeskStation, bool bWatchUser, void * pData, UINT uSize);
	//给单个玩家发送游戏消息
	void SendGameData(BYTE bDeskStation, BYTE bMainID, BYTE bAssID, BYTE bHandleCode);
	//给单个玩家发送游戏消息
	void SendGameData(BYTE bDeskStation, void * pData, UINT uSize, BYTE bMainID, BYTE bAssID, BYTE bHandleCode);
	//给所有玩家发送游戏消息
	void SendAllGameData( BYTE bMainID, BYTE bAssID, BYTE bHandleCode);
	//给所有玩家发送游戏消息
	void SendAllGameData( void * pData, UINT uSize, BYTE bMainID, BYTE bAssID, BYTE bHandleCode);


public:
	//定义数据
	CGameDataEx m_GameData;//游戏数据
	CUserDataEx m_UserData;//玩家数据

	CCheckCPGActionEx	m_CPGAction;//吃碰杠检测方法
	CCheckHuPaiEx		m_CheckHuPai;//糊牌检测方法
	CCountFenEx			m_CountFen;//算分

};
