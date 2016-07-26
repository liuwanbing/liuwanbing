#include "stdafx.h"
//#include "../Client/stdafx.h"
#include "GameData.h"

GameData::GameData(void)
{
	///是否可以离开游戏
	m_bCanLeaveGame = false;
	///逃跑要扣的罚分
	uRunPublish = 0;					
	///房间的倍率
	basepoint = 0;	
	///要申请的事件id
	ApplyThingID = 255;
	memset(m_byThingLog,255,sizeof(m_byThingLog));//事件记录只记录后10个事件：0事件号，1相关牌值，2相关位置1,3相关位置2，4附加类型
	memset(m_byThingNext,255,sizeof(m_byThingNext));
	m_bShowUserId = false;				//是否显示所有玩家的id
	InitData();
	LoadIni();
}

GameData::~GameData(void)
{

}


///设置所有事件的后接事件
void GameData::SetThingNext()
{
	//游戏开始事件
	T_Begin.byNext = m_byThingNext[THING_BEGIN];
	//首局以东为庄事件	
	T_DongNt.byNext = m_byThingNext[THING_DONG_NT];
	//首局掷2颗色子的点数和为庄事件	
	T_TwoSeziNt.byNext = m_byThingNext[THING_2SEZI_NT];
	//首局掷2颗色子定庄家和起牌点事件	
	T_TwoSeziNtAndGetPai.byNext = m_byThingNext[THING_2SEZI_NT_GP];
	//首局轮流掷2颗色子定庄家事件	
	T_TurnSeziNt.byNext = m_byThingNext[THING_TURN_2SEZI_NT];
	//掷2颗色子事件定起牌方向事件
	T_TwoSeziDir.byNext = m_byThingNext[THING_2SEZI_DIR];
	//掷2颗色子事件定起牌方向和起牌点事件
	T_TwoSeziDirAndGetPai.byNext = m_byThingNext[THING_2SEZI_DIR_GP];
	//掷2颗色子事件定起牌点事件	
	T_TwoSeziGetPai.byNext = m_byThingNext[THING_2SEZI_GP];
//出牌前事件
	//发牌事件	
	T_SendPai.byNext = m_byThingNext[THING_SEND_PAI];
	//跳牌事件	
	T_TiaoPai.byNext = m_byThingNext[THING_TIAO_PAI];
	//掷2颗色子定精牌事件
	T_TwoSeziJing.byNext = m_byThingNext[THING_2SEZI_JING];
	//掷1颗色子定金牌事件
	T_OneSeziJin.byNext = m_byThingNext[THING_1SEZI_JIN];
	//为所有玩家补花事件
	T_AllBuHua.byNext = m_byThingNext[THING_ALL_BUHUA];
	//单个玩家补花事件	
	T_OneBuHua.byNext = m_byThingNext[THING_ONE_BUHUA];
//正常动作事件
	//庄家开始发牌通知事件
	T_BeginOutPai.byNext = m_byThingNext[THING_BEGIN_OUT];
	//出牌事件
	T_OutPai.byNext = m_byThingNext[THING_OUT_PAI];
	//抓牌事件
	T_ZhuaPai.byNext = m_byThingNext[THING_ZHUA_PAI];
	//吃牌事件
	T_ChiPai.byNext = m_byThingNext[THING_CHI_PAI];
	//碰牌事件
	T_PengPai.byNext = m_byThingNext[THING_PENG_PAI];
	//坎牌事件
	T_KanPai.byNext = m_byThingNext[THING_KAN_PAI];
	//扫虎事件
	T_SaoHu.byNext = m_byThingNext[THING_SAO_HU];
	//杠牌事件
	T_GangPai.byNext = m_byThingNext[THING_GANG_PAI];
	//听牌事件
	T_TingPai.byNext = m_byThingNext[THING_TING_PAI];
//糊牌及糊牌后事件
	//糊事件
	T_HuPai.byNext = m_byThingNext[THING_HU_PAI];
	//算分事件	
	T_CountFen.byNext = m_byThingNext[THING_COUNT_FEN];
	//结束处理事件
	T_EndHandle.byNext = m_byThingNext[THING_ENG_HANDLE];
	//非正常结束处理事件
	T_UnnormalEndHandle.byNext = m_byThingNext[THING_ENG_UN_NORMAL];
}
///设置某个事件的后接事件
void GameData::SetThingNext(BYTE id)
{
	switch(id)
	{
//发牌前事件
	case THING_BEGIN:			//游戏开始事件
		T_Begin.byNext = m_byThingNext[id];
		break;
	case THING_DONG_NT:			//首局以东为庄事件	
		T_DongNt.byNext = m_byThingNext[id];
		break;
	case THING_2SEZI_NT:		//首局掷2颗色子的点数和为庄事件	
		T_TwoSeziNt.byNext = m_byThingNext[id];
		break;
	case THING_2SEZI_NT_GP:		//首局掷2颗色子定庄家和起牌点事件	
		T_TwoSeziNtAndGetPai.byNext = m_byThingNext[id];
		break;
	case THING_TURN_2SEZI_NT:	//首局轮流掷2颗色子定庄家事件	
		T_TurnSeziNt.byNext = m_byThingNext[id];
		break;
	case THING_2SEZI_DIR:		//掷2颗色子事件定起牌方向事件
		T_TwoSeziDir.byNext = m_byThingNext[id];
		break;
	case THING_2SEZI_DIR_GP:	//掷2颗色子事件定起牌方向和起牌点事件
		T_TwoSeziDirAndGetPai.byNext = m_byThingNext[id];
		break;
	case THING_2SEZI_GP:		//掷2颗色子事件定起牌点事件	
		T_TwoSeziGetPai.byNext = m_byThingNext[id];
		break;
//出牌前事件
	case THING_SEND_PAI:		//发牌事件	
		T_SendPai.byNext = m_byThingNext[id];
		break;
	case THING_TIAO_PAI:		//跳牌事件	
		T_TiaoPai.byNext = m_byThingNext[id];
		break;
	case THING_2SEZI_JING:		//掷2颗色子定精牌事件
		T_TwoSeziJing.byNext = m_byThingNext[id];
		break;
	case THING_1SEZI_JIN:		//掷1颗色子定金牌事件
		T_OneSeziJin.byNext = m_byThingNext[id];
		break;
	case THING_ALL_BUHUA:		//为所有玩家补花事件
		T_AllBuHua.byNext = m_byThingNext[id];
		break;
	case THING_ONE_BUHUA:		//单个玩家补花事件	
		T_OneBuHua.byNext = m_byThingNext[id];
		break;
//正常动作事件
	case THING_BEGIN_OUT:		//庄家开始发牌通知事件
		T_BeginOutPai.byNext = m_byThingNext[id];
		break;
	case THING_OUT_PAI:			//出牌事件
		T_OutPai.byNext =		m_byThingNext[id];
		break;
	case THING_ZHUA_PAI:		//抓牌事件
		T_ZhuaPai.byNext = m_byThingNext[id];
		break;
	case THING_CPG_NOTIFY:		//杠吃碰通知事件事件
		break;
	case THING_CHI_PAI:			//吃牌事件
		T_ChiPai.byNext = m_byThingNext[id];
		break;
	case THING_PENG_PAI:		//碰牌事件
		T_PengPai.byNext = m_byThingNext[id];
		break;
	case THING_KAN_PAI:			//坎牌事件
		T_KanPai.byNext = m_byThingNext[id];
		break;
	case THING_SAO_HU:			//扫虎事件
		T_SaoHu.byNext = m_byThingNext[id];
		break;
	case THING_GANG_PAI:		//杠牌事件
		T_GangPai.byNext = m_byThingNext[id];
		break;
	case THING_TING_PAI:		//听牌事件
		T_TingPai.byNext = m_byThingNext[id];
		break;
//其他动作事件
//糊牌及糊牌后事件
	case THING_HU_PAI:			//糊事件
		T_HuPai.byNext = m_byThingNext[id];
		break;
	case THING_COUNT_FEN:		//算分事件	
		T_CountFen.byNext = m_byThingNext[id];
		break;
	case THING_ENG_HANDLE:		//结束处理事件
		T_EndHandle.byNext = m_byThingNext[id];
		break;
	case THING_ENG_UN_NORMAL:	//非正常结束处理事件
		T_UnnormalEndHandle.byNext = m_byThingNext[id];
		break;
	}
	 
}

///初始化数据
void GameData::InitData()
{

	memset(m_byThingRecord,255,sizeof(m_byThingRecord));
	memset(m_bGangState,0,sizeof(m_bGangState));//记录杠牌状态，拥于杠开和杠后炮

	///待执行事件
	m_byNextAction = ACTION_NO;
	///上次发生的事件号
	m_byThingLast = 255;		
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

	///跳牌事件
	T_TiaoPai.Clear();

	///掷色子2颗色子定精牌事件
	T_TwoSeziJing.Clear();

	///掷色子1颗色子定金牌事件
	T_OneSeziJin.Clear();

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

	///坎牌牌事件
	T_KanPai.Clear();

	///扫虎事件
	T_SaoHu.Clear();

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

	memset(m_byThingLog,255,sizeof(m_byThingLog));//事件记录只记录后10个事件：0事件号，1相关牌值，2相关位置1,3相关位置2，4附加类型
}

///在人配置文件
void GameData::LoadIni()
{

	m_mjRule.bySeziNum		= 2;			//筛子的个数
	m_mjRule.byAllMjCount   = 136;		//麻将的数量 
	m_mjRule.bHaveFlower	= 0;			//是否有花牌
	m_mjRule.bGangZhong		= 0;			//是否杠红中
	m_mjRule.bGangFa		= 0;				//是否杠发财
	m_mjRule.bHaveFlower	= 0;			//是否杠白板
	m_mjRule.bHaveHongZhong	= 1;		//是否有红中
	m_mjRule.bHaveFaCai		= 1;			//是否有发财
	m_mjRule.bHaveBaiBan	= 1;			//是否有白板
	m_mjRule.bHaveFengPai	= 1;		//是否有东南西北
	m_mjRule.bHaveFlower	= 0;			//是否有花牌
	m_mjRule.byHandCardNum	= 13;		//手牌张数
	m_mjRule.byGamePassNum	= 0;		//流局牌数
	m_mjRule.byGamePassType	= 0;		//流局留牌类型
	m_mjRule.byNextNtType	= 0;		//下局庄家的确定类型
	m_mjRule.bHaveJing		= 0;			//有无财神
	m_mjRule.bRandJing		= 0;           //随机数定精
	m_mjRule.bMainJing		= 0;			//正精
	m_mjRule.bLessJing		= 0;			//副精
	m_mjRule.bHaveQFeng		= 0;			//是否有圈风
	m_mjRule.byMenPaiNum[0]	= 17;		//各家门牌数
	m_mjRule.byMenPaiNum[1]	= 17;		//各家门牌数
	m_mjRule.byMenPaiNum[2]	= 17;		//各家门牌数
	m_mjRule.byMenPaiNum[3]	= 17;		//各家门牌数
	m_mjRule.byOtherJing	= 0;			//其他财神

	m_mjAction.bChi				= 0;			//吃
	m_mjAction.bChiFeng			= 0;		//吃风牌
	m_mjAction.bChiJian			= 0;		//吃中发白
	m_mjAction.bPeng			= 1;			//碰
	m_mjAction.bKan				= 0;			//坎
	m_mjAction.bSaoHu			= 0;			//扫虎
	m_mjAction.bGang			= 1;			//杠
	m_mjAction.bBuGang			= 1;			//补杠
	m_mjAction.bTing			= 0;			//听
	m_mjAction.bQiangChi		= 0;		//抢吃(吃后听牌)

	//m_mjRule.byGoodPaiChance	= f.GetKeyVal(key,TEXT("byGoodPaiChance"),0);	//发好牌的几率
	//m_mjRule.bGetGoodPai	= f.GetKeyVal(key,TEXT("bAutoBegin"),0);			//是否随机发好牌
#pragma warning(push)
#pragma warning(disable:4800)
	CString s = CINIFile::GetAppPath ();/////本地路径

	CINIFile pz(s + SKIN_FOLDER  + _T("_s.ini"));

	CString key = TEXT("game");// 麻将常规属性


	m_mjRule.byAutoOutTime	= pz.GetKeyVal(key,TEXT("byAutoOutTime"),1);		//自动出牌时间
	m_mjRule.byOutTime		= pz.GetKeyVal(key,TEXT("byOutTime"),20);			//出牌时间
	m_mjRule.byThinkTime	= pz.GetKeyVal(key,TEXT("byThinkTime"),10);			//思考时间
	m_mjRule.byBeginTime	= pz.GetKeyVal(key,TEXT("byBeginTime"),30);			//开始等待时间
	m_mjRule.bAutoBegin		= pz.GetKeyVal(key,TEXT("bAutoBegin"),1);			//时间到了是否自动开始

	m_mjRule.bNetCutTuoGuan	= pz.GetKeyVal(key,TEXT("bNetCutTuoGuan"),1);		//是否断线托管
	m_mjRule.bForceTuoGuan	= pz.GetKeyVal(key,TEXT("bForceTuoGuan"),1);		//是否强退托管
	m_bShowUserId =			pz.GetKeyVal(key,TEXT("bShowUserId"),0);			//是否显示所有玩家的id
	m_mjRule.byGamePassNum	= pz.GetKeyVal(key,TEXT("GamePassNum"),0);;		//流局牌数


	CINIFile f(s+_T("\\mj\\") +SKIN_FOLDER  + _T("_s_Rule.ini"));


#pragma warning(pop)
}