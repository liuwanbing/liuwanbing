#include "StdAfx.h"
#include "IMjlogicEx.h"
//#include "../Client/StdAfx.h"
//#include <iostream>
//using namespace std;
#include "../Client/ClientGameDlg.h"
///描述：构造函数
IMjLogicEx::IMjLogicEx()
{	
	///当前骰子的点数
	m_nSeZi0 = 0;
	m_nSeZi1 = 0;
}

///描述：析构函数
IMjLogicEx::~IMjLogicEx()
{

}

	//******************被框架调到的函数********************************	
///托管
bool IMjLogicEx::OnTuoGuan()
{
	return IMjLogic::OnTuoGuan();
}
///断线重连接口
bool IMjLogicEx::SetWaitSetGame(void * pStationData, UINT uDataSize)
{
	return IMjLogic::SetWaitSetGame(pStationData,uDataSize);
}
bool IMjLogicEx::SetWaitAgree(void * pStationData, UINT uDataSize)
{
	return IMjLogic::SetWaitAgree(pStationData,uDataSize);
}
void IMjLogicEx::SetSendCardStation(void * pStationData)
{
	IMjLogic::SetSendCardStation(pStationData);
}
void IMjLogicEx::SetPlayingStation(void * pStationData,int iLessCount)
{
	IMjLogic::SetPlayingStation(pStationData, iLessCount);
	GameStationDataEx * pGameStation=(GameStationDataEx *)pStationData;

	for(int i=0;i<4;i++)
	{
		g_UserData.m_byZuoLaZhuag[i] = pGameStation->m_byZuoLaZhuag[i];
	}

}

//bool IMjLogicEx::GameUserCome(BYTE bDeskStation, UserItemStructInExe * pUserItem, bool bWatchUser)
//{
//	return IMjLogic::GameUserCome(bDeskStation, pUserItem, bWatchUser);
//}
//bool IMjLogicEx::GameUserLeft(BYTE bDeskStation, UserItemStructInExe * pUserItem, bool bWatchUser)
//{
//	return IMjLogic::GameUserLeft(bDeskStation, pUserItem, bWatchUser);
//}
/////////////////////////事件处理//////////////////////////////////////////////
///游戏开始
void IMjLogicEx::GameBegin(void * pNetData, UINT uDataSize)
{
	IMjLogic::GameBegin(pNetData,uDataSize);
}
///以东为庄
void IMjLogicEx::MakeDongNt(void * pNetData, UINT uDataSize)
{
	IMjLogic::MakeDongNt(pNetData,uDataSize);
}
///首局掷2颗色子的点数和为庄事件
void IMjLogicEx::SeziMakeNt(void * pNetData, UINT uDataSize)
{
	IMjLogic::SeziMakeNt(pNetData,uDataSize);
}
///首局掷2颗色子定庄家和起牌点事件
void IMjLogicEx::SeziMakeNtAndGetPai(void * pNetData, UINT uDataSize)
{
	IMjLogic::SeziMakeNtAndGetPai(pNetData,uDataSize);
}
///首局轮流掷2颗色子定庄家事件
void IMjLogicEx::TurnSeziMakeNt(void * pNetData, UINT uDataSize)
{
	IMjLogic::TurnSeziMakeNt(pNetData,uDataSize);
}
///掷2颗色子事件定起牌方向事件
void IMjLogicEx::SeziMakeDir(void * pNetData, UINT uDataSize)
{
	IMjLogic::SeziMakeDir(pNetData,uDataSize);
}

///掷2颗色子事件定起牌方向和起牌点事件
void IMjLogicEx::SeziMakeDirAndGetPai(void * pNetData, UINT uDataSize)
{
	IMjLogic::SeziMakeDirAndGetPai(pNetData,uDataSize);
}
///掷2颗色子事件定起牌点事件
void IMjLogicEx::SeziMakeGetPai(void * pNetData, UINT uDataSize)
{
	IMjLogic::SeziMakeGetPai(pNetData,uDataSize);
}
///发牌事件
void IMjLogicEx::SendCard(void * pNetData, UINT uDataSize)
{
	IMjLogic::SendCard(pNetData,uDataSize);
}
///跳牌事件，庄家跳牌
void IMjLogicEx::TiaoPai(void * pNetData,UINT uDataSize)
{
	IMjLogic::TiaoPai(pNetData,uDataSize);
}
///掷2颗色子定精牌事件
void IMjLogicEx::SeziMakeJing(void * pNetData, UINT uDataSize)
{
	IMjLogic::SeziMakeJing(pNetData,uDataSize);
}
//为所有玩家补花事件
void IMjLogicEx::AllBuHua(void * pNetData, UINT uDataSize)
{
	IMjLogic::AllBuHua(pNetData,uDataSize);
}
///单个玩家补花事件
void IMjLogicEx::OneBuHua(void * pNetData, UINT uDataSize)
{
	IMjLogic::OneBuHua(pNetData,uDataSize);
}
///庄家开始发牌通知事件，确定精牌后服务器发送消息通知玩家出牌
void IMjLogicEx::BeginOutCard(void * pNetData, UINT uDataSize)
{
	IMjLogic::BeginOutCard(pNetData,uDataSize);
}
///出牌事件，玩家出牌
void IMjLogicEx::UserOutCard(void * pNetData, UINT uDataSize)
{
	IMjLogic::UserOutCard(pNetData,uDataSize);
}
///抓牌事件
void IMjLogicEx::UserZhuaAPai(void * pNetData, UINT uDataSize)
{
	IMjLogic::UserZhuaAPai(pNetData,uDataSize);
}
///吃碰杠通知事件
void IMjLogicEx::CPGNotify(void * pNetData, UINT uDataSize)
{
	IMjLogic::CPGNotify(pNetData,uDataSize);
}
///吃牌事件
void IMjLogicEx::UserChi(void * pNetData, UINT uDataSize)
{
	IMjLogic::UserChi(pNetData,uDataSize);
}
///碰牌事件
void IMjLogicEx::UserPeng(void * pNetData, UINT uDataSize)
{
	IMjLogic::UserPeng(pNetData,uDataSize);
}
///坎牌事件
void IMjLogicEx::UserKan(void * pNetData, UINT uDataSize)
{
	IMjLogic::UserKan(pNetData,uDataSize);
}
///扫虎事件
void IMjLogicEx::UserSaoHu(void * pNetData, UINT uDataSize)
{
	IMjLogic::UserSaoHu(pNetData,uDataSize);
}
///杠牌事件
void IMjLogicEx::UserGang(void * pNetData, UINT uDataSize)
{
	IMjLogic::UserGang(pNetData,uDataSize);
}
///听牌事件
void IMjLogicEx::UserTing(void * pNetData, UINT uDataSize)
{
	IMjLogic::UserTing(pNetData,uDataSize);
}
///糊事件
void IMjLogicEx::UserHu(void * pNetData, UINT uDataSize)
{
	IMjLogic::UserHu(pNetData,uDataSize);
}
///算分事件
void IMjLogicEx::CountFen(void * pNetData, UINT uDataSize)
{
	IMjLogic::CountFen(pNetData,uDataSize);
}
///结束处理事件，游戏正常结束
void IMjLogicEx::H081_UserContinueEnd(void * pNetData, UINT uDataSize)
{
	IMjLogic::H081_UserContinueEnd(pNetData,uDataSize);
}
///非正常结束处理事件，有人强退游戏结束
void IMjLogicEx::H080_UserLeftOrGameAnQuanFinish(void * pNetData, UINT uDataSize)
{
	IMjLogic::H080_UserLeftOrGameAnQuanFinish(pNetData,uDataSize);
}
///玩家托管
void IMjLogicEx::UserTuoGuan(void * pNetData, UINT uDataSize)
{
	IMjLogic::UserTuoGuan(pNetData,uDataSize);
}
//设置麻将规则
void IMjLogicEx::SetMjRule(void * pNetData, UINT uDataSize)
{
	IMjLogic::SetMjRule(pNetData,uDataSize);
}

void IMjLogicEx::ResetGameStation(int iGameStation)
{
	IMjLogic::ResetGameStation(iGameStation);
}
///自动出牌,机器人的代码也在此函数
void IMjLogicEx::AutoOutCard(BYTE pai,bool AI)	
{
	//IMjLogic::AutoOutCard(pai,AI);

	g_pCClientGameDlg->KillTimer(ID_OUT_CARD);
	if(!AI)//不是机器人，不需要人工智能。出最后抓来的牌就可以了
	{
		BYTE outpai = g_UserData.GetLastHandPai(m_Station);
		if(g_UserData.m_StrJing.CheckIsJing(outpai))
		{//确保不会自动打出财神牌
			for(int i=0;i<g_UserData.GetHandPaiCount(m_Station)-1;i++)
			{
				if(!g_UserData.m_StrJing.CheckIsJing(g_UserData.m_byArHandPai[m_Station][i]))
				{
					outpai = g_UserData.m_byArHandPai[m_Station][i];
					break;
				}
			}
		}
		IChuPai(outpai);
	}
}
/////处理以上没有提到的消息
//bool IMjLogicEx::HandleGameMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize)
//{
//	return IMjLogic::HandleGameMessage(pNetHead,pNetData,uDataSize);
//}
//*********************被界面调到的函数*************************
///响应点击右键
void IMjLogicEx::OnRButtonUp()
{
	IMjLogic::OnRButtonUp();
}
///响应鼠标消息
void IMjLogicEx::OnMouseMove(int iCard,int iCardIndex,bool isclick)
{
	IMjLogic::OnMouseMove(iCard, iCardIndex, isclick);
}
///点麻将的某个操作按钮
void IMjLogicEx::OnMjBtnClick( int btntype)
{
	IMjLogic::OnMjBtnClick(btntype);
}

///给逻辑重载
///吃
void IMjLogicEx::OnMjBtnClickChi(BYTE index)
{
	IMjLogic::OnMjBtnClickChi(index);
}
///碰
void IMjLogicEx::OnMjBtnClickPeng()
{
	IMjLogic::OnMjBtnClickPeng();
}
///坎
void IMjLogicEx::OnMjBtnClickKan(BYTE pai)
{
	IMjLogic::OnMjBtnClickKan(pai);
}
///扫虎
void IMjLogicEx::OnMjBtnClickSaoHu()
{
	IMjLogic::OnMjBtnClickSaoHu();
}
///杠
void IMjLogicEx::OnMjBtnClickGang(BYTE index)
{
	IMjLogic::OnMjBtnClickGang(index);
}
///听
void IMjLogicEx::OnMjBtnClickTing(bool bIsYanPai)
{
	IMjLogic::OnMjBtnClickTing(bIsYanPai);
}
///胡
void IMjLogicEx::OnMjBtnClickHu() 
{
	IMjLogic::OnMjBtnClickHu();
}
///过
void IMjLogicEx::OnMjBtnClickguo()
{
	IMjLogic::OnMjBtnClickguo();
}
///出牌
void IMjLogicEx::IChuPai( BYTE pai)
{
	OutputDebugString("出牌：IMjLogicEx");
	IMjLogic::IChuPai(pai);
}
///得到本次能吃牌,杠牌，坎牌的个数
int IMjLogicEx::GetChiGangKanNum(BYTE type)
{
	return IMjLogic::GetChiGangKanNum(type);
}
///清理临时数据（吃碰杠的临时信息）
void IMjLogicEx::ClearTempData()
{
	IMjLogic::ClearTempData();
}
///描述：设置吃碰杠数据（吃碰杠的临时信息）
void IMjLogicEx::GetCPGData(tagCPGNotifyEx * pData)
{
	IMjLogic::GetCPGData(pData);
}

