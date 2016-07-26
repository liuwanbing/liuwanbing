#include "stdafx.h"
#include "ThingHandleEx.h"
#include "../server/ServerManage.h"
#include "../server/DebugPrintf.h"

//全局对象
//ThingHandleEx g_ThingHandle;
	
ThingHandleEx::ThingHandleEx(void)
{
}
ThingHandleEx::~ThingHandleEx(void)
{
}
//断线重连事件
bool ThingHandleEx::GetGameStaton(BYTE bDeskStation, UINT uSocketID, bool bWatchUser,BYTE GameStation)
{
	GameStationDataEx GameStationData;
	::memset(&GameStationData, 0, sizeof(GameStationData));

	char key[200];
    CString s = CINIFile::GetAppPath ();/////本地路径    
	CINIFile f(s +_T("\\")+SKIN_FOLDER  + _T("_s.ini"));
	sprintf(key,"game");
	pDesk->sGameData.m_byFengDing = f.GetKeyVal(key,"fengding",pDesk->sGameData.m_byFengDing);
	pDesk->sGameData.m_iBaseFen =  f.GetKeyVal(key,"basefen",1);//游戏基本分，默认为1
	pDesk->sGameData.m_bForceFinish =  f.GetKeyVal(key,"bforce",0);//是否强退结束版本
	pDesk->sGameData.m_bChengBao =  f.GetKeyVal(key,"bchengbao",pDesk->sGameData.m_bChengBao);//分承包制和家家有

	sprintf(key,"%d_%d",NAME_ID,pDesk->m_RoomId);
	pDesk->sGameData.m_byFengDing = f.GetKeyVal(key,"fengding",pDesk->sGameData.m_byFengDing);
	pDesk->sGameData.m_iBaseFen =  f.GetKeyVal(key,"basefen",pDesk->sGameData.m_iBaseFen);//游戏基本分，默认为1
	pDesk->sGameData.m_bForceFinish =  f.GetKeyVal(key,"bforce",pDesk->sGameData.m_bForceFinish);//是否强退结束版本
	pDesk->sGameData.m_bChengBao =  f.GetKeyVal(key,"bchengbao",pDesk->sGameData.m_bChengBao);//分承包制和家家有

    GameStationData.ucDingQueTime	= pDesk->sGameData.GetMjSetting().ucDingQueTime;
    GameStationData.ucDingQueTime   = pDesk->sGameData.GetMjSetting().ucDingQueTime;

    //GameStationData.byThinkTime     = pDesk->sGameData.GetMjSetting().ucBlockTime;//思考时间
    //GameStationData.byOutTime		= pDesk->sGameData.GetMjSetting().ucOutcardTime;//出牌时间
    //GameStationData.byBeginTime     = pDesk->sGameData.GetMjSetting().ucBeginTime;//开始等待时间

    //GameStationData.ucNextTime      = pDesk->sGameData.GetMjSetting().ucNextTime;
    //GameStationData.bAutoBegin      = pDesk->sGameData.GetMjSetting().bAutoBegin;//时间到了是否自动开始
    GameStationData.byAutoOutTime		= pDesk->sGameData.GetMjSetting().nAutoTime;//自动出牌时间

	GameStationData.byAutoOutTime = pDesk->sGameData.m_mjRule.byAutoOutTime;		//自动出牌时间
	GameStationData.byOutTime = pDesk->sGameData.m_mjRule.byOutTime;				//出牌时间
	GameStationData.byThinkTime = pDesk->sGameData.m_mjRule.byThinkTime;			//思考时间
	GameStationData.byBeginTime = pDesk->sGameData.m_mjRule.byBeginTime;			//开始等待时间
	GameStationData.bAutoBegin =  pDesk->sGameData.m_mjRule.bAutoBegin;			//时间到了是否自动开始


	GameStationData.m_byFengDing = pDesk->sGameData.m_byFengDing;//封顶番数
	GameStationData.m_bShowTax = pDesk->sGameData.m_bShowTax;//是否显示服务费
	GameStationData.m_iBaseFen = pDesk->sGameData.m_iBaseFen;//封顶番数
	GameStationData.m_bForceFinish = pDesk->sGameData.m_bForceFinish;//是否强退结束版本
	GameStationData.m_bChengBao = pDesk->sGameData.m_bChengBao;//分承包制和家家有

	sprintf(key,"基础分测试： 当前基础分%d",pDesk->sGameData.m_iBaseFen);
	OutputDebugString(key);


	GameStationData.iVersion	=  MAIN_VERSION;
	GameStationData.iVersion2	= ASS_VERSION;
	
	//获取房间倍率逃跑罚分
	GameStationData.bShowUserId	= pDesk->sGameData.m_bShowUserId;//是否显示所有玩家的id
	GameStationData.Stationpara = pDesk->m_bGameStation;		 //游戏状态
	pDesk->GetUserAgree(GameStationData.ucAgree);
	if(!bWatchUser)
	{
		pDesk->sUserData.m_bFoceLeave[bDeskStation] = false;
		//断线重连取消托管
		pDesk->sUserData.m_bTuoGuan[bDeskStation] = false;
		//pDesk->sUserData.Isu
	}

	for(int i=0;i<4;++i)
	{
		memset(GameStationData.m_byArHandPai,255,sizeof(GameStationData.m_byArHandPai));
		pDesk->sUserData.CopyGCPData(i,GameStationData.m_UserGCPData[i]);
		pDesk->sUserData.CopyOutPai(i,GameStationData.m_byArOutPai[i]);
		pDesk->sUserData.CopyMenPai(i,GameStationData.m_iArMenPai[i]);
		pDesk->sUserData.CopyHuaPai(i,GameStationData.m_byArHuaPai[i]);
		GameStationData.m_bTing[i] = pDesk->sUserData.m_bTing[i];
		GameStationData.m_bistuoguan[i] = pDesk->sUserData.m_bTuoGuan[i];
		//获取缺门状态
		GameStationData.m_byDingQue[i] = pDesk->sUserData.m_byDingQue[i];
	}
	pDesk->sUserData.CopyHandPai(GameStationData.m_byArHandPai,bDeskStation,false);
	GameStationData.m_byOtherOutPai = pDesk->sUserData.m_byOtherOutPai;
	GameStationData.m_byThingDoing = pDesk->sGameData.m_byThingDoing;
	GameStationData.byQuanFeng = pDesk->sUserData.m_byQuanFeng;
	GameStationData.byNtStation = pDesk->sUserData.m_byNtStation;
	GameStationData.m_byNowOutStation = pDesk->sUserData.m_byNowOutStation;

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
			if(pDesk->sUserData.m_bIsHu[bDeskStation])//玩家已经胡牌了
			{
				memcpy(&GameStationData.m_HuData,&pDesk->sGameData.T_HuPaiXZMJ[bDeskStation],sizeof(tagHuPaiEx));//玩家的胡牌数据
			}
			for(int i=0;i<4;++i)//玩家是否已经胡牌
			{
				if(pDesk->sUserData.m_bIsHu[i])//玩家已经胡牌了
				{
					GameStationData.m_bIsHu[i] = true;
					for(int j=0;j<17;j++)
					{
						GameStationData.m_byArHandPai[i][j] = pDesk->sUserData.m_byArHandPai[i][j];
					}
				}
			}
			//发送数据
			pDesk->SendGameStationEx(bDeskStation,  bWatchUser, &GameStationData, sizeof(GameStationData));
			//if(!pDesk->sUserData.m_bIsHu[bDeskStation] && pDesk->sGameData.T_CPGNotify[bDeskStation].m_byMaxThing > ACTION_NO && pDesk->sGameData.T_CPGNotify[bDeskStation].m_byMaxThing <= ACTION_HU)//如果有吃碰杠就进行处理
			//{
			//	pDesk->SendGameDataEx(bDeskStation, &pDesk->sGameData.T_CPGNotify[bDeskStation], sizeof(pDesk->sGameData.T_CPGNotify[bDeskStation]), MDM_GM_GAME_NOTIFY, THING_CPG_NOTIFY, 0);
			//}
			return true;
		}
	case GS_WAIT_NEXT:		//等待下一盘游戏开始
		{//发送数据
			pDesk->SendGameStationEx(bDeskStation,  bWatchUser, &GameStationData, sizeof(GameStationData));
			return true;
		}
	}
    return true;
}

///接收并处理消息
int ThingHandleEx::HandleNotifyMessage(int bDeskStation, UINT code, BYTE* pData, int uSize, bool bWatchUser)
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
    case THING_DING_QUE:        // 定缺
        {
             return ReceiveUserDingQue(bDeskStation, pData, uSize,   bWatchUser);
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
int ThingHandleEx::ReceiveSeziFinish(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser)
{
	return ThingHandle::ReceiveSeziFinish(bDeskStation, pData, uSize, bWatchUser);
}

// 收到托管消息
int ThingHandleEx::ReceiveUserTuoGuan(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser)
{
	return ThingHandle::ReceiveUserTuoGuan(bDeskStation, pData, uSize, bWatchUser);
}

// 用户出牌
int ThingHandleEx::ReceiveUserOutCard(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser)
{
    tagOutPaiEx * pOutCard = (tagOutPaiEx *)pData;

    if(pOutCard == NULL || bWatchUser || pDesk->m_bGameStation != GS_PLAY_GAME)
    {
        DebugPrintf("用户 %d 出牌失败，非游戏状态", bDeskStation);
		OutputDebugString("服务器： ReceiveUserOutCard   非游戏状态");
        return 0;
    }

    //if(m_curState.thingDoing!=THING_OUT_PAI || m_curState.ucCurChair!=bDeskStation)
    //{
    //    DebugPrintf("用户 %d 出牌失败，非出牌状态", bDeskStation);
    //    return 0;
    //}
	if(pDesk->sUserData.m_bIsHu[bDeskStation])//已经胡牌的玩家，不能出牌了
		return 0;

    if(pOutCard->byPs == 255 || pDesk->sUserData.GetAHandPaiCount(pOutCard->byUser,pOutCard->byPs)==0 || !pDesk->sUserData.IsOutPaiPeople(pOutCard->byUser))
    {
        DebugPrintf("用户 %d 出牌失败，没有此牌", bDeskStation);

		OutputDebugString("服务器： ReceiveUserOutCard   没有此牌");
        return 0;
    }

    // 还有缺
    if(pDesk->sUserData.GetAKindPaiCount(pOutCard->byUser, pDesk->sUserData.m_byDingQue[pOutCard->byUser]) >0 ) 
    {
        // 打出的牌不是缺
        if (pOutCard->byPs/10 != pDesk->sUserData.m_byDingQue[pOutCard->byUser])
        {
            DebugPrintf("用户 %d 出牌失败，还有缺", bDeskStation);

			OutputDebugString("服务器： ReceiveUserOutCard   还有缺");
            return 0;
        }
    }
	pDesk->sGameData.T_HuPai.Clear();//清理浒牌记录 2012-08-15
	SetDoingThing(bDeskStation,THING_OUT_PAI);//设置执行事件

    for(int i=0;i<PLAY_COUNT;++i)//记录杠牌状态，拥于杠开和杠后炮
    {
		if(i != bDeskStation)
			pDesk->sGameData.m_bGangState[i] = false;
	}
	pDesk->sUserData.m_bQiHu[bDeskStation] = false;//清理玩家弃糊状态


	//记录已经出过牌了
	pDesk->sUserData.m_bHaveOut[bDeskStation] = true;

    pDesk->sGameData.T_OutPai = * pOutCard;
    pDesk->sGameData.m_byThingDoing = THING_OUT_PAI;//记录正在执行的事件
    pDesk->sGameData.m_byNextAction = ACTION_NO;
    pDesk->sUserData.m_byOtherOutPai = pOutCard->byPs;
    //删除一张手牌
    pDesk->sUserData.DelleteAHandPai(pOutCard->byUser,pOutCard->byPs);
    //添加一张出牌
    pDesk->sUserData.AddToOutPai(pOutCard->byUser,pOutCard->byPs);
    //清理临时数据
    ClearTempData();
	pDesk->sUserData.m_byBuGangPai = 255;//补杠得分的牌（用在补杠，抓到能补杠的牌后马上补杠才有分，否则无分）
	//记录事件
	LogThing(THING_OUT_PAI,pOutCard->byPs,bDeskStation);


    //玩家点击了听牌
    if(pDesk->sGameData.T_TingPai.bHitTing[pOutCard->byUser] && !pDesk->sUserData.m_bTing[pOutCard->byUser])//玩家点击了听牌,
    {
        pDesk->sGameData.T_TingPai.bHitTing[pOutCard->byUser] = false;
        if(CLogicBase::IsHaveInArr(pDesk->sGameData.T_CPGNotify[pOutCard->byUser].m_byTingCanOut,pOutCard->byPs,HAND_CARD_NUM))
        {
            pDesk->sUserData.m_bTing[pOutCard->byUser] = true;
            pDesk->sGameData.T_OutPai.bTing = true;
        }
        else
        {
            pDesk->sUserData.m_bTing[pOutCard->byUser] = false;
            pDesk->sGameData.T_TingPai.bHitTing[pOutCard->byUser] = false;
        }
    }
    //拷贝出牌
    for(int i=0;i<4;++i)
    {
        pDesk->sUserData.CopyOutPai(i,pDesk->sGameData.T_OutPai.m_byArOutPai[i]);
    }
    ///当前抓牌方向，true 顺时针，false 逆时针
    pDesk->sUserData.m_bZhuaPaiDir = true;
    for(int i=0;i<4;i++)//获取听牌状态
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
    if(!CheckAction(pOutCard->byUser,pOutCard->byPs,2))
    {//没有动作就直接下一个玩家抓牌
        pDesk->sGameData.ApplyThingID = THING_ZHUA_PAI;
        ApplyAction(THING_ZHUA_PAI,100);	//申请下一个事件

        // 进入无状态
        //SetGameCurState(THING_MAX, 0xFF, 0, 0, false);  
    }
    else
    {
        // 进入拦牌状态
        //SetGameCurState(THING_CPG_NOTIFY, 0xFF, pDesk->sGameData.GetMjSetting().ucBlockTime*1000+TIME_NETWORK, 0, false);

        for (int i=0;i<PLAY_COUNT;i++)
        {	
            if(i==bDeskStation || pDesk->sUserData.m_bFoceLeave[i] || pDesk->sGameData.T_CPGNotify[i].m_byMaxThing == ACTION_NO
                || pDesk->sUserData.m_bIsHu[i])
                continue;
            pDesk->SendGameDataEx(i, &pDesk->sGameData.T_CPGNotify[i], sizeof(pDesk->sGameData.T_CPGNotify[i]), MDM_GM_GAME_NOTIFY, THING_CPG_NOTIFY, 0);
            pDesk->SendWatchDataEx(i, &pDesk->sGameData.T_CPGNotify[i], sizeof(pDesk->sGameData.T_CPGNotify[i]), MDM_GM_GAME_NOTIFY, THING_CPG_NOTIFY, 0);


			if(pDesk->sGameData.T_CPGNotify[i].bHu && pDesk->sUserData.m_MenPai.GetMenPaiNum()<4)//最后四张牌能胡必须胡牌
			{
				tagHuPaiEx hu;
				hu.Clear();
				hu.byUser = pDesk->sGameData.T_ZhuaPai.byUser;
				this->ReceiveUserHuCard(i,&hu,sizeof(hu),false);
			}
        }
        
    }
    pDesk->sUserData.m_byApplyMaxAction = ACTION_NO;//玩家当前申请的最大事件：处理完申请后，恢复数据

    for(int i=0;i<4;i++)//
    {
		if(i == bDeskStation)
			continue;
		pDesk->sUserData.m_bIsGangPao[i] = false;//正在杠后炮的状态
	}

    return 0;
}

// 收到用户pass 消息
int ThingHandleEx::ReceiveUserPassCheck(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser)
{
    if(pDesk->sGameData.ApplyThingID != 255 || bWatchUser)//有申请事件就不响应pass消息
    {
       return 0;
    }
    if (pDesk->sGameData.T_CPGNotify[bDeskStation].bHaveReturn || !pDesk->sGameData.T_CPGNotify[bDeskStation].bCanAction)
    {
        return 0;
    }

	
	if(bDeskStation != pDesk->sUserData.m_byNowOutStation && pDesk->sGameData.T_CPGNotify[bDeskStation].bHu)//玩家能糊没有糊牌,记录弃糊
	{
		pDesk->sUserData.m_bQiHu[bDeskStation] = true;//玩家是否弃糊状态
	}

    pDesk->sGameData.T_CPGNotify[bDeskStation].bHaveReturn = true;//记录玩家回应
    pDesk->sGameData.T_CPGNotify[bDeskStation].m_byMaxThing = ACTION_NO;//改变玩家能做的最大动作

    CheckNextAction();	
	pDesk->sGameData.T_CPGNotify[bDeskStation].Clear();

    return 0;
}

//收到用户同意游戏消息
int ThingHandleEx::ReceiveUserAgreeGame(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser)
{
	return ThingHandle::ReceiveUserAgreeGame(bDeskStation, pData, uSize, bWatchUser);
}

//有事请求离开
int ThingHandleEx::ReceiveUserHaveThing(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser)
{
	return ThingHandle::ReceiveUserHaveThing(bDeskStation, pData, uSize, bWatchUser);
}

//同意用户离开
int ThingHandleEx::ReceiveArgeeUserLeft(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser)
{
	return ThingHandle::ReceiveArgeeUserLeft(bDeskStation, pData, uSize, bWatchUser);
}

// 用户碰牌消息
int ThingHandleEx::ReceiveUserPengCard(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser)
{
	char buff[500];

    tagPengPaiEx * pPengCard = (tagPengPaiEx *)pData;
    if(pPengCard == NULL || bWatchUser ||  pDesk->m_bGameStation != GS_PLAY_GAME)
    {
        return 0;
    }

    if(bDeskStation == pDesk->sUserData.m_byNowOutStation || !pDesk->sGameData.T_CPGNotify[bDeskStation].bPeng || pPengCard->byPs == 255 || !pDesk->sGameData.m_mjAction.bPeng)
    {
        //不合法动作
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
int ThingHandleEx::ReceiveUserGangCard(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser)
{
    tagGangPaiEx * pGangCard = (tagGangPaiEx *)pData;

    if(pGangCard == NULL || bWatchUser || pDesk->m_bGameStation != GS_PLAY_GAME)
    {
        DebugPrintf("玩家 %d 请求杠牌，被拒绝，当前不是游戏状态");
        return 0;
    }
    if(!pDesk->sGameData.T_CPGNotify[bDeskStation].bGang || pGangCard->byPs == 255 || !pDesk->sGameData.m_mjAction.bGang)
    {
        DebugPrintf("玩家 %d 请求杠牌，被拒绝，他不能杠");
        //不合法动作
        return 0;
    }


	OutputDebugString("流程测试： 服务器 ReceiveUserGangCard  收到杠牌合法消息");

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
                pDesk->SendGameDataEx(i, &pDesk->sGameData.T_GangPai, sizeof(pDesk->sGameData.T_GangPai), MDM_GM_GAME_NOTIFY , THING_GANG_PAI, 0);
                pDesk->SendWatchDataEx(i, &pDesk->sGameData.T_GangPai, sizeof(pDesk->sGameData.T_GangPai), MDM_GM_GAME_NOTIFY , THING_GANG_PAI, 0);
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
                    pDesk->SendGameDataEx(i, &pDesk->sGameData.T_GangPai, sizeof(pDesk->sGameData.T_GangPai), MDM_GM_GAME_NOTIFY , THING_GANG_PAI, 0);
                    pDesk->SendWatchDataEx(i, &pDesk->sGameData.T_GangPai, sizeof(pDesk->sGameData.T_GangPai), MDM_GM_GAME_NOTIFY , THING_GANG_PAI, 0);
                }
            }
            else///有人抢杠，发送抢杠消息给别人检测
            {
                for (int i=0;i<PLAY_COUNT;i++)
                {	
                    if(!pDesk->sGameData.T_CPGNotify[i].bCanAction || pDesk->sUserData.m_bFoceLeave[i])
                        continue;
                    pDesk->SendGameDataEx(i, &pDesk->sGameData.T_CPGNotify[i], sizeof(pDesk->sGameData.T_CPGNotify[i]),  MDM_GM_GAME_NOTIFY, THING_CPG_NOTIFY, 0);
                    pDesk->SendWatchDataEx(i, &pDesk->sGameData.T_CPGNotify[i], sizeof(pDesk->sGameData.T_CPGNotify[i]),  MDM_GM_GAME_NOTIFY, THING_CPG_NOTIFY, 0);
                }
                if(pDesk->sGameData.m_mjRule.bNetCutTuoGuan)
                {
                    NetCutAutoHandle();
                }
                return 0;
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
            return 0;
        }
        break;
    }
    return 0;
}

// 用户听牌消息
int ThingHandleEx::ReceiveUserTingCard(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser)
{
	return ThingHandle::ReceiveUserTingCard(bDeskStation, pData, uSize, bWatchUser);
}

// 用户糊牌消息
int ThingHandleEx::ReceiveUserHuCard(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser)
{
	OutputDebugString("抢杠检测：ReceiveUserHuCard 000");
    // 还有缺
    if(pDesk->sUserData.GetAKindPaiCount(bDeskStation, pDesk->sUserData.m_byDingQue[bDeskStation]) >0 ) 
    {
        return 0;
    }

	OutputDebugString("抢杠检测：ReceiveUserHuCard 111");
    tagHuPaiEx * pHuCard = (tagHuPaiEx *)pData;
    if(pHuCard == NULL || bWatchUser || pDesk->m_bGameStation != GS_PLAY_GAME)
    {
        return 0;
    }

	OutputDebugString("抢杠检测：ReceiveUserHuCard 222");
    if(!pDesk->sGameData.T_CPGNotify[bDeskStation].bHu)
    {
        //不合法动作
        return 0;
    }
	OutputDebugString("抢杠检测：ReceiveUserHuCard 333");

    if(pDesk->sUserData.m_byApplyMaxAction < ACTION_HU)
    {
        pDesk->sUserData.m_byApplyMaxAction = ACTION_HU;//玩家当前申请的最大事件
    }

    if(bDeskStation == pDesk->sUserData.m_byNowOutStation)//当前出牌玩家糊牌(自摸)
    {
        pDesk->sGameData.T_HuPai.bZimo = true;
        pDesk->sGameData.T_HuPai.byDianPao = 255;
        pDesk->sGameData.T_HuPai.byPs = 255;
    }
    else
    {
        pDesk->sGameData.T_HuPai.byDianPao = pDesk->sUserData.m_byNowOutStation;
        pDesk->sGameData.T_HuPai.byPs = pDesk->sUserData.m_byOtherOutPai;// pHuCard->byPs;				//点炮的牌值
    }

    pDesk->sGameData.T_HuPai.bCanH[bDeskStation] = true;
    pDesk->sGameData.T_HuPai.byUserNum++;
    pDesk->sGameData.T_HuPai.byNt = pDesk->sUserData.m_byNtStation;		//庄家位置

    pDesk->sGameData.T_CPGNotify[bDeskStation].bHaveReturn = true;//记录玩家回应
    pDesk->sGameData.T_CPGNotify[bDeskStation].m_byMaxThing = ACTION_HU;//改变玩家能做的最大动作

    if(pDesk->sGameData.T_HuPai.byUser == 255)
    {
        pDesk->sGameData.T_HuPai.byUser = bDeskStation;//pHuCard->byUser;
    }
    else if(!pDesk->sGameData.T_HuPai.bZimo)//选择靠糊牌玩家最近的玩家糊牌
    {
        //点炮糊牌，并且有多个玩家可以糊牌（一炮多响）
        pDesk->sGameData.T_HuPai.byUser = bDeskStation;//GetCanHuUser(pDesk->sUserData.m_byNowOutStation,pDesk->sGameData.T_HuPai.bCanH,pDesk->//sGameData.T_HuPai.byUser);//获取离糊牌玩家近的玩家
		pDesk->sGameData.T_HuPai.bDuoXiang = true;//一炮多响

    }


	char buff[800];
	sprintf(buff,"服务器：抓牌 收到湖牌信息 %d  位置%d",pDesk->sGameData.T_HuPai.byUser,bDeskStation);
	OutputDebugString(buff); 

    if(pDesk->sGameData.m_byNextAction < ACTION_HU)
    {
        pDesk->sGameData.m_byNextAction = ACTION_HU;
    }
    for(int i=0;i<MAX_HUPAI_TYPE;++i)
    {
        pDesk->sGameData.T_HuPai.byHuType[bDeskStation][i] = pDesk->sGameData.T_CPGNotify[bDeskStation].byHuType[i];//获取胡牌类型
    }
    for(int i=0;i<HAND_CARD_NUM;++i)
    {
        pDesk->sGameData.T_HuPai.m_byArHandPai[bDeskStation][i] = pDesk->sUserData.m_byArHandPai[bDeskStation][i];
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

		char strdug[300];
		//获取各个玩家的分数
		for (int i=0; i<PLAY_COUNT; ++i)
		{
			sprintf(strdug,"血战麻将测试：ReceiveUserHuCard  位置 %d 总分 %d",i,pDesk->sGameData.T_HuPaiXZMJ[bDeskStation].iZongFen[i]);
			OutputDebugString(strdug);
		}

		pDesk->sUserData.m_bIsHu[bDeskStation] = true;

		for(int i=0;i<PLAY_COUNT;i++)
		{
			pDesk->sGameData.T_HuPaiXZMJ[bDeskStation].m_bIsHu[i] = pDesk->sUserData.m_bIsHu[i];
			if(!pDesk->sUserData.m_bIsHu[i])
				continue;
			//获取糊牌类型
			for(int j=0;j<MAX_HUPAI_TYPE;j++)
			{
				if(i==bDeskStation)
				{
					pDesk->sGameData.T_HuPaiXZMJ[bDeskStation].byHuType[i][j] = pDesk->sGameData.T_HuPai.byHuType[i][j];
				}
				else
				{
					pDesk->sGameData.T_HuPaiXZMJ[bDeskStation].byHuType[i][j] = pDesk->sGameData.T_HuPaiXZMJ[i].byHuType[i][j];
				}
			}
		}

        //通知玩家糊牌了
        memset(pDesk->sGameData.T_HuPaiXZMJ[bDeskStation].m_byArHandPai,255,sizeof(pDesk->sGameData.T_HuPaiXZMJ[bDeskStation].m_byArHandPai));
        pDesk->sUserData.CopyHandPai(pDesk->sGameData.T_HuPaiXZMJ[bDeskStation].m_byArHandPai,0,true);
        for (int i=0;i<PLAY_COUNT;i++)
        {	
            pDesk->sUserData.CopyGCPData(i,pDesk->sGameData.T_HuPaiXZMJ[bDeskStation].m_UserGCPData[i]);
            pDesk->SendGameDataEx(i, &pDesk->sGameData.T_HuPaiXZMJ[bDeskStation], sizeof(pDesk->sGameData.T_HuPaiXZMJ[bDeskStation]), MDM_GM_GAME_NOTIFY, THING_HU_PAI, 0);
            pDesk->SendWatchDataEx(i, &pDesk->sGameData.T_HuPaiXZMJ[bDeskStation], sizeof(pDesk->sGameData.T_HuPaiXZMJ[bDeskStation]), MDM_GM_GAME_NOTIFY, THING_HU_PAI, 0);
        }
        //pDesk->sGameData.T_HuPai.Clear();

    }
    else
    {
        //检测下一个动作
        CheckNextAction();
    }
    return 0;
}
// 用户定缺消息
int ThingHandleEx::ReceiveUserDingQue(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser)
{
    tagDingQueEx *pCmd = (tagDingQueEx *)pData;
    if (uSize!=sizeof (tagDingQueEx))
    {
        return -1;
    }
	if (pDesk->sGameData.m_byThingDoing != THING_DING_QUE || bWatchUser)    // 正在定缺中
    {
        return 0;
    }

    if (255==pDesk->sUserData.m_byDingQue[bDeskStation] ||( pCmd->byQue>=0 && pCmd->byQue<3))   
    {
        pDesk->sUserData.m_byDingQue[bDeskStation] = pCmd->byQue%3;
		pDesk->sGameData.T_DingQue.byQuePai[bDeskStation] = pCmd->byQue%3;
    }
	pDesk->sGameData.T_DingQue.bFinish[bDeskStation] = true;

	OutputDebugString("血战麻将测试： 服务器 ReceiveUserDingQue 0000");

	pDesk->sGameData.T_DingQue.byUser = bDeskStation;
	pDesk->sGameData.T_DingQue.bNotify = false;
    for (int i=0;i<PLAY_COUNT;i++)
    {	
        pDesk->SendGameDataEx(i, &pDesk->sGameData.T_DingQue, sizeof(pDesk->sGameData.T_DingQue), MDM_GM_GAME_NOTIFY, THING_DING_QUE, 0);
        pDesk->SendWatchDataEx(i, &pDesk->sGameData.T_DingQue, sizeof(pDesk->sGameData.T_DingQue), MDM_GM_GAME_NOTIFY, THING_DING_QUE, 0);
    }
    bool bDingQueOver = true;
    //判断是否定缺完成 
    for (int i=0; i<PLAY_COUNT; ++i)
    {
        if (!pDesk->sGameData.T_DingQue.bFinish[i])
        {
            bDingQueOver = false;
            break;
        }
    }
    if (bDingQueOver)
    { // 开始打牌
        pDesk->sGameData.ApplyThingID = THING_BEGIN_OUT;
        ApplyAction(THING_BEGIN_OUT, 100);
    }
    return 0;
}

///名称：CheckNextAction
///描述：检测下一步动作
void ThingHandleEx::CheckNextAction()
{
    if(!IsCanHandle())
    {
        return ;
    }	
    BYTE NextID=255,StarTime=255;//下个事件的id,255时为互动事件需要客户返回，StarTime多长时间后开始执行（100毫秒为单位）

    MJTHINGS nextState = THING_MAX;
    BYTE ucNextChair = 0xFF;
    unsigned int unLasting = 0;
    unsigned int unWaiting = 0;
    bool bNotify = false;
    switch(pDesk->sGameData.m_byNextAction)//确定了下一步的响应事件：吃，碰，杠，胡
    {
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
            nextState = THING_OUT_PAI;
            ucNextChair = pDesk->sGameData.T_PengPai.byUser;
            unLasting = pDesk->sGameData.GetMjSetting().ucOutcardTime*1000-TIME_NETWORK;
            unWaiting = 0;
            bNotify = true;
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
    case ACTION_HU:					//糊动作
        {
            DebugPrintf("为 %d 执行胡牌操作", pDesk->sGameData.T_HuPai.byUser);

			char pchar[500];
			//如果是抢杠要把玩家手中的牌删掉
			if(pDesk->sGameData.T_CPGNotify[pDesk->sGameData.T_HuPai.byUser].bQiangGang) //抢杠糊
			{
				pDesk->sUserData.DelleteAHandPai(pDesk->sGameData.T_CPGNotify[pDesk->sGameData.T_HuPai.byUser].m_byDianPao,pDesk->sGameData.T_CPGNotify[pDesk->sGameData.T_HuPai.byUser].m_iHuPs);
				sprintf(pchar,"抢杠检测： 抢杠胡删掉 杠牌玩家%d手中牌 %d",pDesk->sGameData.T_HuPai.byUser,pDesk->sGameData.T_CPGNotify[pDesk->sGameData.T_HuPai.byUser].m_iHuPs);
				OutputDebugString(pchar);
			}
            if(!pDesk->sGameData.T_HuPai.bZimo)
            {
				for(int i=0;i<4;++i)
				{
					if(pDesk->sGameData.T_HuPai.bCanH[i])
					{
						pDesk->sUserData.AddToHandPai(i,pDesk->sGameData.T_CPGNotify[i].m_iHuPs);//
						OutputDebugString("抢杠检测：添加点炮牌");

						sprintf(pchar,"抢杠检测： 玩家%d 添加点炮牌  %d",i,pDesk->sGameData.T_CPGNotify[i].m_iHuPs);
						OutputDebugString(pchar);
					}
				}
            }

            HandleHuPai(NextID,StarTime);
			///获取各个玩家的分数数据
			BYTE st = pDesk->sGameData.T_HuPai.byUser;
            ///通知玩家糊牌了
            memset(pDesk->sGameData.T_HuPaiXZMJ[st].m_byArHandPai,255,sizeof(pDesk->sGameData.T_HuPaiXZMJ[st].m_byArHandPai));


            pDesk->sUserData.CopyHandPai(pDesk->sGameData.T_HuPaiXZMJ[st].m_byArHandPai,0,true);
            for (int i=0;i<PLAY_COUNT;i++)
            {	
                pDesk->sUserData.CopyOutPai(i,pDesk->sGameData.T_HuPaiXZMJ[st].m_byArOutPai[i]);
            }

			for(int i=0;i<PLAY_COUNT;i++)
			{
				pDesk->sGameData.T_HuPaiXZMJ[st].m_bIsHu[i] = pDesk->sUserData.m_bIsHu[i];
				if(!pDesk->sUserData.m_bIsHu[i])
					continue;
				//获取糊牌类型
				for(int j=0;j<MAX_HUPAI_TYPE;j++)
				{
					if(i==st)
					{
						pDesk->sGameData.T_HuPaiXZMJ[st].byHuType[i][j] = pDesk->sGameData.T_HuPai.byHuType[i][j];
					}
					else
					{
						pDesk->sGameData.T_HuPaiXZMJ[st].byHuType[i][j] = pDesk->sGameData.T_HuPaiXZMJ[i].byHuType[i][j];
					}
				}
			}

            for (int i=0;i<PLAY_COUNT;i++)
            {	
                pDesk->sUserData.CopyGCPData(i,pDesk->sGameData.T_HuPaiXZMJ[st].m_UserGCPData[i]);
                pDesk->SendGameDataEx(i, &pDesk->sGameData.T_HuPaiXZMJ[st], sizeof(pDesk->sGameData.T_HuPaiXZMJ[st]), MDM_GM_GAME_NOTIFY, THING_HU_PAI, 0);
                pDesk->SendWatchDataEx(i, &pDesk->sGameData.T_HuPaiXZMJ[st], sizeof(pDesk->sGameData.T_HuPaiXZMJ[st]), MDM_GM_GAME_NOTIFY, THING_HU_PAI, 0);
            }
            // 确定下一个动作
            ////pDesk->sGameData.T_HuPai.Clear();
        }
        break;
    case ACTION_NO:
        {
            if(pDesk->sGameData.m_byThingDoing == THING_OUT_PAI)//出牌事件
            {
                DebugPrintf("拦牌CheckNextAction返回Action_no，不再等待拦牌，进入抓牌动作");

				OutputDebugString("流程测试： CheckNextAction ACTION_NO");

                NextID = THING_ZHUA_PAI;//执行抓牌事件
                StarTime = 1;
            }
            else
            {
                DebugPrintf("拦牌CheckNextAction返回Action_no，继续等待拦牌");
            }
        }
        break;
    }
    pDesk->sGameData.ApplyThingID = NextID;
    ApplyAction(NextID,StarTime*100);//申请下一个事件
  
    //SetGameCurState(nextState, ucNextChair, unLasting, unWaiting, bNotify);
}

BYTE ThingHandleEx::GetCanHuUser(BYTE outst,bool hu[],BYTE hust)//获取离糊牌玩家近的玩家
{

	return ThingHandle::GetCanHuUser(outst,hu, hust);
}

///获取下家位置
BYTE ThingHandleEx::GetNextStation(BYTE station,bool shun)
{
    BYTE re = 255;
    if(shun)//顺时针
    {
        int nLoop = 8;
        re = (station + 1)%PLAY_COUNT;
        while (nLoop-->0)
        {            
            if (!pDesk->sUserData.m_bIsHu[ re ])
            {
                break;
            }
            re = (re + 1)%PLAY_COUNT;
        }
    }
    else
    {
        int nLoop = 8;
        re = (station + 3)%PLAY_COUNT;
        while (nLoop-->0)
        {            
            if (!pDesk->sUserData.m_bIsHu[ re ])
            {
                break;
            }
            re = (re+3)%PLAY_COUNT;
        }
    }
    return re;
}

///清理临时数据
void ThingHandleEx::ClearTempData()
{
	ThingHandle::ClearTempData();
}

///检查是否可以进行下一步处理
bool ThingHandleEx::IsCanHandle()
{
    int num = 0, re = 0;
    BYTE max = ACTION_NO;
    BYTE station = pDesk->sGameData.T_OutPai.byUser;
	bool qianggang = false;

    for(int i=0;i<4;++i)//查找出最大事件
    {
        if (pDesk->sUserData.m_bIsHu[i])        // 已胡牌玩家
        {
            continue;
        }
        if(pDesk->sGameData.T_CPGNotify[i].bQiangGang && pDesk->sGameData.T_CPGNotify[i].bHu)
			qianggang = true;
    }

    if(qianggang)//抢杠事件
    {//考虑抢杠胡等操作


        station = pDesk->sGameData.T_GangPai.byUser;
        for(int i=0;i<4;++i)//查找出最大事件
        {
            if (pDesk->sUserData.m_bIsHu[i])        // 已胡牌玩家
            {
                continue;
            }
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
        for(int i=0;i<4;++i)//最大事件个数，和最大事件回应的个数
        {
            if (pDesk->sUserData.m_bIsHu[i])        // 已胡牌玩家
            {
                continue;
            }
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
        for(int i=0;i<4;++i)//查找出最大事件
        {
            if (pDesk->sUserData.m_bIsHu[i])        // 已胡牌玩家
            {
                continue;
            }
            if(i==station || pDesk->sGameData.T_CPGNotify[i].m_byMaxThing <= max || pDesk->sGameData.T_CPGNotify[i].m_byMaxThing==255)
                continue;
            max = pDesk->sGameData.T_CPGNotify[i].m_byMaxThing;
        }
        if(max == ACTION_NO)
        {
            pDesk->sGameData.m_byNextAction = ACTION_NO;
            return true;
        }
        for(int i=0;i<4;++i)//最大事件个数，和最大事件回应的个数
        {
            if (pDesk->sUserData.m_bIsHu[i])        // 已胡牌玩家
            {
                continue;
            }
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

///////////流程事件处理///////////////////////////////////////////////////////////////////////////////////////

//请求事件,type要申请的事件
void ThingHandleEx::ApplyAction(int type,int time)
{
	if(type >= THING_BEGIN && type <= THING_ENG_UN_NORMAL)
	{
		pDesk->sGameData.ApplyThingID= type;
	}

	char buff[500];
	sprintf(buff,"事件：ApplyAction 申请事件%d ",pDesk->sGameData.ApplyThingID);
	OutputDebugString(buff);

    if(pDesk->sGameData.ApplyThingID != 255)
    {
        pDesk->SetTimer(10,time);
    }
}

///名称：GameCPU	dwj1-2
///描述：游戏事件处理中心，
void ThingHandleEx::GameCPU(int type)
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
            ExecuteAllBuHua(nextID,time);
        }
        break;
    case THING_ONE_BUHUA://单个玩家抓到花牌后补花事件
        {
            ExecuteOneBuHua(nextID,time);
        }
        break;
    case THING_DING_QUE://定缺事件
        {
            ExecuteDingQue(nextID,time);
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
            ExecuteZhuaPai(nextID,time);
        }
        break;
    case THING_COUNT_FEN://算分事件
        {
            ExecuteCountFen(nextID,time);
        }
        break;
    case THING_ENG_HANDLE://结束处理事件
        {
            ExecuteNormalEnd(nextID,time);
        }
        break;
    case THING_ENG_UN_NORMAL://异常结束处理事件
        {
            ExecuteUnNormalEnd(nextID,time);
        }
        break;
    }

	

    pDesk->sGameData.ApplyThingID = nextID;
    ApplyAction(nextID,time*100);//申请下一个事件

}

///开始事件处理
void ThingHandleEx::ExecuteGameBegin(BYTE &nextID,BYTE &time)
{   
    pDesk->sGameData.T_Begin.byDo = 1;
    pDesk->sGameData.InitData();//初始化数据
    pDesk->sUserData.InitData();       

	//memset(m_bGangState,0,sizeof(m_bGangState));//记录杠牌状态，拥于杠开和杠后炮


	pDesk->KillTimer(TIME_WAIT_MSG);
	pDesk->SetTimer(TIME_WAIT_MSG,1000);//重新设置超时计时器

	pDesk->sGameData.m_byThingDoing = THING_BEGIN;
	SetDoingThing(0,THING_BEGIN);//设置执行事件

    if(pDesk->sUserData.m_byNtStation!=255)
    {
        //已经有了庄家
        nextID = THING_2SEZI_DIR_GP;//掷色子发牌
    }
    else 
    {
        srand(GetTickCount());
        // 随机定庄
        pDesk->sUserData.m_byNtStation = rand()%PLAY_COUNT;
        nextID = THING_2SEZI_DIR_GP;//掷色子发牌
    }

    pDesk->sGameData.T_Begin.byNt = pDesk->sUserData.m_byNtStation;			// 庄家位置
    pDesk->sGameData.T_Begin.byQuanFeng =  pDesk->sUserData.m_byQuanFeng;	// 圈风设置
    memset(pDesk->sGameData.T_Begin.byMenPai,255,sizeof(pDesk->sGameData.T_Begin.byMenPai));

	pDesk->sUserData.m_byTempNt = 255;//记录下局换庄玩家

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
    time = 1;

	//记录事件
	LogThing( pDesk->sGameData.m_byThingDoing);
}

///以第一个玩家为庄家（以东为庄事件）
void ThingHandleEx::ExecuteDongNt(BYTE &nextID,BYTE &time)
{
	ThingHandle::ExecuteDongNt(nextID,time);	
	//记录事件
	LogThing( pDesk->sGameData.m_byThingDoing);
	SetDoingThing(0,THING_DONG_NT);//设置执行事件
}

///掷2颗色子确定庄家（点数和）
void ThingHandleEx::ExecuteTwoSeziNt(BYTE &nextID,BYTE &time)	
{
	ThingHandle::ExecuteTwoSeziNt(nextID,time);	
}

///掷2颗色子确定庄家和起牌方向（点数和庄家，大点方向,未使用）
void ThingHandleEx::ExecuteTwoSeziNtAndDir(BYTE &nextID,BYTE &time)
{
	ThingHandle::ExecuteTwoSeziNtAndDir(nextID,time);
}

///轮流掷色子确定庄家
void ThingHandleEx::ExecuteTurnSeziNt(BYTE &nextID,BYTE &time)	
{
	ThingHandle::ExecuteTurnSeziNt(nextID,time);	
}

///掷2颗色子确定庄家+起牌方向+起牌敦数（点数和庄家，大点方向，小点敦数）
void ThingHandleEx::ExecuteTwoSeziNtDirGp(BYTE &nextID,BYTE &time)	
{
	ThingHandle::ExecuteTwoSeziNtDirGp(nextID,time);	
	pDesk->sUserData.m_bWaitSezi = true;
}

///掷2颗色子确定起牌方向（点数和确定方向）
void ThingHandleEx::ExecuteTwoSeziDir(BYTE &nextID,BYTE &time)	
{
	ThingHandle::ExecuteTwoSeziDir(nextID,time);
	pDesk->sUserData.m_bWaitSezi = true;
}

///掷2颗色子确定起牌方向和起牌敦数（点数和方向，小点敦数）
void ThingHandleEx::ExecuteTwoSeziDirGp(BYTE &nextID,BYTE &time)	
{    
	SetDoingThing(0,THING_2SEZI_DIR_GP);//设置执行事件

    pDesk->sGameData.m_byThingDoing = THING_2SEZI_DIR_GP;//记录正在发生的事件
    ///掷2颗色子确定起牌位置（点数和）和起牌点（最小点）事件	
    pDesk->sGameData.T_TwoSeziDirAndGetPai.byUser = pDesk->sUserData.m_byNtStation;
    //摇骰子
    CLogicBase::GetSeziData(&pDesk->sGameData.T_TwoSeziDirAndGetPai.bySezi0,&pDesk->sGameData.T_TwoSeziDirAndGetPai.bySezi1);
    pDesk->sGameData.T_TwoSeziDirAndGetPai.bySezi0 = pDesk->sGameData.T_TwoSeziDirAndGetPai.bySezi0%6+1;
    pDesk->sGameData.T_TwoSeziDirAndGetPai.bySezi1 = pDesk->sGameData.T_TwoSeziDirAndGetPai.bySezi1%6+1;
    //起牌方向
    pDesk->sUserData.m_MenPai.byGetPaiDir = (pDesk->sGameData.T_TwoSeziDirAndGetPai.bySezi1 + pDesk->sGameData.T_TwoSeziDirAndGetPai.bySezi0)%4-1;
    //起牌点数
    pDesk->sUserData.m_MenPai.byGetPai = min(pDesk->sGameData.T_TwoSeziDirAndGetPai.bySezi0, pDesk->sGameData.T_TwoSeziDirAndGetPai.bySezi1);
    if(pDesk->sGameData.T_TwoSeziDirAndGetPai.bySezi0 > pDesk->sGameData.T_TwoSeziDirAndGetPai.bySezi1)
    {
        pDesk->sUserData.m_MenPai.byGetPai = pDesk->sGameData.T_TwoSeziDirAndGetPai.bySezi1-1;
    }
    pDesk->sGameData.T_TwoSeziDirAndGetPai.byGetPai = pDesk->sUserData.m_MenPai.byGetPai;

    //通知客户端
    for (int i=0;i<PLAY_COUNT;i++)
    {
        pDesk->SendGameDataEx(i, &pDesk->sGameData.T_TwoSeziDirAndGetPai, sizeof(pDesk->sGameData.T_TwoSeziDirAndGetPai), MDM_GM_GAME_NOTIFY, THING_2SEZI_DIR_GP, 0);
    }
    pDesk->SendWatchDataEx(PLAY_COUNT, &pDesk->sGameData.T_TwoSeziDirAndGetPai, sizeof(pDesk->sGameData.T_TwoSeziDirAndGetPai), MDM_GM_GAME_NOTIFY, THING_2SEZI_DIR_GP, 0);
    
    //设置等待色子动画结束状态
    //memset(pDesk->sUserData.m_bSiziFinish,0,sizeof(pDesk->sUserData.m_bSiziFinish));
    //pDesk->sUserData.m_bWaitSezi = true;
	pDesk->sUserData.m_bWaitSezi = true;
    nextID = THING_SEND_PAI;
    time = 30;

	//记录事件
	LogThing( pDesk->sGameData.m_byThingDoing);
}

///掷2颗色子确定起牌敦数（点数和敦数）
void ThingHandleEx::ExecuteTwoSeziGp(BYTE &nextID,BYTE &time)	
{
	ThingHandle::ExecuteTwoSeziGp(nextID,time);	
}

///加注事件（确定庄家后，可以通过选择加注增加输赢）
void ThingHandleEx::ExecuteAddNote(BYTE &nextID,BYTE &time)	
{
	ThingHandle::ExecuteAddNote(nextID,time);
}

///发牌
void ThingHandleEx::ExecuteSendPai(BYTE &nextID,BYTE &time)	
{
    pDesk->sGameData.m_byThingDoing = THING_SEND_PAI;//记录正在发生的事件

	SetDoingThing(0,THING_SEND_PAI);//设置执行事件

    ///当前抓牌方向，true 顺时针，false 逆时针
    pDesk->sUserData.m_bZhuaPaiDir = true;    
    ///发牌事件
    pDesk->sGameData.T_SendPai.byDo = 1;	//    
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



    for(int i=0;i<PLAY_COUNT;i++)
    {	
        memset(pDesk->sGameData.T_SendPai.m_byArHandPai,255,sizeof(pDesk->sGameData.T_SendPai.m_byArHandPai));
        pDesk->sUserData.CopyHandPai(pDesk->sGameData.T_SendPai.m_byArHandPai,i,false);
        pDesk->SendGameDataEx(i, &pDesk->sGameData.T_SendPai, sizeof(pDesk->sGameData.T_SendPai), MDM_GM_GAME_NOTIFY, THING_SEND_PAI, 0);
        pDesk->SendWatchDataEx(i, &pDesk->sGameData.T_SendPai, sizeof(pDesk->sGameData.T_SendPai), MDM_GM_GAME_NOTIFY, THING_SEND_PAI, 0);
    }

    nextID = THING_DING_QUE;//下一个事件的代号：开始定缺
    time = 10;

	pDesk->sUserData.m_bWaitSezi = false;

	//记录事件
	LogThing( pDesk->sGameData.m_byThingDoing );
}

///掷2颗色子定精事件
void ThingHandleEx::ExecuteTwoSeziMakeJing(BYTE &nextID,BYTE &time)	
{
	ThingHandle::ExecuteTwoSeziMakeJing(nextID,time);	
}

///所有玩家补花事件
void ThingHandleEx::ExecuteAllBuHua(BYTE &nextID,BYTE &time)	
{
	ThingHandle::ExecuteAllBuHua(nextID,time);	
}

///单个玩家补花事件
void ThingHandleEx::ExecuteOneBuHua(BYTE &nextID,BYTE &time)
{
	ThingHandle::ExecuteOneBuHua(nextID,time);	
}

///定缺事件
void ThingHandleEx::ExecuteDingQue(BYTE &nextID,BYTE &time)
{
    pDesk->sGameData.m_byThingDoing = THING_DING_QUE;//记录正在发生的事件
	SetDoingThing(0,THING_DING_QUE);//设置执行事件

	OutputDebugString("血战麻将：服务器 ExecuteDingQue ");
    // 等待玩家定缺
    nextID = 255;       // 无下个事件，等待定缺完成
	time = 255;

	//先搞一个随机缺门
	srand(GetTickCount());
	for(int i=0;i<PLAY_COUNT;++i)
	{
		pDesk->sUserData.m_byDingQue[i] = rand()%3;
	}


	//发送通知
	pDesk->sGameData.T_DingQue.bNotify = true;
    for (int i=0;i<PLAY_COUNT;i++)
    {	

		OutputDebugString("血战麻将：服务器 ExecuteDingQue  发送定缺通知");

		pDesk->sGameData.T_DingQue.byUser = i;
        pDesk->SendGameDataEx(i, &pDesk->sGameData.T_DingQue, sizeof(pDesk->sGameData.T_DingQue), MDM_GM_GAME_NOTIFY, THING_DING_QUE, 0);
    }
    pDesk->SendWatchDataEx(PLAY_COUNT, &pDesk->sGameData.T_DingQue, sizeof(pDesk->sGameData.T_DingQue), MDM_GM_GAME_NOTIFY, THING_DING_QUE, 0);
	
    //SetGameCurState(THING_DING_QUE, 0xFF, pDesk->sGameData.GetMjSetting().ucDingQueTime*1000+TIME_NETWORK);
	
	
	//记录事件
	LogThing( pDesk->sGameData.m_byThingDoing );
	NetCutAutoHandle();
}

///开始出牌通知事件
void ThingHandleEx::ExecuteBeginOut(BYTE &nextID,BYTE &time)
{    

	char strdug[300];
	OutputDebugString("血战麻将测试：ExecuteBeginOut");
    pDesk->sGameData.m_byThingDoing = THING_BEGIN_OUT;//记录正在发生的事件

	SetDoingThing(pDesk->sUserData.m_byNtStation,THING_BEGIN_OUT);//设置执行事件

    ///开始出牌通知事件 
    pDesk->sGameData.T_BeginOutPai.byNt = pDesk->sUserData.m_byNtStation;//庄家位置
    pDesk->sGameData.T_BeginOutPai.byUser = pDesk->sUserData.m_byNtStation;//庄家位置
    pDesk->sUserData.m_byNowOutStation = pDesk->sUserData.m_byNtStation;//出牌位置
    for(int i=0;i<4;i++)
    {
		pDesk->sGameData.T_BeginOutPai.byQuePai[i] = pDesk->sUserData.m_byDingQue[i];//获取缺门牌
        pDesk->sUserData.CopyMenPai(i,pDesk->sGameData.T_BeginOutPai.byMenPai[i]);//拷贝门牌
    }
    ChangeMenPai();//转换门牌


	sprintf(strdug,"事件：ExecuteGameBegin 杠状态 %d %d %d %d",pDesk->sGameData.m_bGangState[0] ,pDesk->sGameData.m_bGangState[1] ,pDesk->sGameData.m_bGangState[2] ,pDesk->sGameData.m_bGangState[3] );
	OutputDebugString(strdug);

    //检测动作
    pDesk->sGameData.T_CPGNotify[pDesk->sUserData.m_byNtStation].Clear();
    CheckAction(pDesk->sUserData.m_byNtStation,pDesk->sUserData.GetLastHandPai(pDesk->sUserData.m_byNowOutStation),0);    
    //发送通知
    for (int i=0;i<PLAY_COUNT;i++)
    {	
        pDesk->SendGameDataEx(i, &pDesk->sGameData.T_BeginOutPai, sizeof(pDesk->sGameData.T_BeginOutPai), MDM_GM_GAME_NOTIFY, THING_BEGIN_OUT, 0);
    }
    pDesk->SendWatchDataEx(PLAY_COUNT, &pDesk->sGameData.T_BeginOutPai, sizeof(pDesk->sGameData.T_BeginOutPai), MDM_GM_GAME_NOTIFY, THING_BEGIN_OUT, 0);


    DebugPrintf("游戏开始，进入出牌状态，%d为庄", pDesk->sUserData.m_byNtStation);
    // 进入出牌状态
    //SetGameCurState(THING_OUT_PAI, pDesk->sUserData.m_byNtStation, pDesk->sGameData.GetMjSetting().ucOutcardTime*1000+TIME_NETWORK,0,false);
    
    pDesk->sGameData.T_CPGNotify[pDesk->sUserData.m_byNtStation].bZhuaPai = true;
    if (pDesk->sGameData.T_CPGNotify[pDesk->sUserData.m_byNtStation].bCanAction)
    {
		sprintf(strdug,"血战麻将测试：ExecuteBeginOut 发送通知 庄家位置%d ",pDesk->sUserData.m_byNtStation);
		OutputDebugString(strdug);
        pDesk->SendGameDataEx(pDesk->sUserData.m_byNtStation, &pDesk->sGameData.T_CPGNotify[pDesk->sUserData.m_byNtStation] ,sizeof(tagCPGNotifyEx), MDM_GM_GAME_NOTIFY, THING_CPG_NOTIFY, 0);
        pDesk->SendWatchDataEx(pDesk->sUserData.m_byNtStation, &pDesk->sGameData.T_CPGNotify[pDesk->sUserData.m_byNtStation] ,sizeof(tagCPGNotifyEx), MDM_GM_GAME_NOTIFY, THING_CPG_NOTIFY, 0);
    }

    nextID = 255;    

	//记录事件
	LogThing( pDesk->sGameData.m_byThingDoing );
}

///抓牌事件
void ThingHandleEx::ExecuteZhuaPai(BYTE &nextID,BYTE &time)	
{
    ///抓牌牌事件
    nextID = 255;
    BYTE card = 255;
    if(pDesk->sUserData.m_MenPai.GetMenPaiNum()<=pDesk->sGameData.m_mjRule.byGamePassNum)
    {
		pDesk->sGameData.T_HuPai.Clear();
        nextID = THING_ENG_HANDLE;
        time = 1;
		
			OutputDebugString("服务器：ExecuteZhuaPai");
    }
    else
    {
		char buff[800];
		sprintf(buff,"服务器：抓牌 ExecuteZhuaPai  ----- MenPaiNum=%d PassNum=%d  donging = %d",pDesk->sUserData.m_MenPai.GetMenPaiNum(),pDesk->sGameData.m_mjRule.byGamePassNum,pDesk->sGameData.m_byThingDoing);
		OutputDebugString(buff); 
		//if()

        if( pDesk->sGameData.m_byThingDoing == THING_OUT_PAI)//出牌事件
        {
            pDesk->sUserData.m_byNowOutStation = GetNextStation(pDesk->sUserData.m_byNowOutStation);//抓牌位置
        }

        if( THING_HU_PAI == pDesk->sGameData.m_byThingDoing)//胡牌事件
        {
			char nick[100];
			pDesk->GetUserNickName(pDesk->sGameData.T_HuPai.byUser,nick);
			sprintf(buff,"服务器：抓牌 湖牌玩家：%s 位置%d",nick,pDesk->sGameData.T_HuPai.byUser);
			OutputDebugString(buff); 
            //pDesk->sUserData.m_byNowOutStation = GetNextStation(pDesk->sUserData.m_byNowOutStation);//抓牌位置
            //pDesk->sUserData.m_byNowOutStation = GetNextStation(pDesk->sGameData.T_HuPai.byUser);//抓牌位置
			BYTE st = pDesk->sGameData.T_HuPai.byUser;
			for(int i=0;i<3;++i)
			{
				st = GetNextStation(st);
				if(!pDesk->sUserData.m_bIsHu[st])//
					break;
			}

			pDesk->sUserData.m_byNowOutStation = st;//抓牌位置
			pDesk->GetUserNickName(st,nick);
			sprintf(buff,"服务器：抓牌 出牌玩家：%s",nick);
			OutputDebugString(buff); 

        }
		pDesk->sGameData.T_HuPai.Clear();//清理浒牌记录


        pDesk->sGameData.m_byThingDoing = THING_ZHUA_PAI;//记录正在发生的事件

		SetDoingThing(pDesk->sUserData.m_byNowOutStation,THING_ZHUA_PAI);//设置执行事件

		pDesk->sUserData.m_bQiHu[pDesk->sUserData.m_byNowOutStation] = false;//清理玩家弃糊状态

        pDesk->sGameData.T_ZhuaPai.byUser = pDesk->sUserData.m_byNowOutStation;//庄家位置
        card = GetPai(pDesk->sUserData.m_byNowOutStation,pDesk->sUserData.m_bZhuaPaiDir,0,&pDesk->sGameData.T_ZhuaPai.byGetPaiIndex);//正常抓牌
        pDesk->sGameData.T_ZhuaPai.byPs = card;		//抓牌的值

		//抓到空牌强制结束
		if(card==255 && (pDesk->sUserData.m_MenPai.GetMenPaiNum()-5)<=pDesk->sGameData.m_mjRule.byGamePassNum)
		{
			nextID = THING_ENG_HANDLE;
			time = 1;
		}

		//记录事件
		LogThing( THING_ZHUA_PAI,card,pDesk->sUserData.m_byNowOutStation);

        ///如果设置了要抓的牌(发牌器版本)
        if(pDesk->sUserData.m_SetData[pDesk->sUserData.m_byNowOutStation].bySetType==2)
        {		
            DebugPrintf("设置了发牌器抓牌");
            pDesk->sUserData.DelleteAHandPai(pDesk->sUserData.m_byNowOutStation,card);
            card = pDesk->sUserData.m_SetData[pDesk->sUserData.m_byNowOutStation].mjdata[0];
            pDesk->sUserData.AddToHandPai(pDesk->sUserData.m_byNowOutStation,card);

			pDesk->sUserData.m_SetData[pDesk->sUserData.m_byNowOutStation].Init();
        }

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

        ChangeMenPai();//转换门牌

        for(int i=0;i<4;i++)
        {
            pDesk->sUserData.CopyMenPai(i,pDesk->sGameData.T_ZhuaPai.byMenPai[i]);//拷贝门牌
            pDesk->sUserData.CopyHuaPai(i,pDesk->sGameData.T_ZhuaPai.m_byArHuaPai[i]);//拷贝花牌
            pDesk->sUserData.CopyOutPai(i,pDesk->sGameData.T_ZhuaPai.m_byArOutPai[i]);//拷贝出牌
        }

		pDesk->sUserData.m_byBuGangPai = card;//补杠得分的牌（用在补杠，抓到能补杠的牌后马上补杠才有分，否则无分）


		//sprintf(buff,"牌数：抓牌 玩家%d 牌%d  ",pDesk->sUserData.m_byNowOutStation,pDesk->sUserData.m_byBuGangPai);
		//OutputDebugString(buff);
    }

    for(int i=0;i<PLAY_COUNT;++i)//记录杠牌状态，拥于杠开和杠后炮
    {
		if(i != pDesk->sUserData.m_byNowOutStation)
			 pDesk->sGameData.m_bGangState[i] = false;
	}


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
    {
        // 进入出牌状态
        //SetGameCurState(THING_OUT_PAI, pDesk->sGameData.T_ZhuaPai.byUser,  pDesk->sGameData.GetMjSetting().ucOutcardTime*1000*2+TIME_NETWORK,0,false);

        //id=255说明即没有流局也不是补花，可以发送吃碰杠通知，如果断线就直接出牌
        pDesk->sGameData.T_CPGNotify[pDesk->sGameData.T_ZhuaPai.byUser].byUser = pDesk->sGameData.T_ZhuaPai.byUser;
        pDesk->sGameData.T_CPGNotify[pDesk->sGameData.T_ZhuaPai.byUser].bZhuaPai = true;

        if (pDesk->sGameData.T_CPGNotify[pDesk->sGameData.T_ZhuaPai.byUser].bCanAction)
        {
			if(pDesk->sGameData.T_CPGNotify[pDesk->sGameData.T_ZhuaPai.byUser].bHu && pDesk->sUserData.m_MenPai.GetMenPaiNum()<4)//最后四张牌能胡必须胡牌
			{
				//tagHuPaiEx hu;
				//hu.Clear();
				//hu.byUser = pDesk->sGameData.T_ZhuaPai.byUser;
				//this->ReceiveUserHuCard(pDesk->sGameData.T_ZhuaPai.byUser,&hu,sizeof(hu),false);

			}

			OutputDebugString("事件：ExecuteZhuaPai 发送拦牌消息 111 ");

            pDesk->SendGameDataEx(pDesk->sGameData.T_ZhuaPai.byUser, &pDesk->sGameData.T_CPGNotify[pDesk->sGameData.T_ZhuaPai.byUser]
            , sizeof(tagCPGNotifyEx), MDM_GM_GAME_NOTIFY, THING_CPG_NOTIFY, 0);
            pDesk->SendWatchDataEx(pDesk->sGameData.T_ZhuaPai.byUser, &pDesk->sGameData.T_CPGNotify[pDesk->sGameData.T_ZhuaPai.byUser]
            , sizeof(tagCPGNotifyEx), MDM_GM_GAME_NOTIFY, THING_CPG_NOTIFY, 0);
        } 
		////断线自动处理
		//if(pDesk->sUserData.m_bFoceLeave[pDesk->sUserData.m_byNowOutStation])
		//{
		//	this->NetCutAutoHandle();
		//}
    }
}

///算分事件
void ThingHandleEx::ExecuteCountFen(BYTE &nextID,BYTE &time)
{
	ThingHandle::ExecuteCountFen(nextID,time);	
}

///正常游戏结束事件
void ThingHandleEx::ExecuteNormalEnd(BYTE &nextID,BYTE &time)
{    
	pDesk->sGameData.m_byThingDoing = THING_ENG_HANDLE;//当前执行事件

    pDesk->sGameData.T_CountFen.Clear();  
	pDesk->sUserData.CopyHandPai(pDesk->sGameData.T_CountFen.m_byArHandPai,0,true);
	pDesk->sUserData.CopyHandPai(pDesk->sGameData.T_HuPai.m_byArHandPai,0,true);

    int nHuCount = 0;
    for (int i=0; i<PLAY_COUNT; ++i)
    {
        if (pDesk->sUserData.m_bIsHu[i])
        {
            nHuCount++;    
			pDesk->sGameData.T_CountFen.bHu[i] = true;
			pDesk->sGameData.T_CountFen.byUser = i;
			pDesk->sGameData.T_CountFen.m_bIsZiMo[i] = pDesk->sGameData.T_HuPaiXZMJ[i].bZimo;  //是否自摸
			pDesk->sGameData.T_CountFen.m_byDianPao[i] = pDesk->sGameData.T_HuPaiXZMJ[i].byDianPao; //点炮位置
			pDesk->sGameData.T_CountFen.m_iZongFan[i] = pDesk->sGameData.T_HuPaiXZMJ[i].m_iZongFan[i];//总番数
			pDesk->sGameData.T_CountFen.m_iGengCount[i] = pDesk->sUserData.m_byGengCount[i];//玩家根数

			for(int j=0;j<20;++j)
			{
				pDesk->sGameData.T_CountFen.byHuType[i][j] =  pDesk->sGameData.T_HuPaiXZMJ[i].byHuType[i][j];
			}
        }
    }

	if(nHuCount <= 2)
	{//流局，查花猪，查叫
		pDesk->sGameData.T_CountFen.byUser = 255;
		tagHuPaiEx hu;
		hu.Clear();
		pDesk->sCountFen.CountFen(hu,pDesk->sGameData.T_CountFen);
		
	}
	else
	{
		pDesk->sGameData.T_CountFen.m_bHuaJiao = false;
	}

    // 把所有结算合在一起
    for (int i=0; i<PLAY_COUNT; ++i)
    {
		//总分
        pDesk->sGameData.T_CountFen.iZongFen[i] += (pDesk->sGameData.T_CountFenXZMJ[0].iZongFen[i]+ pDesk->sGameData.T_CountFenXZMJ[1].iZongFen[i]+pDesk->sGameData.T_CountFenXZMJ[2].iZongFen[i]+ pDesk->sGameData.T_CountFenXZMJ[3].iZongFen[i]);
		//胡牌分
		pDesk->sGameData.T_CountFen.iHuFen[i] += (pDesk->sGameData.T_CountFenXZMJ[0].iHuFen[i]+ pDesk->sGameData.T_CountFenXZMJ[1].iHuFen[i]+pDesk->sGameData.T_CountFenXZMJ[2].iHuFen[i]+ pDesk->sGameData.T_CountFenXZMJ[3].iHuFen[i]);
		//刮风下雨
		//pDesk->sGameData.T_CountFen.iGangFen[i] += (pDesk->sGameData.T_CountFenXZMJ[0].iGangFen[i]+ pDesk->sGameData.T_CountFenXZMJ[1].iGangFen[i]+pDesk->sGameData.T_CountFenXZMJ[2].iGangFen[i]+ pDesk->sGameData.T_CountFenXZMJ[3].iGangFen[i]);
		//呼叫转移
        pDesk->sGameData.T_CountFen.m_iZhuanYi[i] += (pDesk->sGameData.T_CountFenXZMJ[0].m_iZhuanYi[i]+ pDesk->sGameData.T_CountFenXZMJ[1].m_iZhuanYi[i]+pDesk->sGameData.T_CountFenXZMJ[2].m_iZhuanYi[i]+ pDesk->sGameData.T_CountFenXZMJ[3].m_iZhuanYi[i]);

		if(pDesk->sGameData.T_CountFenXZMJ[0].m_bZhuanYi || pDesk->sGameData.T_CountFenXZMJ[1].m_bZhuanYi || pDesk->sGameData.T_CountFenXZMJ[2].m_bZhuanYi || pDesk->sGameData.T_CountFenXZMJ[3].m_bZhuanYi)
		{
			pDesk->sGameData.T_CountFen.m_bZhuanYi = true;
		}
    }

	//刮风下雨最后算
	int igang[4];
	memset(igang,0,sizeof(igang));
	pDesk->sCountFen.CountGangFen(igang);

    for (int i=0; i<PLAY_COUNT; ++i)
    {
		pDesk->sGameData.T_CountFen.iZongFen[i] += igang[i];
		pDesk->sGameData.T_CountFen.iGangFen[i] = igang[i];
	}


    ///游戏结束处理事件
	if(pDesk->sUserData.m_byTempNt>0 && pDesk->sUserData.m_byTempNt<PLAY_COUNT)
	{
		pDesk->sUserData.m_byNtStation = pDesk->sUserData.m_byTempNt;//记录下局换庄玩家
	}

    pDesk->sGameData.T_CountFen.byNextNt = pDesk->sUserData.m_byNtStation;
	//记录事件
	LogThing( pDesk->sGameData.m_byThingDoing);

	pDesk->KillTimer(TIME_CHECK_GAME_MSG);
    //调用平台结束游戏接口
    pDesk->GameFinish(pDesk->sGameData.T_HuPai.byUser,GF_NORMAL);

	pDesk->KillTimer(TIME_WAIT_MSG);
	this->ReSetOutTime();
}

///异常游戏结束事件事件
void ThingHandleEx::ExecuteUnNormalEnd(BYTE &nextID,BYTE &time)
{
	ThingHandle::ExecuteUnNormalEnd(nextID,time);
}

///获取牌数据，
void ThingHandleEx::DisPatchCard()
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
}

///描述：转换门牌数据,将总门牌数组中的数据转换到每个玩家的门牌数组中
void ThingHandleEx::ChangeMenPai()
{
	//ThingHandle::ChangeMenPai();
	//int count = 0;
	//int num = 0;
	//for(int i=0;i<4;i++)
	//{
	//	num = 27;
	//	if(i == 0 || i == 2)
	//	{
	//		num += 1;
	//	}
	//	else 
	//	{
	//		num -= 1;
	//	}
	//	memset(pDesk->sUserData.m_iArMenPai[i],255,sizeof(pDesk->sUserData.m_iArMenPai[i]));
	//	for(int j=count;j<(count + num) ;++j)//排列
	//	{
	//		if(pDesk->sUserData.m_MenPai.byMenPai[j] != 255)
	//		{
	//			pDesk->sUserData.m_iArMenPai[i][j-count] = 0;
	//		}
	//		else
	//		{
	//			pDesk->sUserData.m_iArMenPai[i][j-count] = 255;
	//		}
	//	}		
	//	count += num;
	//}
	for(int i=0;i<28;++i)//0位置的门牌
	{
		if(pDesk->sUserData.m_MenPai.byMenPai[i] != 255)
		{
			pDesk->sUserData.m_iArMenPai[0][i] = 0;
		}
		else
		{
			pDesk->sUserData.m_iArMenPai[0][i] = 255;
		}
	}
	for(int i=28;i<54;++i)//1位置的门牌
	{
		if(pDesk->sUserData.m_MenPai.byMenPai[i] != 255)
		{
			pDesk->sUserData.m_iArMenPai[1][i-28] = 0;
		}
		else
		{
			pDesk->sUserData.m_iArMenPai[1][i-28] = 255;
		}
	}
	for(int i=54;i<82;++i)//2位置的门牌
	{
		if(pDesk->sUserData.m_MenPai.byMenPai[i] != 255)
		{
			pDesk->sUserData.m_iArMenPai[2][i-54] = 0;
		}
		else
		{
			pDesk->sUserData.m_iArMenPai[2][i-54] = 255;
		}
	}
	for(int i=82;i<108;++i)//3位置的门牌
	{
		if(pDesk->sUserData.m_MenPai.byMenPai[i] != 255)
		{
			pDesk->sUserData.m_iArMenPai[3][i-82] = 0;
		}
		else
		{
			pDesk->sUserData.m_iArMenPai[3][i-82] = 255;
		}
	}
}

///描述：抓一张牌
///@param station 抓牌玩家的位置 , head  是否从前面抓,type 抓牌类型 0 正常抓牌，1开始前发牌，2开前补花抓牌，3补花
BYTE ThingHandleEx::GetPai(int station,bool head,BYTE type,BYTE *pIndex)
{
	return ThingHandle::GetPai(station, head, type,pIndex);
}

///检测玩家station当前能做哪些动作
///描述：station 检测位置,pai 别的玩家打出的牌，type 类型 出牌 0 抓牌，1杠开，2 点炮，3抢杠
bool ThingHandleEx::CheckAction(int station,BYTE pai,BYTE type)
{
    for(int i=0;i<4;++i)
    {
        //if (pDesk->sUserData.m_bIsHu[i])        // 已胡牌玩家
        //{
        //    continue;
        //}
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
            if(!pDesk->sUserData.m_bTing[station] && pDesk->sCPGAction.CanGang(station,pai,pDesk->sGameData.T_CPGNotify[station].m_iGangData ,true))
            {
                pDesk->sGameData.T_CPGNotify[station].bGang = true;
            }
            if(pDesk->sGameData.T_CPGNotify[station].bHu || pDesk->sGameData.T_CPGNotify[station].bGang)
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
                if (pDesk->sUserData.m_bIsHu[i] || pDesk->sUserData.m_bFoceLeave[i])        // 已胡牌玩家,或者断线状态
                {
                    continue;
                }

                if(i==station)
                    continue;
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
				//玩家弃糊状态,不能弃先糊后
				if(!pDesk->sUserData.m_bQiHu[i] && pDesk->sCheckHuPai.CanHu(i,pai,pDesk->sGameData.T_CPGNotify[i].byHuType,false))
				{
					pDesk->sGameData.T_CPGNotify[i].bHu = true;
					pDesk->sGameData.T_CPGNotify[i].m_byMaxThing = ACTION_HU;
					pDesk->sGameData.T_CPGNotify[i].m_iHuPs = pai;
					pDesk->sGameData.T_CPGNotify[i].m_byDianPao = station;
				}
                if(pDesk->sGameData.T_CPGNotify[i].bHu || pDesk->sGameData.T_CPGNotify[i].bPeng || pDesk->sGameData.T_CPGNotify[i].bGang 
                    || pDesk->sGameData.T_CPGNotify[i].bTing)
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
                if (pDesk->sUserData.m_bIsHu[i]|| pDesk->sUserData.m_bFoceLeave[i])        // 已胡牌玩家,或者断线状态
                {
                    continue;
                }
                if(i==station)
                    continue;
				if(pDesk->sUserData.m_bQiHu[i] )
				{
					OutputDebugString("抢杠检测：弃糊，不能抢杠糊");
				}
				//糊
				if(!pDesk->sUserData.m_bQiHu[i] && pDesk->sCheckHuPai.CanHu(i,pai,pDesk->sGameData.T_CPGNotify[i].byHuType,false))
				{
					OutputDebugString("抢杠检测：玩家能抢杠糊");

					pDesk->sCheckHuPai.SetAHupaiType(HUPAI_TYPE_QIANG_GANG, pDesk->sGameData.T_CPGNotify[i].byHuType);//设置抢杠牌型
					pDesk->sCheckHuPai.ReSetAHupaiType(HUPAI_TYPE_GANG_PAO, pDesk->sGameData.T_CPGNotify[i].byHuType);//去掉杠后炮牌型

					pDesk->sGameData.T_CPGNotify[i].bHu = true;
					pDesk->sGameData.T_CPGNotify[i].m_iHuPs = pai;//记录胡的牌
					pDesk->sGameData.T_CPGNotify[i].m_byDianPao = station;		
					pDesk->sGameData.T_CPGNotify[i].bQiangGang = true; //抢杠糊

					pDesk->sGameData.T_CPGNotify[i].m_byMaxThing = ACTION_HU;
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
void ThingHandleEx::NetCutAutoHandle()
{
	//ThingHandle::NetCutAutoHandle();
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
				AutoOutOrGet();
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
			OutputDebugString("超时处理：NetCutAutoHandle 000");
			if(pDesk->sUserData.m_bFoceLeave[pDesk->sUserData.m_byNowOutStation])//如果出牌玩家断线
			{
				AutoOutOrGet();
			}
		}
		break;
	case THING_DING_QUE:// 定缺
		{
			OutputDebugString("超时处理：OnStateTimer 4444");

            for (int i=0; i<PLAY_COUNT; ++i)
            {
				if(!pDesk->sUserData.m_bFoceLeave[i])//玩家玩家断线
					continue;
				tagDingQueEx cmdDingQue;
				cmdDingQue.byQue = 255/*rand()%3*/;   // 随机定
				ReceiveUserDingQue(i, &cmdDingQue, sizeof(cmdDingQue), false);
			}
		}
		break;
	}
}

//防卡桌自动处理
void ThingHandleEx::AutoHandle()
{
	//ThingHandle::AutoHandle();
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
			int nHuCount = 0;
			for (int i=0; i<PLAY_COUNT; ++i)
			{
				if (pDesk->sUserData.m_bIsHu[i])
				{
					nHuCount++;            
				}
			}
			if (nHuCount>=3)
			{        			
				pDesk->sGameData.ApplyThingID = THING_ENG_HANDLE;//下一个事件，游戏结束事件
			}
			else
			{// 轮到下一个玩家抓牌
				pDesk->sGameData.ApplyThingID  = THING_ZHUA_PAI;
			}	
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
	case THING_DING_QUE:// 定缺
		{
			OutputDebugString("强退托管：THING_DING_QUE 000");
            for (int i=0; i<PLAY_COUNT; ++i)
            {
				tagDingQueEx cmdDingQue;
				cmdDingQue.byQue = 255;   // 随机定
				ReceiveUserDingQue(i, &cmdDingQue, sizeof(cmdDingQue), false);
			}
		}
		break;
	}
}

///自动处理出牌或者抓牌
void ThingHandleEx::AutoOutOrGet()
{
	OutputDebugString("强退托管：AutoOutOrGet 000");
	bool haveout = false;
	for(int i=0;i<PLAY_COUNT;++i)
	{
		if(!pDesk->sUserData.IsOutPaiPeople(i))//出牌玩家
			continue;
		haveout = true;
		tagOutPaiEx outpai;
		outpai.Clear();
		outpai.byUser = i;
		outpai.byPs = pDesk->sUserData.GetLastHandPai(i);

		//缺门的牌必须打出
		for(int j=0;j<pDesk->sUserData.GetHandPaiCount(i); ++j)
		{
			if(pDesk->sUserData.m_byArHandPai[i][j]/10 == pDesk->sUserData.m_byDingQue[i])//找到非缺门的牌
			{
				outpai.byPs = pDesk->sUserData.m_byArHandPai[i][j];
				break;
			}
		}

		OutputDebugString("超时处理：AutoOutOrGet 111");

		ReceiveUserOutCard(i,&outpai,sizeof(outpai),false);
		break;

	}
	if(!haveout)//没有出牌的玩家，启动抓牌事件
	{
		pDesk->sGameData.ApplyThingID = THING_ZHUA_PAI;
		ApplyAction(pDesk->sGameData.ApplyThingID,200);//申请下一个事件
	}
}



//////////////////////////////动作处理//////////////////////////////////////////////////////

///吃牌事件
void ThingHandleEx::HandleChiPai(BYTE &nextID,BYTE &time)
{	
	pDesk->sUserData.m_byBuGangPai = 255;//补杠得分的牌（用在补杠，抓到能补杠的牌后马上补杠才有分，否则无分）
	memset(pDesk->sGameData.m_bGangState,0,sizeof(pDesk->sGameData.m_bGangState));//记录杠牌状态，拥于杠开和杠后炮
	ThingHandle::HandleChiPai(nextID,time);
}

///碰牌事件
void ThingHandleEx::HandlePengPai(BYTE &nextID,BYTE &time)	
{
	pDesk->sUserData.m_byBuGangPai = pDesk->sGameData.temp_PengPai.byPs;//补杠得分的牌（用在补杠，抓到能补杠的牌后马上补杠才有分，否则无分）
    pDesk->sGameData.m_byThingDoing = THING_PENG_PAI;//记录正在执行的事件

	memset(pDesk->sGameData.m_bGangState,0,sizeof(pDesk->sGameData.m_bGangState));//记录杠牌状态，拥于杠开和杠后炮

    ///碰牌牌事件
    pDesk->sGameData.T_PengPai = pDesk->sGameData.temp_PengPai;//获取数据
    pDesk->sUserData.m_byNowOutStation = pDesk->sGameData.temp_PengPai.byUser;
	pDesk->sGameData.T_PengPai.byUser = pDesk->sGameData.temp_PengPai.byUser;

	SetDoingThing( pDesk->sUserData.m_byNowOutStation,THING_PENG_PAI);//设置执行事件

    //碰牌牌事件
    GCPStructEx peng;
    peng.Init();

    peng.byType = ACTION_PENG;
    peng.iBeStation = pDesk->sGameData.temp_PengPai.byBePeng;
    peng.iOutpai = pDesk->sGameData.temp_PengPai.byPs;
    peng.iStation = pDesk->sGameData.temp_PengPai.byUser;
    BYTE station = pDesk->sGameData.temp_PengPai.byUser;

	//记录事件
	LogThing( THING_PENG_PAI,peng.iOutpai, peng.iStation,peng.iOutpai,ACTION_PENG);

    //出牌玩家删除最后一张出牌
    pDesk->sUserData.DelleteLastOutPai(pDesk->sGameData.T_OutPai.byUser);
    for(int i=0;i<2;i++)
    {	
        pDesk->sUserData.DelleteAHandPai(pDesk->sGameData.temp_PengPai.byUser,pDesk->sGameData.temp_PengPai.byPs);//删除手牌
    }
	//保存三张碰的牌
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

    ////判断是否能听牌
    //if(pDesk->sGameData.m_mjAction.bTing)//有听牌
    //{
    //    if(pDesk->sCheckHuPai.CanTing(station,pDesk->sGameData.T_CPGNotify[station].m_byTingCanOut))//检测能否听牌
    //    {
    //        pDesk->sGameData.T_CPGNotify[station].bTing = true;
    //    }
    //}

    pDesk->sGameData.m_byNextAction = ACTION_NO;
    pDesk->sUserData.m_byApplyMaxAction = ACTION_NO;//玩家当前申请的最大事件：处理完申请后，恢复数据

	//if(pDesk->sUserData.GetAHandPaiCount(station,pDesk->sGameData.temp_PengPai.byPs)>0)////记录无分杠（补杠，手中有4张牌先碰后杠无分，抓牌后能补杠但是不在当前圈杠无分）
	//{
	//	for(int i=0;i<4;++i)
	//	{
	//		if(pDesk->sUserData.m_NoFenGang[station][i] != 255)
	//			continue;
	//		pDesk->sUserData.m_NoFenGang[station][i] = pDesk->sGameData.temp_PengPai.byPs;
	//		break;
	//	}
	//}



}

///杠事件
void ThingHandleEx::HandleGangPai(BYTE &nextID,BYTE &time)	
{
    pDesk->sGameData.m_byThingDoing = THING_GANG_PAI;//记录正在执行的事件
	nextID = THING_ZHUA_PAI;
	time = 1;
	///杠牌牌事件
	pDesk->sGameData.T_GangPai = pDesk->sGameData.temp_GangPai;//获取数据
	pDesk->sUserData.m_byNowOutStation = pDesk->sGameData.temp_GangPai.byUser;

	pDesk->sGameData.m_bGangState[pDesk->sGameData.temp_GangPai.byUser] = true;//记录杠牌状态，拥于杠开和杠后炮

	SetDoingThing( pDesk->sUserData.m_byNowOutStation,THING_GANG_PAI);//设置执行事件

	GCPStructEx gang;
	gang.Init();
	gang.iBeStation = pDesk->sGameData.temp_GangPai.byBeGang;
	gang.iOutpai = pDesk->sGameData.temp_GangPai.byPs;
	gang.iStation = pDesk->sGameData.temp_GangPai.byUser;

	gang.byData[0] = pDesk->sGameData.temp_GangPai.byPs;
	gang.byData[1] = pDesk->sGameData.temp_GangPai.byPs;
	gang.byData[2] = pDesk->sGameData.temp_GangPai.byPs;
	gang.byData[3] = pDesk->sGameData.temp_GangPai.byPs;

	pDesk->sUserData.m_bIsGangPao[pDesk->sGameData.temp_GangPai.byUser] = true;//正在杠后炮的状态

	//记录事件
	LogThing( THING_GANG_PAI,gang.iOutpai, gang.iStation,gang.iOutpai,pDesk->sGameData.temp_GangPai.byType);

	memset(pDesk->sGameData.T_GangPai.iGangFen,0,sizeof(pDesk->sGameData.T_GangPai.iGangFen));//清理杠分记录
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
			memset(pDesk->sGameData.T_GangPai.iGangFen,0,sizeof(pDesk->sGameData.T_GangPai.iGangFen));

            // 下雨（暗杠）向未胡者收 2
            BYTE byGangUser = pDesk->sGameData.temp_GangPai.byUser;
			int count = pDesk->sUserData.m_stGangFen[byGangUser].count;
            for (int i=0; i<PLAY_COUNT; ++i) 
            {
                if (i==byGangUser || pDesk->sUserData.m_bIsHu[i])
                {
                    continue;
                }
                pDesk->sUserData.m_byGFXY[ byGangUser ][i] += 2;
                pDesk->sUserData.m_byGFXY[ byGangUser ][4] += 2;
                //pDesk->sUserData.m_byGFXY[ i ][byGangUser] -= 2;
                //pDesk->sUserData.m_byGFXY[ i ][4] -= 2;
				pDesk->sGameData.T_GangPai.iGangFen[byGangUser] += 2;
				pDesk->sGameData.T_GangPai.iGangFen[i] = -2;

				pDesk->sUserData.m_stGangFen[byGangUser].fen[count][byGangUser] += 2;
				pDesk->sUserData.m_stGangFen[byGangUser].fen[count][i] -= 2;
            }
			pDesk->sUserData.m_stGangFen[byGangUser].fen[count][PLAY_COUNT] = ACTION_AN_GANG;
			pDesk->sUserData.m_stGangFen[byGangUser].count++;

		}
		break;
	case ACTION_MING_GANG://明杠
		{
			for(int i=0;i<3;++i)
			{	
				pDesk->sUserData.DelleteAHandPai(pDesk->sGameData.temp_GangPai.byUser,pDesk->sGameData.temp_GangPai.byPs);//删除手牌
			}
			gang.byType = ACTION_MING_GANG;
			pDesk->sUserData.DelleteLastOutPai(pDesk->sGameData.temp_GangPai.byBeGang);//出牌玩家删除最后一张出牌

            // 下雨（直杠）向引杠者收2
            BYTE byGangUser = pDesk->sGameData.temp_GangPai.byUser;
            BYTE byYingGangUser = pDesk->sGameData.temp_GangPai.byBeGang;
            
			memset(pDesk->sGameData.T_GangPai.iGangFen,0,sizeof(pDesk->sGameData.T_GangPai.iGangFen));
			pDesk->sGameData.T_GangPai.iGangFen[byGangUser] = 2;
			pDesk->sGameData.T_GangPai.iGangFen[byYingGangUser] = -2;

            pDesk->sUserData.m_byGFXY[ byGangUser ][byYingGangUser] += 2;//杠牌玩家得2分，引杠玩家-2分
            pDesk->sUserData.m_byGFXY[ byGangUser ][4] += 2;

			int count = pDesk->sUserData.m_stGangFen[byGangUser].count;
			pDesk->sUserData.m_stGangFen[byGangUser].fen[count][byGangUser] = 2;
			pDesk->sUserData.m_stGangFen[byGangUser].fen[count][byYingGangUser] = -2;


			if(!pDesk->sGameData.m_bChengBao)//非承包制（家家有 明杠）lym 2012-1-3
			{
				for (int i=0; i<PLAY_COUNT; ++i)//收取其他玩家各1分 lym 2012-1-3
				{
					if (i==byGangUser || pDesk->sUserData.m_bIsHu[i] || i == byYingGangUser)
					{
						continue;
					}
					pDesk->sUserData.m_byGFXY[ byGangUser ][i] += 1;
					pDesk->sUserData.m_byGFXY[ byGangUser ][4]+=1;

					pDesk->sGameData.T_GangPai.iGangFen[byGangUser] += 1;
					pDesk->sGameData.T_GangPai.iGangFen[i] = -1;


					pDesk->sUserData.m_stGangFen[byGangUser].fen[count][byGangUser] += 1;
					pDesk->sUserData.m_stGangFen[byGangUser].fen[count][i] -= 1;
				}	
			}
			pDesk->sUserData.m_stGangFen[byGangUser].fen[count][PLAY_COUNT] = ACTION_MING_GANG;
			pDesk->sUserData.m_stGangFen[byGangUser].count++;
		}
		break;
	case ACTION_BU_GANG://补杠
        {
            pDesk->sUserData.DelleteAHandPai(pDesk->sGameData.temp_GangPai.byUser,pDesk->sGameData.temp_GangPai.byPs);//删除手牌
            pDesk->sUserData.DelleteAGCPData(pDesk->sGameData.temp_GangPai.byUser,ACTION_PENG,pDesk->sGameData.temp_GangPai.byPs);//删除相应碰牌
            gang.byType = ACTION_BU_GANG;
            // 刮风（补杠）向未胡者收1（马上杠才能得分 lym 2012-1-3）
            BYTE byGangUser = pDesk->sGameData.temp_GangPai.byUser;
			int count = pDesk->sUserData.m_stGangFen[byGangUser].count;

			char buff[500];
			sprintf(buff,"牌数：补杠 玩家%d 牌%d  预定牌%d",byGangUser,pDesk->sGameData.temp_GangPai.byPs,pDesk->sUserData.m_byBuGangPai);
			OutputDebugString(buff);

			if(pDesk->sUserData.m_byBuGangPai == pDesk->sGameData.temp_GangPai.byPs)
			{
				memset(pDesk->sGameData.T_GangPai.iGangFen,0,sizeof(pDesk->sGameData.T_GangPai.iGangFen));
				for (int i=0; i<PLAY_COUNT; ++i)
				{
					if (i==byGangUser || pDesk->sUserData.m_bIsHu[i])
					{
						continue;
					}
					pDesk->sUserData.m_byGFXY[ byGangUser ][i] += 1;
					pDesk->sUserData.m_byGFXY[ byGangUser ][4]+=1;
					pDesk->sGameData.T_GangPai.iGangFen[byGangUser] += 1;
					pDesk->sGameData.T_GangPai.iGangFen[i] = -1;

					pDesk->sUserData.m_stGangFen[byGangUser].fen[count][byGangUser] += 1;
					pDesk->sUserData.m_stGangFen[byGangUser].fen[count][i] -= 1;
				}
			}
			pDesk->sUserData.m_stGangFen[byGangUser].fen[count][PLAY_COUNT] = ACTION_BU_GANG;
			pDesk->sUserData.m_stGangFen[byGangUser].count++;
        }
        break;
    }
    pDesk->sUserData.AddToGCP(pDesk->sGameData.temp_GangPai.byUser,&gang);

    for(int i=0;i<4;++i)
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
    pDesk->sUserData.m_bZhuaPaiDir = true;
    pDesk->sUserData.m_byApplyMaxAction = ACTION_NO;//玩家当前申请的最大事件：处理完申请后，恢复数据
	pDesk->sUserData.m_byBuGangPai = 255;//补杠得分的牌（用在补杠，抓到能补杠的牌后马上补杠才有分，否则无分）

	char strdug[300];
  

}

///听事件
void ThingHandleEx::HandleTingPai(BYTE &nextID,BYTE &time)
{
	ThingHandle::HandleTingPai(nextID,time);
}

///糊事件
void ThingHandleEx::HandleHuPai(BYTE &nextID,BYTE &time)	
{
    pDesk->sGameData.m_byThingDoing = THING_HU_PAI;//记录正在执行的事件

    BYTE byHuUser = pDesk->sGameData.T_HuPai.byUser;    // 胡牌玩家
    pDesk->sUserData.m_bIsHu[ byHuUser ] = true;

	SetDoingThing( byHuUser,THING_HU_PAI);//设置执行事件
		

	char buff[800];
	sprintf(buff,"服务器：抓牌 湖牌处理 %d  ",pDesk->sGameData.T_HuPai.byUser);
	OutputDebugString(buff); 

	//记录事件
	LogThing( THING_HU_PAI,pDesk->sGameData.T_HuPai.byPs, pDesk->sGameData.T_HuPai.byUser,pDesk->sGameData.T_HuPai.byDianPao,pDesk->sGameData.T_HuPai.bDuoXiang );

	memset(pDesk->sGameData.T_CountFen.bCanHu,0,sizeof(pDesk->sGameData.T_CountFen.bCanHu));
	for(int i=0;i<PLAY_COUNT;i++)
	{
		pDesk->sGameData.T_CountFen.bCanHu[i] = pDesk->sGameData.T_HuPai.bCanH[i];
	}

	////杠上炮就，不算杠牌玩家最后一个杠的分 lym 2011-12-16
	//if(CMjRef::CheckHupaiType(HUPAI_TYPE_GANG_PAO, pDesk->sGameData.T_HuPai.byHuType[byHuUser], MAX_HUPAI_TYPE) )   // 杠上炮
	//{  
	//	BYTE dian = pDesk->sGameData.T_HuPai.byDianPao;
	//	//将胡牌玩家的杠分退换
	//	for(int i=0;i>=0;i--)
	//	{
	//		if(dian == 255)
	//			break;
	//		if(pDesk->sUserData.m_UserGCPData[dian][i].byType == ACTION_AN_GANG)//暗杠动作
	//		{
	//			for(int j=0;j<PLAY_COUNT;++j)
	//			{
	//				if(j != dian && (!pDesk->sUserData.m_bIsHu[j] || pDesk->sGameData.T_CountFen.bCanHu[j]))
	//				{
	//					pDesk->sUserData.m_byGFXY[dian][j] -= 2;
	//					pDesk->sUserData.m_byGFXY[dian][4] -= 2;
	//				}
	//			}
	//			break;
	//		}
	//		else if(pDesk->sUserData.m_UserGCPData[dian][i].byType == ACTION_BU_GANG)//补杠动作
	//		{
	//			for(int j=0;j<PLAY_COUNT;++j)
	//			{
	//				if(j != dian && (!pDesk->sUserData.m_bIsHu[j] || pDesk->sGameData.T_CountFen.bCanHu[j]))
	//				{
	//					pDesk->sUserData.m_byGFXY[dian][j] -= 1;
	//					pDesk->sUserData.m_byGFXY[dian][4] -= 1;
	//				}
	//			}
	//			break;
	//		}
	//		else if(pDesk->sUserData.m_UserGCPData[dian][i].byType == ACTION_MING_GANG)//明杠动作：退换被杠玩家的分
	//		{
	//			pDesk->sUserData.m_byGFXY[dian][pDesk->sUserData.m_UserGCPData[dian][i].iBeStation] -= 2;
	//			pDesk->sUserData.m_byGFXY[dian][4] -= 2;
	//			break;
	//		}
	//	}
	//}

	if(pDesk->sGameData.T_HuPai.bDuoXiang )//一炮多响
	{
		
		OutputDebugString("一炮多响：糊牌");
		if(pDesk->sUserData.m_byTempNt == 255)//记录下局换庄玩家
		{
			pDesk->sUserData.m_byTempNt = pDesk->sGameData.T_CPGNotify[byHuUser].m_byDianPao;
		OutputDebugString("一炮多响：记录庄家");
		}
		for (int i=0; i<PLAY_COUNT; ++i)
		{
			if(!pDesk->sGameData.T_HuPai.bCanH[i])
				continue;
			pDesk->sUserData.m_bIsHu[ i ] = true;
			pDesk->sGameData.T_HuPai.byUser = i;    // 胡牌玩家
			// 算分并记录
			CountFenAndRecord();
		}
	}
	else
	{
		if(pDesk->sUserData.m_byTempNt == 255)//记录下局换庄玩家
		{
			pDesk->sUserData.m_byTempNt = byHuUser;
		}
		// 算分并记录
		CountFenAndRecord();
	}
    pDesk->sGameData.T_CPGNotify[byHuUser].Clear();

    int nHuCount = 0;
    for (int i=0; i<PLAY_COUNT; ++i)
    {
        if (pDesk->sUserData.m_bIsHu[i])
        {
            nHuCount++;            
        }
    }
    if (nHuCount>=3)
    {        
        // 算分事件
        nextID = THING_ENG_HANDLE;//下一个事件，游戏结束事件
        time = 1;	
    }
    else
    {
        // 轮到下一个玩家抓牌
        nextID = THING_ZHUA_PAI;
        time = 1;
    }	

	char strdug[300];
	//获取各个玩家的分数
	for (int i=0; i<PLAY_COUNT; ++i)
    {
		pDesk->sGameData.T_HuPaiXZMJ[byHuUser].iGangFen[i] = pDesk->sUserData.m_byGFXY[i][4];
		pDesk->sGameData.T_HuPaiXZMJ[byHuUser].iMoney[i] = pDesk->sGameData.T_CountFen.iZongFen[i];
		pDesk->sGameData.T_HuPaiXZMJ[byHuUser].iZongFen[i] = pDesk->sGameData.T_CountFen.iZongFen[i];
		pDesk->sGameData.T_HuPaiXZMJ[byHuUser].iHuFen[i] = pDesk->sGameData.T_CountFen.iHuFen[i]; 

		pDesk->sGameData.T_HuPaiXZMJ[byHuUser].byGengCount = pDesk->sGameData.T_CountFen.byGengCount;//根数
	}
	sprintf(buff,"服务器：抓牌 湖牌处理 完成 %d  ",pDesk->sGameData.T_HuPai.byUser);
	OutputDebugString(buff); 

}

void ThingHandleEx::CountFenAndRecord()
{
    BYTE byHuUser = pDesk->sGameData.T_HuPai.byUser;
	pDesk->sGameData.T_CountFen.byUser=byHuUser;

    pDesk->sUserData.m_bIsHu[byHuUser] = true;
    for(int i=0;i<PLAY_COUNT;i++)
    {
        pDesk->sGameData.T_CountFen.bHu[i] = pDesk->sUserData.m_bIsHu[i];
		pDesk->sGameData.T_CountFen.bCanHu[i] = pDesk->sGameData.T_HuPai.bCanH[i];
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

    ///记录算分
    memcpy(&pDesk->sGameData.T_HuPaiXZMJ[ byHuUser ], &pDesk->sGameData.T_HuPai, sizeof(pDesk->sGameData.T_HuPai));
    memcpy(&pDesk->sGameData.T_CountFenXZMJ[ byHuUser ], &pDesk->sGameData.T_CountFen, sizeof(pDesk->sGameData.T_CountFen));

    pDesk->sUserData.m_byNowOutStation = byHuUser;

	for(int i=0;i<PLAY_COUNT;i++)
    {
		if(!pDesk->sUserData.m_bIsHu[i])//没有胡牌的玩家
			continue;
		pDesk->sGameData.T_HuPaiXZMJ[byHuUser].m_bIsHu[i] = true;
		pDesk->sGameData.T_HuPaiXZMJ[byHuUser].m_bIsZiMo[i] = pDesk->sGameData.T_HuPaiXZMJ[i].bZimo;//是否自摸
		pDesk->sGameData.T_HuPaiXZMJ[byHuUser].m_byDianPao[i] = pDesk->sGameData.T_HuPaiXZMJ[i].byDianPao;//点炮位置
		pDesk->sGameData.T_HuPaiXZMJ[byHuUser].m_iZongFan[i] = pDesk->sGameData.T_HuPaiXZMJ[i].m_iZongFan[i];

		pDesk->sGameData.T_HuPaiXZMJ[byHuUser].m_iGengCount[i] = pDesk->sUserData.m_byGengCount[i];//玩家根数
	}
}

///算分
void ThingHandleEx::HandleCountFen(BYTE &nextID,BYTE &time)
{
	ThingHandle::HandleCountFen(nextID,time);	
}

/////结束处理事件
//void ThingHandleEx::HandleEnd(BYTE &nextID,BYTE &time)
//{
//	ThingHandle::HandleEnd(nextID,time);	
//}

/////强退结束事件
//void ThingHandleEx::UnnormalEnd(BYTE &nextID,BYTE &time)
//{
//	ThingHandle::UnnormalEnd(nextID,time);	
//}



/// 处理计时器消息
void ThingHandleEx::OnGameTimer(UINT uTimerID)
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
            //AutoHandle();
			ThingHandle::OnGameTimer( uTimerID);
        }
        break;
    case TIME_STEP:
        {
            //OnStateTimer();
        }
        break;
	case TIME_WAIT_MSG: // 超过事件无响应，是超时自动处理，系统自动处理
		{
			OutTimeHandle();
		}
		break;
    }
}



/**
* @brief 设置当前状态
* @param[in]       curState            要设置成什么状态
* @param[in]       unLastingTime       状态持续时间
* @param[in]       unWaitingTime       状态开始前的等待
* @param[in]       bNotify             是否发消息通知客户端
*/
void ThingHandleEx::SetGameCurState(MJTHINGS curState, BYTE ucCurChair, unsigned int unLastingTime, unsigned int unWaitingTime,
                     bool bNotify)
{
    m_curState.thingDoing = curState;
    m_curState.ucCurChair = ucCurChair;
    m_curState.unStartTime = m_unCurStateTime;
    m_curState.unEndTime = m_unCurStateTime+unLastingTime;

    m_curState.unWaitingTime = unWaitingTime;
    m_curState.bNotify = bNotify;

    if (bNotify && 0==unWaitingTime && THING_MAX!=curState)
    {
        // 通知客户端
        MJ_State mjstate;        
        mjstate.thingDoing = curState;
        mjstate.ucCurChair = ucCurChair;
        mjstate.ucTime = unLastingTime/1000;
        for (int i=0; i<PLAY_COUNT; ++i)
        {
            pDesk->SendGameDataEx(i, &mjstate, sizeof(mjstate), 0, ASS_SET_STATE, 0);
        }
        m_curState.bNotify = false;
    }
}

void ThingHandleEx::StartStateTimer()
{
    pDesk->SetTimer(TIME_STEP, TIME_STEP_TIME);
    m_unCurStateTime = 0;
    m_curState.thingDoing = THING_MAX;
}
void ThingHandleEx::EndStateTimer()
{
    pDesk->KillTimer(TIME_STEP);
    m_unCurStateTime = 0;
    m_curState.thingDoing = THING_MAX;
}


void ThingHandleEx::OnStateTimer()
{
	OutputDebugString("超时处理：OnStateTimer");
    m_unCurStateTime += TIME_STEP_TIME;
    if (m_curState.unWaitingTime<TIME_STEP_TIME)
    {
        m_curState.unWaitingTime = 0;
    }
    else
    {
        m_curState.unWaitingTime -= TIME_STEP_TIME;
    }

    if (THING_MAX==m_curState.thingDoing)
    {
        return;
    }
	OutputDebugString("超时处理：OnStateTimer 111");
    if (0==m_curState.unWaitingTime && m_curState.bNotify)
    {    
        // 通知客户端
        MJ_State mjstate;        
        mjstate.thingDoing = m_curState.thingDoing;
        mjstate.ucCurChair = m_curState.ucCurChair;
        mjstate.ucTime = (m_curState.unEndTime-m_curState.unStartTime-TIME_NETWORK)/1000;
        for (int i=0; i<PLAY_COUNT; ++i)
        {
            pDesk->SendGameDataEx(i, &mjstate, sizeof(mjstate), 0, ASS_SET_STATE, 0);
        }
        m_curState.bNotify = false;

		OutputDebugString("超时处理：OnStateTimer 222");
    }

	if(pDesk->sUserData.m_bFoceLeave[m_curState.ucCurChair] && m_unCurStateTime>3000)//玩家处于断线状态
	{
		OutputDebugString("超时处理：OnStateTimer 4444");
		NetCutAutoHandle();
	}

    if (m_unCurStateTime >= m_curState.unEndTime && m_curState.thingDoing==m_curState.thingDoing)
    {
		OutputDebugString("超时处理：OnStateTimer 333");
        AutoHandle();
    }
}

//事件记录
void ThingHandleEx::LogThing(BYTE thingId,BYTE pai,BYTE st1,BYTE st2,BYTE data)
{//事件记录只记录后10个事件：0事件号，1相关牌值，2相关位置1,3相关位置2，4附加类型
	if(pDesk->sGameData.m_byThingLog[9][0] != 255)//满了
	{
		for(int i=0;i<9;++i)//后移一个
		{
			pDesk->sGameData.m_byThingLog[i][0] = pDesk->sGameData.m_byThingLog[i+1][0];
			pDesk->sGameData.m_byThingLog[i][1] = pDesk->sGameData.m_byThingLog[i+1][1];
			pDesk->sGameData.m_byThingLog[i][2] = pDesk->sGameData.m_byThingLog[i+1][2];
			pDesk->sGameData.m_byThingLog[i][3] = pDesk->sGameData.m_byThingLog[i+1][3];
			pDesk->sGameData.m_byThingLog[i][4] = pDesk->sGameData.m_byThingLog[i+1][4];
		}
		pDesk->sGameData.m_byThingLog[9][0] = thingId;
		pDesk->sGameData.m_byThingLog[9][1] = pai;
		pDesk->sGameData.m_byThingLog[9][2] = st1;
		pDesk->sGameData.m_byThingLog[9][3] = st2;
		pDesk->sGameData.m_byThingLog[9][4] = data;
	}
	else
	{
		for(int i=0;i<10;++i)
		{
			if(pDesk->sGameData.m_byThingLog[i][0] == 255)
			{
				pDesk->sGameData.m_byThingLog[i][0] = thingId;
				pDesk->sGameData.m_byThingLog[i][1] = pai;
				pDesk->sGameData.m_byThingLog[i][2] = st1;
				pDesk->sGameData.m_byThingLog[i][3] = st2;
				pDesk->sGameData.m_byThingLog[i][4] = data;
				break;
			}
		}
	}
	
}


/// 设置当前正在发生的事件
void ThingHandleEx::SetDoingThing(BYTE station,BYTE thing)
{

	pDesk->sGameData.m_byThingDoing = thing;
	/// 恢复超时数据
	ReSetOutTime();
	switch(thing)
	{		
	case THING_DING_QUE://定缺事件		
		{
			SetOutTime(station,pDesk->sGameData.m_MjSet.ucDingQueTime,thing,true);
		}
		break;
	case THING_BEGIN_OUT://庄家开始发牌通知事件		
		{
			SetOutTime(station,pDesk->sGameData.m_MjSet.ucOutcardTime,thing,true);
		}
		break;
	case THING_OUT_PAI://出牌事件
		{
			//if(pDesk->sUserData.m_bTuoGuan[station] || pDesk->sUserData.m_bTing[station])
			//{
			//	SetOutTime(station,pDesk->sGameData.m_MjSet.byAutoTime,thing,true);
			//	break;
			//}
			SetOutTime(station,pDesk->sGameData.m_MjSet.ucBlockTime,thing,true);
		}
		break;
	case THING_ZHUA_PAI://抓牌事件
		{
			if(pDesk->sUserData.m_bTuoGuan[station] || pDesk->sUserData.m_bTing[station] || pDesk->sUserData.m_bFoceLeave[station])
			{
				SetOutTime(station,pDesk->sGameData.m_MjSet.nAutoTime,thing,true);
				break;
			}
			SetOutTime(station,pDesk->sGameData.m_MjSet.ucOutcardTime + pDesk->sGameData.m_MjSet.ucBlockTime,thing,true);
		}
		break;
	case THING_CHI_PAI://吃牌事件
	case THING_PENG_PAI://碰牌事件
		{
			if(pDesk->sUserData.m_bTuoGuan[station] || pDesk->sUserData.m_bTing[station])
			{
				SetOutTime(station,pDesk->sGameData.m_MjSet.nAutoTime,thing,true);
				break;
			}
			SetOutTime(station,pDesk->sGameData.m_MjSet.ucOutcardTime,thing,true);
		}
		break;
	case THING_HU_PAI:	//糊事件	
		{
			SetOutTime(station,pDesk->sGameData.m_MjSet.nAutoTime,thing,true);
		}
		break;
	case THING_BEGIN:	//游戏开始事件	
	case THING_DONG_NT:	//首局以东为庄事件					         
	case THING_2SEZI_NT:	//首局掷2颗色子的点数和为庄事件		   
	case THING_2SEZI_NT_GP:	//首局掷2颗色子定庄家和起牌点事件	
	case THING_TURN_2SEZI_NT://首局轮流掷2颗色子定庄家事件		
	case THING_2SEZI_DIR://掷2颗色子事件定起牌方向事件		
	case THING_2SEZI_DIR_GP://掷2颗色子事件定起牌方向和起牌点事件
	case THING_2SEZI_GP://掷2颗色子事件定起牌点事件		
	case THING_2SEZI_NT_DIR_GP:	//掷2颗色子事件定起牌点事件	
	case THING_SEND_PAI://发牌事件							
	case THING_TIAO_PAI://跳牌事件							
	case THING_2SEZI_JING://掷2颗色子定精牌事件			
	case THING_1SEZI_JIN://掷1颗色子定金牌事件
	case THING_ALL_BUHUA://为所有玩家补花事件				
	case THING_ONE_BUHUA://单个玩家补花事件	
	case THING_GANG_PAI://杠牌事件
	case THING_COUNT_FEN://算分事件	
		{

		}
		break;
	}
	
}


