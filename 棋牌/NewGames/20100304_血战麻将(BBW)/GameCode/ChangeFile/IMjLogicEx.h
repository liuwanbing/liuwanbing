#pragma once
//#include "StdAfx.h"
//#include "../Client/StdAfx.h"
#include "../Client/IMjLogic.h"


/// 重载麻将逻辑类:麻将的特殊逻辑在该类中实现，如果没有特殊性直接调用父类即可
class IMjLogicEx:public  IMjLogic
{
public:

	IMjLogicEx();
	virtual ~IMjLogicEx();


public:
	//******************被框架调到的函数********************************	
    ///托管
	virtual bool OnTuoGuan();
	///断线重连接口
	virtual bool SetWaitSetGame(void * pStationData, UINT uDataSize);
	virtual bool SetWaitAgree(void * pStationData, UINT uDataSize);
	virtual void SetSendCardStation(void * pStationData);
	virtual void SetPlayingStation(void * pStationData,int iLessCount);

	//virtual bool GameUserCome(BYTE bDeskStation, UserItemStruct * pUserItem, bool bWatchUser);
	//virtual bool GameUserLeft(BYTE bDeskStation, UserItemStruct * pUserItem, bool bWatchUser);

	/////////////////////////事件处理//////////////////////////////////////////////
	///游戏开始
	virtual void GameBegin(void * pNetData, UINT uDataSize);
	///以东为庄
	virtual void MakeDongNt(void * pNetData, UINT uDataSize);
	///首局掷2颗色子的点数和为庄事件
	virtual void SeziMakeNt(void * pNetData, UINT uDataSize);
	///首局掷2颗色子定庄家和起牌点事件
	virtual void SeziMakeNtAndGetPai(void * pNetData, UINT uDataSize);
	///首局轮流掷2颗色子定庄家事件
	virtual void TurnSeziMakeNt(void * pNetData, UINT uDataSize);
	///掷2颗色子事件定起牌方向事件
	virtual void SeziMakeDir(void * pNetData, UINT uDataSize);
	///掷2颗色子事件定起牌方向和起牌点事件
	virtual void SeziMakeDirAndGetPai(void * pNetData, UINT uDataSize);
	///掷2颗色子事件定起牌点事件
	virtual void SeziMakeGetPai(void * pNetData, UINT uDataSize);
	///发牌事件
	virtual void SendCard(void * pNetData, UINT uDataSize);
	///跳牌事件，庄家跳牌
	virtual void TiaoPai(void * pNetData,UINT uDataSize);
	///掷2颗色子定精牌事件
	virtual void SeziMakeJing(void * pNetData, UINT uDataSize);
	//为所有玩家补花事件
	virtual void AllBuHua(void * pNetData, UINT uDataSize);
	///单个玩家补花事件
	virtual void OneBuHua(void * pNetData, UINT uDataSize);
	///庄家开始发牌通知事件，确定精牌后服务器发送消息通知玩家出牌
	virtual void BeginOutCard(void * pNetData, UINT uDataSize);
	///出牌事件，玩家出牌
	virtual void UserOutCard(void * pNetData, UINT uDataSize);
	///抓牌事件
	virtual void UserZhuaAPai(void * pNetData, UINT uDataSize);
	///吃碰杠通知事件
	virtual void CPGNotify(void * pNetData, UINT uDataSize);
	///吃牌事件
	virtual void UserChi(void * pNetData, UINT uDataSize);
	///碰牌事件
	virtual void UserPeng(void * pNetData, UINT uDataSize);
	///坎牌事件
	virtual void UserKan(void * pNetData, UINT uDataSize);
	///扫虎事件
	virtual void UserSaoHu(void * pNetData, UINT uDataSize);
	///杠牌事件
	virtual void UserGang(void * pNetData, UINT uDataSize);
	///听牌事件
	virtual void UserTing(void * pNetData, UINT uDataSize);
	///糊事件
	virtual void UserHu(void * pNetData, UINT uDataSize);
	///算分事件
	virtual void CountFen(void * pNetData, UINT uDataSize);
	///结束处理事件，游戏正常结束
	virtual void H081_UserContinueEnd(void * pNetData, UINT uDataSize);
	///非正常结束处理事件，有人强退游戏结束
	virtual void H080_UserLeftOrGameAnQuanFinish(void * pNetData, UINT uDataSize);
	///玩家托管
	virtual void UserTuoGuan(void * pNetData, UINT uDataSize);
	//设置麻将规则
	virtual void SetMjRule(void * pNetData, UINT uDataSize);

	virtual void ResetGameStation(int iGameStation);

	///自动出牌,机器人的代码也在此函数
	virtual void AutoOutCard(BYTE pai=255,bool AI=false);		

    ///处理以上没有提到的消息
	//virtual bool HandleGameMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize); 

	//*********************被界面调到的函数*************************
	///响应点击右键
	virtual void OnRButtonUp();
	///响应鼠标消息
	virtual void OnMouseMove(int iCard,int iCardIndex,bool isclick=false);
	///点麻将的某个操作按钮
	virtual void OnMjBtnClick( int btntype);	

public:
	//**********************自用函数****************************
	///给逻辑重载
	///吃
	virtual void OnMjBtnClickChi(BYTE index = 0); 
	///碰
	virtual void OnMjBtnClickPeng( );
	///坎
	virtual void OnMjBtnClickKan(BYTE pai = 255);
	///扫虎
	virtual void OnMjBtnClickSaoHu();
	///杠
	virtual void OnMjBtnClickGang(BYTE index = 0);
	///听
	virtual void OnMjBtnClickTing(bool bIsYanPai = false);	
	///胡
	virtual void OnMjBtnClickHu();   
	///过
	virtual void OnMjBtnClickguo(void); 
	///出牌
	virtual void IChuPai( BYTE pai = 255);	

public:
	///得到本次能吃牌,杠牌，坎牌的个数
	virtual int GetChiGangKanNum(BYTE type);
	///清理临时数据（吃碰杠的临时信息）
	virtual void ClearTempData();
	///描述：设置吃碰杠数据（吃碰杠的临时信息）
	virtual void GetCPGData(tagCPGNotifyEx * pData);

	public:


};