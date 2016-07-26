#include "stdafx.h"
//#include "../Client/stdafx.h"
#include "CGameData.h"

CGameData::CGameData(void)
{
	///逃跑要扣的罚分
	uRunPublish = 0;					
	///房间的倍率
	basepoint = 0;	
	///要申请的事件id
	ApplyThingID = 255;
	m_byApplyMaxAction = 0;//玩家当前申请的最大事件
	memset(m_byThingNext,255,sizeof(m_byThingNext));
	m_bShowUserId = false;				//是否显示所有玩家的id
	InitData();
	LoadIni();
}

CGameData::~CGameData(void)
{

}


///设置所有事件的后接事件
void CGameData::SetThingNext()
{
	//游戏开始事件
	T_Begin.byNext = m_byThingNext[EVENT_BEGIN];
	//首局以东为庄事件	
	T_DongNt.byNext = m_byThingNext[EVENT_DONG_NT];
	//首局掷2颗色子的点数和为庄事件	
	T_TwoSeziNt.byNext = m_byThingNext[EVENT_2SEZI_NT];
	//首局掷2颗色子定庄家和起牌点事件	
	T_TwoSeziNtAndGetPai.byNext = m_byThingNext[EVENT_2SEZI_NT_GP];
	//首局轮流掷2颗色子定庄家事件	
	T_TurnSeziNt.byNext = m_byThingNext[EVENT_TURN_2SEZI_NT];
	//掷2颗色子事件定起牌方向事件
	T_TwoSeziDir.byNext = m_byThingNext[EVENT_2SEZI_DIR];
	//掷2颗色子事件定起牌方向和起牌点事件
	T_TwoSeziDirAndGetPai.byNext = m_byThingNext[EVENT_2SEZI_DIR_GP];
	//掷2颗色子事件定起牌点事件	
	T_TwoSeziGetPai.byNext = m_byThingNext[EVENT_2SEZI_GP];
//出牌前事件
	//发牌事件	
	T_SendPai.byNext = m_byThingNext[EVENT_SEND_PAI];
	//掷2颗色子定精牌事件
	T_TwoSeziJing.byNext = m_byThingNext[EVENT_2SEZI_JING];
	//为所有玩家补花事件
	T_AllBuHua.byNext = m_byThingNext[EVENT_ALL_BUHUA];
	//单个玩家补花事件	
	T_OneBuHua.byNext = m_byThingNext[EVENT_ONE_BUHUA];
//正常动作事件
	//庄家开始发牌通知事件
	T_BeginOutPai.byNext = m_byThingNext[EVENT_BEGIN_OUT];
	//出牌事件
	T_OutPai.byNext = m_byThingNext[EVENT_OUT_PAI];
	//抓牌事件
	T_ZhuaPai.byNext = m_byThingNext[EVENT_ZHUA_PAI];
	//吃牌事件
	T_ChiPai.byNext = m_byThingNext[EVENT_CHI_PAI];
	//碰牌事件
	T_PengPai.byNext = m_byThingNext[EVENT_PENG_PAI];
	//杠牌事件
	T_GangPai.byNext = m_byThingNext[EVENT_GANG_PAI];
	//听牌事件
	T_TingPai.byNext = m_byThingNext[EVENT_TING_PAI];
//糊牌及糊牌后事件
	//糊事件
	T_HuPai.byNext = m_byThingNext[EVENT_HU_PAI];
	//算分事件	
	T_CountFen.byNext = m_byThingNext[EVENT_COUNT_FEN];
	//结束处理事件
	T_EndHandle.byNext = m_byThingNext[EVENT_END_HANDLE];
	//非正常结束处理事件
	T_UnnormalEndHandle.byNext = m_byThingNext[EVENT_ENG_UN_NORMAL];
}
///设置某个事件的后接事件
void CGameData::SetThingNext(BYTE id)
{
	switch(id)
	{
//发牌前事件
	case EVENT_BEGIN:			//游戏开始事件
		T_Begin.byNext = m_byThingNext[id];
		break;
	case EVENT_DONG_NT:			//首局以东为庄事件	
		T_DongNt.byNext = m_byThingNext[id];
		break;
	case EVENT_2SEZI_NT:		//首局掷2颗色子的点数和为庄事件	
		T_TwoSeziNt.byNext = m_byThingNext[id];
		break;
	case EVENT_2SEZI_NT_GP:		//首局掷2颗色子定庄家和起牌点事件	
		T_TwoSeziNtAndGetPai.byNext = m_byThingNext[id];
		break;
	case EVENT_TURN_2SEZI_NT:	//首局轮流掷2颗色子定庄家事件	
		T_TurnSeziNt.byNext = m_byThingNext[id];
		break;
	case EVENT_2SEZI_DIR:		//掷2颗色子事件定起牌方向事件
		T_TwoSeziDir.byNext = m_byThingNext[id];
		break;
	case EVENT_2SEZI_DIR_GP:	//掷2颗色子事件定起牌方向和起牌点事件
		T_TwoSeziDirAndGetPai.byNext = m_byThingNext[id];
		break;
	case EVENT_2SEZI_GP:		//掷2颗色子事件定起牌点事件	
		T_TwoSeziGetPai.byNext = m_byThingNext[id];
		break;
//出牌前事件
	case EVENT_SEND_PAI:		//发牌事件	
		T_SendPai.byNext = m_byThingNext[id];
		break;
	case EVENT_2SEZI_JING:		//掷2颗色子定精牌事件
		T_TwoSeziJing.byNext = m_byThingNext[id];
		break;
	case EVENT_ALL_BUHUA:		//为所有玩家补花事件
		T_AllBuHua.byNext = m_byThingNext[id];
		break;
	case EVENT_ONE_BUHUA:		//单个玩家补花事件	
		T_OneBuHua.byNext = m_byThingNext[id];
		break;
//正常动作事件
	case EVENT_BEGIN_OUT:		//庄家开始发牌通知事件
		T_BeginOutPai.byNext = m_byThingNext[id];
		break;
	case EVENT_OUT_PAI:			//出牌事件
		T_OutPai.byNext =		m_byThingNext[id];
		break;
	case EVENT_ZHUA_PAI:		//抓牌事件
		T_ZhuaPai.byNext = m_byThingNext[id];
		break;
	case EVENT_CPG_NOTIFY:		//杠吃碰通知事件事件
		break;
	case EVENT_CHI_PAI:			//吃牌事件
		T_ChiPai.byNext = m_byThingNext[id];
		break;
	case EVENT_PENG_PAI:		//碰牌事件
		T_PengPai.byNext = m_byThingNext[id];
		break;
	case EVENT_GANG_PAI:		//杠牌事件
		T_GangPai.byNext = m_byThingNext[id];
		break;
	case EVENT_TING_PAI:		//听牌事件
		T_TingPai.byNext = m_byThingNext[id];
		break;
//其他动作事件
//糊牌及糊牌后事件
	case EVENT_HU_PAI:			//糊事件
		T_HuPai.byNext = m_byThingNext[id];
		break;
	case EVENT_COUNT_FEN:		//算分事件	
		T_CountFen.byNext = m_byThingNext[id];
		break;
	case EVENT_END_HANDLE:		//结束处理事件
		T_EndHandle.byNext = m_byThingNext[id];
		break;
	case EVENT_ENG_UN_NORMAL:	//非正常结束处理事件
		T_UnnormalEndHandle.byNext = m_byThingNext[id];
		break;
	}
}

///初始化数据
void CGameData::InitData()
{
	m_byApplyMaxAction = 0;//玩家当前申请的最大事件
	//事件记录只记录后10个事件：0事件号，1相关牌值，2相关位置1,3相关位置2，4附加类型
	memset(m_byThingLog,255,sizeof(m_byThingLog));
	///游戏当前状态：出牌前状态，出牌状态，拦牌状态，抢杠状态，游戏结束状态，其他状态
	m_byGameState = STATE_BEGIN_OUT;  
	///待执行事件
	m_byNextAction = ACTION_NO;	
	///正在发生的事件号
	m_byThingDoing = 255;  
	///吃牌牌事件,临时数据
	temp_ChiPai.Clear();
	///碰牌牌事件,临时数据
	temp_PengPai.Clear();
	///杠牌牌事件,临时数据
	temp_GangPai.Clear();
	///游戏开始事件
	T_Begin.Clear();
	///以东为庄事件
	T_DongNt.Clear();
	///掷2颗色子的点数和为庄事件
	T_TwoSeziNt.Clear();
	///掷2颗色子确定庄家和起牌点位置事件
	T_TwoSeziNtAndGetPai.Clear();
	///轮流掷2颗色子确定庄家
	T_TurnSeziNt.Clear();
	///掷2颗色子确定起牌位置事件
	T_TwoSeziDir.Clear();
	///掷2颗色子确定起牌位置（点数和）和起牌点（最小点）事件
	T_TwoSeziDirAndGetPai.Clear();
	///掷2颗色子确定起牌敦数事件
	T_TwoSeziGetPai.Clear();
	///发牌事件
	T_SendPai.Clear();
	///掷色子2颗色子定精牌事件
	T_TwoSeziJing.Clear();
	///所有玩家补花事件
	T_AllBuHua.Clear();
	///单个玩家补花事件
	T_OneBuHua.Clear();
	///开始出牌通知事件
	T_BeginOutPai.Clear();
	///出牌事件
	T_OutPai.Clear();
	///抓牌牌事件
	T_ZhuaPai.Clear();
	/////吃碰杠糊牌通知事件牌事件
	//T_CPGNotify[4];

	///吃牌牌事件
	T_ChiPai.Clear();
	///碰牌牌事件
	T_PengPai.Clear();
	///杠牌牌事件
	T_GangPai.Clear();
	///听牌牌事件
	T_TingPai.Clear();
	///糊牌牌事件
	T_HuPai.Clear();
	///算分事件
	T_CountFen.Clear();
	///游戏结束处理事件
	T_EndHandle.Clear();
	///非正常结束处理事件
	T_UnnormalEndHandle.Clear();

	SetThingNext();

}

///在人配置文件
void CGameData::LoadIni()
{
	m_mjRule.byAllMjCount   = 136;		//麻将的数量 
	m_mjRule.bHaveFlower	= 0;		//是否有花牌
	m_mjRule.bGangZhong		= 0;		//是否杠红中
	m_mjRule.bGangFa		= 0;		//是否杠发财
	m_mjRule.bHaveFlower	= 0;		//是否杠白板
	m_mjRule.bHaveHongZhong	= 1;		//是否有红中
	m_mjRule.bHaveFaCai		= 1;		//是否有发财
	m_mjRule.bHaveBaiBan	= 1;		//是否有白板
	m_mjRule.bHaveFengPai	= 1;		//是否有东南西北
	m_mjRule.bHaveFlower	= 0;		//是否有花牌
	m_mjRule.byHandCardNum	= 13;		//手牌张数
	m_mjRule.byGamePassNum	= 0;		//流局牌数
	m_mjRule.byNextNtType	= 0;		//下局庄家的确定类型
	m_mjRule.bHaveJing		= 0;		//有无财神

	m_mjRule.bHaveQFeng		= 0;		//是否有圈风
	m_mjRule.byMenPaiNum[0]	= 17;		//各家门牌数
	m_mjRule.byMenPaiNum[1]	= 17;		//各家门牌数
	m_mjRule.byMenPaiNum[2]	= 17;		//各家门牌数
	m_mjRule.byMenPaiNum[3]	= 17;		//各家门牌数
	m_mjRule.byOtherJing	= 0;		//其他财神

	m_mjAction.bChi				= 1;	//吃
	m_mjAction.bChiFeng			= 0;	//吃风牌
	m_mjAction.bChiJian			= 0;	//吃中发白
	m_mjAction.bPeng			= 1;	//碰
	m_mjAction.bGang			= 1;	//杠
	m_mjAction.bBuGang			= 1;	//补杠
	m_mjAction.bTing			= 0;	//听
	m_mjAction.bQiangChi		= 0;	//抢吃(吃后听牌)

//
//#pragma warning(push)
//#pragma warning(disable:4800)
//	Tstring s = CINIFile::GetAppPath ();/////本地路径
//
//	CINIFile pz(s + SKIN_FOLDER  + _T("_s.ini"));
//
//	Tstring key = TEXT("game");// 麻将常规属性
//
//
//	m_mjRule.bNetCutTuoGuan	= pz.GetKeyVal(key,TEXT("bNetCutTuoGuan"),1);		//是否断线托管
//	m_mjRule.bForceTuoGuan	= pz.GetKeyVal(key,TEXT("bForceTuoGuan"),1);		//是否强退托管
//	m_bShowUserId =			pz.GetKeyVal(key,TEXT("bShowUserId"),0);			//是否显示所有玩家的id
//
//
//	CINIFile f(s+_T("\\mj\\") +SKIN_FOLDER  + _T("_s_Rule.ini"));
//
//
//	key = TEXT("BaseRule");// 麻将常规属性


//#pragma warning(pop)
}


//加载牌数据
BYTE CGameData::LoadPaiData(MjMenPaiData & menpai)
{
	BYTE index=0;
	memset(menpai.byMenPai,255,sizeof(menpai.byMenPai));//
	for(int i=0;i<4;i++)
	{
		for(int j=0;j<27;j++)
		{
			menpai.byMenPai[index] = stcArr_A_Mj_WTT[j];
			index++;
		}
		if(m_mjRule.bHaveBaiBan)//有白板
		{
			menpai.byMenPai[index] = stcArr_A_Mj_ZI[2];
			index++;
		}
		if(m_mjRule.bHaveFaCai)//有发财
		{
			menpai.byMenPai[index] = stcArr_A_Mj_ZI[1];
			index++;
		}
		if(m_mjRule.bHaveHongZhong)//有红中
		{
			menpai.byMenPai[index] = stcArr_A_Mj_ZI[0];
			index++;
		}
		if(m_mjRule.bHaveFengPai)//有东南西北
		{
			for(int j=0;j<4;j++)
			{
				menpai.byMenPai[index] = stcArr_A_Mj_FENG[j];
				index++;
			}
		}
	}
	if(m_mjRule.bHaveFlower)//有花牌
	{
		for(int i=0;i<8;i++)
		{
			menpai.byMenPai[index] = stcArr_A_Mj_Other[i];
			index++;
		}
	}
	menpai.byAllPaiNum = index;
	return menpai.byAllPaiNum;
}

///收到玩家的动作请求
void CGameData::AcceptAction(BYTE station,BYTE type)
{
	if(station<0 || station>=PLAY_COUNT || type<ACTION_NO || type>ACTION_HU)//不合法的请求
		return;
	T_CPGNotify[station].bHaveReturn = true;//记录玩家回应
	T_CPGNotify[station].m_byMaxThing = type;//改变玩家能做的最大动作
	if(m_byNextAction < type || m_byNextAction == 255)
	{
		m_byNextAction = type;
	}
	if(m_byApplyMaxAction < type|| m_byApplyMaxAction == 255)
	{
		m_byApplyMaxAction = type;//玩家当前申请的最大事件
	}
}


///获取最大的请求
BYTE CGameData::GetMaxRequet(BYTE station)
{
	BYTE max = 0;
	for(int i=0;i<4;++i)//查找出最大事件
	{
		if(i==station || T_CPGNotify[i].m_byMaxThing <= max || T_CPGNotify[i].m_byMaxThing==255)
			continue;
		max = T_CPGNotify[i].m_byMaxThing;
	}
	return max;
}

///指定请求是否全部回应
bool CGameData::IsNotifyAllReply(BYTE station,BYTE request)
{
	BYTE num = 0,re=0;
	for(int i=0;i<PLAY_COUNT;++i)//最大事件个数，和最大事件回应的个数
	{
		if(i==station)
			continue;
		if(request == T_CPGNotify[i].m_byMaxThing && T_CPGNotify[i].m_byMaxThing!=255)
		{
			num++;
			if(T_CPGNotify[i].bHaveReturn)
				re++;
		}
	}
	return num==re;
}

/// 设置超时数据
void CGameData::SetOutTime(BYTE station,BYTE time,BYTE thing,bool effect)
{
	m_MjWait.byWaitThing = thing;
	m_MjWait.byCurChair = station;
	m_MjWait.byTime = time + 2;
	m_MjWait.bEffect = effect;
}
/// 恢复超时数据
void CGameData::ReSetOutTime()
{
	m_MjWait.Clear();
}


/// 获得玩家的吃牌数量和能组吃的牌
int CGameData::GetUserChiCount(BYTE station,BYTE handpai[])
{
	int count = 0,num=0;
	if(!T_CPGNotify[station].bChi)
		return 0;
	for(int i=0;i<3;++i)
	{	
		if( T_CPGNotify[station].m_iChi[i][0]== 255)
			continue;
		count++;
		for(int j=0;j<3;++j)
		{
			handpai[num] = T_CPGNotify[station].m_iChi[i][j];
			num++;
		}
	}
	return count;
}

/// 获得玩家的杠牌牌数量和能杠的牌
int CGameData::GetUserGangCount(BYTE station,BYTE handpai[])
{
	int count = 0,num=0;
	BYTE tmp = 255;
	if(!T_CPGNotify[station].bGang)
		return 0;
	for(int i=0;i<4;++i)
	{	
		if(T_CPGNotify[station].m_iGangData[i][1]== 255)
			continue;
		count++;
		handpai[num] = T_CPGNotify[station].m_iGangData[i][1];
		num++;
	}
	return count;
}

//获得能杠牌的数量
int CGameData::GetCanGangCount(BYTE station)
{
	int count =0;
	for(int i=0;i<4;++i)
	{	
		if(T_CPGNotify[station].m_iGangData[i][1]== 255)
			continue;
		count++;
	}
	return count;
}
//获得能吃牌的数量
int CGameData::GetCanChiCount(BYTE station)
{
	int count =0;
	for(int i=0;i<3;++i)
	{	
		if(T_CPGNotify[station].m_iChi[i][0]== 255)
			continue;
		count++;
	}
	return count;
}


///清理临时数据
void CGameData::ClearTempData()
{
	//吃牌牌事件,临时数据
	temp_ChiPai.Clear();
	//碰牌牌事件,临时数据
	temp_PengPai.Clear();
	//杠牌牌事件,临时数据
	temp_GangPai.Clear();	
	//清理杠牌数据
	T_GangPai.Clear();	
}






