
#include "StdAfx.h"
#include "GameDataObject.h"
#include "../GameMessage/GameAlgorithm.h"
#include "../GameMessage/UpGradeLogic.h"

///名称：CGameDataObject
///描述：初始化分数设置，胡牌类型等
///@param 
///@return 
CGameDataObject::CGameDataObject(void)
{	
	memset(m_UserMoney, 0, sizeof(m_UserMoney));
	m_iDealPeople = 255;
}

///名称：析构函数
///描述：~CGameDataObject
///@param 
///@return 
CGameDataObject::~CGameDataObject(void)
{

}

///描述：初始化
void CGameDataObject::Init()
{
	//HalfInit();
	//清理用户数据
	for(int i = 0; i < PLAY_COUNT; i++)
	{
		m_UserData[i].Init();
	}

	//最近的成绩单
	//m_tagResult.Clear();

	//庄家
	m_byNTUser = 255;

	memset(m_bNoBet, 0, sizeof(m_bNoBet));
	memset(m_nBetMoney, 0, sizeof(m_nBetMoney));
	memset(m_bGiveUp, true, sizeof(m_bGiveUp));
    memset(m_bResultCards, 0, sizeof(m_bResultCards));
	memset(m_byAllPools, 0, sizeof(m_byAllPools));	
	memset(m_bAllBet, 0, sizeof(m_bAllBet));

	memset(m_iCards, 0, sizeof(m_iCards));
	m_iCardsCount = 0;

	m_nMaxBetMoney = 0;
	m_nBetPool = 0;
	m_iCheckCount = 0;
    m_bRaiseStation = -1;
	m_bBetStation = 255;
    m_bAllBetStation = -1;
	m_bFirstStation = -1;
    m_iBetCounts = 0;
	m_bGiveUpOver = false;
    m_bNewTurn = false;
	m_bNewTurns = false;

    //用户当前可做的动作标记
    m_byVerbFlag = 0x00;
}

///描述：半初始化，用于回合之间数据完全的部分清理，因为有的数据需要保留。
void CGameDataObject::HalfInit()
{
	//大多数游戏都会有的数据///////////////////////////////////

	//清除扑克源数据
	::memset(m_byCards,255,sizeof(m_byCards));

	//清除底牌的数组
	::memset(m_byBackCards,255,sizeof(m_byBackCards));

	//发牌位置
	m_iGetCardPos = 0;

	//清理用户数据
	for(int i = 0; i < PLAY_COUNT; i++)
	{
		m_UserData[i].HalfInit();
	}

	//当前游戏活动的圈数
	m_nTurnNums = 0;

	//德克萨斯扑克游戏特有的数据//////////////////////////////////

	//当前活动的用户
	m_byTokenUser	= 255;
	m_bSmallBlind	= 255;
	m_bBigBlind		= 255;
	// 是否有玩家加注
	m_bRaise = false;

	// 记录加注玩家
	m_iRaiseCounts = 0;

	// 记录加注玩家
	m_iAllinUser = 0;

	// 记录跟注玩家
	m_iCallCounts = 0;

	// 记录弃牌玩家
	m_iFoldCount = 0;

	m_bAutoSendCard = false;

	return;
}
															
///描述：设置玩家为未知是否同意底注状态
///@param bDeskStation玩家位置
void CGameDataObject::SetUnknowStateTo(BYTE bDeskStation)
{
	m_UserData[bDeskStation].m_nState = CUserDataSvrObject::UNKNOWN;
	return;
}

///描述：设置玩家为同意底注状态
///@param bDeskStation玩家位置
void CGameDataObject::SetAgreeStateTo(BYTE bDeskStation)
{
	m_UserData[bDeskStation].m_nState = CUserDataSvrObject::AGREE_DESKCFG;
	return;
}

///描述：设置玩家为不同意底注状态
///@param bDeskStation玩家位置
void CGameDataObject::SetDisAgreeStateTo(BYTE bDeskStation)
{
	m_UserData[bDeskStation].m_nState = CUserDataSvrObject::DISAGREE_DESKCFG;
	return;
}

///描述：为某个玩家发牌
///@param bDeskStation玩家位置，由外界保证不越界；iSendNum发牌数量。
///@return 
bool CGameDataObject::DealCardsTo(BYTE bDeskStation,int iSendNum)
{
	BYTE byGetPai = 0;
	CUserDataSvrObject* pUserData = &m_UserData[bDeskStation];
	
	for (int i = 0; i < iSendNum; ++i)
	{
		if (m_iGetCardPos < MAX_CARDS_NUM)
		{
			byGetPai = m_byCards[m_iGetCardPos++];
		}

		pUserData->m_byMyCards[pUserData->m_iCardsNum++] = byGetPai;
	}
	
	return true;
}

///名称：GetAgreeNums
///描述：得到有多少个玩家同意游戏（同意底注）
int CGameDataObject::GetAgreeNums()
{
	int iAgreeConuter = 0;
	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		if (m_UserData[i].m_nState == CUserDataSvrObject::AGREE_DESKCFG)
		{
			iAgreeConuter++;
		}
	}

	return iAgreeConuter;
}

//得到有多少个玩家已发完牌
bool CGameDataObject::IsAllPlayerSendCardFinish()
{
	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		if (m_UserData[i].m_iCardsNum > 0)
		{
			if (!m_UserData[i].m_bSendCardFinish)
			{
				return false;
			}
		}
	}
	return true;
}

//获取活动玩家的数量
int CGameDataObject::GetActivePlayerNums()
{
	int iNums = 0;

	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		if (m_UserData[i].m_iCardsNum > 0)
		{
			iNums++;
		}
	}

	return iNums;
}

//获取下注金币
__int64 CGameDataObject::GetBetMoney(BYTE bDeskStation, bool bAll)
{
	int i, j = 0;
	__int64 nCount = 0;

	if (bAll)
	{
		if (bDeskStation == 255)
		{
			// 获取所有玩家下注金币
			for (i = 0; i < PLAY_COUNT; i++)
			{
				for (j = 0; j < 8; j++)
				{
					nCount += m_nBetMoney[i][j];
				}
			}
		}
		else 
		{
			// 获取玩家金币
			for (i = 0; i < 8; i++)
			{
				nCount += m_nBetMoney[bDeskStation][i];
			}
		}
	}
	else
	{
		// 获取指定的边池所有金币
		for (i = 0; i < PLAY_COUNT; i++)
		{
			nCount += m_nBetMoney[i][bDeskStation];
		}
	}

	return nCount;
}

///名称：SetVerbs
///描述：设置玩家的动作
///@param pCards取得的牌值放于此
///@return
void CGameDataObject::MakeVerbs(BYTE byFlag)
{
    m_byVerbFlag = byFlag;
}

//玩家获得好牌
void CGameDataObject::GetGoodCard(BYTE bDeskStation)
{
	//玩家还没有获得手牌 直接返回
	if(m_iGetCardPos <= 0)
	{
		return;
	}

	BYTE byDeskCard[5] = {0};	//桌面牌

	::CopyMemory(byDeskCard,&m_byCards[m_iGetCardPos],sizeof(BYTE)*5);	//获取低牌

	int iCardKind[PLAY_COUNT] = {0};

	BYTE byResultCard[7] = {0};
	for(int i = 0; i < PLAY_COUNT; ++i)
	{
		//此位置玩家没有参与游戏
		if(m_UserData[i].m_iCardsNum <= 0)
		{
			continue;
		}

		iCardKind[i] = m_GamgeLogic.AnalysisCard(m_UserData[i].m_byMyCards,2,byDeskCard,5,byResultCard);
	}


	int iMaxPeople = 255;
	int iTempCardKind = 0;
	for(int i = 0; i < PLAY_COUNT; ++i)
	{
		if(iCardKind[i] > iTempCardKind)
		{
			iTempCardKind = iCardKind[i];
			iMaxPeople = i;
		}
	}

	//最大牌者就是自己 或者 自己的牌型 与 最大者牌型相同 直接返回
	if(bDeskStation == iMaxPeople || iCardKind[bDeskStation] == iCardKind[iMaxPeople])
	{
		return;
	}

	//交换自己 与 最大者的 牌
	BYTE byTempCard[2] = {0};
	::CopyMemory(byTempCard,m_UserData[bDeskStation].m_byMyCards,sizeof(BYTE)*2);
	::CopyMemory(m_UserData[bDeskStation].m_byMyCards,m_UserData[iMaxPeople].m_byMyCards,sizeof(BYTE)*2);
	::CopyMemory(m_UserData[iMaxPeople].m_byMyCards,byTempCard,sizeof(BYTE)*2);
}