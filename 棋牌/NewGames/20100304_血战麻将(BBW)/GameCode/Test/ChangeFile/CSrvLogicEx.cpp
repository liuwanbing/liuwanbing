#include "stdafx.h"
#include "CSrvLogicEx.h"
#include "../TestDlg.h"

CSrvLogicEx::CSrvLogicEx(void)
{	
}

CSrvLogicEx::~CSrvLogicEx(void)
{
}

///////////////////////////////消息接收///////////////////////////////////////////////////
//断线重连事件
bool CSrvLogicEx::GetGameStaton(BYTE bDeskStation, bool bWatchUser,BYTE GameStation)
{
	//return CSrvLogic::GetGameStaton(bDeskStation, bWatchUser, GameStation);

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
	memcpy(GameStationData.m_byMenPaiCount,m_UserData.m_byMenPaiCount,sizeof(m_UserData.m_byMenPaiCount));//各方向门牌数

	pDesk->GetUserAgree(GameStationData.ucAgree);//获取各个玩家的同意状态

	GameStationData.m_byMaster = m_GameData.m_byMaster;//桌主位置
	GameStationData.m_iBaseFen = m_GameData.m_iBaseFen;//基础分
	memcpy(GameStationData.m_bReBese,m_GameData.m_bReBese,sizeof(m_GameData.m_bReBese));//各方向门牌数

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
	m_GameData.m_MjSet.byYaZhuTime = pz.GetKeyVal(key,TEXT("byYaZhuTime"),10);	//押注等待时间

	memcpy(&GameStationData.m_MjSet,&m_GameData.m_MjSet,sizeof(GameStationData.m_MjSet));//麻将设置（主要是时间设置）
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
// 游戏数据包处理函数
int CSrvLogicEx::HandleNotifyMessage(int bDeskStation, UINT code, BYTE* pData, int uSize, bool bWatchUser)
{
	CSrvLogic::HandleNotifyMessage(bDeskStation, code,pData, uSize, bWatchUser);
	if(code == EVENT_YA_ZHU)//押注事件
	{
		ReceiveUserYaZhu(bDeskStation, pData, uSize, bWatchUser);
	}
	else if(code == EVENT_MAKE_DIFEN)//确定底分事件
	{
		ReceiveMakeDiFen(bDeskStation, pData, uSize, bWatchUser);//
	}
	return 0;
}

// 用户出牌
int CSrvLogicEx::ReceiveUserOutCard(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser)
{
	return CSrvLogic::ReceiveUserOutCard(bDeskStation, pData, uSize,bWatchUser);
}

// 用户押注
int CSrvLogicEx::ReceiveUserYaZhu(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser)
{
	tagYaZhuEx * pCmd = (tagYaZhuEx *)pData;
	if(pCmd == NULL || bWatchUser || pDesk->GetGameStation() < GS_SEND_CARD || uSize!=sizeof(tagYaZhuEx) || m_GameData.m_byThingDoing != EVENT_YA_ZHU)
	{//不合法动作
		OutputDebugString("服务器 事件：押注消息不合法");
		return 0;
	}
	if(m_GameData.T_YaZhu.byUserNote[bDeskStation] != 255)//回应了消息就不再接收
	{
		OutputDebugString("服务器 事件：押注消息以回应");
		return 0;
	}

	if(bDeskStation == m_UserData.m_byNtStation)//庄家回应
	{	
		m_GameData.T_YaZhu.byUserNote[bDeskStation] = pCmd->byNote;
		m_UserData.m_byYaZhu = pCmd->byNtNote;
		m_GameData.T_YaZhu.byUserNote[bDeskStation] = pCmd->byNote;
		if( pCmd->byNtNote == 0)//庄家不押注
		{
			memset(m_UserData.m_bYaZhu,0,sizeof(m_UserData.m_bYaZhu));
			ApplyAction(EVENT_BEGIN_OUT,2*100);//申请下一个事件：开始出牌

			OutputDebugString("服务器 事件：庄家不押注");
		}
		else
		{
			//通知闲家押注
			tagYaZhuEx yazhu;
			yazhu.Clear();
			yazhu.byNotify = 1;
			yazhu.byNtNote = pCmd->byNtNote;
			//发送游戏消息
			SendAllGameData( &yazhu, sizeof(yazhu), ASS_GM_GAME_NOTIFY, EVENT_YA_ZHU, 0);
			OutputDebugString("服务器 事件：通知闲家押注");
			this->AutoHandle(true);//做一次断线处理
		}
	}
	else 
	{
		m_GameData.T_YaZhu.byUserNote[bDeskStation] = pCmd->byNote;
		m_UserData.m_bYaZhu[bDeskStation] = pCmd->byNote;
		for(int i=0;i<PLAY_COUNT;++i)
		{
			if(m_GameData.T_YaZhu.byUserNote[i] == 255)//有玩家没有回应
				return 0;
		}
		//所有玩家都回应了
		ApplyAction(EVENT_BEGIN_OUT,2*100);//申请下一个事件：开始出牌
	}
}

// 确定底分
int CSrvLogicEx::ReceiveMakeDiFen(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser)//
{
	tagFanDiFenEx * pCmd = (tagFanDiFenEx *)pData;
	if(pCmd == NULL || bWatchUser || pDesk->IsPlayGame(0) || uSize!=sizeof(tagFanDiFenEx) )
	{//不合法动作
		OutputDebugString("服务器 事件：确定底分消息不合法");
		return 0;
	}
	if(m_GameData.m_iBaseFen > 0 && pCmd->byType == 0)//别的玩家已经设置了底分
	{
		return 0;
	}
	if(pCmd->byType == 0)//设置基础分
	{
		m_GameData.m_iBaseFen  = pCmd->iBaseFen;
		m_GameData.m_byMaster = bDeskStation;
		//发送各个玩家的同意
		tagFanDiFenEx difen;
		difen.Clear();
		difen.byType = 1;
		difen.iBaseFen = pCmd->iBaseFen;
		difen.byMaster = bDeskStation;
		SendAllGameData(&difen,sizeof(tagFanDiFenEx),ASS_GM_GAME_NOTIFY,EVENT_MAKE_DIFEN,0);//发送

		OutputDebugString("服务器 事件：确定底分 成功 ");
	}
	m_GameData.m_bReBese[bDeskStation] = true;
}

// 用户糊牌消息
int CSrvLogicEx::ReceiveUserHuCard(BYTE bDeskStation, void * pData, UINT uSize, bool bWatchUser)
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
		BYTE fanshu[4];
		memset(fanshu,0,sizeof(fanshu));
		for(int i=0;i<PLAY_COUNT;++i)
		{
			fanshu[i] = m_GameData.T_CPGNotify[i].m_byFan;
		}
		m_GameData.T_HuPai.byUser = CLogicBase::GetCanHuUser(m_GameData.T_CPGNotify[bDeskStation].m_byDianPao,m_GameData.T_HuPai.bCanH,m_GameData.T_HuPai.byUser,fanshu);//获取离糊牌玩家近的玩家
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


///////////////////////////////事件执行/////////////////////////////////////////////////


///名称：GameCPU
///描述：游戏事件处理中心，
///@param type要处理的事件代号
void CSrvLogicEx::GameCPU(int type)
{
	CSrvLogic::GameCPU(type);

	if(type == EVENT_YA_ZHU)//押注事件
	{	
		char strdug[500];
		sprintf(strdug,"事件：CSrvLogicEx::GameCPU type =%d 出牌位置 %d",type,m_UserData.m_byNowOutStation);
		OutputDebugString(strdug);

		BYTE nextID = 255,time = 255;
		ExecuteYaZhu(nextID,time);

		m_GameData.ApplyThingID = nextID;
		ApplyAction(nextID,time*100);//申请下一个事件
	}

}

///开始事件处理
void CSrvLogicEx::ExecuteGameBegin(BYTE &nextID,BYTE &time)
{
	pDesk->KillTimer(TIME_WAIT_MSG);
	pDesk->SetTimer(TIME_WAIT_MSG,1000);//重新设置超时计时器

	if(m_UserData.m_byNtStation == 255)//无庄家
	{
		nextID = EVENT_DONG_NT;//随机定庄家
	}
	else//有庄家,
	{
		nextID = EVENT_2SEZI_GP;//色子定起牌顿数
	}

	time = 2;	//200毫秒间隔

	m_UserData.InitData();
	m_GameData.InitData();
	m_UserData.m_StrJing.AddJing(CMjEnum::MJ_TYPE_BAI);//白板为财神

	m_GameData.T_Begin.byNt = m_UserData.m_byNtStation;			// 庄家位置			
	m_GameData.T_Begin.byPlayCount = m_UserData.byPlayCount;	// 游戏进行的局数	
	m_GameData.T_Begin.byQuanFeng =  m_UserData.m_byQuanFeng;	// 圈风设置	

	//确定风位：东南西北 固定不变
	m_UserData.m_byMenFeng[0] =0;//风位固定不变+
	m_GameData.T_Begin.m_byMenFeng[0] = 0;

	m_UserData.m_byMenFeng[1] =3;//风位固定不变+
	m_GameData.T_Begin.m_byMenFeng[1] = 3;

	m_UserData.m_byMenFeng[2] =2;//风位固定不变+
	m_GameData.T_Begin.m_byMenFeng[2] = 2;

	m_UserData.m_byMenFeng[3] =1;//风位固定不变+
	m_GameData.T_Begin.m_byMenFeng[3] = 1;

	BYTE count[4];
	memset(count,28,sizeof(count));//14敦
	int index = rand()%4;
	count[index] = 26;//13敦
	m_UserData.SetMenPaiCount(count);
	memcpy(m_GameData.T_Begin.m_byMenPaiCount,m_UserData.m_byMenPaiCount,sizeof(m_GameData.T_Begin.m_byMenPaiCount));
	for(int i=0;i<4;i++)
	{
		for(int j=0;j<m_UserData.m_byMenPaiCount[i];++j)
		{
			m_GameData.T_Begin.byMenPai[i][j] = 0; //门牌
		}
	}
	///当前抓牌方向，true 顺时针，false 逆时针
	m_UserData.m_bZhuaPaiDir = true;
	//发送开始消息
	SendAllGameData(&m_GameData.T_Begin,sizeof(m_GameData.T_Begin),ASS_GM_GAME_NOTIFY,EVENT_BEGIN,0);
	//超时设置，
	SetDoingThing(EVENT_BEGIN,0);//设置正在发生的事件
	SetGameState(STATE_BEGIN_OUT);//设置游戏状态
}

///掷2颗色子确定庄家（桂林麻将一颗色子定庄）
void CSrvLogicEx::ExecuteDongNt(BYTE &nextID,BYTE &time)
{	
	nextID = EVENT_2SEZI_GP;//定庄后就掷2可色子发牌：和定反向，小点定起牌顿数
	time = 2;	//5秒间隔
	srand(GetTickCount());
	m_UserData.m_byNtStation = rand()%PLAY_COUNT;//庄家位置：随机
	m_GameData.T_DongNt.byNt = m_UserData.m_byNtStation;
	m_UserData.m_MenPai.byGetPaiDir = m_UserData.m_byNtStation;//起牌方向(每局初始化为庄家门前起牌)

	//发送游戏消息
	SendAllGameData( &m_GameData.T_DongNt, sizeof(m_GameData.T_DongNt), ASS_GM_GAME_NOTIFY, EVENT_DONG_NT, 0);
	//超时设置
	SetDoingThing(EVENT_DONG_NT,0);//设置正在发生的事件
}

///掷2颗色子确定起牌敦数（点数和敦数）
void CSrvLogicEx::ExecuteTwoSeziGp(BYTE &nextID,BYTE &time)
{
	CSrvLogic::ExecuteTwoSeziGp(nextID,time);
}

///发牌
void CSrvLogicEx::ExecuteSendPai(BYTE &nextID,BYTE &time)	
{
	CSrvLogic::ExecuteSendPai(nextID,time);
	nextID = EVENT_YA_ZHU;//下一个事件的代号：押注
	time = 20;//留2秒发牌动画时间
}

///开始出牌通知事件
void CSrvLogicEx::ExecuteBeginOut(BYTE &nextID,BYTE &time)
{
	CSrvLogic::ExecuteBeginOut(nextID,time);
}

///抓牌事件
void CSrvLogicEx::ExecuteZhuaPai(BYTE &nextID,BYTE &time)
{
	CSrvLogic::ExecuteZhuaPai(nextID,time);
}

//碰牌事件
void CSrvLogicEx::ExecutePengPai(BYTE &nextID,BYTE &time)
{
	m_UserData.ReSetQiHu(m_GameData.temp_PengPai.byUser);//先清空弃糊
	CSrvLogic::ExecutePengPai(nextID,time);
}

//杠牌事件
void CSrvLogicEx::ExecuteGangPai(BYTE &nextID,BYTE &time)
{
	m_UserData.ReSetQiHu(m_GameData.temp_GangPai.byUser);//先清空弃糊
	CSrvLogic::ExecuteGangPai(nextID,time);
}

//糊牌事件
void CSrvLogicEx::ExecuteHuPai(BYTE &nextID,BYTE &time)
{
	CSrvLogic::ExecuteHuPai(nextID,time);
}

///算分事件
void CSrvLogicEx::ExecuteCountFen(BYTE &nextID,BYTE &time)
{
	CSrvLogic::ExecuteCountFen(nextID,time);
}

///正常游戏结束事件
void CSrvLogicEx::ExecuteNormalEnd(BYTE &nextID,BYTE &time)
{
	CSrvLogic::ExecuteNormalEnd(nextID,time);
}

///押注事件
void CSrvLogicEx::ExecuteYaZhu(BYTE &nextID,BYTE &time)
{
	//发送庄家押注消息
	tagYaZhuEx yazhu;
	yazhu.Clear();
	yazhu.byNotify = 0;
	//发送游戏消息
	SendAllGameData( &yazhu, sizeof(yazhu), ASS_GM_GAME_NOTIFY, EVENT_YA_ZHU, 0);
	//超时设置
	SetDoingThing(EVENT_YA_ZHU,0);//设置正在发生的事件
}


///名称：CheckAction 吃碰杠动作检测
///@param ：station 检测位置,pai 别的玩家打出的牌，type 类型 出牌 0 抓牌，1杠开，2 点炮，3抢杠
///@return true 能做动作，false 不能做动作
bool CSrvLogicEx::CheckAction(int station,BYTE pai,BYTE type)
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
			//能糊牌就设置弃糊
			m_UserData.ReSetQiHu(station);//先清空弃糊
			if(m_GameData.T_CPGNotify[station].bHu )
			{
				m_UserData.SetQiHu(station,m_GameData.T_CPGNotify[station].m_byFan);
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
				//糊牌检测弃糊
				if(m_GameData.T_CPGNotify[i].bHu )
				{
					if(m_UserData.CheckQiHu(i,m_GameData.T_CPGNotify[i].m_byFan))
					{//弃糊不能糊牌
						m_GameData.T_CPGNotify[i].bHu = false;
						m_GameData.T_CPGNotify[i].m_byFan = 0;
					}
					else
					{//设置弃糊番数
						m_UserData.SetQiHu(i,m_GameData.T_CPGNotify[i].m_byFan);
					}
				}
				if(m_GameData.T_CPGNotify[i].bHu || m_GameData.T_CPGNotify[i].bPeng || m_GameData.T_CPGNotify[i].bGang )
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
				m_CheckHuPai.CanHu(i,pai,m_GameData.T_CPGNotify[i],false,station);
				if(m_GameData.T_CPGNotify[i].bHu)
				{
					if(m_UserData.CheckQiHu(i,m_GameData.T_CPGNotify[i].m_byFan))
					{//弃糊不能糊牌
						m_GameData.T_CPGNotify[i].bHu = false;
						m_GameData.T_CPGNotify[i].m_byFan = 0;
					}
					else
					{//设置弃糊番数
						m_UserData.SetQiHu(i,m_GameData.T_CPGNotify[i].m_byFan);
					}
				}
				if(m_GameData.T_CPGNotify[i].bHu)//糊
				{
					action=true;
					m_GameData.T_CPGNotify[i].byPs = pai;
					m_GameData.T_CPGNotify[i].bZimo = false;
					m_GameData.T_CPGNotify[i].bCanAction = true;
				}
			}
		}
		break;
	}
	return action;
}


//防卡桌自动处理：2分钟内没有收到任何游戏消息，认为是游戏卡死，调用本函数，根据当前执行事件和玩家的状态进行自动处理，恢复游戏流程
void CSrvLogicEx::AutoHandle(bool bNetCut)
{
	CSrvLogic::AutoHandle( bNetCut);
	if(m_GameData.m_byThingDoing == EVENT_YA_ZHU)//押注事件：超时
	{
		if(!bNetCut)
		{
			for(int i=0;i<PLAY_COUNT;++i)
			{
				if(m_GameData.T_YaZhu.byUserNote[i] == 255)//有玩家没有回应
				{
					m_UserData.m_bYaZhu[i] = false;
				}
			}
			ApplyAction(EVENT_BEGIN_OUT,20);//申请出牌事件
		}
		else//断线处理
		{
			if(m_UserData.IsUserState(m_UserData.m_byNtStation,USER_STATE_CUT))//庄家断线
			{
				if(m_UserData.m_byYaZhu == 255)//庄家没有回应
				{//模拟一个放弃消息
					tagYaZhuEx yazhu;
					yazhu.Clear();
					yazhu.byNote = 0;
					yazhu.byNtNote = 0;
					ReceiveUserYaZhu(m_UserData.m_byNtStation,&yazhu,sizeof(tagYaZhuEx),false);
				}
			}
			for(int i=0;i<PLAY_COUNT;++i)
			{
				if(i == m_UserData.m_byNtStation)
					continue;
				if(!m_UserData.IsUserState(i,USER_STATE_CUT))
					continue;
				tagYaZhuEx yazhu;
				yazhu.Clear();
				yazhu.byNote = 0;
				ReceiveUserYaZhu(i,&yazhu,sizeof(tagYaZhuEx),false);
			}
		}
	}
}

///自动处理出牌或者抓牌
void CSrvLogicEx::AutoOutOrGet()
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
		if(outpai.byPs == CMjEnum::MJ_TYPE_BAI)//白板是财神不能打出
		{
			for(int j=0;j<m_UserData.GetHandPaiCount(i)-1;++j)
			{
				if(m_UserData.GetHandPaiFromIndex(i,j) !=  CMjEnum::MJ_TYPE_BAI)
				{
					outpai.byPs = m_UserData.GetHandPaiFromIndex(i,j);
					ReceiveUserOutCard(i,&outpai,sizeof(outpai),false);
					return;
				}
			}
		}
		ReceiveUserOutCard(i,&outpai,sizeof(outpai),false);
		break;
	}
	if(!haveout)//没有出牌的玩家，启动抓牌事件
	{
		ApplyAction(EVENT_ZHUA_PAI,200);//申请下一个事件
	}
}

/// 玩家坐桌
void CSrvLogicEx::UserSitDesk(BYTE bDeskStation)
{
	//通知玩家设置底分
	if(m_GameData.m_iBaseFen <= 0)//没有底分
	{
		//发送各个玩家的同意
		tagFanDiFenEx difen;
		difen.Clear();
		difen.byType = 0;
		difen.iBaseFen = m_GameData.m_iBaseFen;
		difen.byMaster = m_GameData.m_byMaster;
		//发送游戏消息
		SendGameData( bDeskStation,&difen,sizeof(difen), ASS_GM_GAME_NOTIFY, EVENT_MAKE_DIFEN, 0);
	}
}

/// 设置当前正在发生的事件：同时设置一个等待时间，超过时间客户端没有响应就自动处理
void CSrvLogicEx::SetDoingThing(BYTE thingId,BYTE st1,BYTE st2,BYTE pai,BYTE data)
{
	CSrvLogic::SetDoingThing(thingId, st1, st2, pai, data);
	if(thingId == EVENT_YA_ZHU)//押注事件
	{
		m_GameData.SetOutTime(st1,m_GameData.m_MjSet.byYaZhuTime *2,thingId,true);
	}
}