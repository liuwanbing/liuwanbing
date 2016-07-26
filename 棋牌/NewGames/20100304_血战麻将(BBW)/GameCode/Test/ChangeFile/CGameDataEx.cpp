#include "stdafx.h"
//#include "../Client/stdafx.h"
#include "CGameDataEx.h"


CGameDataEx::CGameDataEx(void)
{
	CGameData::CGameData();
	m_iBaseFen = 0;//游戏底分
	m_byMaster = 255;//桌主
	memset(m_bReBese,0,sizeof(m_bReBese));//是否回应了基础分设置
	LoadIni();
}

CGameDataEx::~CGameDataEx(void)
{
	CGameData::~CGameData();
}

///设置所有事件的后接事件
void CGameDataEx::SetThingNext()
{
	CGameData::SetThingNext();
}

///设置某个事件的后接事件
void CGameDataEx::SetThingNext(BYTE id)
{
	CGameData::SetThingNext(id);
}

//virtual 初始化数据
void CGameDataEx::InitData()
{
	CGameData::InitData();
	///翻醒事件数据包
	T_DiFen.Clear();
	//押注事件数据包
	T_YaZhu.Clear();
}

///初始化数据
void CGameDataEx::LoadIni()
{
	CGameData::LoadIni();
	m_mjRule.byGamePassNum	= 0;		//流局牌数,测试
	m_mjAction.bChi			= 0;	//不能吃
	m_mjRule.byAllMjCount   = 110;		//麻将的数量 
	m_mjRule.byNextNtType	= 1;		//下局庄家的确定类型
}

///初始化房间特殊数据
void CGameDataEx::LoadRoomIni(int id)
{
}

//加载牌数据
BYTE CGameDataEx::LoadPaiData(MjMenPaiData & menpai)
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
	}
	for(int i=0;i<2;i++)
	{//2张白板
		menpai.byMenPai[index] = stcArr_A_Mj_ZI[2];
		index++;
	}
	menpai.byAllPaiNum = index;
	return menpai.byAllPaiNum;
}


void  CGameDataEx::ClearBaseFen()//清空基础分信息
{
	m_iBaseFen = 0;//游戏底分
	m_byMaster = 255;//桌主
	memset(m_bReBese,0,sizeof(m_bReBese));//是否回应了基础分设置
}
