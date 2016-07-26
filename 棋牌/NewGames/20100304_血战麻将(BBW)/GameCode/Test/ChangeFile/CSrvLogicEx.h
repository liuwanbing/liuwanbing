#pragma once

#include "../GameMessage/CSrvLogic.h"
//class CServerGameDesk;

//服务器流程逻辑类

class CSrvLogicEx:public CSrvLogic
{
public:
	CSrvLogicEx(void);
	~CSrvLogicEx(void);
	
public:	
	//断线重连事件
	virtual bool GetGameStaton(BYTE bDeskStation, bool bWatchUser,BYTE GameStation);
	// 游戏数据包处理函数
	virtual int HandleNotifyMessage(int bDeskStation, UINT code, BYTE* pData, int uSize, bool bWatchUser=false);
	// 用户出牌
	virtual int ReceiveUserOutCard(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser);
	// 用户糊牌消息
	virtual int ReceiveUserHuCard(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser);


	//游戏事件请求处理中心，确定下一个事件是什么，返回事件值,type申请的事件类型
	virtual void GameCPU(int type);
	///开始事件处理
	virtual void ExecuteGameBegin(BYTE &nextID,BYTE &time);
	///掷2颗色子确定庄家（桂林麻将一颗色子定庄）
	virtual void ExecuteDongNt(BYTE &nextID,BYTE &time);	
	///掷2颗色子确定起牌敦数（点数和敦数）
	virtual void ExecuteTwoSeziGp(BYTE &nextID,BYTE &time);	

	///发牌
	virtual void ExecuteSendPai(BYTE &nextID,BYTE &time);	
	///开始出牌通知事件
	virtual void ExecuteBeginOut(BYTE &nextID,BYTE &time);	
	///抓牌事件
	virtual void ExecuteZhuaPai(BYTE &nextID,BYTE &time);	

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


	///检测玩家station当前能做哪些动作
	virtual bool CheckAction(int station,BYTE pai,BYTE type);
	//放卡桌自动处理
	virtual void AutoHandle(bool bNetCut=false);
	///自动处理出牌或者抓牌
	virtual void AutoOutOrGet();

	/// 玩家坐桌
	virtual void UserSitDesk(BYTE bDeskStation);
	/// 设置当前正在发生的事件
	virtual void SetDoingThing(BYTE thingId,BYTE st1=255,BYTE st2=255,BYTE pai=255,BYTE data=255);
public:
	////////自定义事件/////////////////////////

	// 用户押注
	int ReceiveUserYaZhu(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser);
	// 确定底分
	int ReceiveMakeDiFen(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser);//

	///押注事件
	void ExecuteYaZhu(BYTE &nextID,BYTE &time);


};