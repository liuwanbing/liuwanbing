#include "stdafx.h"
#include "ThingHandle.h"
#include "../server/ServerManage.h"

ThingHandle::ThingHandle(void)
{	
}

ThingHandle::~ThingHandle(void)
{
}

//拷贝父类的指针
void ThingHandle::SetDeskPoint(CServerGameDesk *desk)
{
	pDesk = desk;
}

//断线重连事件：玩家坐下桌子就会通过该接口获取游戏的数据
bool ThingHandle::GetGameStaton(BYTE bDeskStation, bool bWatchUser,BYTE GameStation)
{
	GameStationDataEx GameStationData;
	::memset(&GameStationData, 0, sizeof(GameStationData));
	GameStationData.iVersion	=  MAIN_VERSION;
	GameStationData.iVersion2	= ASS_VERSION;
	//获取房间倍率逃跑罚分
	GameStationData.bShowUserId	= pDesk->sGameData.m_bShowUserId;//是否显示所有玩家的id
	GameStationData.Stationpara = pDesk->m_bGameStation;		 //游戏状态
	//断线重连取消托管
	pDesk->sUserData.m_bTuoGuan[bDeskStation] = false;
	for(int i=0;i<4;++i)
	{
		memset(GameStationData.m_byArHandPai,255,sizeof(GameStationData.m_byArHandPai));

		pDesk->sUserData.CopyGCPData(i,GameStationData.m_UserGCPData[i]);
		pDesk->sUserData.CopyOutPai(i,GameStationData.m_byArOutPai[i]);
		pDesk->sUserData.CopyMenPai(i,GameStationData.m_iArMenPai[i]);
		pDesk->sUserData.CopyHuaPai(i,GameStationData.m_byArHuaPai[i]);
		GameStationData.m_bTing[i] = pDesk->sUserData.m_bTing[i];
		GameStationData.m_bistuoguan[i] = pDesk->sUserData.m_bTuoGuan[i];
	}
	pDesk->sUserData.CopyHandPai(GameStationData.m_byArHandPai,bDeskStation,false);
	GameStationData.m_byOtherOutPai = pDesk->sUserData.m_byOtherOutPai;
	GameStationData.m_byThingDoing = pDesk->sGameData.m_byThingDoing;
	GameStationData.byQuanFeng = pDesk->sUserData.m_byQuanFeng;
	GameStationData.byNtStation = pDesk->sUserData.m_byNtStation;
	GameStationData.m_byNowOutStation = pDesk->sUserData.m_byNowOutStation;
	//GameStationData.m_StrJing = pDesk->sUserData.m_StrJing;

	GameStationData.byAutoOutTime = pDesk->sGameData.m_mjRule.byAutoOutTime;		//自动出牌时间
	GameStationData.byOutTime = pDesk->sGameData.m_mjRule.byOutTime;				//出牌时间
	GameStationData.byThinkTime = pDesk->sGameData.m_mjRule.byThinkTime;			//思考时间
	GameStationData.byBeginTime = pDesk->sGameData.m_mjRule.byBeginTime;			//开始等待时间
	GameStationData.bAutoBegin =  pDesk->sGameData.m_mjRule.bAutoBegin;			//时间到了是否自动开始

	if(!bWatchUser)
	{
		pDesk->sUserData.m_bFoceLeave[bDeskStation] = false;
		//断线重连取消托管
		pDesk->sUserData.m_bTuoGuan[bDeskStation] = false;
		//pDesk->sUserData.Isu
	}
	OutputDebugString(TEXT("麻将-重连：服务器 OnGetGameStation  "));
	switch (pDesk->m_bGameStation)
	{
	case GS_WAIT_SETGAME:		//游戏没有开始状态
		{
			//发送数据
			pDesk->SendGameStationEx(bDeskStation, bWatchUser, &GameStationData, sizeof(GameStationData));
			MessageStruct Mess;
			ZeroMemory(Mess.Message, 200);
			lstrcpy(Mess.Message, TEXT("歡迎參加麻將游戲,祝您玩的開心！"));
			pDesk->SendGameDataEx(bDeskStation, &Mess, sizeof(Mess), MDM_GM_GAME_NOTIFY, ASS_MESSAGE, 0);
			return true;
		}
	case GS_WAIT_ARGEE:			//等待玩家开始状态
	case GS_SEND_CARD:		//发牌状态(还未发牌)
		{
			//发送数据
			pDesk->SendGameStationEx(bDeskStation,  bWatchUser, &GameStationData, sizeof(GameStationData));
			return true;
		}
	case GS_PLAY_GAME:	//游戏中状态
		{
			//发送数据
			pDesk->SendGameStationEx(bDeskStation,  bWatchUser, &GameStationData, sizeof(GameStationData));
			if(pDesk->sGameData.T_CPGNotify[bDeskStation].m_byMaxThing > ACTION_NO && pDesk->sGameData.T_CPGNotify[bDeskStation].m_byMaxThing <= ACTION_HU)//如果有吃碰杠就进行处理
			{
				pDesk->SendGameDataEx(bDeskStation, &pDesk->sGameData.T_CPGNotify[bDeskStation], sizeof(pDesk->sGameData.T_CPGNotify[bDeskStation]), MDM_GM_GAME_NOTIFY, THING_CPG_NOTIFY, 0);
			}
			return true;
		}
	case GS_WAIT_NEXT:		//等待下一盘游戏开始
		{
			//发送数据
			pDesk->SendGameStationEx(bDeskStation,  bWatchUser, &GameStationData, sizeof(GameStationData));
			return true;
		}
	}
	return true;
}

///接收并处理消息
int ThingHandle::HandleNotifyMessage(int bDeskStation, UINT code, BYTE* pData, int uSize, bool bWatchUser)
{
	pDesk->KillTimer(TIME_CHECK_GAME_MSG);
	pDesk->SetTimer(TIME_CHECK_GAME_MSG,TIME_DEFAULT_HANDLE);//重新设置超时计时器

	switch (code)
	{
	case ASS_GM_SHORT_STOP:
		{
			return 0;
		}
		break;
	case ASS_SEZIFINISH:	//色子动画结束
		{
			return ReceiveSeziFinish(bDeskStation, pData, uSize, bWatchUser);
		}
	case ASS_AUTOCARD://托管
		{	
			return ReceiveUserTuoGuan(bDeskStation, pData, uSize, bWatchUser);
		}
		break;
	case THING_OUT_PAI:
		{
			// 用户出牌
			return ReceiveUserOutCard(bDeskStation, pData, uSize, bWatchUser);
		}
		break;
	case ASS_PASS_CHECK:		
		{
			return ReceiveUserPassCheck(bDeskStation, pData, uSize,   bWatchUser);
		}
		break;
	case ASS_GM_AGREE_GAME:		
		{
			//收到用户同意游戏消息
			return ReceiveUserAgreeGame(bDeskStation, pData, uSize,   bWatchUser);
		}
		break;
	case THING_CHI_PAI:	
		{
			// 用户吃牌
			return ReceiveUserChiCard(bDeskStation, pData, uSize, bWatchUser);
		}
		break;
	case THING_PENG_PAI:
		{// 用户碰牌
			return ReceiveUserPengCard(bDeskStation, pData, uSize, bWatchUser);
		}
		break;;
	case THING_GANG_PAI:
		{// 用户杠牌
			return ReceiveUserGangCard(bDeskStation, pData, uSize, bWatchUser);
		}
		break;
	case THING_TING_PAI:
		{// 用户听牌
			return ReceiveUserTingCard(bDeskStation, pData, uSize, bWatchUser);
		}
		break;
	case THING_HU_PAI:		
		{// 用户糊牌
			return ReceiveUserHuCard(bDeskStation, pData, uSize, bWatchUser);
		}
		break;
	case ASS_HAVE_THING:	
		{//有事请求离开
			return ReceiveUserHaveThing(bDeskStation, pData, uSize,   bWatchUser);
		}
		break;
	case ASS_LEFT_RESULT: 
		{//同意用户离开
			return ReceiveArgeeUserLeft(bDeskStation, pData, uSize,   bWatchUser);
		}
		break;
	case ASS_EXIT://等待开始一下局时退出
		{
			if(pDesk->m_bGameStation==23)
				return pDesk->GameFinish(bDeskStation,GF_SALE);
			else
				return 0;
		}
		break;
#ifdef MJ_CAN_SETPAI//超级客户端抓牌，发牌
	case ASS_SUPER://超级客户端,进行牌设置
		{
			ReceiveSuperChangePai(bDeskStation,pData, uSize, bWatchUser);
		}
		break;
#endif // MJ_CAN_SETPAI
	}
	return 0;
}

////////////////////////////////////////处理接收消息//////////////////////////////////////////////////////

// 收到用户色子动画结束消息
int ThingHandle::ReceiveSeziFinish(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser)
{
	if(!pDesk->sUserData.m_bWaitSezi || bWatchUser)//是否处理等待色子动画结束状态
		return 0;
	pDesk->sUserData.m_bSiziFinish[bDeskStation] = true;

	char strbug[800];
	wsprintf(strbug,TEXT("麻将-事件：服务器 ReceiveSeziFinish  位置 bDeskStation =%d ApplyThingID=%d"),bDeskStation,pDesk->sGameData.ApplyThingID);
	OutputDebugString(strbug);

	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(!pDesk->sUserData.m_bSiziFinish[i])//有玩家没有返回色子结束消息
			return 0;
	}


	//处理结束时间
	memset(pDesk->sUserData.m_bSiziFinish,0,sizeof(pDesk->sUserData.m_bSiziFinish));
	pDesk->sUserData.m_bWaitSezi = false;

	ApplyAction(0,500);
	return 0;
}

// 收到托管消息
int ThingHandle::ReceiveUserTuoGuan(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser)
{
	if(bWatchUser)
		return 0;
	if(bWatchUser || pDesk->m_bGameStation > GS_PLAY_GAME || pDesk->m_bGameStation < GS_SEND_CARD)//不在游戏中不能托管
		return 0;

    Usertouguan *pCmd = (Usertouguan *)pData;
	Usertouguan touguan;
	touguan.desk = bDeskStation;
	touguan.touguang = pCmd->touguang;

	pDesk->sUserData.m_bTuoGuan[bDeskStation]=touguan.touguang;
	for(int i=0;i<PLAY_COUNT;++i)
	{
		touguan.bTuoGuan[i] = pDesk->sUserData.m_bTuoGuan[i];		// 各个玩家托管状态
	}
	for(int i=0;i<PLAY_COUNT;i++)
	{
		pDesk->SendGameDataEx(i,&touguan,sizeof(Usertouguan),MDM_GM_GAME_NOTIFY,ASS_AUTOCARD,0);
		pDesk->SendWatchDataEx(i,&touguan,sizeof(Usertouguan),MDM_GM_GAME_NOTIFY,ASS_AUTOCARD,0);
	}
	return 0;
}

// 用户出牌
int ThingHandle::ReceiveUserOutCard(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser)
{
	tagOutPaiEx * pOutCard = (tagOutPaiEx *)pData;

	if(pOutCard == NULL || bWatchUser || pDesk->m_bGameStation != GS_PLAY_GAME)
	{
		return 0;
	}
	if(pOutCard->byPs == 255 || pDesk->sUserData.GetAHandPaiCount(bDeskStation,pOutCard->byPs)==0 || !pDesk->sUserData.IsOutPaiPeople(bDeskStation))
	{
		return 0;
	}

	pDesk->sGameData.T_OutPai = * pOutCard;
	pDesk->sGameData.m_byThingDoing = THING_OUT_PAI;//记录正在执行的事件
	pDesk->sGameData.m_byNextAction = ACTION_NO;
	pDesk->sUserData.m_byOtherOutPai = pOutCard->byPs;
	//删除一张手牌
	pDesk->sUserData.DelleteAHandPai(bDeskStation,pOutCard->byPs);
	//添加一张出牌
	pDesk->sUserData.AddToOutPai(bDeskStation,pOutCard->byPs);
	//记录已经出过牌了
	pDesk->sUserData.m_bHaveOut[bDeskStation] = true;
	//清理临时数据
	ClearTempData();

	//玩家点击了听牌
	if(pDesk->sGameData.T_TingPai.bHitTing[bDeskStation] && !pDesk->sUserData.m_bTing[bDeskStation])//玩家点击了听牌,
	{
		pDesk->sGameData.T_TingPai.bHitTing[bDeskStation] = false;
		if(CLogicBase::IsHaveInArr(pDesk->sGameData.T_CPGNotify[bDeskStation].m_byTingCanOut,pOutCard->byPs,HAND_CARD_NUM))
		{
			pDesk->sUserData.m_bTing[bDeskStation] = true;
			pDesk->sGameData.T_OutPai.bTing = true;
		}
		else
		{
			pDesk->sUserData.m_bTing[bDeskStation] = false;
			pDesk->sGameData.T_TingPai.bHitTing[bDeskStation] = false;
		}
	}
	//拷贝出牌
	for(int i=0;i<PLAY_COUNT;++i)
	{
		pDesk->sUserData.CopyOutPai(i,pDesk->sGameData.T_OutPai.m_byArOutPai[i]);
	}
	///当前抓牌方向，true 顺时针，false 逆时针
	pDesk->sUserData.m_bZhuaPaiDir = true;
	for(int i=0;i<PLAY_COUNT;i++)//获取听牌状态
	{
		pDesk->sGameData.T_OutPai.bUserTing[i] = pDesk->sUserData.m_bTing[i];
	}
	//发送出牌数据
	for (int i=0;i<PLAY_COUNT;i++)
	{	
		memset(pDesk->sGameData.T_OutPai.m_byArHandPai,255,sizeof(pDesk->sGameData.T_OutPai.m_byArHandPai));
		pDesk->sUserData.CopyHandPai(pDesk->sGameData.T_OutPai.m_byArHandPai,i,false);
		pDesk->SendGameDataEx(i, &pDesk->sGameData.T_OutPai, sizeof(pDesk->sGameData.T_OutPai), MDM_GM_GAME_NOTIFY, THING_OUT_PAI, 0);
		pDesk->SendWatchDataEx(i, &pDesk->sGameData.T_OutPai, sizeof(pDesk->sGameData.T_OutPai), MDM_GM_GAME_NOTIFY, THING_OUT_PAI, 0);
	}

	//动作检测：没有人能响应就直接抓牌
	if(!CheckAction(bDeskStation,pOutCard->byPs,2))
	{//没有动作就直接下一个玩家抓牌
		pDesk->sGameData.ApplyThingID = THING_ZHUA_PAI;
		ApplyAction(THING_ZHUA_PAI,500);	//申请下一个事件
	}
	else
	{
		for (int i=0;i<PLAY_COUNT;i++)
		{	
			if(i==bDeskStation || pDesk->sUserData.m_bFoceLeave[i] || pDesk->sGameData.T_CPGNotify[i].m_byMaxThing == ACTION_NO)
				continue;
			pDesk->SendGameDataEx(i, &pDesk->sGameData.T_CPGNotify[i], sizeof(pDesk->sGameData.T_CPGNotify[i]), MDM_GM_GAME_NOTIFY, THING_CPG_NOTIFY, 0);
			pDesk->SendWatchDataEx(i, &pDesk->sGameData.T_CPGNotify[i], sizeof(pDesk->sGameData.T_CPGNotify[i]), MDM_GM_GAME_NOTIFY, THING_CPG_NOTIFY, 0);
		}
		if(pDesk->sGameData.m_mjRule.bNetCutTuoGuan)
		{
			NetCutAutoHandle();
		}
	}
	pDesk->sUserData.m_byApplyMaxAction = ACTION_NO;//玩家当前申请的最大事件：处理完申请后，恢复数据

	return 0;
}

// 收到用户pass 消息
int ThingHandle::ReceiveUserPassCheck(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser)
{
	if(pDesk->sGameData.ApplyThingID != 255 || bWatchUser)//有申请事件就不响应pass消息
		return 0;

	pDesk->sGameData.T_CPGNotify[bDeskStation].bHaveReturn = true;//记录玩家回应
	pDesk->sGameData.T_CPGNotify[bDeskStation].m_byMaxThing = ACTION_NO;//改变玩家能做的最大动作
	CheckNextAction();	
	return 0;
}

//收到用户同意游戏消息
int ThingHandle::ReceiveUserAgreeGame(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser)
{
	//用户同意游戏
	if (bWatchUser || (pDesk->m_bGameStation != GS_WAIT_ARGEE) && (pDesk->m_bGameStation != GS_WAIT_NEXT)) 
		return 0;
	for(int i=0;i<HAND_CARD_NUM;i++)
	{
		pDesk->SendGameDataEx(i, MDM_GM_GAME_NOTIFY, ASS_GM_AGREE_GAME, 0);
		pDesk->SendWatchDataEx(i, MDM_GM_GAME_NOTIFY, ASS_GM_AGREE_GAME, 0);
	}
	return 0;
}

//有事请求离开
int ThingHandle::ReceiveUserHaveThing(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser)
{
	if(bWatchUser)				return 0;
	HaveThingStruct * pThing=(HaveThingStruct *)pData;
	if (pDesk->m_bGameStation<GS_SEND_CARD) return 0;
	//{//发送用户请求离开
	HaveThingStruct HaveThing;
	HaveThing.pos = bDeskStation;
	::CopyMemory(HaveThing.szMessage,pThing->szMessage,60*sizeof(char));
	for (int i=0;i<PLAY_COUNT;i++)
	{
		if (i!=bDeskStation) pDesk->SendGameDataEx(i,&HaveThing,sizeof(HaveThing),MDM_GM_GAME_NOTIFY,ASS_HAVE_THING,0);
	}
	return 0;
}

//同意用户离开
int ThingHandle::ReceiveArgeeUserLeft(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser)
{
	if(bWatchUser)				return false;
	LeaveResultStruct * pLeave=(LeaveResultStruct *)pData;
	if (pLeave->bArgeeLeave)
	{
		pDesk->sUserData.m_byAgreeLeaveNum++;
	}
	else
	{
		pDesk->sUserData.m_byAgreeLeaveNum = 0;//同意结束游戏的玩家数量
	}
	if(pDesk->sUserData.m_byAgreeLeaveNum<(PLAY_COUNT-1))
	{//发送玩家反馈

		LeaveResultStruct Leave;
		Leave.bDeskStation=bDeskStation;
		Leave.bArgeeLeave=pLeave->bArgeeLeave;
		for (int i=0;i<PLAY_COUNT;i++) 
			if (i!=bDeskStation) pDesk->SendGameDataEx(i,&Leave,sizeof(Leave),MDM_GM_GAME_NOTIFY,ASS_LEFT_RESULT,0);
	}
	else 
	{
		pDesk->sUserData.m_byAgreeLeaveNum = 0;
		pDesk->GameFinish(255,GF_SALE);		
	}
	return 0;
}

// 用户吃牌消息
int ThingHandle::ReceiveUserChiCard(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser)
{

	tagChiPaiEx * pChiCard = (tagChiPaiEx *)pData;
	if(pChiCard == NULL || bWatchUser || pDesk->m_bGameStation != GS_PLAY_GAME)
		return 0;
	if(bDeskStation == pDesk->sUserData.m_byNowOutStation || !pDesk->sGameData.T_CPGNotify[bDeskStation].bChi || pChiCard->byPs == 255 || !pDesk->sGameData.m_mjAction.bChi)
	{//不合法动作
		return 0;
	}
	if(pDesk->sUserData.m_byApplyMaxAction < ACTION_CHI)
	{
		pDesk->sUserData.m_byApplyMaxAction = ACTION_CHI;//玩家当前申请的最大事件
	}
	pDesk->sGameData.temp_ChiPai.Clear();
	pDesk->sGameData.temp_ChiPai = * pChiCard;//记录临时吃牌数据

	pDesk->sGameData.T_CPGNotify[pChiCard->byUser].bHaveReturn = true;//记录玩家回应
	pDesk->sGameData.T_CPGNotify[pChiCard->byUser].m_byMaxThing = ACTION_CHI;//改变玩家能做的最大动作
	if(pDesk->sGameData.m_byNextAction < ACTION_CHI)
	{
		pDesk->sGameData.m_byNextAction = ACTION_CHI;
	}
	//检测下一个动作
	CheckNextAction();
	return 0;
}

// 用户碰牌消息
int ThingHandle::ReceiveUserPengCard(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser)
{
	tagPengPaiEx * pPengCard = (tagPengPaiEx *)pData;
	if(pPengCard == NULL || bWatchUser ||  pDesk->m_bGameStation != GS_PLAY_GAME)
		return 0;
	if(bDeskStation == pDesk->sUserData.m_byNowOutStation || !pDesk->sGameData.T_CPGNotify[bDeskStation].bPeng || pPengCard->byPs == 255 || !pDesk->sGameData.m_mjAction.bPeng)
	{//不合法动作
		return 0;
	}
	if(pDesk->sUserData.m_byApplyMaxAction < ACTION_PENG)
	{
		pDesk->sUserData.m_byApplyMaxAction = ACTION_PENG;//玩家当前申请的最大事件
	}
	pDesk->sGameData.temp_PengPai.Clear();
	pDesk->sGameData.temp_PengPai = * pPengCard;//记录临时碰牌数据
	pDesk->sGameData.T_CPGNotify[pPengCard->byUser].bHaveReturn = true;//记录玩家回应
	pDesk->sGameData.T_CPGNotify[pPengCard->byUser].m_byMaxThing = ACTION_PENG;//改变玩家能做的最大动作
	if(pDesk->sGameData.m_byNextAction < ACTION_PENG)
	{
		pDesk->sGameData.m_byNextAction = ACTION_PENG;
	}
	//检测下一个动作
	CheckNextAction();
	return 0;
}

// 用户杠牌消息
int ThingHandle::ReceiveUserGangCard(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser)
{
	tagGangPaiEx * pGangCard = (tagGangPaiEx *)pData;
	if(pGangCard == NULL || bWatchUser || pDesk->m_bGameStation != GS_PLAY_GAME)
		return 0;
	if(!pDesk->sGameData.T_CPGNotify[bDeskStation].bGang || pGangCard->byPs == 255 || !pDesk->sGameData.m_mjAction.bGang)
	{//不合法动作
		return 0;
	}

	bool action = false;
	pDesk->sGameData.temp_GangPai.Clear();
	pDesk->sGameData.temp_GangPai = * pGangCard;//记录临时杠牌数据
	if(pGangCard->byType == ACTION_BU_GANG)
	{
		action = CheckAction(pGangCard->byUser,pGangCard->byPs,3);//检测是否有截取补杠的动作：胡牌等
	}
	if(pGangCard->byType == ACTION_MING_GANG)//明杠，记录请求
	{
		pDesk->sGameData.T_CPGNotify[pGangCard->byUser].bHaveReturn = true;//记录玩家回应
		pDesk->sGameData.T_CPGNotify[pGangCard->byUser].m_byMaxThing = ACTION_MING_GANG;//改变玩家能做的最大动作
		if(pDesk->sGameData.m_byNextAction < ACTION_MING_GANG)
		{
			pDesk->sGameData.m_byNextAction = ACTION_MING_GANG;
		}
	}

	switch(pGangCard->byType)
	{
	case ACTION_AN_GANG:
		{
			if(pDesk->sUserData.m_byApplyMaxAction < ACTION_AN_GANG)
			{
				pDesk->sUserData.m_byApplyMaxAction = ACTION_AN_GANG;//玩家当前申请的最大事件
			}
			//暗杠可以即时执行动作
			BYTE NextID=255,StarTime=255;//下个事件的id,255时为互动事件需要客户返回，StarTime多长时间后开始执行（100毫秒为单位）
			HandleGangPai(NextID,StarTime);//处理事件数据
			pDesk->sGameData.ApplyThingID = NextID;
			ApplyAction(NextID,StarTime*100);//申请下一个事件
			//发送杠牌消息
			for (int i=0;i<PLAY_COUNT;i++)
			{	
				pDesk->sUserData.CopyHandPai(pDesk->sGameData.T_GangPai.m_byArHandPai,i,false);
				pDesk->SendGameDataEx(i, &pDesk->sGameData.T_GangPai, sizeof(pDesk->sGameData.T_GangPai), MDM_GM_GAME_NOTIFY, THING_GANG_PAI, 0);
				pDesk->SendWatchDataEx(i, &pDesk->sGameData.T_GangPai, sizeof(pDesk->sGameData.T_GangPai), MDM_GM_GAME_NOTIFY, THING_GANG_PAI, 0);
			}
		}
		break;
	case ACTION_BU_GANG:
		{
			if(pDesk->sUserData.m_byApplyMaxAction < ACTION_BU_GANG)
			{
				pDesk->sUserData.m_byApplyMaxAction = ACTION_BU_GANG;//玩家当前申请的最大事件
			}
			if(!action)//无人抢杠，直接处理
			{
				BYTE NextID=255,StarTime=255;//下个事件的id,255时为互动事件需要客户返回，StarTime多长时间后开始执行（100毫秒为单位）
				HandleGangPai(NextID,StarTime);//处理事件数据
				pDesk->sGameData.ApplyThingID = NextID;
				ApplyAction(NextID,StarTime*100);//申请下一个事件
				for (int i=0;i<PLAY_COUNT;i++)
				{	
					pDesk->sUserData.CopyGCPData(i,pDesk->sGameData.T_GangPai.m_UserGCPData[i]);
				}
				//发送杠牌消息
				for (int i=0;i<PLAY_COUNT;i++)
				{						
					pDesk->sUserData.CopyHandPai(pDesk->sGameData.T_GangPai.m_byArHandPai,i,false);
					pDesk->SendGameDataEx(i, &pDesk->sGameData.T_GangPai, sizeof(pDesk->sGameData.T_GangPai), MDM_GM_GAME_NOTIFY, THING_GANG_PAI, 0);
					pDesk->SendWatchDataEx(i, &pDesk->sGameData.T_GangPai, sizeof(pDesk->sGameData.T_GangPai), MDM_GM_GAME_NOTIFY, THING_GANG_PAI, 0);
				}
			}
			else///有人抢杠，发送抢杠消息给别人检测
			{
				for (int i=0;i<PLAY_COUNT;i++)
				{	
					if(!pDesk->sGameData.T_CPGNotify[i].bCanAction || pDesk->sUserData.m_bFoceLeave[i])
						continue;
					pDesk->SendGameDataEx(i, &pDesk->sGameData.T_CPGNotify[i], sizeof(pDesk->sGameData.T_CPGNotify[i]), MDM_GM_GAME_NOTIFY, THING_CPG_NOTIFY, 0);
					pDesk->SendWatchDataEx(i, &pDesk->sGameData.T_CPGNotify[i], sizeof(pDesk->sGameData.T_CPGNotify[i]), MDM_GM_GAME_NOTIFY, THING_CPG_NOTIFY, 0);
				}
				if(pDesk->sGameData.m_mjRule.bNetCutTuoGuan)
				{
					NetCutAutoHandle();
				}
				return true;
			}
		}
		break;
	case ACTION_MING_GANG://明杠
		{
			if(pDesk->sUserData.m_byApplyMaxAction < ACTION_MING_GANG)
			{
				pDesk->sUserData.m_byApplyMaxAction = ACTION_MING_GANG;//玩家当前申请的最大事件
			}
			//检测下一个动作
			CheckNextAction();
			return true;
		}
		break;
	}

	return 0;
}

// 用户听牌消息
int ThingHandle::ReceiveUserTingCard(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser)
{
	tagTingPaiEx * pTingCard = (tagTingPaiEx *)pData;
	if(pTingCard == NULL || bWatchUser || pDesk->m_bGameStation != GS_PLAY_GAME)
		return 0;
	if(!pDesk->sGameData.T_CPGNotify[bDeskStation].bTing || !pDesk->sGameData.m_mjAction.bTing)
	{//不合法动作
		return 0;
	}
	if(pDesk->sUserData.m_byApplyMaxAction < ACTION_TING)
	{
		pDesk->sUserData.m_byApplyMaxAction = ACTION_TING;//玩家当前申请的最大事件
	}
	pDesk->sGameData.T_TingPai.bHitTing[pTingCard->byUser] = true;
	//通知所有玩家
	for (int i=0;i<PLAY_COUNT;i++)
	{	
		pDesk->SendGameDataEx(i, &pDesk->sGameData.T_TingPai, sizeof(pDesk->sGameData.T_TingPai), MDM_GM_GAME_NOTIFY, THING_TING_PAI, 0);
		pDesk->SendWatchDataEx(i, &pDesk->sGameData.T_TingPai, sizeof(pDesk->sGameData.T_TingPai), MDM_GM_GAME_NOTIFY, THING_TING_PAI, 0);
	}
	return 0;
}

// 用户糊牌消息
int ThingHandle::ReceiveUserHuCard(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser)
{
	tagHuPaiEx * pHuCard = (tagHuPaiEx *)pData;
	if(pHuCard == NULL || bWatchUser || pDesk->m_bGameStation != GS_PLAY_GAME)
		return 0;
	if(!pDesk->sGameData.T_CPGNotify[bDeskStation].bHu)
	{//不合法动作
		return 0;
	}

	if(pDesk->sUserData.m_byApplyMaxAction < ACTION_HU)
	{
		pDesk->sUserData.m_byApplyMaxAction = ACTION_HU;//玩家当前申请的最大事件
	}

	if(bDeskStation == pDesk->sUserData.m_byNowOutStation)//当前出牌玩家糊牌(自摸)
	{
		pDesk->sGameData.T_HuPai.bZimo = true;
	}
	pDesk->sGameData.T_HuPai.byDianPao = pHuCard->byDianPao;
	pDesk->sGameData.T_HuPai.bCanH[pHuCard->byUser] = true;
	pDesk->sGameData.T_HuPai.byUserNum++;
	pDesk->sGameData.T_HuPai.byNt = pDesk->sUserData.m_byNtStation;		//庄家位置
	pDesk->sGameData.T_HuPai.byPs = pHuCard->byPs;				//点炮的牌值

	pDesk->sGameData.T_CPGNotify[pHuCard->byUser].bHaveReturn = true;//记录玩家回应
	pDesk->sGameData.T_CPGNotify[pHuCard->byUser].m_byMaxThing = ACTION_HU;//改变玩家能做的最大动作

	if(pDesk->sGameData.T_HuPai.byUser == 255)
	{
		pDesk->sGameData.T_HuPai.byUser = pHuCard->byUser;
	}
	else if(!pDesk->sGameData.T_HuPai.bZimo)//选择靠糊牌玩家最近的玩家糊牌
	{//点炮糊牌，并且有多个玩家可以糊牌
		pDesk->sGameData.T_HuPai.byUser = GetCanHuUser(pDesk->sUserData.m_byNowOutStation,pDesk->sGameData.T_HuPai.bCanH,pDesk->sGameData.T_HuPai.byUser);//获取离糊牌玩家近的玩家
	}

	if(pDesk->sGameData.m_byNextAction < ACTION_HU)
	{
		pDesk->sGameData.m_byNextAction = ACTION_HU;
	}
	for(int i=0;i<MAX_HUPAI_TYPE;++i)
	{
		pDesk->sGameData.T_HuPai.byHuType[pHuCard->byUser][i] = pDesk->sGameData.T_CPGNotify[pHuCard->byUser].byHuType[i];
	}
	for(int i=0;i<HAND_CARD_NUM;++i)
	{
		pDesk->sGameData.T_HuPai.m_byArHandPai[pHuCard->byUser][i] = pDesk->sUserData.m_byArHandPai[pHuCard->byUser][i];
	}
	for(int i=0;i<4;++i)
	{
		pDesk->sUserData.CopyGCPData(i,pDesk->sGameData.T_HuPai.m_UserGCPData[i]);
	}

	if(pDesk->sGameData.T_HuPai.bZimo)//玩家自摸可以立即
	{
		BYTE NextID=255,StarTime=255;//下个事件的id,255时为互动事件需要客户返回，StarTime多长时间后开始执行（100毫秒为单位）
		HandleHuPai(NextID,StarTime);//处理事件数据
		pDesk->sGameData.ApplyThingID = NextID;
		ApplyAction(NextID,StarTime*100);//申请下一个事件
		//发送糊牌通知
		///通知玩家糊牌了
		memset(pDesk->sGameData.T_HuPai.m_byArHandPai,255,sizeof(pDesk->sGameData.T_HuPai.m_byArHandPai));
		pDesk->sUserData.CopyHandPai(pDesk->sGameData.T_HuPai.m_byArHandPai,0,true);
		for (int i=0;i<PLAY_COUNT;i++)
		{	
			pDesk->sUserData.CopyGCPData(i,pDesk->sGameData.T_HuPai.m_UserGCPData[i]);
			pDesk->SendGameDataEx(i, &pDesk->sGameData.T_HuPai, sizeof(pDesk->sGameData.T_HuPai), MDM_GM_GAME_NOTIFY, THING_HU_PAI, 0);
			pDesk->SendWatchDataEx(i, &pDesk->sGameData.T_HuPai, sizeof(pDesk->sGameData.T_HuPai), MDM_GM_GAME_NOTIFY, THING_HU_PAI, 0);
		}
	}
	else
	{
		//检测下一个动作
		CheckNextAction();
	}
	return 0;
}

///收到超级客户端设置牌消息
void ThingHandle::ReceiveSuperChangePai(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser)
{
	if(bWatchUser || bDeskStation<0 || bDeskStation>3)
		return;
	SuperStruct *Super =(SuperStruct* )pData;
	bool cpg = false;
	pDesk->sUserData.m_SetData[bDeskStation].Init();
	pDesk->sUserData.m_SetData[bDeskStation].bySetType = Super->bySetType;
	pDesk->sUserData.m_SetData[bDeskStation].mjcount = Super->mjcount;
	for(int i=0;i<Super->mjcount;++i)
	{
		pDesk->sUserData.m_SetData[bDeskStation].mjdata[i] = Super->mjdata[i];
		pDesk->sUserData.m_SetData[bDeskStation].handdata[i] = Super->handdata[i];
	}
	//设置类型：0 换所有牌，1换选定牌，2设置自己的下一张牌，3设置牌强的下一张牌
	switch(Super->bySetType)
	{
	case 0://换所有手牌
		{
			pDesk->sUserData.ChangeAllHandPai(bDeskStation,Super->mjdata,Super->mjcount);
			///如果是当前出牌玩家，检测并返回检测结果
			if(bDeskStation == pDesk->sUserData.m_byNowOutStation)//设置牌玩家是当前出牌玩家
			{
				if(CheckAction(bDeskStation,pDesk->sUserData.GetLastHandPai(bDeskStation),0))
				{
					cpg = true;
				}
			}
		}
		break;
	case 1://换部分手牌
		{
			pDesk->sUserData.ChangeSomeHandPai(bDeskStation,Super->mjdata,Super->handdata,Super->mjcount);
			///如果是当前出牌玩家，检测并返回检测结果
			if(bDeskStation == pDesk->sUserData.m_byNowOutStation)//设置牌玩家是当前出牌玩家
			{
				if(CheckAction(bDeskStation,pDesk->sUserData.GetLastHandPai(bDeskStation),0))
				{
					cpg = true;
				}
			}
		}	
		break;
	case 2://设置自己的下一张牌
		{	
			pDesk->sUserData.m_bySetMoPai[bDeskStation]	 = Super->mjdata[0];
		}
		break;
	case 3://设置牌墙的下一张牌
		{
			pDesk->sUserData.SetGameNextPai(Super->mjdata[0]);
		}
		break;
	}
	if(Super->bySetType == 0 || Super->bySetType == 1)
	{
		SuperStruct superdata = *Super;
		for(int i=0;i<HAND_CARD_NUM;i++)
		{
			superdata.mjdata[i] = pDesk->sUserData.m_byArHandPai[bDeskStation][i];//复制手牌数据发送给客户端更新

		}
		pDesk->SendGameDataEx(bDeskStation, &superdata, sizeof(SuperStruct), MDM_GM_GAME_NOTIFY, ASS_SUPER, 0);
		pDesk->SendWatchDataEx(bDeskStation, &superdata, sizeof(SuperStruct), MDM_GM_GAME_NOTIFY, ASS_SUPER, 0);
	}
	if(cpg && bDeskStation == pDesk->sUserData.m_byNowOutStation)//换牌后能进行吃碰杠操作
	{
		if(!pDesk->sUserData.m_bFoceLeave[bDeskStation])
		{
			pDesk->sGameData.T_CPGNotify[bDeskStation].bZhuaPai = true;
			pDesk->SendGameDataEx(bDeskStation, &pDesk->sGameData.T_CPGNotify[bDeskStation], sizeof(tagCPGNotifyEx), MDM_GM_GAME_NOTIFY, THING_CPG_NOTIFY, 0);
			pDesk->SendWatchDataEx(bDeskStation, &pDesk->sGameData.T_CPGNotify[bDeskStation], sizeof(tagCPGNotifyEx), MDM_GM_GAME_NOTIFY, THING_CPG_NOTIFY, 0);
		}
	}
}


///名称：CheckNextAction
///描述：检测下一步动作
void ThingHandle::CheckNextAction()
{
	if(!IsCanHandle())
	{
		return ;
	}	
	BYTE NextID=255,StarTime=255;//下个事件的id,255时为互动事件需要客户返回，StarTime多长时间后开始执行（100毫秒为单位）

	switch(pDesk->sGameData.m_byNextAction)//确定了下一步的响应事件：吃，碰，杠，胡
	{
	case ACTION_CHI:				//吃牌动作
		{
			HandleChiPai(NextID,StarTime);
			//通知各个玩家
			for (int i=0;i<PLAY_COUNT;i++)
			{	
				memset(pDesk->sGameData.T_ChiPai.m_byArHandPai,255,sizeof(pDesk->sGameData.T_ChiPai.m_byArHandPai));
				pDesk->sUserData.CopyHandPai(pDesk->sGameData.T_ChiPai.m_byArHandPai,i,false);
				pDesk->SendGameDataEx(i, &pDesk->sGameData.T_ChiPai, sizeof(pDesk->sGameData.T_ChiPai), MDM_GM_GAME_NOTIFY, THING_CHI_PAI, 0);
				pDesk->SendWatchDataEx(i, &pDesk->sGameData.T_ChiPai, sizeof(pDesk->sGameData.T_ChiPai), MDM_GM_GAME_NOTIFY, THING_CHI_PAI, 0);
			}
			//判断是否能听牌
			if(pDesk->sGameData.m_mjAction.bTing && pDesk->sGameData.T_CPGNotify[pDesk->sGameData.T_ChiPai.byUser].bTing)//有听牌
			{//发送听牌拦牌
				pDesk->sGameData.T_CPGNotify[pDesk->sGameData.T_ChiPai.byUser].byUser = pDesk->sGameData.T_ChiPai.byUser;
				pDesk->sGameData.T_CPGNotify[pDesk->sGameData.T_ChiPai.byUser].bZhuaPai = true;
				pDesk->SendGameDataEx(pDesk->sGameData.T_ChiPai.byUser, &pDesk->sGameData.T_CPGNotify[pDesk->sGameData.T_ChiPai.byUser], sizeof(tagCPGNotifyEx), MDM_GM_GAME_NOTIFY, THING_CPG_NOTIFY, 0);
				pDesk->SendWatchDataEx(pDesk->sGameData.T_ChiPai.byUser, &pDesk->sGameData.T_CPGNotify[pDesk->sGameData.T_ChiPai.byUser], sizeof(tagCPGNotifyEx), MDM_GM_GAME_NOTIFY, THING_CPG_NOTIFY, 0);
			}
		}
		break;
	case ACTION_PENG:				//碰牌动作
		{
			HandlePengPai(NextID,StarTime);
			//通知各个玩家
			for (int i=0;i<PLAY_COUNT;i++)
			{	
				memset(pDesk->sGameData.T_PengPai.m_byArHandPai,255,sizeof(pDesk->sGameData.T_PengPai.m_byArHandPai));
				pDesk->sUserData.CopyHandPai(pDesk->sGameData.T_PengPai.m_byArHandPai,i,false);
				pDesk->SendGameDataEx(i, &pDesk->sGameData.T_PengPai, sizeof(pDesk->sGameData.T_PengPai), MDM_GM_GAME_NOTIFY, THING_PENG_PAI, 0);
				pDesk->SendWatchDataEx(i, &pDesk->sGameData.T_PengPai, sizeof(pDesk->sGameData.T_PengPai), MDM_GM_GAME_NOTIFY, THING_PENG_PAI, 0);
			}

			//判断是否能听牌
			if(pDesk->sGameData.m_mjAction.bTing && pDesk->sGameData.T_CPGNotify[pDesk->sGameData.T_PengPai.byUser].bTing)//有听牌
			{//发送听牌拦牌
				pDesk->sGameData.T_CPGNotify[pDesk->sGameData.T_PengPai.byUser].byUser = pDesk->sGameData.T_PengPai.byUser;
				pDesk->sGameData.T_CPGNotify[pDesk->sGameData.T_PengPai.byUser].bZhuaPai = true;
				pDesk->SendGameDataEx(pDesk->sGameData.T_PengPai.byUser, &pDesk->sGameData.T_CPGNotify[pDesk->sGameData.T_PengPai.byUser], sizeof(tagCPGNotifyEx), MDM_GM_GAME_NOTIFY, THING_CPG_NOTIFY, 0);
				pDesk->SendWatchDataEx(pDesk->sGameData.T_PengPai.byUser, &pDesk->sGameData.T_CPGNotify[pDesk->sGameData.T_PengPai.byUser], sizeof(tagCPGNotifyEx), MDM_GM_GAME_NOTIFY, THING_CPG_NOTIFY, 0);
			}
		}
		break;
	case ACTION_BU_GANG://补杠动作
	case ACTION_MING_GANG:			//明杠动作
		{
			HandleGangPai(NextID,StarTime);
			//通知各个玩家
			for (int i=0;i<PLAY_COUNT;i++)
			{	
				memset(pDesk->sGameData.T_GangPai.m_byArHandPai,255,sizeof(pDesk->sGameData.T_GangPai.m_byArHandPai));
				pDesk->sUserData.CopyHandPai(pDesk->sGameData.T_GangPai.m_byArHandPai,i,false);
				pDesk->SendGameDataEx(i, &pDesk->sGameData.T_GangPai, sizeof(pDesk->sGameData.T_GangPai), MDM_GM_GAME_NOTIFY, THING_GANG_PAI, 0);
				pDesk->SendWatchDataEx(i, &pDesk->sGameData.T_GangPai, sizeof(pDesk->sGameData.T_GangPai), MDM_GM_GAME_NOTIFY, THING_GANG_PAI, 0);
			}
		}
		break;
	case ACTION_CHI_TNG:			//抢吃（吃后听牌）
		{
		}
		break;
	case ACTION_HU:					//糊动作
		{
			HandleHuPai(NextID,StarTime);

			///通知玩家糊牌了
			memset(pDesk->sGameData.T_HuPai.m_byArHandPai,255,sizeof(pDesk->sGameData.T_HuPai.m_byArHandPai));
			if(!pDesk->sGameData.T_HuPai.bZimo)
			{
				pDesk->sUserData.AddToHandPai(pDesk->sGameData.T_HuPai.byUser,pDesk->sGameData.T_HuPai.byPs);//
			}
			pDesk->sUserData.CopyHandPai(pDesk->sGameData.T_HuPai.m_byArHandPai,0,true);
			for (int i=0;i<PLAY_COUNT;i++)
			{	
				pDesk->sUserData.CopyOutPai(i,pDesk->sGameData.T_HuPai.m_byArOutPai[i]);
			}
			for (int i=0;i<PLAY_COUNT;i++)
			{	
				pDesk->sUserData.CopyGCPData(i,pDesk->sGameData.T_HuPai.m_UserGCPData[i]);
				pDesk->SendGameDataEx(i, &pDesk->sGameData.T_HuPai, sizeof(pDesk->sGameData.T_HuPai), MDM_GM_GAME_NOTIFY, THING_HU_PAI, 0);
				pDesk->SendWatchDataEx(i, &pDesk->sGameData.T_HuPai, sizeof(pDesk->sGameData.T_HuPai), MDM_GM_GAME_NOTIFY, THING_HU_PAI, 0);
			}
		}
		break;
	case ACTION_NO:
		{
			if(pDesk->sGameData.m_byThingDoing == THING_OUT_PAI)//出牌事件
			{
				NextID = THING_ZHUA_PAI;//执行抓牌事件
				StarTime = 2;
			}
		}
		break;
	}
	pDesk->sGameData.ApplyThingID = NextID;
	ApplyAction(NextID,StarTime*100);//申请下一个事件

}

///检查是否可以进行下一步处理
bool ThingHandle::IsCanHandle()
{
	int num = 0, re = 0;
	BYTE max = ACTION_NO;
	BYTE station = pDesk->sGameData.T_OutPai.byUser;

	if(pDesk->sGameData.m_byThingDoing == THING_GANG_PAI)//杠牌事件
	{//考虑抢杠胡等操作
		station = pDesk->sGameData.T_GangPai.byUser;
		for(int i=0;i<4;++i)//查找出最大事件
		{
			if(i==station || pDesk->sGameData.T_CPGNotify[i].m_byMaxThing <= max || pDesk->sGameData.T_CPGNotify[i].m_byMaxThing==255)
				continue;
			max = pDesk->sGameData.T_CPGNotify[i].m_byMaxThing;
		}
		if(max == ACTION_NO)
		{
			pDesk->sGameData.m_byNextAction = ACTION_BU_GANG;//继续执行完补杠
			return true;
		}
		//查找是否有玩家响应
		for(int i=0;i<PLAY_COUNT;++i)//最大事件个数，和最大事件回应的个数
		{
			if(i==station)
				continue;
			if(max == pDesk->sGameData.T_CPGNotify[i].m_byMaxThing && pDesk->sGameData.T_CPGNotify[i].m_byMaxThing!=255)
			{
				num++;
				if(pDesk->sGameData.T_CPGNotify[i].bHaveReturn)
					re++;
			}
		}
		if(num == re)//都回应了
		{
			pDesk->sGameData.m_byNextAction = max;
			return true;
		}
	}
	else//一般出牌检查
	{
		for(int i=0;i<PLAY_COUNT;++i)//查找出最大事件
		{
			if(i==station || pDesk->sGameData.T_CPGNotify[i].m_byMaxThing <= max || pDesk->sGameData.T_CPGNotify[i].m_byMaxThing==255)
				continue;
			max = pDesk->sGameData.T_CPGNotify[i].m_byMaxThing;
		}
		if(max == ACTION_NO)
		{
			pDesk->sGameData.m_byNextAction = ACTION_NO;
			return true;
		}
		for(int i=0;i<PLAY_COUNT;++i)//最大事件个数，和最大事件回应的个数
		{
			if(i==station)
				continue;
			if(max == pDesk->sGameData.T_CPGNotify[i].m_byMaxThing && pDesk->sGameData.T_CPGNotify[i].m_byMaxThing!=255)
			{
				num++;
				if(pDesk->sGameData.T_CPGNotify[i].bHaveReturn)
					re++;
			}
		}
		if(num == re)//都回应了
		{
			pDesk->sGameData.m_byNextAction = max;
			return true;
		}
	}
	return false;
}


BYTE ThingHandle::GetCanHuUser(BYTE outst,bool hu[],BYTE hust)//获取离糊牌玩家近的玩家
{
	BYTE station = outst;
	for(int i=0;i<PLAY_COUNT-1;++i)
	{
		station = GetNextStation(station);
		if(hu[station])
			return station;
	}
	return hust;
}

///获取下家位置
BYTE ThingHandle::GetNextStation(BYTE station,bool shun)
{
	BYTE re = 255;
	if(shun)//顺时针
	{
		re = (station + 1)%PLAY_COUNT;
	}
	else
	{
		re = (station + 3)%PLAY_COUNT;
	}
	return re;
}

///清理临时数据
void ThingHandle::ClearTempData()
{
	//吃牌牌事件,临时数据
	pDesk->sGameData.temp_ChiPai.Clear();
	//碰牌牌事件,临时数据
	pDesk->sGameData.temp_PengPai.Clear();
	//杠牌牌事件,临时数据
	pDesk->sGameData.temp_GangPai.Clear();	
	//清理杠牌数据
	pDesk->sGameData.T_GangPai.Clear();	
}

///////////流程事件处理///////////////////////////////////////////////////////////////////////////////////////


//请求事件,type要申请的事件
void ThingHandle::ApplyAction(int type,int time)
{
	char strbug[200];
	wsprintf(strbug,TEXT("麻将-事件：服务器 事件申请  SetTimer type =%d  time=%d ApplyThingID=%d"),type,time,pDesk->sGameData.ApplyThingID);
	OutputDebugString(strbug);

	if(pDesk->sGameData.ApplyThingID != 255)
	{
		pDesk->SetTimer(10,time);
	}
}

///名称：GameCPU
///描述：游戏事件处理中心，
///@param type要处理的事件代号
///@return 
void ThingHandle::GameCPU(int type)
{
	//设置等待色子动画结束状态
	memset(pDesk->sUserData.m_bSiziFinish,0,sizeof(pDesk->sUserData.m_bSiziFinish));
	pDesk->sUserData.m_bWaitSezi = true;


	BYTE nextID = 255,time = 255;
	switch(type)
	{
	case THING_BEGIN://游戏开始事件
		{
			ExecuteGameBegin(nextID,time);
		}
		break;
	case THING_DONG_NT://首局以东为庄事件
		{
			ExecuteDongNt(nextID,time);
		}
		break;
	case THING_2SEZI_NT	://首局掷2颗色子的点数和为庄事件
		{
			ExecuteTwoSeziNt(nextID,time);
		}
		break;
	case THING_2SEZI_NT_GP://首局掷2颗色子定庄家和起牌点事件
		{
			ExecuteTwoSeziNtAndDir(nextID,time);
		}
		break;
	case THING_TURN_2SEZI_NT://首局轮流掷2颗色子定庄家事件
		{
			ExecuteTurnSeziNt(nextID,time);
		}
		break;
	case THING_2SEZI_DIR://掷2颗色子事件定起牌方向事件
		{
			ExecuteTwoSeziDir(nextID,time);
		}
		break;
	case THING_2SEZI_DIR_GP	://掷2颗色子事件定起牌方向和起牌点事件
		{
			ExecuteTwoSeziDirGp(nextID,time);
		}
		break;
	case THING_2SEZI_GP	://掷2颗色子事件定起牌点事件
		{
			ExecuteTwoSeziGp(nextID,time);
		}
		break;
	case THING_2SEZI_NT_DIR_GP://掷色子确定庄家方向顿数
		{
			ExecuteTwoSeziNtDirGp(nextID,time);
		}
		break;
	case THING_SEND_PAI://发牌事件
		{
			ExecuteSendPai(nextID,time);
		}
		break;
	case THING_2SEZI_JING://掷2颗色子定精牌事件
		{
			ExecuteTwoSeziMakeJing(nextID,time);
		}
		break;
	case THING_ALL_BUHUA://为所有玩家补花事件
		{
			OutputDebugString("麻将：服务器  为所有玩家补花事件");
			ExecuteAllBuHua(nextID,time);
		}
		break;
	case THING_ONE_BUHUA://单个玩家抓到花牌后补花事件
		{
			OutputDebugString("麻将：服务器  单个玩家补花事件");
			ExecuteOneBuHua(nextID,time);
		}
		break;
	case THING_BEGIN_OUT://庄家开始发牌通知事件
		{
			pDesk->m_bGameStation = GS_PLAY_GAME;
			ExecuteBeginOut(nextID,time);
		}
		break;
	case THING_ZHUA_PAI	://抓牌事件
		{	
			OutputDebugString("麻将-事件：服务器  THING_ZHUA_PAI 抓牌事件");
			ExecuteZhuaPai(nextID,time);
		}
		break;
	case THING_COUNT_FEN://算分事件
		{
			OutputDebugString("麻将：服务器  算分事件");
			ExecuteCountFen(nextID,time);
		}
		break;
	case THING_ENG_HANDLE://结束处理事件
		{
			OutputDebugString("new:服务器  抓牌事件 结束处理事件");
			ExecuteNormalEnd(nextID,time);
		}
		break;
	case THING_ENG_UN_NORMAL://异常结束处理事件
		{
			OutputDebugString("麻将：服务器  异常结束处理事件");
			ExecuteUnNormalEnd(nextID,time);
		}
		break;
	}
	pDesk->sGameData.ApplyThingID = nextID;
	ApplyAction(nextID,time*100);//申请下一个事件

}


///开始事件处理
void ThingHandle::ExecuteGameBegin(BYTE &nextID,BYTE &time)
{
	pDesk->KillTimer(TIME_WAIT_MSG);
	pDesk->SetTimer(TIME_WAIT_MSG,1000);//重新设置超时计时器

	nextID = 255;
	time = 10;
	pDesk->sGameData.m_byThingDoing = THING_BEGIN;
	pDesk->sGameData.T_Begin.byDo = 1;
	pDesk->sGameData.InitData();//初始化数据
	pDesk->sUserData.InitData();
	pDesk->sGameData.SetThingNext();//设置后接事件
	pDesk->sGameData.m_byThingDoing = pDesk->sGameData.T_Begin.byNo;//记录正在发生事件

	if(pDesk->sUserData.m_byNtStation!=255)
	{//已经有了庄家
		nextID = THING_2SEZI_DIR_GP;//掷色子发牌
	}
	else 
	{
		pDesk->sUserData.m_byNtStation = 0;
		nextID = THING_DONG_NT;//执行以东为庄事件
	}
	pDesk->sGameData.T_Begin.byNt = pDesk->sUserData.m_byNtStation;			// 庄家位置				
	pDesk->sGameData.T_Begin.byQuanFeng =  pDesk->sUserData.m_byQuanFeng;	// 圈风设置			
	memset(pDesk->sGameData.T_Begin.byMenPai,255,sizeof(pDesk->sGameData.T_Begin.byMenPai));

	for(int i=0;i<4;i++)
	{
		for(int j=0;j<pDesk->sGameData.m_mjRule.byAllMjCount/4;j++)
		{
			pDesk->sGameData.T_Begin.byMenPai[i][j] = 0; //门牌
		}
	}

	pDesk->sGameData.T_Begin.byDo = 10;
	pDesk->sUserData.m_MenPai.byGetPaiDir = pDesk->sUserData.m_byNtStation;
	///当前抓牌方向，true 顺时针，false 逆时针
	pDesk->sUserData.m_bZhuaPaiDir = true;

	pDesk->sUserData.m_MenPai.byGetPaiDir = pDesk->sUserData.m_byNtStation;//起牌方向(每局初始化为庄家门前起牌)


	//发送开始消息
	for (int i=0;i<PLAY_COUNT;i++)
	{
		pDesk->SendGameDataEx(i,&pDesk->sGameData.T_Begin,sizeof(pDesk->sGameData.T_Begin),MDM_GM_GAME_NOTIFY,THING_BEGIN,0);
		pDesk->SendWatchDataEx(i,&pDesk->sGameData.T_Begin,sizeof(pDesk->sGameData.T_Begin),MDM_GM_GAME_NOTIFY,THING_BEGIN,0);
	}
}

///以第一个玩家为庄家（以东为庄事件）
void ThingHandle::ExecuteDongNt(BYTE &nextID,BYTE &time)
{
	pDesk->sGameData.m_byThingDoing = THING_DONG_NT;
	nextID = THING_2SEZI_DIR_GP;
	time = 2;	//200毫秒间隔
	pDesk->sUserData.m_byNtStation = 0;
	pDesk->sGameData.T_DongNt.byNt = pDesk->sUserData.m_byNtStation;			// 庄家位置	
	pDesk->sUserData.m_MenPai.byGetPaiDir = pDesk->sUserData.m_byNtStation;

	pDesk->sGameData.ApplyThingID = nextID;
	ApplyAction(nextID,200);//申请下一个事件
	pDesk->sUserData.m_MenPai.byGetPaiDir = pDesk->sUserData.m_byNtStation;//起牌方向(每局初始化为庄家门前起牌)

	for (int i=0;i<PLAY_COUNT;i++)
	{
		pDesk->SendGameDataEx(i,&pDesk->sGameData.T_DongNt,sizeof(pDesk->sGameData.T_DongNt),MDM_GM_GAME_NOTIFY,THING_DONG_NT,0);
		pDesk->SendWatchDataEx(i,&pDesk->sGameData.T_DongNt,sizeof(pDesk->sGameData.T_DongNt),MDM_GM_GAME_NOTIFY,THING_DONG_NT,0);
	}
}

///掷2颗色子确定庄家（点数和）
void ThingHandle::ExecuteTwoSeziNt(BYTE &nextID,BYTE &time)	
{
	pDesk->sGameData.m_byThingDoing = THING_2SEZI_NT;
	nextID = THING_2SEZI_DIR_GP;
	time = 2;	//200毫秒间隔
	///掷2颗色子的点数和为庄事件
	pDesk->sGameData.T_TwoSeziNt.byUser = 0;//庄家位置
	pDesk->sGameData.T_TwoSeziNt.byDo = 10;//事件结束
	pDesk->sUserData.m_MenPai.byGetPaiDir = pDesk->sUserData.m_byNtStation;
	if(pDesk->sUserData.m_byNtStation!=255)
	{//已经有庄家了就不掷色子了
		return;
	}
	time = 50;	//5秒间隔
	CLogicBase::GetSeziData(&pDesk->sGameData.T_TwoSeziNt.bySezi0,&pDesk->sGameData.T_TwoSeziNt.bySezi1);
	pDesk->sUserData.m_byNtStation = (pDesk->sGameData.T_TwoSeziNt.bySezi0 + pDesk->sGameData.T_TwoSeziNt.bySezi1)%4;

	pDesk->sGameData.T_TwoSeziNt.byNt = pDesk->sUserData.m_byNtStation;
	pDesk->sUserData.m_MenPai.byGetPaiDir = pDesk->sUserData.m_byNtStation;

	pDesk->sGameData.ApplyThingID = nextID;

	ApplyAction(nextID,5000);//申请下一个事件
	pDesk->sUserData.m_MenPai.byGetPaiDir = pDesk->sUserData.m_byNtStation;//起牌方向(每局初始化为庄家门前起牌)
	for (int i=0;i<PLAY_COUNT;i++)
	{
		pDesk->SendGameDataEx(i, &pDesk->sGameData.T_TwoSeziNt, sizeof(pDesk->sGameData.T_TwoSeziNt), MDM_GM_GAME_NOTIFY, THING_2SEZI_NT, 0);
	}
	pDesk->SendWatchDataEx(PLAY_COUNT, &pDesk->sGameData.T_TwoSeziNt, sizeof(pDesk->sGameData.T_TwoSeziNt), MDM_GM_GAME_NOTIFY, THING_2SEZI_NT, 0);
	//设置等待色子动画结束状态
	memset(pDesk->sUserData.m_bSiziFinish,0,sizeof(pDesk->sUserData.m_bSiziFinish));
	pDesk->sUserData.m_bWaitSezi = true;
}

///掷2颗色子确定庄家和起牌方向（点数和庄家，大点方向,未使用）
void ThingHandle::ExecuteTwoSeziNtAndDir(BYTE &nextID,BYTE &time)
{

}

///轮流掷色子确定庄家
void ThingHandle::ExecuteTurnSeziNt(BYTE &nextID,BYTE &time)	
{
	pDesk->sGameData.m_byThingDoing = THING_TURN_2SEZI_NT;
	///轮流掷2颗色子确定庄家
	CLogicBase::GetSeziData(&pDesk->sGameData.T_TurnSeziNt.bySezi0,&pDesk->sGameData.T_TurnSeziNt.bySezi1);

	nextID = THING_2SEZI_DIR_GP;
	pDesk->sGameData.ApplyThingID = nextID;
	ApplyAction(nextID,5000);//申请下一个事件
	pDesk->sUserData.m_MenPai.byGetPaiDir = pDesk->sUserData.m_byNtStation;//起牌方向(每局初始化为庄家门前起牌)
	for (int i=0;i<PLAY_COUNT;i++)
	{
		pDesk->SendGameDataEx(i, &pDesk->sGameData.T_TurnSeziNt, sizeof(pDesk->sGameData.T_TurnSeziNt), MDM_GM_GAME_NOTIFY, THING_TURN_2SEZI_NT, 0);
	}
	pDesk->SendWatchDataEx(PLAY_COUNT, &pDesk->sGameData.T_TurnSeziNt, sizeof(pDesk->sGameData.T_TurnSeziNt), MDM_GM_GAME_NOTIFY, THING_TURN_2SEZI_NT, 0);	
	//设置等待色子动画结束状态
	memset(pDesk->sUserData.m_bSiziFinish,0,sizeof(pDesk->sUserData.m_bSiziFinish));
	pDesk->sUserData.m_bWaitSezi = true;
}

///掷2颗色子确定庄家+起牌方向+起牌敦数（点数和庄家，大点方向，小点敦数）
void ThingHandle::ExecuteTwoSeziNtDirGp(BYTE &nextID,BYTE &time)	
{
	pDesk->sGameData.m_byThingDoing = THING_2SEZI_NT_GP;
	///掷2颗色子确定庄家和起牌点位置事件
	nextID = THING_SEND_PAI;

	pDesk->sGameData.T_TwoSeziNtAndGetPai.byUser = 0;
	CLogicBase::GetSeziData(&pDesk->sGameData.T_TwoSeziNtAndGetPai.bySezi0,&pDesk->sGameData.T_TwoSeziNtAndGetPai.bySezi1);
	if(pDesk->sUserData.m_byNtStation!=255)
	{//已经有了庄家
		pDesk->sGameData.T_TwoSeziNtAndGetPai.byUser = pDesk->sUserData.m_byNtStation;
	}
	else
	{
		pDesk->sUserData.m_byNtStation = (pDesk->sGameData.T_TwoSeziNtAndGetPai.bySezi0 + pDesk->sGameData.T_TwoSeziNtAndGetPai.bySezi1)%4;
	}
	pDesk->sGameData.T_TwoSeziNtAndGetPai.byNt = pDesk->sUserData.m_byNtStation;
	pDesk->sGameData.T_TwoSeziNtAndGetPai.byGetPai = pDesk->sGameData.T_TwoSeziNtAndGetPai.bySezi0;
	pDesk->sUserData.m_MenPai.byGetPaiDir = pDesk->sGameData.T_TwoSeziNtAndGetPai.bySezi1%4;
	if(pDesk->sGameData.T_TwoSeziNtAndGetPai.bySezi0>pDesk->sGameData.T_TwoSeziNtAndGetPai.bySezi1)
	{
		pDesk->sGameData.T_TwoSeziNtAndGetPai.byGetPai = pDesk->sGameData.T_TwoSeziNtAndGetPai.bySezi1;
		pDesk->sUserData.m_MenPai.byGetPaiDir = pDesk->sGameData.T_TwoSeziNtAndGetPai.bySezi0%4;
	}

	pDesk->sGameData.ApplyThingID = nextID;
	ApplyAction(nextID,5000);//申请下一个事件
	pDesk->sUserData.m_MenPai.byGetPaiDir = pDesk->sUserData.m_byNtStation;//起牌方向(每局初始化为庄家门前起牌)
	for (int i=0;i<PLAY_COUNT;i++)
	{
		pDesk->SendGameDataEx(i, &pDesk->sGameData.T_TwoSeziNtAndGetPai, sizeof(pDesk->sGameData.T_TwoSeziNtAndGetPai), MDM_GM_GAME_NOTIFY, THING_2SEZI_NT, 0);
	}
	pDesk->SendWatchDataEx(PLAY_COUNT, &pDesk->sGameData.T_TwoSeziNtAndGetPai, sizeof(pDesk->sGameData.T_TwoSeziNtAndGetPai), MDM_GM_GAME_NOTIFY, THING_2SEZI_NT, 0);	
	//设置等待色子动画结束状态
	memset(pDesk->sUserData.m_bSiziFinish,0,sizeof(pDesk->sUserData.m_bSiziFinish));
	pDesk->sUserData.m_bWaitSezi = true;
}

///掷2颗色子确定起牌方向（点数和确定方向）
void ThingHandle::ExecuteTwoSeziDir(BYTE &nextID,BYTE &time)	
{
	pDesk->sGameData.m_byThingDoing = THING_2SEZI_DIR;
	///掷2颗色子确定起牌位置事件
	nextID = THING_2SEZI_GP;//准备执行掷色子确定棋牌顿数事件

	pDesk->sGameData.T_TwoSeziDir.byUser = pDesk->sUserData.m_byNtStation;
	pDesk->sGameData.T_TwoSeziDir.byNt = pDesk->sUserData.m_byNtStation;
	CLogicBase::GetSeziData(&pDesk->sGameData.T_TwoSeziDir.bySezi0,&pDesk->sGameData.T_TwoSeziDir.bySezi1);
	pDesk->sUserData.m_MenPai.byGetPaiDir = (pDesk->sGameData.T_TwoSeziDir.bySezi1 + pDesk->sGameData.T_TwoSeziDir.bySezi0)%4;
	pDesk->sGameData.T_TwoSeziDir.byDirection = pDesk->sUserData.m_MenPai.byGetPaiDir;

	pDesk->sGameData.ApplyThingID = nextID;
	ApplyAction(nextID,5000);//申请下一个事件
	for (int i=0;i<PLAY_COUNT;i++)
	{
		pDesk->SendGameDataEx(i, &pDesk->sGameData.T_TwoSeziDir, sizeof(pDesk->sGameData.T_TwoSeziDir), MDM_GM_GAME_NOTIFY, THING_2SEZI_DIR, 0);
	}
	pDesk->SendWatchDataEx(PLAY_COUNT, &pDesk->sGameData.T_TwoSeziDir, sizeof(pDesk->sGameData.T_TwoSeziDir), MDM_GM_GAME_NOTIFY, THING_2SEZI_DIR, 0);
	//设置等待色子动画结束状态
	memset(pDesk->sUserData.m_bSiziFinish,0,sizeof(pDesk->sUserData.m_bSiziFinish));
	pDesk->sUserData.m_bWaitSezi = true;
}

///掷2颗色子确定起牌方向和起牌敦数（点数和方向，小点敦数）
void ThingHandle::ExecuteTwoSeziDirGp(BYTE &nextID,BYTE &time)	
{
	pDesk->sGameData.m_byThingDoing = THING_2SEZI_DIR_GP;
	///掷2颗色子确定起牌位置（点数和）和起牌点（最小点）事件	
	nextID = THING_SEND_PAI;
	pDesk->sGameData.T_TwoSeziDirAndGetPai.byUser = pDesk->sUserData.m_byNtStation;
	CLogicBase::GetSeziData(&pDesk->sGameData.T_TwoSeziDirAndGetPai.bySezi0,&pDesk->sGameData.T_TwoSeziDirAndGetPai.bySezi1);
	pDesk->sGameData.T_TwoSeziDirAndGetPai.bySezi0 = pDesk->sGameData.T_TwoSeziDirAndGetPai.bySezi0%6+1;
	pDesk->sGameData.T_TwoSeziDirAndGetPai.bySezi1 = pDesk->sGameData.T_TwoSeziDirAndGetPai.bySezi1%6+1;
	//起牌方向
	pDesk->sUserData.m_MenPai.byGetPaiDir = (pDesk->sGameData.T_TwoSeziDirAndGetPai.bySezi1 + pDesk->sGameData.T_TwoSeziDirAndGetPai.bySezi0)%4-1;
	//起牌点数
	pDesk->sUserData.m_MenPai.byGetPai = pDesk->sGameData.T_TwoSeziDirAndGetPai.bySezi0-1;
	if(pDesk->sGameData.T_TwoSeziDirAndGetPai.bySezi0 > pDesk->sGameData.T_TwoSeziDirAndGetPai.bySezi1)
	{
		pDesk->sUserData.m_MenPai.byGetPai = pDesk->sGameData.T_TwoSeziDirAndGetPai.bySezi1-1;
	}
	pDesk->sGameData.T_TwoSeziDirAndGetPai.byGetPai = pDesk->sUserData.m_MenPai.byGetPai;

	pDesk->sGameData.ApplyThingID = nextID;
	ApplyAction(nextID,5000);//申请下一个事件
	for (int i=0;i<PLAY_COUNT;i++)
	{
		pDesk->SendGameDataEx(i, &pDesk->sGameData.T_TwoSeziDirAndGetPai, sizeof(pDesk->sGameData.T_TwoSeziDirAndGetPai), MDM_GM_GAME_NOTIFY, THING_2SEZI_DIR_GP, 0);
	}
	pDesk->SendWatchDataEx(PLAY_COUNT, &pDesk->sGameData.T_TwoSeziDirAndGetPai, sizeof(pDesk->sGameData.T_TwoSeziDirAndGetPai), MDM_GM_GAME_NOTIFY, THING_2SEZI_DIR_GP, 0);
	//设置等待色子动画结束状态
	memset(pDesk->sUserData.m_bSiziFinish,0,sizeof(pDesk->sUserData.m_bSiziFinish));
	pDesk->sUserData.m_bWaitSezi = true;
}

///掷2颗色子确定起牌敦数（点数和敦数）
void ThingHandle::ExecuteTwoSeziGp(BYTE &nextID,BYTE &time)	
{
	pDesk->sGameData.m_byThingDoing = THING_2SEZI_GP;
	///掷2颗色子确定起牌敦数事件
	nextID = THING_SEND_PAI	;//准备执行发牌事件		
	pDesk->sGameData.T_TwoSeziGetPai.byUser = pDesk->sUserData.m_byNtStation;
	CLogicBase::GetSeziData(&pDesk->sGameData.T_TwoSeziGetPai.bySezi0,&pDesk->sGameData.T_TwoSeziGetPai.bySezi1);
	pDesk->sUserData.m_MenPai.byGetPai = (pDesk->sGameData.T_TwoSeziGetPai.bySezi1 + pDesk->sGameData.T_TwoSeziGetPai.bySezi0)%12;
	pDesk->sGameData.T_TwoSeziGetPai.byGetPai = pDesk->sUserData.m_MenPai.byGetPai;

	pDesk->sGameData.ApplyThingID = nextID;
	ApplyAction(nextID,5000);//申请下一个事件
	for (int i=0;i<PLAY_COUNT;i++)
	{
		pDesk->SendGameDataEx(i, &pDesk->sGameData.T_TwoSeziGetPai, sizeof(pDesk->sGameData.T_TwoSeziGetPai), MDM_GM_GAME_NOTIFY, THING_2SEZI_GP, 0);
	}
	pDesk->SendWatchDataEx(PLAY_COUNT, &pDesk->sGameData.T_TwoSeziGetPai, sizeof(pDesk->sGameData.T_TwoSeziGetPai), MDM_GM_GAME_NOTIFY, THING_2SEZI_GP, 0);
	//设置等待色子动画结束状态
	memset(pDesk->sUserData.m_bSiziFinish,0,sizeof(pDesk->sUserData.m_bSiziFinish));
	pDesk->sUserData.m_bWaitSezi = true;
}

///加注事件（确定庄家后，可以通过选择加注增加输赢）
void ThingHandle::ExecuteAddNote(BYTE &nextID,BYTE &time)	
{
}

///发牌
void ThingHandle::ExecuteSendPai(BYTE &nextID,BYTE &time)	
{
	///当前抓牌方向，true 顺时针，false 逆时针
	pDesk->sUserData.m_bZhuaPaiDir = true;
	pDesk->sGameData.m_byThingDoing = THING_SEND_PAI;
	///发牌事件
	pDesk->sGameData.T_SendPai.byDo = 1;	//
	nextID = THING_BEGIN_OUT;//下一个事件的代号：开始出牌
	time = 5;
	//打乱牌
	DisPatchCard();

	for(int i=0;i<PLAY_COUNT;i++)
	{
		for(int j=0;j<MAX_HAND_PAI-1;++j)
		{
			GetPai(i,true,1);
		}
	}

	//庄家多抓一张牌
	BYTE card = 255;
	card=GetPai(pDesk->sUserData.m_byNtStation,true,1);
	//分配门牌
	ChangeMenPai();
	for(int i=0;i<PLAY_COUNT;++i)
	{
		pDesk->sUserData.CopyMenPai(i,pDesk->sGameData.T_SendPai.byMenPai[i]);//拷贝门牌
		pDesk->sUserData.SortHandPai(i,false);
	}

	//测试设置庄家的牌，测试天糊
#ifdef MJ_CAN_SETPAI

    CString s = CINIFile::GetAppPath ();/////本地路径    
	CINIFile f(s +_T("\\")+SKIN_FOLDER  + _T("_s.ini"));

	pDesk->sUserData.m_byArHandPai[pDesk->sUserData.m_byNtStation][0] = f.GetKeyVal("test","pai1",pDesk->sUserData.m_byArHandPai[pDesk->sUserData.m_byNtStation][0]);
	pDesk->sUserData.m_byArHandPai[pDesk->sUserData.m_byNtStation][1] = f.GetKeyVal("test","pai2",pDesk->sUserData.m_byArHandPai[pDesk->sUserData.m_byNtStation][1]);
	pDesk->sUserData.m_byArHandPai[pDesk->sUserData.m_byNtStation][2] = f.GetKeyVal("test","pai3",pDesk->sUserData.m_byArHandPai[pDesk->sUserData.m_byNtStation][2]);
	pDesk->sUserData.m_byArHandPai[pDesk->sUserData.m_byNtStation][3] = f.GetKeyVal("test","pai4",pDesk->sUserData.m_byArHandPai[pDesk->sUserData.m_byNtStation][3]);
	pDesk->sUserData.m_byArHandPai[pDesk->sUserData.m_byNtStation][4] = f.GetKeyVal("test","pai5",pDesk->sUserData.m_byArHandPai[pDesk->sUserData.m_byNtStation][4]);
	pDesk->sUserData.m_byArHandPai[pDesk->sUserData.m_byNtStation][5] = f.GetKeyVal("test","pai6",pDesk->sUserData.m_byArHandPai[pDesk->sUserData.m_byNtStation][5]);
	pDesk->sUserData.m_byArHandPai[pDesk->sUserData.m_byNtStation][6] = f.GetKeyVal("test","pai7",pDesk->sUserData.m_byArHandPai[pDesk->sUserData.m_byNtStation][6]);
	pDesk->sUserData.m_byArHandPai[pDesk->sUserData.m_byNtStation][7] = f.GetKeyVal("test","pai8",pDesk->sUserData.m_byArHandPai[pDesk->sUserData.m_byNtStation][7]);
	pDesk->sUserData.m_byArHandPai[pDesk->sUserData.m_byNtStation][8] = f.GetKeyVal("test","pai9",pDesk->sUserData.m_byArHandPai[pDesk->sUserData.m_byNtStation][8]);
	pDesk->sUserData.m_byArHandPai[pDesk->sUserData.m_byNtStation][9] = f.GetKeyVal("test","pai10",pDesk->sUserData.m_byArHandPai[pDesk->sUserData.m_byNtStation][9]);
	pDesk->sUserData.m_byArHandPai[pDesk->sUserData.m_byNtStation][10] = f.GetKeyVal("test","pai11",pDesk->sUserData.m_byArHandPai[pDesk->sUserData.m_byNtStation][10]);
	pDesk->sUserData.m_byArHandPai[pDesk->sUserData.m_byNtStation][11] = f.GetKeyVal("test","pai12",pDesk->sUserData.m_byArHandPai[pDesk->sUserData.m_byNtStation][11]);
	pDesk->sUserData.m_byArHandPai[pDesk->sUserData.m_byNtStation][12] = f.GetKeyVal("test","pai13",pDesk->sUserData.m_byArHandPai[pDesk->sUserData.m_byNtStation][12]);
	pDesk->sUserData.m_byArHandPai[pDesk->sUserData.m_byNtStation][13] = f.GetKeyVal("test","pai14",pDesk->sUserData.m_byArHandPai[pDesk->sUserData.m_byNtStation][13]);

	OutputDebugString("血战麻将：服务器 ExecuteSendPai ");

#endif //

	for(int i=0;i<PLAY_COUNT;i++)
	{	
		memset(pDesk->sGameData.T_SendPai.m_byArHandPai,255,sizeof(pDesk->sGameData.T_SendPai.m_byArHandPai));
		pDesk->sUserData.CopyHandPai(pDesk->sGameData.T_SendPai.m_byArHandPai,i,false);
		pDesk->SendGameDataEx(i, &pDesk->sGameData.T_SendPai, sizeof(pDesk->sGameData.T_SendPai), MDM_GM_GAME_NOTIFY, THING_SEND_PAI, 0);
		pDesk->SendWatchDataEx(i, &pDesk->sGameData.T_SendPai, sizeof(pDesk->sGameData.T_SendPai), MDM_GM_GAME_NOTIFY, THING_SEND_PAI, 0);
	}
	pDesk->sGameData.ApplyThingID = nextID;
	//ApplyAction(nextID,500);//申请下一个事件



}

///掷2颗色子定精事件
void ThingHandle::ExecuteTwoSeziMakeJing(BYTE &nextID,BYTE &time)	
{
	pDesk->sGameData.m_byThingDoing = THING_2SEZI_JING;
	///掷色子2颗色子定精牌事件
	nextID = THING_SEND_PAI;
	pDesk->sGameData.T_TwoSeziJing.byUser = pDesk->sUserData.m_byNtStation;
	CLogicBase::GetSeziData(&pDesk->sGameData.T_TwoSeziJing.bySezi0,&pDesk->sGameData.T_TwoSeziJing.bySezi1);

	int num = pDesk->sGameData.T_TwoSeziJing.bySezi0 + pDesk->sGameData.T_TwoSeziJing.bySezi1;
	num = num%13;//两颗色子值不能超过12
	if(num <0 )
	{
		num = -num;
	}
	num = (2*num +1);
	if(pDesk->sUserData.m_MenPai.byEndIndex>=num)
	{
		pDesk->sUserData.m_MenPai.byCaiShenIndex = pDesk->sUserData.m_MenPai.byEndIndex -num;
	}
	else
	{
		pDesk->sUserData.m_MenPai.byCaiShenIndex = pDesk->sUserData.m_MenPai.byAllPaiNum + pDesk->sUserData.m_MenPai.byEndIndex + 1 - num;
	}
	pDesk->sUserData.m_StrJing.Init();
	if(pDesk->sUserData.m_StrJing.AddJing(pDesk->sUserData.m_MenPai.byMenPai[pDesk->sUserData.m_MenPai.byCaiShenIndex]))
	{
	}

	if(pDesk->sUserData.m_MenPai.byMenPai[pDesk->sUserData.m_MenPai.byCaiShenIndex] == 255)//
	{
		pDesk->sUserData.m_MenPai.byCaiShenIndex = pDesk->sUserData.m_MenPai.byEndIndex;
		pDesk->sUserData.m_StrJing.Init();
		pDesk->sUserData.m_StrJing.AddJing(pDesk->sUserData.m_MenPai.byMenPai[pDesk->sUserData.m_MenPai.byCaiShenIndex]);
	}
	//删除反开的财神牌
	pDesk->sUserData.m_MenPai.byMenPai[pDesk->sUserData.m_MenPai.byCaiShenIndex] = 255;

#ifdef MJ_CAN_SETPAI
    CString s = CINIFile::GetAppPath ();/////本地路径    
	CINIFile f(s +_T("\\")+SKIN_FOLDER  + _T("_s.ini"));
	pDesk->sUserData.m_StrJing.byPai[0] = f.GetKeyVal("test","baopai",pDesk->sUserData.m_StrJing.byPai[0]);
	
#endif //MJ_CAN_SETPAI
	pDesk->sGameData.T_TwoSeziJing.m_StrJing = pDesk->sUserData.m_StrJing;

	pDesk->sGameData.ApplyThingID = nextID;
	ApplyAction(nextID,5000);//申请下一个事件
	for (int i=0;i<PLAY_COUNT;i++)
	{
		pDesk->SendGameDataEx(i, &pDesk->sGameData.T_TwoSeziJing, sizeof(pDesk->sGameData.T_TwoSeziJing), MDM_GM_GAME_NOTIFY, THING_2SEZI_JING, 0);
	}
	pDesk->SendWatchDataEx(PLAY_COUNT, &pDesk->sGameData.T_TwoSeziJing, sizeof(pDesk->sGameData.T_TwoSeziJing), MDM_GM_GAME_NOTIFY, THING_2SEZI_JING, 0);
	//设置等待色子动画结束状态
	memset(pDesk->sUserData.m_bSiziFinish,0,sizeof(pDesk->sUserData.m_bSiziFinish));
	pDesk->sUserData.m_bWaitSezi = true;
}

///所有玩家补花事件
void ThingHandle::ExecuteAllBuHua(BYTE &nextID,BYTE &time)	
{
	pDesk->sGameData.m_byThingDoing = THING_ALL_BUHUA;
	///所有玩家补花事件
	nextID = THING_BEGIN_OUT;
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(!pDesk->sUserData.IsHuaPaiInHand(i))
			continue;
		while(pDesk->sUserData.IsHuaPaiInHand(i))
		{
			int num = pDesk->sUserData.MoveHuaPaiFormHand(i);
			for(int j=0;j<num;j++)
			{
				GetPai(i,false,1);//开始前补花抓牌
			}
		}
	}
	ApplyAction(nextID,500);//申请下一个事件
	///发送牌数据
	for (int i=0;i<PLAY_COUNT;i++)
	{		
		memset(pDesk->sGameData.T_AllBuHua.m_byArHandPai,255,sizeof(pDesk->sGameData.T_AllBuHua.m_byArHandPai));
		pDesk->sUserData.CopyHandPai(pDesk->sGameData.T_AllBuHua.m_byArHandPai,i,false);
		pDesk->SendGameDataEx(i, &pDesk->sGameData.T_AllBuHua, sizeof(pDesk->sGameData.T_AllBuHua), MDM_GM_GAME_NOTIFY, THING_ALL_BUHUA, 0);
		pDesk->SendWatchDataEx(i, &pDesk->sGameData.T_AllBuHua, sizeof(pDesk->sGameData.T_AllBuHua), MDM_GM_GAME_NOTIFY, THING_ALL_BUHUA, 0);
	}
	pDesk->sGameData.ApplyThingID = nextID;
	ApplyAction(nextID,1500);
}

///单个玩家补花事件
void ThingHandle::ExecuteOneBuHua(BYTE &nextID,BYTE &time)
{
	nextID = 255;
	pDesk->sGameData.m_byThingDoing = THING_ONE_BUHUA;
	///单个玩家补花事件	
	pDesk->sGameData.T_OneBuHua.byUser = pDesk->sUserData.m_byNowOutStation;
	while(pDesk->sUserData.IsHuaPaiInHand(pDesk->sUserData.m_byNowOutStation))
	{
		int num = pDesk->sUserData.MoveHuaPaiFormHand(pDesk->sUserData.m_byNowOutStation);
		for(int i=0;i<num;i++)
		{
			pDesk->sGameData.T_OneBuHua.byPs = GetPai(pDesk->sUserData.m_byNowOutStation,false,3);//正常抓牌
		}
	}
	for(int i=0;i<4;i++)
	{
		pDesk->sUserData.CopyMenPai(i,pDesk->sGameData.T_OneBuHua.byMenPai[i]);//拷贝门牌
		pDesk->sUserData.CopyHuaPai(i,pDesk->sGameData.T_OneBuHua.m_byArHuaPai[i]);//拷贝花牌
	}
	pDesk->sGameData.ApplyThingID = nextID;
	ApplyAction(nextID,200);//申请下一个事件
	///发送牌数据
	for (int i=0;i<PLAY_COUNT;i++)
	{			
		memset(pDesk->sGameData.T_OneBuHua.m_byArHandPai,255,sizeof(pDesk->sGameData.T_OneBuHua.m_byArHandPai));
		pDesk->sUserData.CopyHandPai(pDesk->sGameData.T_OneBuHua.m_byArHandPai,i,false);
		pDesk->SendGameDataEx(i, &pDesk->sGameData.T_OneBuHua, sizeof(pDesk->sGameData.T_OneBuHua), MDM_GM_GAME_NOTIFY, THING_ONE_BUHUA, 0);
		pDesk->SendWatchDataEx(i, &pDesk->sGameData.T_OneBuHua, sizeof(pDesk->sGameData.T_OneBuHua), MDM_GM_GAME_NOTIFY, THING_ONE_BUHUA, 0);
	}
}


///开始出牌通知事件
void ThingHandle::ExecuteBeginOut(BYTE &nextID,BYTE &time)
{
	pDesk->sGameData.m_byThingDoing = THING_BEGIN_OUT;
	///开始出牌通知事件
	//T_BeginOutPai;	
	pDesk->sGameData.T_BeginOutPai.byNt = pDesk->sUserData.m_byNtStation;//庄家位置
	pDesk->sGameData.T_BeginOutPai.byUser = pDesk->sUserData.m_byNtStation;//庄家位置
//	pDesk->sGameData.T_BeginOutPai.m_StrJing = pDesk->sUserData.m_StrJing;//精牌
	pDesk->sUserData.m_byNowOutStation = pDesk->sUserData.m_byNtStation;//出牌位置
	for(int i=0;i<4;i++)
	{
		pDesk->sUserData.CopyMenPai(i,pDesk->sGameData.T_BeginOutPai.byMenPai[i]);//拷贝门牌
	}
	ChangeMenPai();//转换门牌
	//检测动作
	pDesk->sGameData.T_CPGNotify[pDesk->sUserData.m_byNtStation].Clear();
	CheckAction(pDesk->sUserData.m_byNtStation,pDesk->sUserData.GetLastHandPai(pDesk->sUserData.m_byNowOutStation),0);

	//if(pDesk->sUserData.m_bFoceLeave[pDesk->sUserData.m_byNtStation] && pDesk->sGameData.m_mjRule.bNetCutTuoGuan)//如果庄家断线,而且有断线托管
	//{
	//	//NetCutAutoHandle(pDesk->sUserData.m_byNtStation);
	//}
	pDesk->sGameData.ApplyThingID = 255;
	ApplyAction(pDesk->sGameData.ApplyThingID,200);//申请下一个事件
	//发送通知
	for (int i=0;i<PLAY_COUNT;i++)
	{	
		pDesk->SendGameDataEx(i, &pDesk->sGameData.T_BeginOutPai, sizeof(pDesk->sGameData.T_BeginOutPai), MDM_GM_GAME_NOTIFY, THING_BEGIN_OUT, 0);
	}
	pDesk->SendWatchDataEx(PLAY_COUNT, &pDesk->sGameData.T_BeginOutPai, sizeof(pDesk->sGameData.T_BeginOutPai), MDM_GM_GAME_NOTIFY, THING_BEGIN_OUT, 0);

	if(!pDesk->sUserData.m_bFoceLeave[ pDesk->sGameData.T_BeginOutPai.byUser])
	{
		pDesk->sGameData.T_CPGNotify[pDesk->sUserData.m_byNtStation].bZhuaPai = true;
		pDesk->SendGameDataEx(pDesk->sUserData.m_byNtStation, &pDesk->sGameData.T_CPGNotify[pDesk->sUserData.m_byNtStation], sizeof(tagCPGNotifyEx), MDM_GM_GAME_NOTIFY, THING_CPG_NOTIFY, 0);
		pDesk->SendWatchDataEx(pDesk->sUserData.m_byNtStation, &pDesk->sGameData.T_CPGNotify[pDesk->sUserData.m_byNtStation], sizeof(tagCPGNotifyEx), MDM_GM_GAME_NOTIFY, THING_CPG_NOTIFY, 0);
	}
	else//断线处理
	{
		NetCutAutoHandle();
	}
}

///抓牌事件
void ThingHandle::ExecuteZhuaPai(BYTE &nextID,BYTE &time)	
{
	///抓牌牌事件
	nextID = 255;
	BYTE card = 255;
	if(pDesk->sUserData.m_MenPai.GetMenPaiNum()<=pDesk->sGameData.m_mjRule.byGamePassNum)
	{
		nextID = THING_ENG_HANDLE;
	}
	else
	{
		if(pDesk->sGameData.m_byThingDoing == THING_OUT_PAI)//出牌事件
		{
			pDesk->sUserData.m_byNowOutStation = GetNextStation(pDesk->sUserData.m_byNowOutStation);//抓牌位置
		}

		pDesk->sGameData.m_byThingDoing = THING_ZHUA_PAI;//记录正在发生的事件

		pDesk->sGameData.T_ZhuaPai.byUser = pDesk->sUserData.m_byNowOutStation;//庄家位置
		card = GetPai(pDesk->sUserData.m_byNowOutStation,pDesk->sUserData.m_bZhuaPaiDir,0);//正常抓牌
		pDesk->sGameData.T_ZhuaPai.byPs = card;		//抓牌的值

		if(card == 255 && ( pDesk->sUserData.m_MenPai.GetMenPaiNum()<=pDesk->sGameData.m_mjRule.byGamePassNum +2))
		{//出了问题，临时解决
			nextID = THING_ENG_HANDLE;
			return;
		}

		///如果设置了要抓的牌(发牌器版本)
		if(pDesk->sUserData.m_SetData[pDesk->sUserData.m_byNowOutStation].bySetType==2)
		{		
			pDesk->sUserData.DelleteAHandPai(pDesk->sUserData.m_byNowOutStation,card);
			card = pDesk->sUserData.m_SetData[pDesk->sUserData.m_byNowOutStation].mjdata[0];
			pDesk->sUserData.AddToHandPai(pDesk->sUserData.m_byNowOutStation,card);

			pDesk->sUserData.m_SetData[pDesk->sUserData.m_byNowOutStation].Init();
		}
		if(pDesk->sGameData.m_mjRule.bHaveFlower && pDesk->sUserData.CheckIsHuaPai(card))
		{
			//花牌补花
			nextID = THING_ONE_BUHUA;
			time = 5;
			pDesk->sGameData.T_ZhuaPai.bHead = false;		//抓牌的方向
			pDesk->sGameData.T_ZhuaPai.bCanOutPai = false;	//抓牌后是否可以出牌
		}
		else
		{
			//动作检测
			pDesk->sGameData.T_CPGNotify[pDesk->sUserData.m_byNowOutStation].Clear();
			CheckAction(pDesk->sUserData.m_byNowOutStation,card,0);
			if(pDesk->sUserData.m_bTuoGuan[pDesk->sUserData.m_byNowOutStation])//玩家托管
			{
				pDesk->sGameData.T_CPGNotify[pDesk->sUserData.m_byNowOutStation].bChi = false;
				memset(pDesk->sGameData.T_CPGNotify[pDesk->sUserData.m_byNowOutStation].m_iChi,255,sizeof(pDesk->sGameData.T_CPGNotify[pDesk->sUserData.m_byNowOutStation].m_iChi));
				pDesk->sGameData.T_CPGNotify[pDesk->sUserData.m_byNowOutStation].bPeng = false;
			}
			if(pDesk->sUserData.m_bTing[pDesk->sUserData.m_byNowOutStation] && !pDesk->sGameData.m_mjAction.bGangAfterTing)
			{
				pDesk->sGameData.T_CPGNotify[pDesk->sUserData.m_byNowOutStation].bGang = false;
				memset(pDesk->sGameData.T_CPGNotify[pDesk->sUserData.m_byNowOutStation].m_iGangData,255,sizeof(pDesk->sGameData.T_CPGNotify[pDesk->sUserData.m_byNowOutStation].m_iGangData));
			}

		}
		ChangeMenPai();//转换门牌
		for(int i=0;i<4;i++)
		{
			pDesk->sUserData.CopyMenPai(i,pDesk->sGameData.T_ZhuaPai.byMenPai[i]);//拷贝门牌
			pDesk->sUserData.CopyHuaPai(i,pDesk->sGameData.T_ZhuaPai.m_byArHuaPai[i]);//拷贝花牌
			pDesk->sUserData.CopyOutPai(i,pDesk->sGameData.T_ZhuaPai.m_byArOutPai[i]);//拷贝出牌
		}
	}

	pDesk->sGameData.ApplyThingID = nextID;
	ApplyAction(nextID,200);//申请下一个事件
	BYTE pai = pDesk->sGameData.T_ZhuaPai.byPs ;
	///发送牌数据
	for (int i=0;i<PLAY_COUNT;i++)
	{			
		pDesk->sGameData.T_ZhuaPai.byPs = 255;
		memset(pDesk->sGameData.T_ZhuaPai.m_byArHandPai,255,sizeof(pDesk->sGameData.T_ZhuaPai.m_byArHandPai));
		pDesk->sUserData.CopyHandPai(pDesk->sGameData.T_ZhuaPai.m_byArHandPai,i,false);
		if(pDesk->sGameData.T_ZhuaPai.byUser == i)
		{
			pDesk->sGameData.T_ZhuaPai.byPs = pai;
		}
		pDesk->SendGameDataEx(i, &pDesk->sGameData.T_ZhuaPai, sizeof(pDesk->sGameData.T_ZhuaPai), MDM_GM_GAME_NOTIFY, THING_ZHUA_PAI, 0);
		pDesk->SendWatchDataEx(i, &pDesk->sGameData.T_ZhuaPai, sizeof(pDesk->sGameData.T_ZhuaPai), MDM_GM_GAME_NOTIFY, THING_ZHUA_PAI, 0);
	}
	pDesk->sGameData.T_ZhuaPai.byPs = pai;
	if(nextID == 255)
	{//id=255说明即没有流局也不是补花，可以发送吃碰杠通知，如果断线就直接出牌
		if(pDesk->sGameData.m_mjRule.bNetCutTuoGuan && pDesk->sUserData.m_bFoceLeave[ pDesk->sGameData.T_ZhuaPai.byUser])
		{
			NetCutAutoHandle();
		}
		else if(!pDesk->sUserData.m_bFoceLeave[ pDesk->sGameData.T_ZhuaPai.byUser])
		{
			pDesk->sGameData.T_CPGNotify[pDesk->sGameData.T_ZhuaPai.byUser].byUser = pDesk->sGameData.T_ZhuaPai.byUser;
			pDesk->sGameData.T_CPGNotify[pDesk->sGameData.T_ZhuaPai.byUser].bZhuaPai = true;
			pDesk->SendGameDataEx(pDesk->sGameData.T_ZhuaPai.byUser, &pDesk->sGameData.T_CPGNotify[pDesk->sGameData.T_ZhuaPai.byUser], sizeof(tagCPGNotifyEx), MDM_GM_GAME_NOTIFY, THING_CPG_NOTIFY, 0);
			pDesk->SendWatchDataEx(pDesk->sGameData.T_ZhuaPai.byUser, &pDesk->sGameData.T_CPGNotify[pDesk->sGameData.T_ZhuaPai.byUser], sizeof(tagCPGNotifyEx), MDM_GM_GAME_NOTIFY, THING_CPG_NOTIFY, 0);

		}
	}
}

///算分事件
void ThingHandle::ExecuteCountFen(BYTE &nextID,BYTE &time)
{
	pDesk->sGameData.m_byThingDoing = THING_COUNT_FEN;
	///算分事件
	nextID = THING_ENG_HANDLE;//下一个事件，游戏结束事件
	time = 2;	
	for(int i=0;i<PLAY_COUNT;i++)
	{
		pDesk->sGameData.T_CountFen.bHu[i] = pDesk->sGameData.T_HuPai.bCanH[i];
		//获取糊牌类型
		for(int j=0;j<MAX_HUPAI_TYPE;j++)
		{
			pDesk->sGameData.T_CountFen.byHuType[i][j] = pDesk->sGameData.T_HuPai.byHuType[i][j];
		}
	}
	pDesk->sGameData.T_CountFen.bDH = pDesk->sGameData.T_HuPai.bDuoXiang;
	pDesk->sGameData.T_CountFen.bZiMo = pDesk->sGameData.T_HuPai.bZimo;
	pDesk->sGameData.T_CountFen.byUser = pDesk->sGameData.T_HuPai.byUser;	
	pDesk->sGameData.T_CountFen.byDian = pDesk->sGameData.T_HuPai.byDianPao;		
	pDesk->sGameData.T_CountFen.byPs = pDesk->sGameData.T_HuPai.byPs;	
	pDesk->sGameData.T_CountFen.byNt = pDesk->sUserData.m_byNtStation;
	if(pDesk->sGameData.T_CountFen.bZiMo )
	{
		pDesk->sGameData.T_CountFen.byDian = pDesk->sGameData.T_CountFen.byUser;
	}
	///算分
	pDesk->sCountFen.CountFen(pDesk->sGameData.T_HuPai,pDesk->sGameData.T_CountFen);

	memset(pDesk->sGameData.T_CountFen.m_byArHandPai,255,sizeof(pDesk->sGameData.T_CountFen.m_byArHandPai));
	memset(pDesk->sGameData.T_HuPai.m_byArHandPai,255,sizeof(pDesk->sGameData.T_HuPai.m_byArHandPai));

	pDesk->sUserData.CopyHandPai(pDesk->sGameData.T_CountFen.m_byArHandPai,pDesk->sGameData.T_HuPai.byUser,true);
	pDesk->sUserData.CopyHandPai(pDesk->sGameData.T_HuPai.m_byArHandPai,pDesk->sGameData.T_HuPai.byUser,true);

	for(int i=0;i<PLAY_COUNT;i++)
	{
		pDesk->sUserData.CopyGCPData(i,pDesk->sGameData.T_CountFen.m_UserGCPData[i]);
		pDesk->sGameData.T_CountFen.iHuShu[i] = pDesk->sGameData.T_CountFen.iZongFen[i];
	}
}

///正常游戏结束事件
void ThingHandle::ExecuteNormalEnd(BYTE &nextID,BYTE &time)
{
	pDesk->KillTimer(TIME_WAIT_MSG);

	pDesk->sGameData.m_byThingDoing = THING_ENG_HANDLE;
	///游戏结束处理事件
	pDesk->sGameData.T_CountFen.byLianZhuang = pDesk->sUserData.m_byLianZhuang;
	//确定庄家
	if(pDesk->sGameData.m_mjRule.byNextNtType == 0)
	{///庄赢连庄，否则下家坐庄
		if(!pDesk->sGameData.T_CountFen.bHu[pDesk->sUserData.m_byNtStation] || pDesk->sGameData.T_CountFen.byUser==255)//糊牌中没有庄家或者流局
		{
			pDesk->sUserData.m_byNtStation = GetNextStation(pDesk->sUserData.m_byNtStation);
			pDesk->sUserData.m_byLianZhuang = 1;
		}
		else 
		{
			pDesk->sUserData.m_byLianZhuang++;
		}
	}
	else if(pDesk->sGameData.m_mjRule.byNextNtType == 1)
	{///庄赢或流局连庄，否则下家坐庄
		if(!pDesk->sGameData.T_CountFen.bHu[pDesk->sUserData.m_byNtStation] && pDesk->sGameData.T_CountFen.byUser!=255)//糊牌中没有庄家或者流局
		{
			pDesk->sUserData.m_byNtStation = GetNextStation(pDesk->sUserData.m_byNtStation);
			pDesk->sUserData.m_byLianZhuang = 1;
		}
		else 
		{
			pDesk->sUserData.m_byLianZhuang++;
		}
	}
	else if(pDesk->sGameData.m_mjRule.byNextNtType == 2)
	{///谁赢谁坐庄，流局或庄赢庄家连庄
		if(pDesk->sGameData.T_CountFen.byUser!=pDesk->sUserData.m_byNtStation)
		{
			pDesk->sUserData.m_byNtStation = pDesk->sGameData.T_CountFen.byUser;
			pDesk->sUserData.m_byLianZhuang = 1;
		}
		else
		{
			pDesk->sUserData.m_byLianZhuang++;
		}
	}
	else if(pDesk->sGameData.m_mjRule.byNextNtType == 3)
	{///谁赢谁坐庄，流局下家坐庄
		if(pDesk->sGameData.T_CountFen.byUser==255)//流局
		{
			pDesk->sUserData.m_byNtStation = GetNextStation(pDesk->sUserData.m_byNtStation);
			pDesk->sUserData.m_byLianZhuang = 1;
		}
		else
		{
			if(pDesk->sUserData.m_byNtStation == pDesk->sGameData.T_CountFen.byUser)
			{
				pDesk->sUserData.m_byLianZhuang++;
			}
			else
			{
				pDesk->sUserData.m_byLianZhuang = 1;
			}
			pDesk->sUserData.m_byNtStation = pDesk->sGameData.T_CountFen.byUser;

		}
	}
	else if(pDesk->sGameData.m_mjRule.byNextNtType == 4)
	{//不管结构如果都是下家坐庄
		pDesk->sUserData.m_byNtStation = GetNextStation(pDesk->sUserData.m_byNtStation);
		pDesk->sUserData.m_byLianZhuang = 1;
	}
	pDesk->sGameData.T_CountFen.byNextNt = pDesk->sUserData.m_byNtStation;

	//调用平台结束游戏接口
	pDesk->GameFinish(pDesk->sGameData.T_HuPai.byUser,GF_NORMAL);
}

///异常游戏结束事件事件
void ThingHandle::ExecuteUnNormalEnd(BYTE &nextID,BYTE &time)
{
	pDesk->KillTimer(TIME_WAIT_MSG);
	//调用平台结束游戏接口
	pDesk->GameFinish(255,GF_SAFE);
}


///获取牌数据，
void ThingHandle::DisPatchCard()
{
	//发牌
	BYTE index=0;
	memset(pDesk->sUserData.m_MenPai.byMenPai,255,sizeof(pDesk->sUserData.m_MenPai.byMenPai));//
	for(int i=0;i<4;i++)
	{
		for(int j=0;j<27;j++)
		{
			pDesk->sUserData.m_MenPai.byMenPai[index] = stcArr_A_Mj_WTT[j];
			index++;
		}
		if(pDesk->sGameData.m_mjRule.bHaveBaiBan)//有白板
		{
			pDesk->sUserData.m_MenPai.byMenPai[index] = stcArr_A_Mj_ZI[0];
			index++;
		}
		if(pDesk->sGameData.m_mjRule.bHaveFaCai)//有发财
		{
			pDesk->sUserData.m_MenPai.byMenPai[index] = stcArr_A_Mj_ZI[1];
			index++;
		}
		if(pDesk->sGameData.m_mjRule.bHaveHongZhong)//有红中
		{
			pDesk->sUserData.m_MenPai.byMenPai[index] = stcArr_A_Mj_ZI[2];
			index++;
		}
		if(pDesk->sGameData.m_mjRule.bHaveFengPai)//有东南西北
		{
			for(int j=0;j<4;j++)
			{
				pDesk->sUserData.m_MenPai.byMenPai[index] = stcArr_A_Mj_FENG[j];
				index++;
			}
		}
	}
	if(pDesk->sGameData.m_mjRule.bHaveFlower)//有花牌
	{
		for(int i=0;i<8;i++)
		{
			pDesk->sUserData.m_MenPai.byMenPai[index] = stcArr_A_Mj_Other[i];
			index++;
		}
	}
	///打乱牌
	srand(GetTickCount());
	BYTE temp=255,data=0;
	for(int i=0;i<index;i++)
	{
		temp = rand()%(index-i)+i;
		data = pDesk->sUserData.m_MenPai.byMenPai[temp];
		pDesk->sUserData.m_MenPai.byMenPai[temp] = pDesk->sUserData.m_MenPai.byMenPai[i];
		pDesk->sUserData.m_MenPai.byMenPai[i] = data;
	}
	///////////////
	pDesk->sUserData.m_MenPai.byGetPai = pDesk->sUserData.m_MenPai.byGetPai%13;
	//pDesk->sUserData.m_MenPai.byGetPaiDir = pDesk->sUserData.m_MenPai.byGetPaiDir%4;
	if(pDesk->sUserData.m_MenPai.byGetPaiDir == 255)
	{
		pDesk->sUserData.m_MenPai.byGetPaiDir = pDesk->sUserData.m_byNtStation;
	}
	pDesk->sUserData.m_MenPai.byGetPaiDir = pDesk->sUserData.m_MenPai.byGetPaiDir%4;

	int dun = 0,num=0;
	dun = index/8;
	num = (dun*pDesk->sUserData.m_MenPai.byGetPaiDir + pDesk->sUserData.m_MenPai.byGetPai)*2;
	if(num == 0)
		num=2;
	pDesk->sUserData.m_MenPai.byStartIndex = num;//门牌起始索引
	pDesk->sUserData.m_MenPai.byEndIndex = num-1;	//门牌结束索引
	pDesk->sUserData.m_MenPai.byAllPaiNum = index;//所有牌的数量

	CString str="";
	str.Format(" 财神测试 事件DisPatchCard 牌墙牌 dun=%d 开始位置%d 牌=%d  方向%d 敦数%d",dun,pDesk->sUserData.m_MenPai.byStartIndex,pDesk->sUserData.m_MenPai.byMenPai[num],pDesk->sUserData.m_MenPai.byGetPaiDir,pDesk->sUserData.m_MenPai.byGetPai);
	OutputDebugString(str);

	str.Format(" 财神测试 事件DisPatchCard dun=%d num%d 方向%d 敦数%d",dun,num,pDesk->sUserData.m_MenPai.byGetPaiDir,pDesk->sUserData.m_MenPai.byGetPai);
	OutputDebugString(str);
}


///名称：ChangeMenPai
///描述：转换门牌数据,将总门牌数组中的数据转换到每个玩家的门牌数组中
///@param 
///@return 
void ThingHandle::ChangeMenPai()
{
	int num = 0;
	int index=0;
	num = pDesk->sUserData.m_MenPai.byAllPaiNum/4;
	for(int i=0;i<4;i++)
	{
		memset(pDesk->sUserData.m_iArMenPai[i],255,sizeof(pDesk->sUserData.m_iArMenPai[i]));
		for(int j=i*num;j<(i+1)*num ;j++)
		{
			if(pDesk->sUserData.m_MenPai.byMenPai[j] != 255)
			{
				pDesk->sUserData.m_iArMenPai[i][j-i*num] = 0;
				if(pDesk->sUserData.m_MenPai.byCaiShenIndex == j && pDesk->sUserData.m_MenPai.byCaiShenIndex != 255)
				{
					pDesk->sUserData.m_iArMenPai[i][j-i*num] = pDesk->sUserData.m_MenPai.byMenPai[j];
				}
			}
			else
			{
				pDesk->sUserData.m_iArMenPai[i][j-i*num] = 255;
			}
		}
	}
}

///名称：GetPai
///描述：抓一张牌
///@param station 抓牌玩家的位置 , head  是否从前面抓,type 抓牌类型 0 正常抓牌，1开始前发牌，2开前补花抓牌，3补花
///@return 抓到的牌值
BYTE ThingHandle::GetPai(int station,bool head,BYTE type,BYTE *pIndex)
{
	BYTE paiIndex = 255;//被抓牌的位置
	if(pDesk->sGameData.m_byThingDoing == THING_ZHUA_PAI && pDesk->sUserData.GetHandPaiCount(station) %3 == 2)//玩家的手牌足够
	{
		OutputDebugString("麻将-听牌：玩家牌足够 把最后的一张牌删掉");
		pDesk->sUserData.DelleteAHandPai(station,pDesk->sUserData.GetLastHandPai(station));
		//return 255;
	}

	int num=0;//遍历次数
	BYTE card=255;
	while(card==255 && num<10)
	{
		if(head)//从前抓
		{
			if(pDesk->sUserData.m_MenPai.byStartIndex>=pDesk->sUserData.m_MenPai.byAllPaiNum)
			{
				pDesk->sUserData.m_MenPai.byStartIndex = 0;
			}
			paiIndex = pDesk->sUserData.m_MenPai.byStartIndex;//被抓牌的位置
			card=pDesk->sUserData.m_MenPai.byMenPai[pDesk->sUserData.m_MenPai.byStartIndex];
			pDesk->sUserData.m_MenPai.byMenPai[pDesk->sUserData.m_MenPai.byStartIndex] = 255;
			pDesk->sUserData.m_MenPai.byStartIndex++;
		}
		else//从后面抓
		{
			if(pDesk->sUserData.m_MenPai.byEndIndex == pDesk->sUserData.m_MenPai.byCaiShenIndex)
			{
				if(pDesk->sUserData.m_MenPai.byEndIndex == 0)
				{
					paiIndex = pDesk->sUserData.m_MenPai.byAllPaiNum-1;//被抓牌的位置
					card = pDesk->sUserData.m_MenPai.byMenPai[pDesk->sUserData.m_MenPai.byAllPaiNum-1];
					pDesk->sUserData.m_MenPai.byMenPai[pDesk->sUserData.m_MenPai.byAllPaiNum-1] = pDesk->sUserData.m_MenPai.byMenPai[0];
					pDesk->sUserData.m_MenPai.byEndIndex=pDesk->sUserData.m_MenPai.byAllPaiNum-1;
					pDesk->sUserData.m_MenPai.byEndIndex=pDesk->sUserData.m_MenPai.byEndIndex;
				}
				else
				{
					paiIndex = pDesk->sUserData.m_MenPai.byEndIndex-1;//被抓牌的位置
					card = pDesk->sUserData.m_MenPai.byMenPai[pDesk->sUserData.m_MenPai.byEndIndex-1];
					pDesk->sUserData.m_MenPai.byMenPai[pDesk->sUserData.m_MenPai.byEndIndex-1] = pDesk->sUserData.m_MenPai.byMenPai[pDesk->sUserData.m_MenPai.byEndIndex];
					pDesk->sUserData.m_MenPai.byMenPai[pDesk->sUserData.m_MenPai.byEndIndex] = 255;
					pDesk->sUserData.m_MenPai.byEndIndex--;
					pDesk->sUserData.m_MenPai.byEndIndex=pDesk->sUserData.m_MenPai.byEndIndex;
				}
			}
			else
			{
				paiIndex = pDesk->sUserData.m_MenPai.byEndIndex;//被抓牌的位置
				card = pDesk->sUserData.m_MenPai.byMenPai[pDesk->sUserData.m_MenPai.byEndIndex];
				pDesk->sUserData.m_MenPai.byMenPai[pDesk->sUserData.m_MenPai.byEndIndex] = 255;
				if(pDesk->sUserData.m_MenPai.byEndIndex == 0)
				{
					pDesk->sUserData.m_MenPai.byEndIndex = pDesk->sUserData.m_MenPai.byAllPaiNum-1;
				}
				else
				{
					pDesk->sUserData.m_MenPai.byEndIndex--;
				}
			}
		}
#ifdef MJ_CAN_SETPAI

		if (pDesk->sUserData.m_bySetMoPai[station] != 255)
		{
			card = pDesk->sUserData.m_bySetMoPai[station];
			pDesk->sUserData.m_bySetMoPai[station] = 255;
		}

#endif //MJ_CAN_SETPAI

		//if(station == pDesk->sUserData.m_byNtStation && pDesk->sGameData.m_byThingDoing == THING_ZHUA_PAI)
		//{
		//	card = pDesk->sUserData.m_StrJing.byPai[0];
		//}
		pDesk->sUserData.AddToHandPai(station,card);
	}
		num++;
	if(pIndex != NULL)
	{
		*pIndex = paiIndex;//被抓牌的位置
	}
	return card;
}

///检测玩家station当前能做哪些动作
///名称：CheckAction
///描述：station 检测位置,pai 别的玩家打出的牌，type 类型 出牌 0 抓牌，1杠开，2 点炮，3抢杠
///@param 
///@return true 能做动作，false 不能做动作
bool ThingHandle::CheckAction(int station,BYTE pai,BYTE type)
{
	for(int i=0;i<4;++i)
	{
		pDesk->sGameData.T_CPGNotify[i].Clear();//清零
	}

	bool action = false;
	switch(type)
	{
	case 0://自摸
	case 1://杠开
		{
			//糊
			if(pDesk->sCheckHuPai.CanHu(station,pai,pDesk->sGameData.T_CPGNotify[station].byHuType,true))
			{
				pDesk->sGameData.T_CPGNotify[station].bHu = true;
				pDesk->sGameData.T_CPGNotify[station].m_iHuPs = pDesk->sUserData.GetLastHandPai(station);
				pDesk->sGameData.T_CPGNotify[station].m_byDianPao = station;
			}
			//杠
			if(!pDesk->sUserData.m_bTing[station] && pDesk->sCPGAction.CanGang(station,pai,pDesk->sGameData.T_CPGNotify[station].m_iGangData,true))
			{
				pDesk->sGameData.T_CPGNotify[station].bGang = true;
			}
			//听
			if(pDesk->sGameData.m_mjAction.bTing && !pDesk->sUserData.m_bTing[station])//玩家没有听牌
			{
				pDesk->sGameData.T_CPGNotify[station].bTing = pDesk->sCheckHuPai.CanTing(station,pDesk->sGameData.T_CPGNotify[station].m_byTingCanOut);
			}
			if(pDesk->sGameData.T_CPGNotify[station].bHu || pDesk->sGameData.T_CPGNotify[station].bGang || pDesk->sGameData.T_CPGNotify[station].bTing)
			{
				action=true;
				pDesk->sGameData.T_CPGNotify[station].bCanAction = true;
			}
		}
		break;
	case 2://点炮
		{
			for(int i=0;i<PLAY_COUNT;++i)
			{
				if(i==station)
					continue;
				//吃
				if(!pDesk->sUserData.m_bTing[i] && GetNextStation(station)==i)
				{
					if(pDesk->sCPGAction.CanChi(i,pai,pDesk->sGameData.T_CPGNotify[i].m_iChi))
					{					
						pDesk->sGameData.T_CPGNotify[i].bChi = true;
						pDesk->sGameData.T_CPGNotify[i].m_byMaxThing = ACTION_CHI;
					}
				}
				//碰
				if(!pDesk->sUserData.m_bTing[i] && pDesk->sCPGAction.CanPeng(i,pai))
				{	
					pDesk->sGameData.T_CPGNotify[i].bPeng = true;
					pDesk->sGameData.T_CPGNotify[i].m_byMaxThing = ACTION_PENG;
				}
				//杠
				if(!pDesk->sUserData.m_bTing[i] && pDesk->sCPGAction.CanGang(i,pai,pDesk->sGameData.T_CPGNotify[i].m_iGangData,false))
				{
					pDesk->sGameData.T_CPGNotify[i].bGang = true;
					pDesk->sGameData.T_CPGNotify[i].m_byMaxThing = ACTION_MING_GANG;

				}
				//糊
				if(pDesk->sCheckHuPai.CanHu(i,pai,pDesk->sGameData.T_CPGNotify[i].byHuType,false))
				{
					pDesk->sGameData.T_CPGNotify[i].bHu = true;
					pDesk->sGameData.T_CPGNotify[i].m_byMaxThing = ACTION_HU;
					pDesk->sGameData.T_CPGNotify[i].m_iHuPs = pai;
					pDesk->sGameData.T_CPGNotify[i].m_byDianPao = station;
				}

				if(pDesk->sGameData.T_CPGNotify[i].bHu || pDesk->sGameData.T_CPGNotify[i].bPeng || pDesk->sGameData.T_CPGNotify[i].bChi || pDesk->sGameData.T_CPGNotify[i].bGang || pDesk->sGameData.T_CPGNotify[i].bTing)
				{
					action=true;
					pDesk->sGameData.T_CPGNotify[i].bCanAction = true;
				}
			}
		}
		break;
	case 3://抢杠
		{
			for(int i=0;i<PLAY_COUNT;++i)
			{
				if(i==station)
					continue;
				//糊
				if(pDesk->sCheckHuPai.CanHu(i,pai,pDesk->sGameData.T_CPGNotify[i].byHuType,false))
				{
					pDesk->sGameData.T_CPGNotify[i].bHu = true;
					pDesk->sGameData.T_CPGNotify[i].m_iHuPs = pai;//记录胡的牌
					pDesk->sGameData.T_CPGNotify[i].m_byDianPao = station;
				}
				if(pDesk->sGameData.T_CPGNotify[i].bHu )
				{
					action=true;
					pDesk->sGameData.T_CPGNotify[i].bCanAction = true;
				}
			}
		}
		break;
	}
	if(action)
	{
		return true;
	}
	return false;
}


///断线自动处理函数
void ThingHandle::NetCutAutoHandle()
{
	switch(pDesk->sGameData.m_byThingDoing)
	{			        				         
	case THING_2SEZI_NT://首局掷2颗色子的点数和为庄事件		   
	case THING_2SEZI_NT_GP://首局掷2颗色子定庄家和起牌点事件	
	case THING_TURN_2SEZI_NT://首局轮流掷2颗色子定庄家事件		
	case THING_2SEZI_DIR://掷2颗色子事件定起牌方向事件		
	case THING_2SEZI_DIR_GP://掷2颗色子事件定起牌方向和起牌点事件
	case THING_2SEZI_GP://掷2颗色子事件定起牌点事件												
	case THING_2SEZI_JING://掷2颗色子定精牌事件			
	case THING_1SEZI_JIN://掷1颗色子定金牌事件
		{
			for(int i=0;i<PLAY_COUNT;++i)
			{
				if(!pDesk->sUserData.m_bSiziFinish[i] && pDesk->sUserData.m_bFoceLeave[i])//该玩家没有发结束消息
				{
					ReceiveSeziFinish(i,NULL,0,false);
				}
			}
		}break;
	case THING_ALL_BUHUA://为所有玩家补花事件
		{
		}
		break;
	case THING_ONE_BUHUA://单个玩家补花事件	
	case THING_CHI_PAI://吃牌事件
	case THING_PENG_PAI://碰牌事件
	case THING_GANG_PAI://杠牌事件
		{
			if(pDesk->sUserData.m_bFoceLeave[pDesk->sUserData.m_byNowOutStation])//如果出牌玩家断线
			{
				AutoOutOrGet();//代替玩家出牌
			}
		}
		break;
	case THING_BEGIN_OUT://庄家开始发牌通知事件	
		{
			if(pDesk->sUserData.m_bFoceLeave[pDesk->sUserData.m_byNtStation])//如果庄家断线
			{
				tagOutPaiEx outpai;
				outpai.Clear();
				outpai.byUser = pDesk->sUserData.m_byNtStation;
				outpai.byPs = pDesk->sUserData.GetLastHandPai(pDesk->sUserData.m_byNtStation);
			OutputDebugString("二人麻将： THING_BEGIN_OUT   ReceiveUserOutCard");
				ReceiveUserOutCard(pDesk->sUserData.m_byNtStation,&outpai,sizeof(outpai),false);
			}
		}
		break;
	case THING_OUT_PAI://出牌事件
		{
			for(int i=0;i<PLAY_COUNT;++i)
			{
				if(pDesk->sUserData.m_bFoceLeave[i] && pDesk->sUserData.m_byNowOutStation != i)//玩家玩家断线
				{
					ReceiveUserPassCheck(i,NULL,0,false);//发送放弃消息
				}
			}
		}
		break;
	case THING_ZHUA_PAI://抓牌事件
		{
			if(pDesk->sUserData.m_bFoceLeave[pDesk->sUserData.m_byNowOutStation])//如果出牌玩家断线
			{
				tagOutPaiEx outpai;
				outpai.Clear();
				outpai.byUser = pDesk->sUserData.m_byNowOutStation;
				outpai.byPs = pDesk->sUserData.GetLastHandPai(pDesk->sUserData.m_byNowOutStation);
				ReceiveUserOutCard(pDesk->sUserData.m_byNowOutStation,&outpai,sizeof(outpai),false);
			}
		}
		break;
	case THING_TING_PAI://听牌事件
		{
			if(pDesk->sUserData.m_bFoceLeave[pDesk->sUserData.m_byNowOutStation])//如果出牌玩家断线
			{
				tagOutPaiEx outpai;
				outpai.Clear();
				outpai.byUser = pDesk->sUserData.m_byNowOutStation;
				outpai.byPs = pDesk->sUserData.GetLastHandPai(pDesk->sUserData.m_byNowOutStation);

			OutputDebugString("二人麻将： THING_TING_PAI   ReceiveUserOutCard");

				ReceiveUserOutCard(pDesk->sUserData.m_byNowOutStation,&outpai,sizeof(outpai),false);
			}
		}
		break;
	}
}

//防卡桌自动处理：2分钟内没有收到任何游戏消息，认为是游戏卡死，调用本函数，根据当前执行事件和玩家的状态进行自动处理，恢复游戏流程
void ThingHandle::AutoHandle()
{
	switch(pDesk->sGameData.m_byThingDoing)
	{			        				         
	case THING_2SEZI_NT://首局掷2颗色子的点数和为庄事件		   
	case THING_2SEZI_NT_GP://首局掷2颗色子定庄家和起牌点事件	
	case THING_TURN_2SEZI_NT://首局轮流掷2颗色子定庄家事件		
	case THING_2SEZI_DIR://掷2颗色子事件定起牌方向事件		
	case THING_2SEZI_DIR_GP://掷2颗色子事件定起牌方向和起牌点事件
	case THING_2SEZI_GP://掷2颗色子事件定起牌点事件												
	case THING_2SEZI_JING://掷2颗色子定精牌事件			
		{//构造色子结束消息
			for(int i=0;i<PLAY_COUNT;++i)
			{
				if(!pDesk->sUserData.m_bSiziFinish[i])//该玩家没有发结束消息
				{
					ReceiveSeziFinish(i,NULL,0,false);
				}
			}
		}
		break;
	case THING_ALL_BUHUA://为所有玩家补花事件
		{//执行开始出牌事件
			pDesk->sGameData.ApplyThingID = THING_BEGIN_OUT;
			ApplyAction(pDesk->sGameData.ApplyThingID,200);//申请下一个事件
		}
		break;
	case THING_ONE_BUHUA://单个玩家补花事件	
	case THING_BEGIN_OUT://庄家开始发牌通知事件	
	case THING_OUT_PAI://出牌事件
	case THING_CHI_PAI://吃牌事件
	case THING_PENG_PAI://碰牌事件
	case THING_ZHUA_PAI://抓牌事件
	case THING_GANG_PAI://杠牌事件
		{
			OutputDebugString("二人麻将： AutoHandle 自动出牌处理 00");
			//代替玩家出牌
			AutoOutOrGet();
		}
		break;
	case THING_HU_PAI://糊事件	
		{
			pDesk->sGameData.ApplyThingID = THING_COUNT_FEN;
			ApplyAction(pDesk->sGameData.ApplyThingID,200);//申请下一个事件
		}
		break;
	case THING_COUNT_FEN://算分事件	
		{
			pDesk->sGameData.ApplyThingID = THING_ENG_HANDLE;
			ApplyAction(pDesk->sGameData.ApplyThingID,200);//申请下一个事件
		}
		break;
	case THING_ENG_HANDLE://结束处理事件
	case THING_ENG_UN_NORMAL://非正常结束处理事件
		{

		}
		break;
	}
}

///自动处理出牌或者抓牌
void ThingHandle::AutoOutOrGet()
{
	bool haveout = false;
	for(int i=0;i<PLAY_COUNT;++i)
	{
		if(pDesk->sUserData.IsOutPaiPeople(i))//出牌玩家
		{
			haveout = true;

			tagOutPaiEx outpai;
			outpai.Clear();
			outpai.byUser = i;
			outpai.byPs = pDesk->sUserData.GetLastHandPai(i);

			OutputDebugString("二人麻将： AutoOutOrGet   ReceiveUserOutCard");

			ReceiveUserOutCard(i,&outpai,sizeof(outpai),false);
			break;
		}
	}
	if(!haveout)//没有出牌的玩家，启动抓牌事件
	{
		pDesk->sGameData.ApplyThingID = THING_ZHUA_PAI;
		ApplyAction(pDesk->sGameData.ApplyThingID,200);//申请下一个事件
	}
}

//////////////////////////////动作处理//////////////////////////////////////////////////////


///吃牌事件
void ThingHandle::HandleChiPai(BYTE &nextID,BYTE &time)
{	
	pDesk->sGameData.m_byThingDoing = THING_CHI_PAI;//记录正在执行的事件
	///吃牌牌事件
	pDesk->sGameData.T_ChiPai = pDesk->sGameData.temp_ChiPai;//获取数据
	pDesk->sUserData.m_byNowOutStation = pDesk->sGameData.temp_ChiPai.byUser;
	//添加吃牌
	GCPStructEx chi;
	chi.Init();
	chi.byType = ACTION_CHI;
	chi.iBeStation = pDesk->sGameData.temp_ChiPai.byBeChi;
	chi.iOutpai = pDesk->sGameData.temp_ChiPai.byPs;
	chi.iStation = pDesk->sGameData.temp_ChiPai.byUser;

	BYTE station = pDesk->sGameData.temp_ChiPai.byUser;

	for(int i=0;i<3;i++)
	{	
		chi.byData[i] = pDesk->sGameData.temp_ChiPai.byChiPs[i];
		if(pDesk->sGameData.temp_ChiPai.byChiPs[i] == pDesk->sGameData.temp_ChiPai.byPs)
			continue;
		pDesk->sUserData.DelleteAHandPai(pDesk->sGameData.temp_ChiPai.byUser,pDesk->sGameData.temp_ChiPai.byChiPs[i]);//删除手牌
	}
	pDesk->sUserData.DelleteLastOutPai(pDesk->sGameData.T_OutPai.byUser);
	pDesk->sUserData.AddToGCP(chi.iStation,&chi);

	for(int i=0;i<PLAY_COUNT;++i)
	{
		pDesk->sUserData.CopyGCPData(i,pDesk->sGameData.T_ChiPai.m_UserGCPData[i]);
		pDesk->sUserData.CopyOutPai(i,pDesk->sGameData.T_ChiPai.m_byArOutPai[i]);
	}
	pDesk->sGameData.m_byNextAction = ACTION_NO;

	//判断是否能听牌
	if(pDesk->sGameData.m_mjAction.bTing)//有听牌
	{
		if(pDesk->sCheckHuPai.CanTing(station,pDesk->sGameData.T_CPGNotify[station].m_byTingCanOut))//检测能否听牌
		{
			pDesk->sGameData.T_CPGNotify[station].bTing = true;
		}
	}
	pDesk->sUserData.m_byApplyMaxAction = ACTION_NO;//玩家当前申请的最大事件：处理完申请后，恢复数据
}

///碰牌事件
void ThingHandle::HandlePengPai(BYTE &nextID,BYTE &time)	
{
	pDesk->sGameData.m_byThingDoing = THING_PENG_PAI;//记录正在执行的事件
	///碰牌牌事件
	pDesk->sGameData.T_PengPai = pDesk->sGameData.temp_PengPai;//获取数据
	pDesk->sUserData.m_byNowOutStation = pDesk->sGameData.temp_PengPai.byUser;
	//碰牌牌事件
	GCPStructEx peng;
	peng.Init();

	peng.byType = ACTION_PENG;
	peng.iBeStation = pDesk->sGameData.temp_PengPai.byBePeng;
	peng.iOutpai = pDesk->sGameData.temp_PengPai.byPs;
	peng.iStation = pDesk->sGameData.temp_PengPai.byUser;
	BYTE station = pDesk->sGameData.temp_PengPai.byUser;

	//出牌玩家删除最后一张出牌
	pDesk->sUserData.DelleteLastOutPai(pDesk->sGameData.T_OutPai.byUser);
	for(int i=0;i<2;i++)
	{	
		pDesk->sUserData.DelleteAHandPai(pDesk->sGameData.temp_PengPai.byUser,pDesk->sGameData.temp_PengPai.byPs);//删除手牌
	}
	for(int i=0;i<3;i++)
	{
		peng.byData[i] = pDesk->sGameData.temp_PengPai.byPs;
	}

	pDesk->sUserData.AddToGCP(pDesk->sGameData.temp_PengPai.byUser,&peng);//添加碰牌

	for(int i=0;i<PLAY_COUNT;++i)
	{
		pDesk->sUserData.CopyGCPData(i,pDesk->sGameData.T_PengPai.m_UserGCPData[i]);
		pDesk->sUserData.CopyOutPai(i,pDesk->sGameData.T_PengPai.m_byArOutPai[i]);
	}

	//判断是否能听牌
	if(pDesk->sGameData.m_mjAction.bTing)//有听牌
	{
		if(pDesk->sCheckHuPai.CanTing(station,pDesk->sGameData.T_CPGNotify[station].m_byTingCanOut))//检测能否听牌
		{
			pDesk->sGameData.T_CPGNotify[station].bTing = true;
		}
	}

	pDesk->sGameData.m_byNextAction = ACTION_NO;
	pDesk->sUserData.m_byApplyMaxAction = ACTION_NO;//玩家当前申请的最大事件：处理完申请后，恢复数据
}

///杠事件
void ThingHandle::HandleGangPai(BYTE &nextID,BYTE &time)	
{
	pDesk->sGameData.m_byThingDoing = THING_GANG_PAI;//记录正在执行的事件
	nextID = THING_ZHUA_PAI;
	time = 3;
	///杠牌牌事件
	pDesk->sGameData.T_GangPai = pDesk->sGameData.temp_GangPai;//获取数据
	pDesk->sUserData.m_byNowOutStation = pDesk->sGameData.temp_GangPai.byUser;

	GCPStructEx gang;
	gang.Init();
	gang.iBeStation = pDesk->sGameData.temp_GangPai.byBeGang;
	gang.iOutpai = pDesk->sGameData.temp_GangPai.byPs;
	gang.iStation = pDesk->sGameData.temp_GangPai.byUser;

	gang.byData[0] = pDesk->sGameData.temp_GangPai.byPs;
	gang.byData[1] = pDesk->sGameData.temp_GangPai.byPs;
	gang.byData[2] = pDesk->sGameData.temp_GangPai.byPs;
	gang.byData[3] = pDesk->sGameData.temp_GangPai.byPs;

	switch(pDesk->sGameData.temp_GangPai.byType)
	{
	case ACTION_AN_GANG://暗杠
		{
			for(int i=0;i<4;++i)
			{	
				pDesk->sUserData.DelleteAHandPai(pDesk->sGameData.temp_GangPai.byUser,pDesk->sGameData.temp_GangPai.byPs);//删除手牌
			}
			gang.byType = ACTION_AN_GANG;

			////动作检测(检测是否还存在暗杠)
			//CheckAction(pDesk->sGameData.temp_GangPai.byUser,255,1);
		}
		break;
	case ACTION_MING_GANG://明杠
		{
			for(int i=0;i<3;++i)
			{	
				pDesk->sUserData.DelleteAHandPai(pDesk->sGameData.temp_GangPai.byUser,pDesk->sGameData.temp_GangPai.byPs);//删除手牌
			}
			gang.byType = ACTION_MING_GANG;
			pDesk->sUserData.DelleteLastOutPai(pDesk->sGameData.temp_GangPai.byUser);//出牌玩家删除最后一张出牌
		}
		break;
	case ACTION_BU_GANG://补杠
		{
			pDesk->sUserData.DelleteAHandPai(pDesk->sGameData.temp_GangPai.byUser,pDesk->sGameData.temp_GangPai.byPs);//删除手牌
			pDesk->sUserData.DelleteAGCPData(pDesk->sGameData.temp_GangPai.byUser,ACTION_PENG,pDesk->sGameData.temp_GangPai.byPs);//删除相应碰牌
			gang.byType = ACTION_BU_GANG;
		}
		break;
	}
	pDesk->sUserData.AddToGCP(pDesk->sGameData.temp_GangPai.byUser,&gang);

	for(int i=0;i<PLAY_COUNT;++i)
	{
		pDesk->sUserData.CopyGCPData(i,pDesk->sGameData.T_GangPai.m_UserGCPData[i]);
		pDesk->sUserData.CopyOutPai(i,pDesk->sGameData.T_GangPai.m_byArOutPai[i]);
	}

	if(pDesk->sGameData.temp_GangPai.byType == ACTION_AN_GANG)
	{
		//CheckAction(pDesk->sGameData.temp_GangPai.byUser,255,0);
		pDesk->sGameData.T_CPGNotify[pDesk->sGameData.temp_GangPai.byUser].bHu = false;
		pDesk->sGameData.T_CPGNotify[pDesk->sGameData.temp_GangPai.byUser].bTing = false;
		pDesk->sGameData.T_CPGNotify[pDesk->sGameData.temp_GangPai.byUser].bKan = false;
		pDesk->sGameData.T_CPGNotify[pDesk->sGameData.temp_GangPai.byUser].bSaoHu = false;
	}
	pDesk->sGameData.m_byNextAction = ACTION_NO;
	///当前抓牌方向，true 顺时针，false 逆时针
	pDesk->sUserData.m_bZhuaPaiDir = false;
	pDesk->sUserData.m_byApplyMaxAction = ACTION_NO;//玩家当前申请的最大事件：处理完申请后，恢复数据
}

///听事件
void ThingHandle::HandleTingPai(BYTE &nextID,BYTE &time)
{
	pDesk->sGameData.m_byThingDoing = THING_TING_PAI;//记录正在执行的事件
	///听牌牌事件
	//T_TingPai;	
	pDesk->sGameData.m_byNextAction = ACTION_NO;
	pDesk->sUserData.m_byApplyMaxAction = ACTION_NO;//玩家当前申请的最大事件：处理完申请后，恢复数据
}

///糊事件
void ThingHandle::HandleHuPai(BYTE &nextID,BYTE &time)	
{
	pDesk->sGameData.m_byThingDoing = THING_HU_PAI;//记录正在执行的事件
	///糊牌牌事件
	//T_HuPai;	
	nextID = THING_COUNT_FEN;
	time = 2;
	memset(pDesk->sGameData.T_HuPai.m_byArHandPai,255,sizeof(pDesk->sGameData.T_HuPai.m_byArHandPai));
	pDesk->sUserData.CopyHandPai(pDesk->sGameData.T_HuPai.m_byArHandPai,0,true);
	pDesk->sGameData.m_byNextAction = ACTION_NO;
	pDesk->sUserData.m_byApplyMaxAction = ACTION_NO;//玩家当前申请的最大事件：处理完申请后，恢复数据
	OutputDebugString("出牌  糊牌事件处理");
}

///算分
void ThingHandle::HandleCountFen(BYTE &nextID,BYTE &time)
{
	pDesk->sGameData.m_byThingDoing = THING_COUNT_FEN;
	///算分事件
	nextID = THING_ENG_HANDLE;
	for(int i=0;i<PLAY_COUNT;i++)
	{
		pDesk->sGameData.T_CountFen.bHu[i] = pDesk->sGameData.T_HuPai.bCanH[i];
		//获取糊牌类型
		for(int j=0;j<MAX_HUPAI_TYPE;j++)
		{
			pDesk->sGameData.T_CountFen.byHuType[i][j] = pDesk->sGameData.T_HuPai.byHuType[i][j];
		}
	}
	pDesk->sGameData.T_CountFen.bDH = pDesk->sGameData.T_HuPai.bDuoXiang;
	pDesk->sGameData.T_CountFen.bZiMo = pDesk->sGameData.T_HuPai.bZimo;
	pDesk->sGameData.T_CountFen.byUser = pDesk->sGameData.T_HuPai.byUser;	
	pDesk->sGameData.T_CountFen.byDian = pDesk->sGameData.T_HuPai.byDianPao;		
	pDesk->sGameData.T_CountFen.byPs = pDesk->sGameData.T_HuPai.byPs;	
	pDesk->sGameData.T_CountFen.byNt = pDesk->sUserData.m_byNtStation;
	if(pDesk->sGameData.T_CountFen.bZiMo )
	{
		pDesk->sGameData.T_CountFen.byDian = pDesk->sGameData.T_CountFen.byUser;
	}
	///算分
	pDesk->sCountFen.CountFen(pDesk->sGameData.T_HuPai,pDesk->sGameData.T_CountFen);

	memset(pDesk->sGameData.T_CountFen.m_byArHandPai,255,sizeof(pDesk->sGameData.T_CountFen.m_byArHandPai));
	memset(pDesk->sGameData.T_HuPai.m_byArHandPai,255,sizeof(pDesk->sGameData.T_HuPai.m_byArHandPai));

	pDesk->sUserData.CopyHandPai(pDesk->sGameData.T_CountFen.m_byArHandPai,pDesk->sGameData.T_HuPai.byUser,true);
	pDesk->sUserData.CopyHandPai(pDesk->sGameData.T_HuPai.m_byArHandPai,pDesk->sGameData.T_HuPai.byUser,true);

	for(int i=0;i<PLAY_COUNT;i++)
	{
		pDesk->sUserData.CopyGCPData(i,pDesk->sGameData.T_CountFen.m_UserGCPData[i]);
		pDesk->sGameData.T_CountFen.iHuShu[i] = pDesk->sGameData.T_CountFen.iZongFen[i];
	}

	pDesk->sGameData.ApplyThingID = nextID;
	ApplyAction(nextID,200);//申请下一个事件
	pDesk->sUserData.m_byApplyMaxAction = ACTION_NO;//玩家当前申请的最大事件：处理完申请后，恢复数据
}
//
/////结束处理事件
//void ThingHandle::HandleEnd(BYTE &nextID,BYTE &time)
//{
//	nextID = 255;
//	pDesk->sGameData.m_byThingDoing = THING_ENG_HANDLE;
//	///游戏结束处理事件
//	//T_EndHandle;	
//	pDesk->sGameData.T_CountFen.byLianZhuang = pDesk->sUserData.m_byLianZhuang;
//	//确定庄家
//	if(pDesk->sGameData.m_mjRule.byNextNtType == 0)
//	{///庄赢连庄，否则下家坐庄
//		if(!pDesk->sGameData.T_CountFen.bHu[pDesk->sUserData.m_byNtStation] || pDesk->sGameData.T_CountFen.byUser==255)//糊牌中没有庄家或者流局
//		{
//			pDesk->sUserData.m_byNtStation = GetNextStation(pDesk->sUserData.m_byNtStation);
//			pDesk->sUserData.m_byLianZhuang = 1;
//		}
//		else 
//		{
//			pDesk->sUserData.m_byLianZhuang++;
//		}
//	}
//	else if(pDesk->sGameData.m_mjRule.byNextNtType == 1)
//	{///庄赢或流局连庄，否则下家坐庄
//		if(!pDesk->sGameData.T_CountFen.bHu[pDesk->sUserData.m_byNtStation] && pDesk->sGameData.T_CountFen.byUser!=255)//糊牌中没有庄家或者流局
//		{
//			pDesk->sUserData.m_byNtStation = GetNextStation(pDesk->sUserData.m_byNtStation);
//			pDesk->sUserData.m_byLianZhuang = 1;
//		}
//		else 
//		{
//			pDesk->sUserData.m_byLianZhuang++;
//		}
//	}
//	else if(pDesk->sGameData.m_mjRule.byNextNtType == 2)
//	{///谁赢谁坐庄，流局或庄赢庄家连庄
//		if(pDesk->sGameData.T_CountFen.byUser!=pDesk->sUserData.m_byNtStation)
//		{
//			pDesk->sUserData.m_byNtStation = pDesk->sGameData.T_CountFen.byUser;
//			pDesk->sUserData.m_byLianZhuang = 1;
//		}
//		else
//		{
//			pDesk->sUserData.m_byLianZhuang++;
//		}
//	}
//	else if(pDesk->sGameData.m_mjRule.byNextNtType == 3)
//	{///谁赢谁坐庄，流局下家坐庄
//		if(pDesk->sGameData.T_CountFen.byUser==255)//流局
//		{
//			pDesk->sUserData.m_byNtStation = GetNextStation(pDesk->sUserData.m_byNtStation);
//			pDesk->sUserData.m_byLianZhuang = 1;
//		}
//		else
//		{
//			if(pDesk->sUserData.m_byNtStation == pDesk->sGameData.T_CountFen.byUser)
//			{
//				pDesk->sUserData.m_byLianZhuang++;
//			}
//			else
//			{
//				pDesk->sUserData.m_byLianZhuang = 1;
//			}
//			pDesk->sUserData.m_byNtStation = pDesk->sGameData.T_CountFen.byUser;
//
//		}
//	}
//	else if(pDesk->sGameData.m_mjRule.byNextNtType == 4)
//	{//不管结构如果都是下家坐庄
//		pDesk->sUserData.m_byNtStation = GetNextStation(pDesk->sUserData.m_byNtStation);
//		pDesk->sUserData.m_byLianZhuang = 1;
//	}
//	pDesk->sGameData.T_CountFen.byNextNt = pDesk->sUserData.m_byNtStation;
//
//	pDesk->sGameData.ApplyThingID = nextID;
//	pDesk->GameFinish(pDesk->sGameData.T_HuPai.byUser,GF_NORMAL);
//}

/////强退结束事件
//void ThingHandle::UnnormalEnd(BYTE &nextID,BYTE &time)
//{
//	pDesk->sGameData.m_byThingDoing = THING_ENG_UN_NORMAL;
//	pDesk->sUserData.m_byNtStation = 255;

//	///非正常结束处理事件
//	//T_UnnormalEndHandle;		
//}

/////////////////////////其他处理////////////////////////////////////////////////////////////

/// 玩家断线
void ThingHandle::UserNetCut(BYTE bDeskStation)
{
	OutputDebugString("麻将-断线：UserNetCut");

	pDesk->sUserData.m_byFoceLeavCount++;			///断线玩家数
	pDesk->sUserData.m_bFoceLeave[bDeskStation] = true;
	if(pDesk->sUserData.m_byFoceLeavCount == 255)
	{
		pDesk->sUserData.m_byFoceLeavCount=0;
	}
	//发送托管消息
	Usertouguan touguan;
	touguan.desk=bDeskStation;
	touguan.touguang=true;
	pDesk->sUserData.m_bTuoGuan[bDeskStation]=true;
	for(int i=0;i<PLAY_COUNT;++i)
	{
		touguan.bTuoGuan[i] = pDesk->sUserData.m_bTuoGuan[i];
	}
	for(int i=0;i<PLAY_COUNT;i++)
		pDesk->SendGameDataEx(i,&touguan,sizeof(Usertouguan),MDM_GM_GAME_NOTIFY,ASS_AUTOCARD,0);
	pDesk->SendWatchDataEx(PLAY_COUNT,&touguan,sizeof(Usertouguan),MDM_GM_GAME_NOTIFY,ASS_AUTOCARD,0);

	NetCutAutoHandle();
		///断线处理
}
/// 玩家离桌
void ThingHandle::UserLeftDesk(BYTE bDeskStation)
{

}
/// 玩家重入
void ThingHandle::UserReCome(BYTE bDeskStation)
{
	Usertouguan touguan;
	touguan.desk=bDeskStation;
	touguan.touguang=false;
	pDesk->sUserData.m_bTuoGuan[bDeskStation]=false;

	for(int i=0;i<PLAY_COUNT;++i)
	{
		touguan.bTuoGuan[i] = pDesk->sUserData.m_bTuoGuan[i];
	}

	for(int i=0;i<PLAY_COUNT;i++)
		pDesk->SendGameDataEx(i,&touguan,sizeof(Usertouguan),MDM_GM_GAME_NOTIFY,ASS_AUTOCARD,0);
	pDesk->SendWatchDataEx(PLAY_COUNT,&touguan,sizeof(Usertouguan),MDM_GM_GAME_NOTIFY,ASS_AUTOCARD,0);

	if(pDesk->sUserData.m_byFoceLeavCount == 255)
	{
		pDesk->sUserData.m_byFoceLeavCount=0;
	}
	if(pDesk->sUserData.m_byFoceLeavCount>0)
	{
		pDesk->sUserData.m_byFoceLeavCount--;			///断线玩家数
	}
	pDesk->sUserData.m_bFoceLeave[bDeskStation] = false;
}

/// 处理计时器消息
void ThingHandle::OnGameTimer(UINT uTimerID)
{
	switch(uTimerID)
	{
	case 10://事件计时器id
		{
			pDesk->KillTimer(uTimerID);
			GameCPU(pDesk->sGameData.ApplyThingID);
		}
		break;
	case TIME_CHECK_GAME_MSG://消息超时，自动处理
		{
			//pDesk->KillTimer(uTimerID);
			AutoHandle();
		}
		break;

	}

}



/// 设置超时数据
void ThingHandle::SetOutTime(BYTE station,BYTE time,BYTE thing,bool effect)
{
	pDesk->sGameData.m_MjWait.byWaitThing = thing;
	pDesk->sGameData.m_MjWait.byCurChair = station;
	pDesk->sGameData.m_MjWait.byTime = time + 2;
	pDesk->sGameData.m_MjWait.bEffect = effect;
}
/// 恢复超时数据
void ThingHandle::ReSetOutTime()
{
	pDesk->sGameData.m_MjWait.Clear();
}
/// 超时处理
void ThingHandle::OutTimeHandle()
{
	if(!pDesk->sGameData.m_MjWait.bEffect || pDesk->sGameData.m_MjWait.byCurChair == 255 || pDesk->sGameData.m_MjWait.byTime == 255)
		return;
	if(pDesk->sGameData.m_MjWait.byWaitThing != pDesk->sGameData.m_byThingDoing)
		return;
	if(pDesk->sGameData.m_MjWait.byTime == 255)
		return;
	pDesk->sGameData.m_MjWait.byTime -= 1;
	if(pDesk->sGameData.m_MjWait.byTime == 0)
	{
		this->AutoHandle();
	}
}






