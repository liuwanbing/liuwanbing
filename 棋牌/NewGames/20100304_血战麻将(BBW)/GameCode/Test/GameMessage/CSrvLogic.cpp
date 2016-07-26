
#include "stdafx.h"
#include "CSrvLogic.h"
#include "../TestDlg.h"

CSrvLogic::CSrvLogic(void)
{	

	m_CPGAction.SetDeskPoint(this);//吃碰杠检测方法
	m_CheckHuPai.SetDeskPoint(this);//糊牌检测方法
	m_CountFen.SetDeskPoint(this);//算分
}
CSrvLogic::~CSrvLogic(void)
{
}
//获取父类的指针
void CSrvLogic::SetDeskPoint(CTestDlg *desk)
{
	pDesk = desk;
}

//断线重连事件：玩家坐下桌子就会通过该接口获取游戏的数据
bool CSrvLogic::GetGameStaton(BYTE bDeskStation, bool bWatchUser,BYTE GameStation)
{

	GameStationDataEx GameStationData;
	GameStationData.Init();
	GameStationData.iVersion	=  MAIN_VERSION;
	GameStationData.iVersion2	=  ASS_VERSION;
	//获取房间倍率逃跑罚分
	GameStationData.Stationpara = pDesk->GetGameStation();		 //游戏状态

	m_UserData.CopyAllGCPData(GameStationData.m_UserGCPData);
	m_UserData.CopyAllOutPai(GameStationData.m_byArOutPai);
	m_UserData.CopyAllMenPai(GameStationData.m_iArMenPai);
	m_UserData.CopyAllHandPai(GameStationData.m_byArHandPai,bDeskStation,false);
	if(!bWatchUser)
	{
		m_UserData.SetUserState(bDeskStation,USER_STATE_CUT,false);//取消断线状态
		m_UserData.SetUserState(bDeskStation,USER_STATE_AUTO,false);//取消托管状态
	}

	GameStationData.m_bitUserState = m_UserData.m_bitUserState;//获取玩家状态
	GameStationData.m_byOtherOutPai = m_UserData.m_byOtherOutPai;
	GameStationData.m_byThingDoing = m_GameData.m_byThingDoing;
	GameStationData.byNtStation = m_UserData.m_byNtStation;
	GameStationData.m_byNowOutStation = m_UserData.m_byNowOutStation;
	GameStationData.m_StrJing = m_UserData.m_StrJing;

	pDesk->GetUserAgree(GameStationData.ucAgree);//获取各个玩家的同意状态

	char path[500];
	sprintf(path,"%s%s_s.ini",CINIFile::GetAppPath (),SKIN_FOLDER);
	CINIFile pz(path);
	char key[500];
	sprintf(key,"game");
	m_GameData.m_MjSet.bAutoBegin	= pz.GetKeyVal(key,TEXT("bAutoBegin"),0);		//是否自动开始
	m_GameData.m_MjSet.byAutoTime = pz.GetKeyVal(key,TEXT("byAutoTime"),3);			//托管出牌时间
	m_GameData.m_MjSet.byBeginTime = pz.GetKeyVal(key,TEXT("byBeginTime"),30);		//开始等待时间
	m_GameData.m_MjSet.byBlockTime = pz.GetKeyVal(key,TEXT("byBlockTime"),15);		//拦牌等待时间
	m_GameData.m_MjSet.byOutcardTime = pz.GetKeyVal(key,TEXT("byOutcardTime"),20);	//出牌等待时间

	memcpy(&GameStationData.m_MjSet,&m_GameData.m_MjSet,sizeof(GameStationData.m_MjSet));//麻将设置（主要是时间设置）
	//char strdug[500];
	//sprintf(strdug,"事件： bAutoBegin %d byAutoTime %d byBeginTime %d byBlockTime %d byOutcardTime %d",GameStationData.m_MjSet.bAutoBegin,GameStationData.m_MjSet.byAutoTime,GameStationData.m_MjSet.byBeginTime,GameStationData.m_MjSet.byBlockTime,GameStationData.m_MjSet.byOutcardTime);
	//OutputDebugString(strdug);

	switch (GameStation)
	{
	case GS_WAIT_SETGAME:		//游戏没有开始状态
		{
			//发送数据
			SendGameStation(bDeskStation, bWatchUser, &GameStationData, sizeof(GameStationData));
			MessageStruct Mess;
			ZeroMemory(Mess.Message, 200);
			lstrcpy(Mess.Message, TEXT("歡迎參加麻將游戲,祝您玩的開心！"));
			SendGameData(bDeskStation, &Mess, sizeof(Mess), ASS_GM_GAME_NOTIFY, ASS_MESSAGE, 0);
		}
		break;
	case GS_WAIT_ARGEE:	//等待玩家开始状态
	case GS_SEND_CARD:	//发牌状态(还未发牌)
	case GS_PLAY_GAME:	//游戏中状态
	case GS_WAIT_NEXT:	//等待下一盘游戏开始
		{
			//发送数据
			SendGameStation(bDeskStation,  bWatchUser, &GameStationData, sizeof(GameStationData));
		}
		break;
	}
	return true;
}

///接收并处理消息
int CSrvLogic::HandleNotifyMessage(int bDeskStation, UINT code, BYTE* pData, int uSize, bool bWatchUser)
{
	pDesk->KillTimer(TIME_CHECK_GAME_MSG);
	pDesk->SetTimer(TIME_CHECK_GAME_MSG,TIME_DEFAULT_HANDLE);//重新设置超时计时器
	switch (code)
	{
	case ASS_SEZIFINISH:	//色子动画结束
		{
			if(m_GameData.m_byGameState != STATE_BEGIN_OUT)///如果是游戏中消息另加状态
			{
				OutputDebugString("状态检测： 事件 ReceiveSeziFinish 不在出牌状态");
				return 0;
			}
			return ReceiveSeziFinish(bDeskStation, pData, uSize, bWatchUser);
		}
	case ASS_AUTOCARD://托管消息
		{	
			return ReceiveUserTuoGuan(bDeskStation, pData, uSize, bWatchUser);
		}
		break;
	case EVENT_OUT_PAI:// 用户出牌消息
		{
			if(m_GameData.m_byGameState != STATE_OUT_CARD)
			{
				OutputDebugString("状态检测：事件 ReceiveUserOutCard 不在出牌状态");
				return 0;
			}
			return ReceiveUserOutCard(bDeskStation, pData, uSize, bWatchUser);
		}
		break;
	case ASS_PASS_CHECK://玩家放弃拦牌消息	
		{
			if(m_GameData.m_byGameState != STATE_OUT_CARD && m_GameData.m_byGameState != STATE_BLOCK_CARD && m_GameData.m_byGameState != STATE_QIANG_GANG)
			{
				OutputDebugString("状态检测：事件 ReceiveUserPassCheck 不在状态");
				return 0;
			}
			return ReceiveUserPassCheck(bDeskStation, pData, uSize,   bWatchUser);
		}
		break;
	case ASS_GM_AGREE_GAME:	//收到用户同意游戏消息	
		{
			if(m_GameData.m_byGameState != STATE_BEGIN_OUT && m_GameData.m_byGameState != STATE_FINISH)
			{
				OutputDebugString("状态检测：事件 ReceiveUserAgreeGame 不在状态");
				return 0;
			}
			return ReceiveUserAgreeGame(bDeskStation, pData, uSize,   bWatchUser);
		}
		break;
	case EVENT_CHI_PAI:	// 用户吃牌 消息
		{
			if(m_GameData.m_byGameState != STATE_BLOCK_CARD)
			{
				OutputDebugString("状态检测：事件 ReceiveUserChiCard 不在拦牌状态");
				return 0;
			}
			return ReceiveUserChiCard(bDeskStation, pData, uSize, bWatchUser);
		}
		break;
	case EVENT_PENG_PAI:// 用户碰牌 消息
		{
			if(m_GameData.m_byGameState != STATE_BLOCK_CARD)
			{
				OutputDebugString("状态检测：事件 ReceiveUserPengCard 不在拦牌状态");
				return 0;
			}
			return ReceiveUserPengCard(bDeskStation, pData, uSize, bWatchUser);
		}
		break;;
	case EVENT_GANG_PAI:// 用户杠牌 消息
		{
			if(m_GameData.m_byGameState != STATE_BLOCK_CARD && m_GameData.m_byGameState != STATE_OUT_CARD)
			{
				OutputDebugString("状态检测：事件 ReceiveUserGangCard 不在状态");
				return 0;
			}
			return ReceiveUserGangCard(bDeskStation, pData, uSize, bWatchUser);
		}
		break;
	case EVENT_TING_PAI:// 用户听牌 消息
		{
			if(m_GameData.m_byGameState != STATE_OUT_CARD)
			{
				OutputDebugString("状态检测：事件 ReceiveUserTingCard 不在状态");
				return 0;
			}
			return ReceiveUserTingCard(bDeskStation, pData, uSize, bWatchUser);
		}
		break;
	case EVENT_HU_PAI:	// 用户糊牌	 消息
		{
			if(m_GameData.m_byGameState != STATE_BLOCK_CARD && m_GameData.m_byGameState != STATE_OUT_CARD && m_GameData.m_byGameState != STATE_QIANG_GANG)
			{
				OutputDebugString("状态检测：事件 ReceiveUserHuCard 不在状态");
				return 0;
			}
			return ReceiveUserHuCard(bDeskStation, pData, uSize, bWatchUser);
		}
		break;
	case ASS_HAVE_THING://有事请求离开	 消息
		{
			return ReceiveUserHaveThing(bDeskStation, pData, uSize,   bWatchUser);
		}
		break;
	case ASS_LEFT_RESULT: //同意用户离开 消息
		{
			return ReceiveArgeeUserLeft(bDeskStation, pData, uSize,   bWatchUser);
		}
		break;
	case ASS_EXIT://等待开始一下局时退出
		{
			//if(pDesk->m_bGameStation==23)
			//	return pDesk->GameFinish(bDeskStation,GF_SALE);
			//else
			//	return 0;
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
int CSrvLogic::ReceiveSeziFinish(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser)
{
	if(!m_UserData.m_bWaitSezi || bWatchUser)//是否处理等待色子动画结束状态
		return 0;
	m_UserData.m_bSiziFinish[bDeskStation] = true;
	if(!m_UserData.IsAllSeziFinish())//有玩家没有返回色子结束消息
		return 0;
	m_UserData.SetWaiteSezi(false);	//不用再等待色子消息了	
	ApplyAction(255,500);		//事件id已经设置好了，申请就可以了
	return 0;
}

// 收到托管消息
int CSrvLogic::ReceiveUserTuoGuan(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser)
{
	if(bWatchUser || uSize != sizeof(Usertouguan))
		return 0;
	if(bWatchUser || pDesk->GetGameStation() > GS_PLAY_GAME || pDesk->GetGameStation() < GS_SEND_CARD)//不在游戏中不能托管
		return 0;
    Usertouguan *pCmd = (Usertouguan *)pData;
	Usertouguan touguan;
	touguan.desk = bDeskStation;
	touguan.touguang = pCmd->touguang;
	m_UserData.SetUserState(bDeskStation,USER_STATE_AUTO,touguan.touguang);//修改托管状态
	touguan.m_bitUserState = m_UserData.m_bitUserState;
	SendAllGameData(&touguan,sizeof(Usertouguan),ASS_GM_GAME_NOTIFY,ASS_AUTOCARD,0);
	return 0;
}

// 用户出牌
int CSrvLogic::ReceiveUserOutCard(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser)
{
	tagOutPaiEx * pOutCard = (tagOutPaiEx *)pData;
	if(pOutCard == NULL || bWatchUser || pDesk->GetGameStation() != GS_PLAY_GAME || uSize!=sizeof(tagOutPaiEx)
		||pOutCard->byPs == 255 || m_UserData.GetAHandPaiCount(bDeskStation,pOutCard->byPs)==0 || !m_UserData.IsOutPaiPeople(bDeskStation))
	{
		char strdug[500];
		sprintf(strdug,"事件： 不合法出动作 %d %d %d %d %d %d %d",bWatchUser,pDesk->GetGameStation() != GS_PLAY_GAME,uSize!=sizeof(tagOutPaiEx),pOutCard->byPs, m_UserData.GetAHandPaiCount(bDeskStation,pOutCard->byPs)==0 , !m_UserData.IsOutPaiPeople(bDeskStation),m_UserData.GetHandPaiCount(bDeskStation));
		OutputDebugString(strdug);

		return 0;
	}
	for(int i=0;i<PLAY_COUNT;++i)
	{
		if(i == bDeskStation)
			continue;
		m_UserData.SetUserState(i,USER_STATE_GANG,false);//消除杠牌状态
	}
	m_GameData.T_OutPai = * pOutCard;
	m_UserData.m_byOtherOutPai = pOutCard->byPs;
	
	OutputDebugString("事件：ReceiveUserOutCard  出牌成功");

	m_UserData.DelleteAHandPai(bDeskStation,pOutCard->byPs);//删除一张手牌
	m_UserData.AddToOutPai(bDeskStation,pOutCard->byPs);//添加一张出牌
	m_UserData.SetUserState(bDeskStation,USER_STATE_HAVE_OUT,true);//设置为出过牌状态
	m_GameData.ClearTempData();//清理临时数据：之前接受的吃碰杠数据
	//玩家点击了听牌
	m_GameData.T_OutPai.bTing = false;
	if(m_GameData.T_TingPai.bHitTing[bDeskStation] && !m_UserData.IsUserState(bDeskStation,USER_STATE_TING))//玩家点击了听牌
	{
		m_GameData.T_TingPai.Clear();
		if(CLogicBase::IsHaveInArr(m_GameData.T_CPGNotify[bDeskStation].m_byTingCanOut,pOutCard->byPs,HAND_CARD_NUM))
		{
			m_UserData.SetUserState(bDeskStation,USER_STATE_TING,true);
			m_GameData.T_OutPai.bTing = true;
		}
	}
	//拷贝出牌
	m_UserData.CopyAllOutPai(m_GameData.T_OutPai.m_byArOutPai);
	///当前抓牌方向，true 顺时针，false 逆时针
	m_UserData.m_bZhuaPaiDir = true;
	m_GameData.T_OutPai.m_bitUserState = m_UserData.m_bitUserState;//获取玩家状态信息
	//发送出牌数据
	for (int i=0;i<PLAY_COUNT;i++)
	{	
		m_UserData.SortHandPai(i,false);
		m_UserData.CopyAllHandPai(m_GameData.T_OutPai.m_byArHandPai,i,false);
		SendGameData(i, &m_GameData.T_OutPai, sizeof(m_GameData.T_OutPai), ASS_GM_GAME_NOTIFY, EVENT_OUT_PAI, 0);
	}
	//动作检测：没有人能响应就直接抓牌
	if(!CheckAction(bDeskStation,pOutCard->byPs,2))
	{//没有动作就直接下一个玩家抓牌
		ApplyAction(EVENT_ZHUA_PAI,500);	//申请下一个事件

		OutputDebugString("事件：ReceiveUserOutCard 没有吃碰杠");
	}
	else
	{
		OutputDebugString("事件：ReceiveUserOutCard 检测到有吃碰杠");

		SetGameState(STATE_BLOCK_CARD);//设置游戏状态
		for (int i=0;i<PLAY_COUNT;i++)
		{	
			m_GameData.T_CPGNotify[i].byUser = i;
			if(i==bDeskStation || m_UserData.IsUserState(i,USER_STATE_CUT) || m_GameData.T_CPGNotify[i].m_byMaxThing == ACTION_NO)
				continue;
			SendGameData(i, &m_GameData.T_CPGNotify[i], sizeof(m_GameData.T_CPGNotify[i]), ASS_GM_GAME_NOTIFY, EVENT_CPG_NOTIFY, 0);
		}
	}
	m_UserData.m_byNowOutStation = CLogicBase::GetNextStation(m_UserData.m_byNowOutStation);//抓牌位置,改变了位置
	SetDoingThing(EVENT_OUT_PAI,bDeskStation);
	return 0;
}
// 收到用户pass 消息
int CSrvLogic::ReceiveUserPassCheck(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser)
{
	if(m_GameData.ApplyThingID != 255 || bWatchUser)//有申请事件就不响应pass消息
		return 0;
	m_GameData.T_TingPai.Clear();//取消听牌点击状态
	m_GameData.AcceptAction(bDeskStation,ACTION_NO);//设置动作接收状态
	CheckNextAction();	
	return 0;
}
//收到用户同意游戏消息
int CSrvLogic::ReceiveUserAgreeGame(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser)
{
	//用户同意游戏
	if (bWatchUser || (pDesk->GetGameStation() != GS_WAIT_ARGEE) && (pDesk->GetGameStation() != GS_WAIT_NEXT)) 
		return 0;
	SendAllGameData( ASS_GM_GAME_NOTIFY, ASS_GM_AGREE_GAME, 0);
	return 0;
}

//有事请求离开
int CSrvLogic::ReceiveUserHaveThing(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser)
{
	return 0;
}
//同意用户离开
int CSrvLogic::ReceiveArgeeUserLeft(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser)
{
	return 0;
}
// 用户吃牌消息
int CSrvLogic::ReceiveUserChiCard(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser)
{
	tagChiPaiEx * pChiCard = (tagChiPaiEx *)pData;
	if(pChiCard == NULL || bWatchUser || pDesk->GetGameStation() != GS_PLAY_GAME || uSize!=sizeof(tagChiPaiEx)
	|| !m_GameData.T_CPGNotify[bDeskStation].bChi || pChiCard->byPs == 255)
	{//不合法动作
		char strdug[500];
		sprintf(strdug,"事件： 不合法吃动作 %d %d %d %d %d",bWatchUser,pDesk->GetGameStation() != GS_PLAY_GAME,uSize!=sizeof(tagChiPaiEx),!m_GameData.T_CPGNotify[bDeskStation].bChi,pChiCard->byPs == 255);
		OutputDebugString(strdug);
		return 0;
	}
	memcpy(&m_GameData.temp_ChiPai,pChiCard,sizeof(m_GameData.temp_ChiPai));//拷贝保存消息数据
	m_GameData.temp_ChiPai.byBeChi = m_GameData.T_CPGNotify[bDeskStation].byOutStation;//记录出牌位置
	m_GameData.AcceptAction(pChiCard->byUser,ACTION_CHI);
	//检测下一个动作
	CheckNextAction();
	OutputDebugString(TEXT("事件：ReceiveUserChiCard 成功接收"));
	return 0;
}
// 用户碰牌消息
int CSrvLogic::ReceiveUserPengCard(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser)
{
	tagPengPaiEx * pPengCard = (tagPengPaiEx *)pData;
	if(pPengCard == NULL || bWatchUser ||  pDesk->GetGameStation() != GS_PLAY_GAME || uSize!=sizeof(tagPengPaiEx)
	|| !m_GameData.T_CPGNotify[bDeskStation].bPeng || pPengCard->byPs == 255)
	{
		return 0;
	}
	memcpy(&m_GameData.temp_PengPai,pPengCard,sizeof(m_GameData.temp_PengPai));//拷贝保存消息数据
	m_GameData.temp_PengPai.byBePeng = m_GameData.T_CPGNotify[bDeskStation].byOutStation;//记录出牌位置
	m_GameData.AcceptAction(pPengCard->byUser,ACTION_PENG);//设置接收的动作：判断下一个动作是什么时需要使用设置的数据
	//检测下一个动作
	CheckNextAction();
	return 0;
}
// 用户杠牌消息
int CSrvLogic::ReceiveUserGangCard(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser)
{
	tagGangPaiEx * pGangCard = (tagGangPaiEx *)pData;
	if(pGangCard == NULL || bWatchUser || pDesk->GetGameStation() != GS_PLAY_GAME || uSize!=sizeof(tagGangPaiEx)
		|| !m_GameData.T_CPGNotify[bDeskStation].bGang || pGangCard->byPs == 255)
	{//不合法动作
		OutputDebugString("事件：杠牌不合法");
		return 0;
	}
	bool have = false;
	for(int i=0;i<4;++i)
	{
		if(pGangCard->byType == m_GameData.T_CPGNotify[bDeskStation].m_iGangData[i][0])//存在该类型
		{
			have = true;
		}
	}
	if(!have)
	{
		OutputDebugString("事件：不存在该类型杠牌");
		return 0;
	}
	bool action = false;
	memcpy(&m_GameData.temp_GangPai,pGangCard,sizeof(m_GameData.temp_GangPai));//拷贝保存消息数据
	m_GameData.temp_GangPai.byBeGang = m_GameData.T_CPGNotify[bDeskStation].byOutStation;//记录出牌位置
	if(pGangCard->byType == ACTION_BU_GANG)
	{
		m_GameData.temp_GangPai.bChenkQ = true;
		m_GameData.T_CPGNotify[bDeskStation].bQiangGang = true;
		action = CheckAction(pGangCard->byUser,pGangCard->byPs,3);//检测是否有截取补杠的动作：抢杠胡牌等
	}

	switch(pGangCard->byType)
	{
	case ACTION_AN_GANG://暗杠可以立即申请执行
		{
			ApplyAction(EVENT_GANG_PAI,2*100);//申请执行杠牌事件
		}
		break;
	case ACTION_BU_GANG:
		{
			m_GameData.AcceptAction(pGangCard->byUser,ACTION_BU_GANG);//设置接收的动作：判断下一个动作是什么时需要使用设置的数据
			if(!action)//无人抢杠，直接处理
			{
				ApplyAction(EVENT_GANG_PAI,2*100);//申请执行杠牌事件
			}
			else///有人抢杠，发送抢杠消息给别人检测
			{
				SetGameState(STATE_QIANG_GANG);//设置游戏状态
				for (int i=0;i<PLAY_COUNT;i++)
				{	
					m_GameData.T_CPGNotify[i].byUser = i;
					if(!m_GameData.T_CPGNotify[i].bCanAction || m_UserData.IsUserState(i,USER_STATE_CUT))
						continue;
					SendGameData(i,&m_GameData.T_CPGNotify[i],sizeof(m_GameData.T_CPGNotify[i]),ASS_GM_GAME_NOTIFY, EVENT_CPG_NOTIFY, 0);
				}
			}
		}
		break;
	case ACTION_MING_GANG://明杠
		{
			m_GameData.AcceptAction(pGangCard->byUser,ACTION_MING_GANG);//设置接收的动作：判断下一个动作是什么时需要使用设置的数据
			CheckNextAction();//检测下一个动作
		}
		break;
	}
	return 0;
}
// 用户听牌消息
int CSrvLogic::ReceiveUserTingCard(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser)
{
	tagTingPaiEx * pTingCard = (tagTingPaiEx *)pData;
	if(pTingCard == NULL || bWatchUser || pDesk->GetGameStation() != GS_PLAY_GAME || uSize!=sizeof(tagTingPaiEx)
		|| !m_GameData.T_CPGNotify[bDeskStation].bTing  )
	{//不合法动作
		return 0;
	}
	m_GameData.AcceptAction(pTingCard->byUser,ACTION_TING);//设置接收的动作：判断下一个动作是什么时需要使用设置的数据
	m_GameData.T_TingPai.bHitTing[pTingCard->byUser] = true;//记录玩家点击了听牌
	//通知所有玩家
	SendAllGameData( &m_GameData.T_TingPai, sizeof(m_GameData.T_TingPai), ASS_GM_GAME_NOTIFY, EVENT_TING_PAI, 0);
	return 0;
}
// 用户糊牌消息
int CSrvLogic::ReceiveUserHuCard(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser)
{
	tagHuPaiEx * pHuCard = (tagHuPaiEx *)pData;
	if(pHuCard == NULL || bWatchUser || pDesk->GetGameStation() != GS_PLAY_GAME || uSize!=sizeof(tagHuPaiEx) || !m_GameData.T_CPGNotify[bDeskStation].bHu)
	{//不合法动作
		return 0;
	}
	m_GameData.AcceptAction(pHuCard->byUser,ACTION_HU);//设置接收的动作：判断下一个动作是什么时需要使用设置的数据

	m_GameData.T_HuPai.bZimo = m_GameData.T_CPGNotify[bDeskStation].bZimo;
	m_GameData.T_HuPai.byDianPao = m_GameData.T_CPGNotify[bDeskStation].m_byDianPao;
	m_GameData.T_HuPai.bCanH[pHuCard->byUser] = true;
	m_GameData.T_HuPai.byUserNum++;
	m_GameData.T_HuPai.byNt = m_UserData.m_byNtStation;		//庄家位置
	m_GameData.T_HuPai.byPs = m_GameData.T_CPGNotify[bDeskStation].m_iHuPs;	//点炮的牌值
	

	if(m_GameData.T_HuPai.byUser == 255)
	{
		m_GameData.T_HuPai.byUser = pHuCard->byUser;
	}
	else if(!m_GameData.T_HuPai.bZimo)//选择靠糊牌玩家最近的玩家糊牌
	{//点炮糊牌，并且有多个玩家可以糊牌
		m_GameData.T_HuPai.bDuoXiang = true;//标志多响
		m_GameData.T_HuPai.byUser = CLogicBase::GetCanHuUser(m_GameData.T_CPGNotify[bDeskStation].m_byDianPao,m_GameData.T_HuPai.bCanH,m_GameData.T_HuPai.byUser);//获取离糊牌玩家近的玩家
	}

	memcpy(m_GameData.T_HuPai.byHuType[pHuCard->byUser],m_GameData.T_CPGNotify[pHuCard->byUser].byHuType,sizeof(m_GameData.T_CPGNotify[pHuCard->byUser].byHuType));//拷贝糊牌类型
	m_UserData.CopyAllGCPData(m_GameData.T_HuPai.m_UserGCPData);//拷贝吃碰杠数据
	if(m_GameData.T_HuPai.bZimo)//玩家自摸可以立即
	{
		ApplyAction(EVENT_HU_PAI,2*100);//申请下一个事件
	}
	else
	{
		//检测下一个动作
		CheckNextAction();
	}
	return 0;
}

///收到超级客户端设置牌消息
void CSrvLogic::ReceiveSuperChangePai(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser)
{
	if(bWatchUser || bDeskStation<0 || bDeskStation>3)
		return;
	SuperStruct *Super =(SuperStruct* )pData;
	bool cpg = false;
	m_UserData.m_SetData[bDeskStation].Init();
	m_UserData.m_SetData[bDeskStation].bySetType = Super->bySetType;
	m_UserData.m_SetData[bDeskStation].mjcount = Super->mjcount;
	for(int i=0;i<Super->mjcount;++i)
	{
		m_UserData.m_SetData[bDeskStation].mjdata[i] = Super->mjdata[i];
		m_UserData.m_SetData[bDeskStation].changedata[i] = Super->changedata[i];
	}
	OutputDebugString("服务器 事件：收到设置牌消息");
	//设置类型：0 换所有牌，1换选定牌，2设置自己的下一张牌，3设置牌强的下一张牌
	switch(Super->bySetType)
	{
	case 0://换所有手牌
		{
			m_UserData.ChangeAllHandPai(bDeskStation,Super->mjdata,Super->mjcount);
		}
		break;
	case 1://换部分手牌
		{
			m_UserData.ChangeSomeHandPai(bDeskStation,Super->mjdata,Super->changedata,Super->mjcount);
		}	
		break;
	case 2://设置自己的下一张牌
		{	
			m_UserData.m_bySetMoPai[bDeskStation] = Super->mjdata[0];
		}
		break;
	case 3://设置牌墙的下一张牌
		{
			m_UserData.SetGameNextPai(Super->mjdata[0]);
		}
		break;
	}
	if(Super->bySetType == 0 || Super->bySetType == 1)
	{
		if(bDeskStation == m_UserData.m_byNowOutStation)/////如果是当前出牌玩家，检测并返回检测结果
		{
			if(CheckAction(bDeskStation,m_UserData.GetLastHandPai(bDeskStation),0))
			{
				cpg = true;
			}
		}
		OutputDebugString("服务器 事件：返回设置牌数据");
		SuperStruct superdata = *Super;
		memcpy(superdata.mjdata,m_UserData.m_byArHandPai[bDeskStation],sizeof(superdata.mjdata));
		SendGameData(bDeskStation, &superdata, sizeof(SuperStruct), ASS_GM_GAME_NOTIFY, ASS_SUPER, 0);
	}
	if(cpg)//换牌后能进行吃碰杠操作
	{
		if(!m_UserData.IsUserState(bDeskStation,USER_STATE_CUT))
		{
			m_GameData.T_CPGNotify[bDeskStation].byUser = bDeskStation;
			SendGameData(bDeskStation, &m_GameData.T_CPGNotify[bDeskStation], sizeof(tagCPGNotifyEx), ASS_GM_GAME_NOTIFY, EVENT_CPG_NOTIFY, 0);
		}
	}
}

///名称：CheckNextAction
///描述：检测下一步动作,玩家发送拦牌消息或者放弃消息就用本函数判断是要继续等待消息还行可以进行下一步动作
void CSrvLogic::CheckNextAction()
{
	BYTE max = 0;
	if(!IsCanHandle(max))//需要继续等待消息，说明有更高优先级的动作要等待
	{
		char strdug[500];
		sprintf(strdug,"事件：IsCanHandle 存在更大事件 %d",max);
		OutputDebugString(strdug);
		return ;
	}	
	switch(m_GameData.m_byNextAction)//确定了下一步的响应事件：吃，碰，杠，胡
	{
	case ACTION_CHI://吃牌动作
		{
			ApplyAction(EVENT_CHI_PAI,100);//申请执行吃牌事件
		}
		break;
	case ACTION_PENG:			//碰牌动作
		{
			ApplyAction(EVENT_PENG_PAI,100);//申请执行碰牌事件
		}
		break;
	case ACTION_BU_GANG://补杠动作
	case ACTION_MING_GANG:			//明杠动作
		{
			ApplyAction(EVENT_GANG_PAI,100);//申请执行杠牌事件
		}
		break;
	case ACTION_HU:					//糊动作
		{
			ApplyAction(EVENT_HU_PAI,100);//申请执行糊牌事件
		}
		break;
	case ACTION_NO:
		{
			if(m_GameData.m_byThingDoing == EVENT_OUT_PAI)//出牌事件
			{
				ApplyAction(EVENT_ZHUA_PAI,2*100);//申请执行抓牌事件
			}
		}
		break;
	}
}

///检查是否可以进行下一步处理
bool CSrvLogic::IsCanHandle(BYTE &mx)
{
	BYTE max = ACTION_NO;
	BYTE station = m_GameData.T_OutPai.byUser;
	if(m_GameData.m_byGameState == STATE_QIANG_GANG)//抢杠状态
	{//抢杠胡等操作
		station = m_GameData.T_GangPai.byUser;
		max = mx = m_GameData.GetMaxRequet(station);
		if(max == ACTION_NO)
		{
			m_GameData.m_byNextAction = ACTION_BU_GANG;//继续执行完补杠
			return true;
		}
	}
	else//一般出牌检查
	{
		max = mx = m_GameData.GetMaxRequet(station);
		if(max == ACTION_NO)//都放弃拦牌了，就启动发牌事件
		{
			m_GameData.m_byNextAction = ACTION_NO;
			return true;
		}
	}
	//最大动作都回应了，可以执行了
	if(m_GameData.IsNotifyAllReply(station,max))
	{			
		m_GameData.m_byNextAction = max;
		return true;
	}
	return false;
}
///////////流程事件处理///////////////////////////////////////////////////////////////////////////////////////

//请求事件,type要申请的事件
void CSrvLogic::ApplyAction(int type,int time)
{
	char strdug[500];
	sprintf(strdug,"事件：ApplyAction type =%d time = %d",type, time);
	OutputDebugString(strdug);

	if(type != 255)
	{
		m_GameData.ApplyThingID = type;
	}
	if(m_GameData.ApplyThingID != 255)
	{
		pDesk->SetTimer(10,time);
	}
}

///名称：GameCPU
///描述：游戏事件处理中心，
///@param type要处理的事件代号
void CSrvLogic::GameCPU(int type)
{
	char strdug[500];
	sprintf(strdug,"事件：GameCPU type =%d 出牌位置 %d",type,m_UserData.m_byNowOutStation);
	OutputDebugString(strdug);

	////设置等待色子动画结束状态
	BYTE nextID = 255,time = 255;
	switch(type)
	{
	case EVENT_BEGIN://游戏开始事件
		{
			ExecuteGameBegin(nextID,time);
		}
		break;
	case EVENT_DONG_NT://首局以东为庄事件
		{
			ExecuteDongNt(nextID,time);
		}
		break;
	case EVENT_2SEZI_NT	://首局掷2颗色子的点数和为庄事件
		{
			ExecuteTwoSeziNt(nextID,time);
		}
		break;
	case EVENT_2SEZI_NT_GP://首局掷2颗色子定庄家和起牌点事件
		{
			ExecuteTwoSeziNtAndGp(nextID,time);
		}
		break;
	case EVENT_2SEZI_DIR://掷2颗色子事件定起牌方向事件
		{
			ExecuteTwoSeziDir(nextID,time);
		}
		break;
	case EVENT_2SEZI_DIR_GP	://掷2颗色子事件定起牌方向和起牌点事件
		{
			ExecuteTwoSeziDirGp(nextID,time);
		}
		break;
	case EVENT_2SEZI_GP	://掷2颗色子事件定起牌点事件
		{
			ExecuteTwoSeziGp(nextID,time);
		}
		break;
	case EVENT_2SEZI_NT_DIR_GP://掷色子确定庄家方向顿数
		{
			ExecuteTwoSeziNtDirGp(nextID,time);
		}
		break;
	case EVENT_SEND_PAI://发牌事件
		{
			pDesk->SetGameStation(GS_SEND_CARD) ;
			ExecuteSendPai(nextID,time);
		}
		break;
	case EVENT_2SEZI_JING://掷2颗色子定精牌事件
		{
			ExecuteTwoSeziMakeJing(nextID,time);
		}
		break;
	case EVENT_ALL_BUHUA://为所有玩家补花事件
		{
			ExecuteAllBuHua(nextID,time);
		}
		break;
	case EVENT_ONE_BUHUA://单个玩家抓到花牌后补花事件
		{
			ExecuteOneBuHua(nextID,time);
		}
		break;
	case EVENT_BEGIN_OUT://庄家开始发牌通知事件
		{
			pDesk->SetGameStation(GS_PLAY_GAME) ;
			ExecuteBeginOut(nextID,time);
		}
		break;
	case EVENT_ZHUA_PAI	://抓牌事件
		{	
			ExecuteZhuaPai(nextID,time);
		}
		break;
	case EVENT_CHI_PAI	://吃牌事件
		{	
			ExecuteChiPai(nextID,time);
		}
		break;
	case EVENT_PENG_PAI	://碰牌事件
		{	
			ExecutePengPai(nextID,time);
		}
		break;
	case EVENT_GANG_PAI	://杠牌事件
		{	
			ExecuteGangPai(nextID,time);
		}
		break;
	case EVENT_HU_PAI://糊牌事件
		{
			ExecuteHuPai(nextID,time);
		}
		break;
	case EVENT_COUNT_FEN://算分事件
		{
			ExecuteCountFen(nextID,time);
		}
		break;
	case EVENT_END_HANDLE://结束处理事件
		{
			ExecuteNormalEnd(nextID,time);
		}
		break;
	case EVENT_ENG_UN_NORMAL://异常结束处理事件
		{
			ExecuteUnNormalEnd(nextID,time);
		}
		break;
	}
	m_GameData.ApplyThingID = nextID;
	ApplyAction(nextID,time*100);//申请下一个事件
}

///开始事件处理
void CSrvLogic::ExecuteGameBegin(BYTE &nextID,BYTE &time)
{
	pDesk->KillTimer(TIME_WAIT_MSG);
	pDesk->SetTimer(TIME_WAIT_MSG,1000);//重新设置超时计时器

	time = 10;
	m_GameData.InitData();//初始化数据
	m_UserData.InitData();

	if(m_UserData.m_byNtStation!=255)//有庄家了
	{//已经有了庄家
		nextID = EVENT_2SEZI_DIR_GP;//掷色子发牌
	}
	else 
	{
		nextID = EVENT_DONG_NT;//执行以东为庄事件
	}
	m_GameData.T_Begin.byNt = m_UserData.m_byNtStation;			// 庄家位置			
	m_GameData.T_Begin.byPlayCount = m_UserData.byPlayCount;	// 游戏进行的局数	
	m_GameData.T_Begin.byQuanFeng =  m_UserData.m_byQuanFeng;	// 圈风设置			
	for(int i=0;i<4;i++)
	{
		for(int j=0;j<m_GameData.m_mjRule.byAllMjCount/4;j++)
		{
			m_GameData.T_Begin.byMenPai[i][j] = 0; //门牌
		}
	}
	///当前抓牌方向，true 顺时针，false 逆时针
	m_UserData.m_bZhuaPaiDir = true;
	m_UserData.m_MenPai.byGetPaiDir = m_UserData.m_byNtStation;//起牌方向(每局初始化为庄家门前起牌)
	//发送开始消息
	SendAllGameData(&m_GameData.T_Begin,sizeof(m_GameData.T_Begin),ASS_GM_GAME_NOTIFY,EVENT_BEGIN,0);
	//超时设置，
	SetDoingThing(EVENT_BEGIN,0);//设置正在发生的事件
	SetGameState(STATE_BEGIN_OUT);//设置游戏状态
}

///以第一个玩家为庄家（以东为庄事件）
void CSrvLogic::ExecuteDongNt(BYTE &nextID,BYTE &time)
{
	nextID = EVENT_2SEZI_DIR_GP;
	time = 2;	//200毫秒间隔
	m_GameData.T_DongNt.byNt = m_UserData.m_byNtStation = 0;			// 庄家位置	
	m_UserData.m_MenPai.byGetPaiDir = m_UserData.m_byNtStation;//起牌方向(每局初始化为庄家门前起牌)
	//发送游戏消息
	SendAllGameData(&m_GameData.T_DongNt,sizeof(m_GameData.T_DongNt),ASS_GM_GAME_NOTIFY,EVENT_DONG_NT,0);
	//超时设置
	SetDoingThing(EVENT_DONG_NT,0);//设置正在发生的事件
}

///掷2颗色子确定庄家（点数和）
void CSrvLogic::ExecuteTwoSeziNt(BYTE &nextID,BYTE &time)	
{
	nextID = EVENT_2SEZI_DIR_GP;//下个事件的id
	time = 2;	//200毫秒间隔
	///掷2颗色子的点数和为庄事件
	m_GameData.T_TwoSeziNt.byUser = 0;//掷色子位置
	if(m_UserData.m_byNtStation!=255)
	{//已经有庄家了就不掷色子了
		return;
	}
	time = 50;	//5秒间隔
	CLogicBase::GetSeziData(&m_GameData.T_TwoSeziNt.bySezi0,&m_GameData.T_TwoSeziNt.bySezi1);
	m_UserData.m_byNtStation = (m_GameData.T_TwoSeziNt.bySezi0 + m_GameData.T_TwoSeziNt.bySezi1)%4;//庄家位置：2色子之和
	m_GameData.T_TwoSeziNt.byNt = m_UserData.m_byNtStation;
	m_UserData.m_MenPai.byGetPaiDir = m_UserData.m_byNtStation;//起牌方向(每局初始化为庄家门前起牌)
	//发送游戏消息
	SendAllGameData( &m_GameData.T_TwoSeziNt, sizeof(m_GameData.T_TwoSeziNt), ASS_GM_GAME_NOTIFY, EVENT_2SEZI_NT, 0);
	//设置等待色子动画结束状态
	m_UserData.SetWaiteSezi(true);
	//超时设置
	SetDoingThing(EVENT_2SEZI_NT,0);//设置正在发生的事件
}
///掷2颗色子确定庄家和起牌顿数（点数和庄家，大点顿数）
void CSrvLogic::ExecuteTwoSeziNtAndGp(BYTE &nextID,BYTE &time)
{
	nextID = EVENT_SEND_PAI;
	time=50;
	CLogicBase::GetSeziData(&m_GameData.T_TwoSeziNtAndGetPai.bySezi0,&m_GameData.T_TwoSeziNtAndGetPai.bySezi1);
	m_UserData.m_byNtStation = (m_GameData.T_TwoSeziNtAndGetPai.bySezi0 + m_GameData.T_TwoSeziNtAndGetPai.bySezi1)%4;//庄家位置：2色子之和
	m_GameData.T_TwoSeziNtAndGetPai.byNt = m_UserData.m_byNtStation;
	m_UserData.m_MenPai.byGetPaiDir = m_UserData.m_byNtStation;//起牌方向(每局初始化为庄家门前起牌)
	m_UserData.m_MenPai.byGetPai = m_GameData.T_TwoSeziNtAndGetPai.bySezi0*2%13;
	if(m_GameData.T_TwoSeziNtAndGetPai.bySezi1>m_GameData.T_TwoSeziNtAndGetPai.bySezi0)
	{
		m_UserData.m_MenPai.byGetPai = m_GameData.T_TwoSeziNtAndGetPai.bySezi1*2%13;
	}
	//发送游戏消息
	SendAllGameData( &m_GameData.T_TwoSeziNtAndGetPai, sizeof(m_GameData.T_TwoSeziNtAndGetPai), ASS_GM_GAME_NOTIFY, EVENT_2SEZI_NT_GP, 0);
	//设置等待色子动画结束状态
	m_UserData.SetWaiteSezi(true);
	//超时设置
	SetDoingThing(EVENT_2SEZI_NT_GP,0);//设置正在发生的事件
}

///掷2颗色子确定庄家+起牌方向+起牌敦数（点数和庄家，大点方向，小点敦数）
void CSrvLogic::ExecuteTwoSeziNtDirGp(BYTE &nextID,BYTE &time)	
{
	nextID = EVENT_SEND_PAI;
	time=50;
	m_GameData.T_TwoSeziNtAndGetPai.byUser = 0;
	CLogicBase::GetSeziData(&m_GameData.T_TwoSeziNtAndGetPai.bySezi0,&m_GameData.T_TwoSeziNtAndGetPai.bySezi1);
	if(m_UserData.m_byNtStation!=255)
	{//已经有了庄家
		m_GameData.T_TwoSeziNtAndGetPai.byUser = m_UserData.m_byNtStation;
	}
	else
	{
		m_UserData.m_byNtStation = (m_GameData.T_TwoSeziNtAndGetPai.bySezi0 + m_GameData.T_TwoSeziNtAndGetPai.bySezi1)%4;
	}
	m_GameData.T_TwoSeziNtAndGetPai.byNt = m_UserData.m_byNtStation;
	m_GameData.T_TwoSeziNtAndGetPai.byGetPai = m_GameData.T_TwoSeziNtAndGetPai.bySezi0;
	m_UserData.m_MenPai.byGetPaiDir = m_GameData.T_TwoSeziNtAndGetPai.bySezi1%4;
	if(m_GameData.T_TwoSeziNtAndGetPai.bySezi0>m_GameData.T_TwoSeziNtAndGetPai.bySezi1)
	{
		m_GameData.T_TwoSeziNtAndGetPai.byGetPai = m_GameData.T_TwoSeziNtAndGetPai.bySezi1;
		m_UserData.m_MenPai.byGetPaiDir = m_GameData.T_TwoSeziNtAndGetPai.bySezi0%4;
	}
	//发送游戏消息
	SendAllGameData( &m_GameData.T_TwoSeziNtAndGetPai, sizeof(m_GameData.T_TwoSeziNtAndGetPai), ASS_GM_GAME_NOTIFY, EVENT_2SEZI_NT, 0);
	//设置等待色子动画结束状态
	m_UserData.SetWaiteSezi(true);
	//超时设置
	SetDoingThing(EVENT_2SEZI_NT_GP,0);//设置正在发生的事件
}

///掷2颗色子确定起牌方向（点数和确定方向）
void CSrvLogic::ExecuteTwoSeziDir(BYTE &nextID,BYTE &time)	
{
	nextID = EVENT_2SEZI_GP;//准备执行掷色子确定棋牌顿数事件
	time=50;
	m_GameData.T_TwoSeziDir.byUser = m_UserData.m_byNtStation;
	m_GameData.T_TwoSeziDir.byNt = m_UserData.m_byNtStation;
	CLogicBase::GetSeziData(&m_GameData.T_TwoSeziDir.bySezi0,&m_GameData.T_TwoSeziDir.bySezi1);
	m_UserData.m_MenPai.byGetPaiDir = (m_GameData.T_TwoSeziDir.bySezi1 + m_GameData.T_TwoSeziDir.bySezi0)%4;
	m_GameData.T_TwoSeziDir.byDirection = m_UserData.m_MenPai.byGetPaiDir;
	//发送游戏消息
	SendAllGameData(&m_GameData.T_TwoSeziDir, sizeof(m_GameData.T_TwoSeziDir), ASS_GM_GAME_NOTIFY, EVENT_2SEZI_DIR, 0);
	//设置等待色子动画结束状态
	m_UserData.SetWaiteSezi(true);
	//超时设置
	SetDoingThing(EVENT_2SEZI_DIR,0);//设置正在发生的事件
}

///掷2颗色子确定起牌方向和起牌敦数（点数和方向，小点敦数）
void CSrvLogic::ExecuteTwoSeziDirGp(BYTE &nextID,BYTE &time)	
{	
	nextID = EVENT_SEND_PAI;
	time=50;
	m_GameData.T_TwoSeziDirAndGetPai.byUser = m_UserData.m_byNtStation;
	CLogicBase::GetSeziData(&m_GameData.T_TwoSeziDirAndGetPai.bySezi0,&m_GameData.T_TwoSeziDirAndGetPai.bySezi1);
	m_UserData.m_MenPai.byGetPaiDir = (m_GameData.T_TwoSeziDirAndGetPai.bySezi1 + m_GameData.T_TwoSeziDirAndGetPai.bySezi0)%4-1;//起牌方向
	m_UserData.m_MenPai.byGetPai = m_GameData.T_TwoSeziDirAndGetPai.bySezi0;	//起牌敦数
	if(m_GameData.T_TwoSeziDirAndGetPai.bySezi0 > m_GameData.T_TwoSeziDirAndGetPai.bySezi1)
	{
		m_UserData.m_MenPai.byGetPai = m_GameData.T_TwoSeziDirAndGetPai.bySezi1;
	}
	m_GameData.T_TwoSeziDirAndGetPai.byGetPai = m_UserData.m_MenPai.byGetPai;

	char str[500];
	sprintf(str,"事件：ExecuteTwoSeziDirGp 位置%d 点数：%d %d",m_GameData.T_TwoSeziDirAndGetPai.byUser,m_GameData.T_TwoSeziDirAndGetPai.bySezi0,m_GameData.T_TwoSeziDirAndGetPai.bySezi1);
	OutputDebugString(str);

	//发送游戏消息
	SendAllGameData( &m_GameData.T_TwoSeziDirAndGetPai, sizeof(m_GameData.T_TwoSeziDirAndGetPai), ASS_GM_GAME_NOTIFY, EVENT_2SEZI_DIR_GP, 0);
	//设置等待色子动画结束状态
	m_UserData.SetWaiteSezi(true);
	//超时设置
	SetDoingThing(EVENT_2SEZI_DIR_GP,0);//设置正在发生的事件
}

///掷2颗色子确定起牌敦数（点数和敦数）
void CSrvLogic::ExecuteTwoSeziGp(BYTE &nextID,BYTE &time)	
{
	nextID = EVENT_SEND_PAI	;//准备执行发牌事件	
	time = 50;
	m_GameData.T_TwoSeziGetPai.byUser = m_UserData.m_byNtStation;
	CLogicBase::GetSeziData(&m_GameData.T_TwoSeziGetPai.bySezi0,&m_GameData.T_TwoSeziGetPai.bySezi1);
	m_UserData.m_MenPai.byGetPai = (m_GameData.T_TwoSeziGetPai.bySezi1 + m_GameData.T_TwoSeziGetPai.bySezi0)%12;
	m_GameData.T_TwoSeziGetPai.byGetPai = m_UserData.m_MenPai.byGetPai;
	//发送游戏消息
	SendAllGameData( &m_GameData.T_TwoSeziGetPai, sizeof(m_GameData.T_TwoSeziGetPai), ASS_GM_GAME_NOTIFY, EVENT_2SEZI_GP, 0);
	//设置等待色子动画结束状态
	m_UserData.SetWaiteSezi(true);
	//超时设置
	SetDoingThing(EVENT_2SEZI_GP,0);//设置正在发生的事件
}

///发牌
void CSrvLogic::ExecuteSendPai(BYTE &nextID,BYTE &time)	
{
	nextID = EVENT_BEGIN_OUT;//下一个事件的代号：开始出牌
	time = 2;
	//打乱牌
	DisPatchCard();
	//发牌
	m_UserData.SendCard();
	//分配门牌
	ChangeMenPai();
	m_UserData.CopyAllMenPai(m_GameData.T_SendPai.byMenPai);//拷贝门牌

	m_GameData.T_SendPai.byAllPaiNum = m_UserData.m_MenPai.byAllPaiNum;//牌总数
	m_GameData.T_SendPai.byStart = m_UserData.m_MenPai.byStart;//拿牌开始位置
	m_GameData.T_SendPai.byEnd = m_UserData.m_MenPai.byEnd;//拿牌结束位置

	for(int i=0;i<PLAY_COUNT;i++)
	{	
		m_UserData.CopyAllHandPai(m_GameData.T_SendPai.m_byArHandPai,i,false);
		SendGameData(i, &m_GameData.T_SendPai, sizeof(m_GameData.T_SendPai), ASS_GM_GAME_NOTIFY, EVENT_SEND_PAI, 0);
	}
	//超时设置
	SetDoingThing(EVENT_SEND_PAI,0);//设置正在发生的事件
}

///掷2颗色子定精事件
void CSrvLogic::ExecuteTwoSeziMakeJing(BYTE &nextID,BYTE &time)	
{
	nextID = EVENT_SEND_PAI;
	m_GameData.T_TwoSeziJing.byUser = m_UserData.m_byNtStation;//默认庄家掷色子定财神
	CLogicBase::GetSeziData(&m_GameData.T_TwoSeziJing.bySezi0,&m_GameData.T_TwoSeziJing.bySezi1);

	int num = m_GameData.T_TwoSeziJing.bySezi0 + m_GameData.T_TwoSeziJing.bySezi1;
	num = num%13;//两颗色子值不能超过12
	num = (2*num +1);// num要从敦（2张）转换成张，所以要乘以2
	if(m_UserData.m_MenPai.byEndIndex>=num)
	{
		m_UserData.m_MenPai.byCaiShenIndex = m_UserData.m_MenPai.byEndIndex -num;
	}
	else
	{
		m_UserData.m_MenPai.byCaiShenIndex = m_UserData.m_MenPai.byAllPaiNum + m_UserData.m_MenPai.byEndIndex + 1 - num;
	}
	m_UserData.m_StrJing.Init();
	if(!m_UserData.m_StrJing.AddJing(m_UserData.m_MenPai.byMenPai[m_UserData.m_MenPai.byCaiShenIndex]))
	{
		OutputDebugString("色子定财神失败");
	}
	if(m_UserData.m_MenPai.byMenPai[m_UserData.m_MenPai.byCaiShenIndex] == 255)//没有得到财神就将最后一张牌作为财神
	{
		m_UserData.m_MenPai.byCaiShenIndex = m_UserData.m_MenPai.byEndIndex;
		m_UserData.m_StrJing.Init();
		m_UserData.m_StrJing.AddJing(m_UserData.m_MenPai.byMenPai[m_UserData.m_MenPai.byCaiShenIndex]);
	}

#ifdef MJ_CAN_SETPAI
 //   Tstring s = CINIFile::GetAppPath ();/////本地路径    
	//CINIFile f(s +_T("\\")+SKIN_FOLDER  + _T("_s.ini"));
	//m_UserData.m_StrJing.byPai[0] = f.GetKeyVal("test","baopai",m_UserData.m_StrJing.byPai[0]);
#endif //MJ_CAN_SETPAI

	m_GameData.T_TwoSeziJing.m_StrJing = m_UserData.m_StrJing;
	//发送游戏消息
	SendAllGameData( &m_GameData.T_TwoSeziJing, sizeof(m_GameData.T_TwoSeziJing), ASS_GM_GAME_NOTIFY, EVENT_2SEZI_JING, 0);
	//设置等待色子动画结束状态：由于该事件要播放色子动画，所以必须要等待动画结束
	m_UserData.SetWaiteSezi(true);
	//超时设置
	SetDoingThing(EVENT_2SEZI_JING,0);//设置正在发生的事件
}

///所有玩家补花事件
void CSrvLogic::ExecuteAllBuHua(BYTE &nextID,BYTE &time)	
{
	nextID = EVENT_BEGIN_OUT;
	time = 5;
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(!m_UserData.IsHuaPaiInHand(i))
			continue;
		while(m_UserData.IsHuaPaiInHand(i))
		{
			m_UserData.MoveHuaPaiFormHand(i);
		}
	}
	m_UserData.CopyAllHuaPai(m_GameData.T_AllBuHua.m_byArHuaPai);//拷贝花牌
	m_UserData.CopyAllMenPai(m_GameData.T_AllBuHua.byMenPai);//拷贝门牌
	///发送牌数据
	for (int i=0;i<PLAY_COUNT;i++)
	{		
		m_UserData.CopyAllHandPai(m_GameData.T_AllBuHua.m_byArHandPai,i,false);
		SendGameData(i, &m_GameData.T_AllBuHua, sizeof(m_GameData.T_AllBuHua), ASS_GM_GAME_NOTIFY, EVENT_ALL_BUHUA, 0);
	}
	//超时设置
	SetDoingThing(EVENT_ALL_BUHUA,0);//设置正在发生的事件
}

///单个玩家补花事件
void CSrvLogic::ExecuteOneBuHua(BYTE &nextID,BYTE &time)
{
	m_GameData.T_OneBuHua.byUser = m_UserData.m_byNowOutStation;
	while(m_UserData.IsHuaPaiInHand(m_UserData.m_byNowOutStation))
	{
		m_UserData.MoveHuaPaiFormHand(m_UserData.m_byNowOutStation);
	}
	m_UserData.CopyAllHuaPai(m_GameData.T_OneBuHua.m_byArHuaPai);//拷贝花牌
	m_UserData.CopyAllMenPai(m_GameData.T_OneBuHua.byMenPai);//拷贝门牌
	///发送牌数据
	for (int i=0;i<PLAY_COUNT;i++)
	{			
		m_UserData.CopyAllHandPai(m_GameData.T_OneBuHua.m_byArHandPai,i,false);
		SendGameData(i, &m_GameData.T_OneBuHua, sizeof(m_GameData.T_OneBuHua), ASS_GM_GAME_NOTIFY, EVENT_ONE_BUHUA, 0);
	}
	//超时设置
	SetDoingThing(EVENT_ONE_BUHUA,m_GameData.T_OneBuHua.byUser);//设置正在发生的事件
}

///开始出牌通知事件
void CSrvLogic::ExecuteBeginOut(BYTE &nextID,BYTE &time)
{
	m_GameData.T_BeginOutPai.byNt = m_UserData.m_byNtStation;//庄家位置
	m_GameData.T_BeginOutPai.byUser = m_UserData.m_byNtStation;//庄家位置
	m_GameData.T_BeginOutPai.m_StrJing = m_UserData.m_StrJing;//精牌
	m_UserData.m_byNowOutStation = m_UserData.m_byNtStation;//出牌位置
	ChangeMenPai();//转换门牌,拷贝再转换
	m_UserData.CopyAllMenPai(m_GameData.T_BeginOutPai.byMenPai);//拷贝门牌
	//检测动作
	CheckAction(m_UserData.m_byNtStation,m_UserData.GetLastHandPai(m_UserData.m_byNowOutStation),0);
	//发送通知
	SendAllGameData(&m_GameData.T_BeginOutPai, sizeof(m_GameData.T_BeginOutPai), ASS_GM_GAME_NOTIFY, EVENT_BEGIN_OUT, 0);
	if(!m_UserData.IsUserState( m_GameData.T_BeginOutPai.byUser,USER_STATE_CUT))//没有断线
	{
		m_GameData.T_CPGNotify[m_UserData.m_byNtStation].byUser = m_UserData.m_byNtStation;
		m_GameData.T_CPGNotify[m_UserData.m_byNtStation].bZhuaPai = true;
		SendGameData(m_UserData.m_byNtStation, &m_GameData.T_CPGNotify[m_UserData.m_byNtStation], sizeof(tagCPGNotifyEx), ASS_GM_GAME_NOTIFY, EVENT_CPG_NOTIFY, 0);
	}
	//超时设置
	SetDoingThing(EVENT_BEGIN_OUT,m_UserData.m_byNtStation);//设置正在发生的事件
	SetGameState(STATE_OUT_CARD);//设置游戏状态
}

///抓牌事件
void CSrvLogic::ExecuteZhuaPai(BYTE &nextID,BYTE &time)	
{
	BYTE card = 255;
	if(m_UserData.m_MenPai.GetMenPaiNum()<=m_GameData.m_mjRule.byGamePassNum)
	{
		nextID = EVENT_END_HANDLE;
		time = 10;
		return;
	}
	else
	{
		for(int i=0;i<PLAY_COUNT;++i)
		{
			if(i == m_UserData.m_byNowOutStation)
				continue;
			m_UserData.SetUserState(i,USER_STATE_GANG,false);//消除杠牌状态：检测杠开和杠后炮时使用
		}
		m_GameData.T_ZhuaPai.byUser = m_UserData.m_byNowOutStation;//当前要出牌的位置
		card = GetPai(m_GameData.T_ZhuaPai.byGetPaiIndex,m_UserData.m_byNowOutStation,m_UserData.m_bZhuaPaiDir,0);//正常抓牌
		m_GameData.T_ZhuaPai.byPs = card;		//抓牌的值
		m_UserData.SuperZhuaPai(card);//超级客户端设置抓牌处理（发牌器版本）
		if(m_GameData.m_mjRule.bHaveFlower && m_UserData.CheckIsHuaPai(card))//手牌中有花牌
		{//花牌补花
			nextID = EVENT_ONE_BUHUA;
			time = 5;
			m_GameData.T_ZhuaPai.bHead = false;			//抓牌的方向
			m_GameData.T_ZhuaPai.bCanOutPai = false;	//抓牌后是否可以出牌
		}
		else
		{//动作检测
			CheckAction(m_UserData.m_byNowOutStation,card,0);
		}
		ChangeMenPai();//转换门牌
		m_UserData.CopyAllHuaPai(m_GameData.T_ZhuaPai.m_byArHuaPai);//拷贝花牌
		m_UserData.CopyAllOutPai(m_GameData.T_ZhuaPai.m_byArOutPai);//拷贝出牌
		m_UserData.CopyAllMenPai(m_GameData.T_ZhuaPai.byMenPai);//拷贝门牌
	}
	BYTE pai = m_GameData.T_ZhuaPai.byPs ;
	///发送牌数据
	for (int i=0;i<PLAY_COUNT;i++)
	{			
		m_GameData.T_ZhuaPai.byPs = 255;
		m_UserData.CopyAllHandPai(m_GameData.T_ZhuaPai.m_byArHandPai,i,false);
		if(m_GameData.T_ZhuaPai.byUser == i)//只有抓牌玩家才能看到被抓的牌
		{
			m_GameData.T_ZhuaPai.byPs = pai;
		}
		SendGameData(i, &m_GameData.T_ZhuaPai, sizeof(m_GameData.T_ZhuaPai), ASS_GM_GAME_NOTIFY, EVENT_ZHUA_PAI, 0);
	}
	m_GameData.T_ZhuaPai.byPs = pai;
	if(nextID == 255)
	{//id=255说明即没有流局也不是补花，可以发送吃碰杠通知，如果断线就直接出牌
		SetGameState(STATE_OUT_CARD);//设置游戏状态
		m_GameData.T_CPGNotify[m_GameData.T_ZhuaPai.byUser].bZhuaPai = true;
		m_GameData.T_CPGNotify[m_GameData.T_ZhuaPai.byUser].byUser = m_GameData.T_ZhuaPai.byUser;
		SendGameData(m_GameData.T_ZhuaPai.byUser, &m_GameData.T_CPGNotify[m_GameData.T_ZhuaPai.byUser], sizeof(tagCPGNotifyEx), ASS_GM_GAME_NOTIFY, EVENT_CPG_NOTIFY, 0);
	}
	//超时设置
	SetDoingThing(EVENT_ZHUA_PAI,m_UserData.m_byNowOutStation);//设置正在发生的事件
}

//吃牌事件
void CSrvLogic::ExecuteChiPai(BYTE &nextID,BYTE &time)
{
	m_GameData.T_ChiPai = m_GameData.temp_ChiPai;//获取数据
	m_UserData.DelleteLastOutPai(m_GameData.temp_ChiPai.byBeChi);//删除出牌玩家的最后出牌，因为被吃了
	m_UserData.m_byNowOutStation = m_GameData.temp_ChiPai.byUser;//改变出牌位置
	//添加吃牌
	GCPStructEx chi;
	CLogicBase::CopyChiData(chi,m_GameData.temp_ChiPai);//拷贝吃牌数据
	for(int i=0;i<3;i++)
	{	
		if(m_GameData.temp_ChiPai.byChiPs[i] == m_GameData.temp_ChiPai.byPs)
			continue;
		m_UserData.DelleteAHandPai(chi.iStation,m_GameData.temp_ChiPai.byChiPs[i]);//删除手牌
	}
	////将被吃的牌放到中间
	memset(chi.byData,255,sizeof(chi.byData));
	chi.byData[1] = m_UserData.m_byOtherOutPai;
	for(int i=0;i<3;++i)
	{
		if(m_GameData.temp_ChiPai.byChiPs[i] == m_UserData.m_byOtherOutPai)
			continue;
		for(int j=0;j<3;++j)
		{
			if(chi.byData[j] == 255)
			{
				chi.byData[j] = m_GameData.temp_ChiPai.byChiPs[i];
				break;
			}
		}
	}
	m_UserData.AddToGCP(chi.iStation,&chi);
	m_UserData.CopyAllGCPData(m_GameData.T_ChiPai.m_UserGCPData);
	m_UserData.CopyAllOutPai(m_GameData.T_ChiPai.m_byArOutPai);
	//关系记录
	m_UserData.m_byGuanXi[m_GameData.temp_ChiPai.byUser][m_GameData.temp_ChiPai.byBeChi] += 1;
	//通知各个玩家
	for (int i=0;i<PLAY_COUNT;i++)
	{	
		m_UserData.CopyAllHandPai(m_GameData.T_ChiPai.m_byArHandPai,i,false);
		SendGameData(i, &m_GameData.T_ChiPai, sizeof(m_GameData.T_ChiPai), ASS_GM_GAME_NOTIFY, EVENT_CHI_PAI, 0);
	}
	//判断是否能听牌
	m_GameData.T_CPGNotify[chi.iStation].Clear();//清理数据
	m_CheckHuPai.CanTing(chi.iStation,m_GameData.T_CPGNotify[chi.iStation]);//检测能否听牌
	if(m_GameData.T_CPGNotify[m_GameData.T_ChiPai.byUser].bTing)//有听牌
	{//发送听牌拦牌
		m_GameData.T_CPGNotify[chi.iStation].byUser = chi.iStation;
		SendGameData(chi.iStation, &m_GameData.T_CPGNotify[chi.iStation], sizeof(tagCPGNotifyEx), ASS_GM_GAME_NOTIFY, EVENT_CPG_NOTIFY, 0);
	}	
	SetGameState(STATE_OUT_CARD);//设置游戏状态
	//超时设置
	SetDoingThing(EVENT_CHI_PAI,m_UserData.m_byNowOutStation);//设置正在发生的事件
}

//碰牌事件
void CSrvLogic::ExecutePengPai(BYTE &nextID,BYTE &time)
{
	m_GameData.T_PengPai = m_GameData.temp_PengPai;//获取数据
	//出牌玩家删除最后一张出牌
	m_UserData.DelleteLastOutPai(m_GameData.temp_PengPai.byBePeng);
	//改变出牌位置
	m_UserData.m_byNowOutStation = m_GameData.temp_PengPai.byUser;
	//碰牌牌事件
	GCPStructEx peng;
	CLogicBase::CopyPengData(peng,m_GameData.temp_PengPai);//拷贝碰牌数据
	//删除手牌
	m_UserData.DelleteSomeHandPai(peng.iStation,peng.byData,2);//删除一些手牌
	m_UserData.AddToGCP(m_GameData.temp_PengPai.byUser,&peng);//添加碰牌
	m_UserData.CopyAllGCPData(m_GameData.T_PengPai.m_UserGCPData);//拷贝吃碰杠数据
	m_UserData.CopyAllOutPai(m_GameData.T_PengPai.m_byArOutPai);//拷贝出牌数据
	//关系记录
	m_UserData.m_byGuanXi[m_GameData.temp_PengPai.byUser][m_GameData.temp_PengPai.byBePeng] += 1;
	for (int i=0;i<PLAY_COUNT;i++)//通知各个玩家
	{	
		m_UserData.CopyAllHandPai(m_GameData.T_PengPai.m_byArHandPai,i,false);//拷贝手牌
		SendGameData(i, &m_GameData.T_PengPai, sizeof(m_GameData.T_PengPai), ASS_GM_GAME_NOTIFY, EVENT_PENG_PAI, 0);
	}
	//判断是否能听牌
	m_GameData.T_CPGNotify[peng.iStation].Clear();//清理数据
	m_CheckHuPai.CanTing(peng.iStation,m_GameData.T_CPGNotify[peng.iStation]);//检测能否听牌
	if(m_GameData.T_CPGNotify[peng.iStation].bTing)//有听牌
	{//发送听牌拦牌
		m_GameData.T_CPGNotify[peng.iStation].byUser = peng.iStation;
		SendGameData(peng.iStation, &m_GameData.T_CPGNotify[peng.iStation], sizeof(tagCPGNotifyEx), ASS_GM_GAME_NOTIFY, EVENT_CPG_NOTIFY, 0);
	}
	SetGameState(STATE_OUT_CARD);//设置游戏状态
	//超时设置
	SetDoingThing(EVENT_PENG_PAI,m_UserData.m_byNowOutStation);//设置正在发生的事件
}

//杠牌事件
void CSrvLogic::ExecuteGangPai(BYTE &nextID,BYTE &time)
{
	nextID = EVENT_ZHUA_PAI;
	time = 3;
	///杠牌牌事件
	m_GameData.T_GangPai = m_GameData.temp_GangPai;//获取数据
	GCPStructEx gang;
	CLogicBase::CopyGangData(gang,m_GameData.temp_GangPai);//拷贝杠牌数据

	switch(m_GameData.temp_GangPai.byType)//杠牌类型
	{
	case ACTION_AN_GANG://暗杠
		{
			m_UserData.DelleteSomeHandPai(m_GameData.temp_GangPai.byUser,gang.byData,4);//删除手牌
		}
		break;
	case ACTION_MING_GANG://明杠
		{
			m_UserData.DelleteSomeHandPai(m_GameData.temp_GangPai.byUser,gang.byData,3);//删除手牌
			m_UserData.DelleteLastOutPai(m_GameData.temp_GangPai.byBeGang);//出牌玩家删除最后一张出牌
			//关系记录
			m_UserData.m_byGuanXi[m_GameData.temp_GangPai.byUser][m_GameData.temp_GangPai.byBeGang] += 1;
		}
		break;
	case ACTION_BU_GANG://补杠
		{
			m_UserData.DelleteAHandPai(m_GameData.temp_GangPai.byUser,m_GameData.temp_GangPai.byPs);//删除手牌
			m_UserData.DelleteAGCPData(m_GameData.temp_GangPai.byUser,ACTION_PENG,m_GameData.temp_GangPai.byPs);//删除相应碰牌
		}
		break;
	}			
	gang.byType = m_GameData.temp_GangPai.byType;
	m_UserData.m_byNowOutStation = m_GameData.temp_GangPai.byUser;//改变出牌位置（一定要在删除最后出牌动作之后）
	m_UserData.SetUserState(m_UserData.m_byNowOutStation,USER_STATE_GANG,true);//置为杠牌状态：检测杠开和杠后炮使用


	m_UserData.AddToGCP(m_GameData.temp_GangPai.byUser,&gang);
	m_UserData.CopyAllGCPData(m_GameData.T_GangPai.m_UserGCPData);
	m_UserData.CopyAllOutPai(m_GameData.T_GangPai.m_byArOutPai);
	m_UserData.m_bZhuaPaiDir = false;///当前抓牌方向，true 顺时针，false 逆时针
	//通知各个玩家
	for (int i=0;i<PLAY_COUNT;i++)
	{	
		m_UserData.CopyAllHandPai(m_GameData.T_GangPai.m_byArHandPai,i,false);
		SendGameData(i, &m_GameData.T_GangPai, sizeof(m_GameData.T_GangPai), ASS_GM_GAME_NOTIFY, EVENT_GANG_PAI, 0);
	}
	//超时设置
	SetDoingThing(EVENT_GANG_PAI,m_UserData.m_byNowOutStation);//设置正在发生的事件
}
//糊牌事件
void CSrvLogic::ExecuteHuPai(BYTE &nextID,BYTE &time)
{
	nextID = EVENT_COUNT_FEN;
	time = 2;
	///通知玩家糊牌了
	if(!m_GameData.T_HuPai.bZimo)
	{
		m_UserData.AddToHandPai(m_GameData.T_HuPai.byUser,m_GameData.T_HuPai.byPs);//
	}
	m_UserData.CopyAllHandPai(m_GameData.T_HuPai.m_byArHandPai,0,true);//明牌拷贝手牌，即所有玩家明牌
	m_UserData.CopyAllOutPai(m_GameData.T_HuPai.m_byArOutPai);//拷贝出牌
	m_UserData.CopyAllGCPData(m_GameData.T_HuPai.m_UserGCPData);//拷贝吃碰杠牌
	SendAllGameData(&m_GameData.T_HuPai, sizeof(m_GameData.T_HuPai), ASS_GM_GAME_NOTIFY, EVENT_HU_PAI, 0);
	//超时设置
	SetDoingThing(EVENT_HU_PAI,m_GameData.T_HuPai.byUser);//设置正在发生的事件
}

///算分事件
void CSrvLogic::ExecuteCountFen(BYTE &nextID,BYTE &time)
{
	///算分事件
	nextID = EVENT_END_HANDLE;//下一个事件，游戏结束事件
	time = 2;	
	memcpy(m_GameData.T_CountFen.bHu,m_GameData.T_HuPai.bCanH,sizeof(m_GameData.T_HuPai.bCanH));//拷贝胡牌玩家
	memcpy(m_GameData.T_CountFen.byHuType,m_GameData.T_HuPai.byHuType,sizeof(m_GameData.T_HuPai.byHuType));//拷贝胡牌类型

	m_GameData.T_CountFen.bDuoXiang = m_GameData.T_HuPai.bDuoXiang;
	m_GameData.T_CountFen.bZimo = m_GameData.T_HuPai.bZimo;
	m_GameData.T_CountFen.byUser = m_GameData.T_HuPai.byUser;
	m_GameData.T_CountFen.byDian = m_GameData.T_HuPai.byDianPao;
	m_GameData.T_CountFen.byPs = m_GameData.T_HuPai.byPs;
	m_GameData.T_CountFen.byNt = m_UserData.m_byNtStation;
	///算分
	m_CountFen.CountFen(m_GameData.T_HuPai,m_GameData.T_CountFen);
	m_UserData.CopyAllHandPai(m_GameData.T_CountFen.m_byArHandPai,0,true);
	m_UserData.CopyAllGCPData(m_GameData.T_CountFen.m_UserGCPData);
	//超时设置，
	SetDoingThing(EVENT_COUNT_FEN,m_GameData.T_CountFen.byUser);//设置正在发生的事件
}

///正常游戏结束事件
void CSrvLogic::ExecuteNormalEnd(BYTE &nextID,BYTE &time)
{
	///游戏结束处理事件
	m_GameData.T_CountFen.byLianZhuang = m_UserData.m_byLianZhuang;
	//确定庄家
	CLogicBase::GetNextNt(m_GameData.T_CountFen,m_UserData.m_byNtStation,m_UserData.m_byLianZhuang,m_GameData.m_mjRule.byNextNtType);
	//获取庄家
	m_GameData.T_CountFen.byNextNt = m_UserData.m_byNtStation;
	//调用平台结束游戏接口
	pDesk->GameFinish(m_GameData.T_HuPai.byUser,GF_NORMAL);
	//超时设置，
	SetDoingThing(EVENT_END_HANDLE,m_GameData.T_EndHandle.byUser);//设置正在发生的事件


	char strdug[500];
	for(int i=0;i<PLAY_COUNT;++i)
	{
		sprintf(strdug,"算分测试：事件 ExecuteNormalEnd 位置%d 杠分%d 糊分%d 总分%d",i,m_GameData.T_CountFen.iGangFen[i],m_GameData.T_CountFen.iHuFen[i],m_GameData.T_CountFen.iZongFen[i]);
		OutputDebugString(strdug);
	}

	//发送结束数据
	SendAllGameData( &m_GameData.T_CountFen, sizeof(m_GameData.T_CountFen), ASS_GM_GAME_NOTIFY, EVENT_END_HANDLE, 0);   
	KillAllTimer();//删除所有计时器
	SetGameState(STATE_FINISH);//设置游戏状态
}

///异常游戏结束事件事件
void CSrvLogic::ExecuteUnNormalEnd(BYTE &nextID,BYTE &time)
{
	KillAllTimer();//删除所有计时器
	//调用平台结束游戏接口
	pDesk->GameFinish(255,GF_SAFE);
}

///获取牌数据
void CSrvLogic::DisPatchCard()
{
	m_GameData.LoadPaiData(m_UserData.m_MenPai);//加载牌数据
	m_UserData.RandCard();//打乱牌
}
///描述：转换门牌数据,将总门牌数组中的数据转换到每个玩家的门牌数组中
void CSrvLogic::ChangeMenPai()
{
	m_UserData.ChangeMenPai();
}
///名称：GetPai 描述：抓一张牌
///@param index 通过参数返回抓牌的索引，station 抓牌玩家的位置 , head  是否从前面抓,type 抓牌类型 0 正常抓牌，1开始前发牌，2开前补花抓牌，3补花
///@return 抓到的牌值
BYTE CSrvLogic::GetPai(BYTE &index,int station,bool head,BYTE type)
{
	index = 255;
	if(m_UserData.GetHandPaiCount(station) %3 == 2)//玩家的手牌足够
	{
		OutputDebugString("事件：GetPai 玩家牌数足够，不能发牌");
		return 255;
	}
	return m_UserData.GetPai(index,station, head, type);
}

///名称：CheckAction 吃碰杠动作检测
///@param ：station 检测位置,pai 别的玩家打出的牌，type 类型 出牌 0 抓牌，1杠开，2 点炮，3抢杠
///@return true 能做动作，false 不能做动作
bool CSrvLogic::CheckAction(int station,BYTE pai,BYTE type)
{
	for(int i=0;i<4;++i)
	{
		m_GameData.T_CPGNotify[i].Clear();//清零
		m_GameData.T_CPGNotify[i].byOutStation = station;//记录出牌位置
	}
	bool action = false;
	switch(type)
	{
	case 0://自摸:自己抓牌动作检测
	case 1://杠开:动作检测
		{
			m_CheckHuPai.CanHu(station,pai,m_GameData.T_CPGNotify[station],true,station);//糊检测
			m_CPGAction.CanGang(station,pai,m_GameData.T_CPGNotify[station],true);//杠检测
			m_CheckHuPai.CanTing(station,m_GameData.T_CPGNotify[station]);//听检测
			if(m_GameData.T_CPGNotify[station].bHu || m_GameData.T_CPGNotify[station].bGang || m_GameData.T_CPGNotify[station].bTing)
			{
				m_GameData.T_CPGNotify[station].bZimo = true;
				action=true;
				m_GameData.T_CPGNotify[station].bCanAction = true;
				m_GameData.T_CPGNotify[station].byPs = pai;
			}
		}
		break;
	case 2://点炮:别人出牌后动作检测
		{
			for(int i=0;i<PLAY_COUNT;++i)
			{
				if(i==station || m_UserData.IsUserState(i,USER_STATE_CUT))//断线玩家不检测
					continue;
				m_CPGAction.CanChi(i,pai,m_GameData.T_CPGNotify[i]);//吃检测
				m_CPGAction.CanPeng(i,pai,m_GameData.T_CPGNotify[i]);//碰检测
				m_CPGAction.CanGang(i,pai,m_GameData.T_CPGNotify[i],false);//杠检测
				m_CheckHuPai.CanHu(i,pai,m_GameData.T_CPGNotify[i],false,station);//糊检测
				if(m_GameData.T_CPGNotify[i].bHu || m_GameData.T_CPGNotify[i].bPeng || m_GameData.T_CPGNotify[i].bChi || m_GameData.T_CPGNotify[i].bGang )
				{
					action=true;
					m_GameData.T_CPGNotify[i].bZimo = false;
					m_GameData.T_CPGNotify[i].byPs = pai;
					m_GameData.T_CPGNotify[i].bCanAction = true;
				}
			}
		}
		break;
	case 3://抢杠：别人补杠时动作检测
		{
			for(int i=0;i<PLAY_COUNT;++i)
			{
				if(i==station || m_UserData.IsUserState(i,USER_STATE_CUT))//断线玩家不检测
					continue;
				if(m_CheckHuPai.CanHu(i,pai,m_GameData.T_CPGNotify[i],false,station))
				{
					action = true;//糊
					m_GameData.T_CPGNotify[i].byPs = pai;
					m_GameData.T_CPGNotify[i].bZimo = false;
		
				}
			}
		}
		break;
	}
	return action;
}

//防卡桌自动处理：2分钟内没有收到任何游戏消息，认为是游戏卡死，调用本函数，根据当前执行事件和玩家的状态进行自动处理，恢复游戏流程
void CSrvLogic::AutoHandle(bool bNetCut)
{
	switch(m_GameData.m_byThingDoing)
	{			        				         
	case EVENT_2SEZI_NT://首局掷2颗色子的点数和为庄事件		   
	case EVENT_2SEZI_NT_GP://首局掷2颗色子定庄家和起牌点事件		
	case EVENT_2SEZI_DIR://掷2颗色子事件定起牌方向事件		
	case EVENT_2SEZI_DIR_GP://掷2颗色子事件定起牌方向和起牌点事件
	case EVENT_2SEZI_GP://掷2颗色子事件定起牌点事件												
	case EVENT_2SEZI_JING://掷2颗色子定精牌事件			
		{//构造色子结束消息
			for(int i=0;i<PLAY_COUNT;++i)
			{
				if(bNetCut)//断线处理：断线是立即响应
				{
					if(!m_UserData.m_bSiziFinish[i] && m_UserData.IsUserState(i,USER_STATE_CUT))//该玩家没有发结束消息
					{
						ReceiveSeziFinish(i,NULL,0,false);
					}
				}
				else
				{
					if(!m_UserData.m_bSiziFinish[i])//该玩家没有发结束消息
					{
						ReceiveSeziFinish(i,NULL,0,false);
					}
				}
			}
		}
		break;
	case EVENT_ALL_BUHUA://为所有玩家补花事件
		{//执行开始出牌事件
			ApplyAction(EVENT_BEGIN_OUT,200);//申请下一个事件
		}
		break;
	case EVENT_ONE_BUHUA://单个玩家补花事件	
	case EVENT_BEGIN_OUT://庄家开始发牌通知事件	
	case EVENT_CHI_PAI://吃牌事件
	case EVENT_PENG_PAI://碰牌事件
	case EVENT_ZHUA_PAI://抓牌事件
	case EVENT_GANG_PAI://杠牌事件
		{
			if(bNetCut)//断线处理：断线是立即响应
			{
				if(m_UserData.IsUserState( m_UserData.m_byNowOutStation,USER_STATE_CUT))//如果庄家断线
				{
					AutoOutOrGet();//代替玩家出牌
				}
			}
			else
			{
				AutoOutOrGet();//代替玩家出牌
			}
		}
		break;
	case EVENT_OUT_PAI://出牌事件
		{
			if(bNetCut)//断线处理：断线是立即响应
			{
				for(int i=0;i<PLAY_COUNT;++i)
				{
					if(m_UserData.IsUserState( i,USER_STATE_CUT) )//玩家玩家断线
					{
						ReceiveUserPassCheck(i,NULL,0,false);//发送放弃消息
					}
				}
			}
			else
			{
				if(m_GameData.m_byNextAction != 255 && m_GameData.m_byNextAction != ACTION_NO)//有人申请了动作
				{
					for(int i=0;i<PLAY_COUNT;++i)
					{
						if(m_GameData.T_CPGNotify[i].bCanAction && !m_GameData.T_CPGNotify[i].bHaveReturn)//有动作但是没有回应
							ReceiveUserPassCheck(i,NULL,0,false);//发送放弃消息
					}
				}
				else
				{
					AutoOutOrGet();//代替玩家出牌
				}
			}
		}
		break;
	case EVENT_HU_PAI://糊事件	
		{
			if(!bNetCut)//断线处理：断线是立即响应
			{
				ApplyAction(EVENT_COUNT_FEN,200);//申请下一个事件
			}
		}
		break;
	case EVENT_COUNT_FEN://算分事件	
		{
			if(!bNetCut)//断线处理：断线是立即响应
			{
				ApplyAction(EVENT_END_HANDLE,200);//申请下一个事件
			}
		}
		break;
	}
}
///自动处理出牌或者抓牌
void CSrvLogic::AutoOutOrGet()
{
	bool haveout = false;
	for(int i=0;i<PLAY_COUNT;++i)
	{
		if(!m_UserData.IsOutPaiPeople(i))//出牌玩家
			continue;
		haveout = true;
		tagOutPaiEx outpai;
		outpai.Clear();
		outpai.byUser = i;
		outpai.byPs = m_UserData.GetLastHandPai(i);
		ReceiveUserOutCard(i,&outpai,sizeof(outpai),false);
		break;
	}
	if(!haveout)//没有出牌的玩家，启动抓牌事件
	{
		ApplyAction(EVENT_ZHUA_PAI,200);//申请下一个事件
	}
}
/////////////////////////其他处理////////////////////////////////////////////////////////////
/// 玩家断线
void CSrvLogic::UserNetCut(BYTE bDeskStation)
{
	m_UserData.SetUserState(bDeskStation,USER_STATE_CUT,true);//设置断线状态
	AutoHandle(true);	///断线处理
}
/// 玩家离桌
void CSrvLogic::UserLeftDesk(BYTE bDeskStation)
{

	if (pDesk->GetGameStation() > GS_PLAY_GAME || pDesk->GetGameStation() < GS_SEND_CARD)  //在游戏中，
	{
		m_UserData.m_byNtStation = 255;
	}
}

/// 玩家坐桌
void CSrvLogic::UserSitDesk(BYTE bDeskStation)
{
}

/// 玩家重入
void CSrvLogic::UserReCome(BYTE bDeskStation)
{
	m_UserData.SetUserState(bDeskStation,USER_STATE_CUT,false);//设置为非断线状态
}
/// 处理计时器消息
void CSrvLogic::OnGameTimer(UINT uTimerID)
{
	switch(uTimerID)
	{
	case 10://事件计时器id
		{
			pDesk->KillTimer(uTimerID);
			GameCPU(m_GameData.ApplyThingID);
		}
		break;
	case TIME_CHECK_GAME_MSG://消息超时，自动处理
		{
			pDesk->KillTimer(uTimerID);
			AutoHandle();
		}
		break;
	case TIME_WAIT_MSG: // 超过事件无响应，是超时自动处理，系统自动处理
		{
			OutTimeHandle();
		}
		break;
	}
}
/// 游戏结束要删除所有计时器
void CSrvLogic::KillAllTimer()
{
	pDesk->KillTimer(10);
	pDesk->KillTimer(TIME_CHECK_GAME_MSG);
	pDesk->KillTimer(TIME_WAIT_MSG);
}

/// 设置当前正在发生的事件：同时设置一个等待时间，超过时间客户端没有响应就自动处理
void CSrvLogic::SetDoingThing(BYTE thingId,BYTE st1,BYTE st2,BYTE pai,BYTE data)
{
	RecordThing( thingId, st1, st2, pai, data);//记录事件
	m_GameData.m_byThingDoing = thingId;//正在发生的事件
	m_GameData.ReSetOutTime();/// 恢复超时数据
	m_GameData.m_byNextAction = ACTION_NO;//下一步动作设置为无动作
	m_UserData.m_byApplyMaxAction = ACTION_NO;//玩家当前申请的最大事件：处理完申请后，恢复数据
	switch(thingId)
	{	     	
	case EVENT_BEGIN_OUT://庄家开始发牌通知事件		
		{
			m_GameData.SetOutTime(st1,m_GameData.m_MjSet.byOutcardTime,thingId,true);
		}
		break;
	case EVENT_OUT_PAI://出牌事件
		{
			AutoHandle(true);
			m_GameData.SetOutTime(st1,m_GameData.m_MjSet.byBlockTime,thingId,true);
		}
		break;
	case EVENT_ZHUA_PAI://抓牌事件
		{
			if(m_UserData.IsUserState(st1,USER_STATE_AUTO) || m_UserData.IsUserState(st1,USER_STATE_TING) || m_UserData.IsUserState(st1,USER_STATE_CUT))//听牌或者托管，是短时间回应
			{
				m_GameData.SetOutTime(st1,m_GameData.m_MjSet.byAutoTime,thingId,true);
				break;
			}
			m_GameData.SetOutTime(st1,m_GameData.m_MjSet.byOutcardTime + m_GameData.m_MjSet.byBlockTime,thingId,true);
		}
		break;
	case EVENT_CHI_PAI://吃牌事件
	case EVENT_PENG_PAI://碰牌事件	
	case EVENT_GANG_PAI://杠牌事件
		{
			if(m_UserData.IsUserState(st1,USER_STATE_AUTO) || m_UserData.IsUserState(st1,USER_STATE_TING)|| m_UserData.IsUserState(st1,USER_STATE_CUT))//听牌或者托管，是短时间回应
			{
				m_GameData.SetOutTime(st1,m_GameData.m_MjSet.byAutoTime,thingId,true);
				break;
			}
			m_GameData.SetOutTime(st1,m_GameData.m_MjSet.byOutcardTime,thingId,true);
		}
		break;
	case EVENT_2SEZI_NT:	//首局掷2颗色子的点数和为庄事件		   
	case EVENT_2SEZI_NT_GP:	//首局掷2颗色子定庄家和起牌点事件	
	case EVENT_2SEZI_DIR://掷2颗色子事件定起牌方向事件		
	case EVENT_2SEZI_DIR_GP://掷2颗色子事件定起牌方向和起牌点事件
	case EVENT_2SEZI_GP://掷2颗色子事件定起牌点事件		
	case EVENT_2SEZI_NT_DIR_GP:	//掷2颗色子事件定起牌点事件	
	case EVENT_2SEZI_JING://掷2颗色子定精牌事件			
		{
			m_GameData.SetOutTime(st1,m_GameData.m_MjSet.byWaiteSeziTime,thingId,true);
		}
		break;
	default://其他事件作为顺序事件处理,5秒钟处理
		{
			m_GameData.SetOutTime(st1,5,thingId,true);
		}
		break;
	}
}
/// 超时处理
void CSrvLogic::OutTimeHandle()
{


	if(!m_GameData.m_MjWait.bEffect || m_GameData.m_MjWait.byCurChair == 255 || m_GameData.m_MjWait.byTime == 255 
		|| m_GameData.m_MjWait.byWaitThing != m_GameData.m_byThingDoing )
	{
		return;
	}
	m_GameData.m_MjWait.byTime -= 1;
	if(m_GameData.m_MjWait.byTime == 0)
	{
		this->AutoHandle();
	}
}
/// 设置游戏状态
void CSrvLogic::SetGameState(BYTE state)
{
	if(state>= STATE_BEGIN_OUT && state <= STATE_FINISH)
	{
		m_GameData.m_byGameState = state;
	}
}
//事件记录
void CSrvLogic::RecordThing(BYTE thingId,BYTE st1,BYTE st2,BYTE pai,BYTE data)
{//事件记录只记录后10个事件：0事件号，1相关位置1,2相关位置2，3相关牌值，4附加类型
	BYTE index = 9;
	if(m_GameData.m_byThingLog[9][0] != 255)//满了
	{
		for(int i=0;i<9;++i)//后移一个
		{
			memcpy(m_GameData.m_byThingLog[i],m_GameData.m_byThingLog[i+1],sizeof(m_GameData.m_byThingLog[i]));
		}
	}
	else
	{
		for(int i=0;i<10;++i)
		{
			if(m_GameData.m_byThingLog[i][0] != 255)
				continue;
			index = i;
			break;
		}
	}
	m_GameData.m_byThingLog[index][0] = thingId;
	m_GameData.m_byThingLog[index][1] = pai;
	m_GameData.m_byThingLog[index][2] = st1;
	m_GameData.m_byThingLog[index][3] = st2;
	m_GameData.m_byThingLog[index][4] = data;
}
//发送游戏状态
void CSrvLogic::SendGameStation(BYTE bDeskStation, bool bWatchUser, void * pData, UINT uSize)
{
	pDesk->SendGameStationEx(bDeskStation,bWatchUser,pData, uSize);
}

//给单个玩家发送游戏消息
void CSrvLogic::SendGameData(BYTE bDeskStation, BYTE bMainID, BYTE bAssID, BYTE bHandleCode)
{
	pDesk->SendGameDataEx(bDeskStation,bMainID,bAssID,bHandleCode);
	pDesk->SendWatchDataEx(bDeskStation,bMainID,bAssID,bHandleCode);
}
//给单个玩家发送游戏消息
void CSrvLogic::SendGameData(BYTE bDeskStation, void * pData, UINT uSize, BYTE bMainID, BYTE bAssID, BYTE bHandleCode)
{
	pDesk->SendGameDataEx(bDeskStation,pData,uSize,bMainID,bAssID,bHandleCode);
	pDesk->SendWatchDataEx(bDeskStation,pData,uSize,bMainID,bAssID,bHandleCode);
}
//给所有玩家发送游戏消息
void CSrvLogic::SendAllGameData(BYTE bMainID, BYTE bAssID, BYTE bHandleCode)
{
	for(int i=0;i<PLAY_COUNT;++i)
	{
		pDesk->SendGameDataEx(i,bMainID,bAssID,bHandleCode);
		pDesk->SendWatchDataEx(i,bMainID,bAssID,bHandleCode);
	}
}
//给所有玩家发送游戏消息
void CSrvLogic::SendAllGameData( void * pData, UINT uSize, BYTE bMainID, BYTE bAssID, BYTE bHandleCode)
{
	for(int i=0;i<PLAY_COUNT;++i)
	{
		pDesk->SendGameDataEx(i,pData,uSize,bMainID,bAssID,bHandleCode);
		pDesk->SendWatchDataEx(i,pData,uSize,bMainID,bAssID,bHandleCode);
	}
}