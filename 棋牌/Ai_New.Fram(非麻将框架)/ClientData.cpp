
#include "StdAfx.h"
#include "ClientData.h"
#include "GameAlgorithm.h"
#include "UpgradeMessage.h"

///名称：CClientDataObject
///描述：初始化分数设置，胡牌类型等
///@param 
///@return 
CClientData::CClientData()
{	
}

///名称：析构函数
///描述：~CClientDataObject
///@param 
///@return 
CClientData::~CClientData(void)
{

}

///描述：初始化
void CClientData::Init()
{
	HalfInit();

	//清理用户数据
	for(int i = 0; i < PLAY_COUNT; i++)
	{
		m_UserData[i].Init();
	}

	//庄家
	m_byNTUser = 255;

}

///描述：半初始化，用于回合之间数据完全的部分清理，因为有的数据需要保留。
void CClientData::HalfInit()
{
	for(int i = 0; i < PLAY_COUNT; i++)
	{
		//清理用户数据
		m_UserData[i].HalfInit();

		//更新玩家的剩余金币
	}

	//起牌打骰子的点数值
	m_byFetchDice[0] = 0;
	m_byFetchDice[1] = 0;

	//在这个玩家门起开始起牌
	m_byFetchUser = 255;

	//起牌的墩数
	m_iFetchPos = 0;

	//当前活动的用户
	m_byTokenUser = 255;

}

///为某个玩家手上的牌排序
void CClientData::SortCardsTo(BYTE bDeskStation)
{
	//GAlg::SortCard(m_UserData[bDeskStation].m_byMyCards,NULL,m_UserData[bDeskStation].m_iCardsNum,&m_tagDeskCfg.Rule);

	return;
}

/*----------------------------------------------------------------------------------------------------------------
	一系列游戏客户端更新的接口，每个游戏都不同
---------------------------------------------------------------------------------------------------------------*/

//服务器自主配置桌子后数据更新
void CClientData::SaveSysDeskCfg(const LPCVOID lpData)
{
	const TDeskCfg* pDeskCfg = (TDeskCfg*)lpData;

	::memcpy(&m_tagDeskCfg,pDeskCfg,sizeof(TDeskCfg));
}

//服务器送来分配玩家坐位,数据更新
void CClientData::SaveAssignDirectInfo(const LPCVOID lpData)
{
	const TAssignDirect* pAssignDir = (TAssignDirect*)lpData;

	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		//得到i的坐向
		m_UserData[i].m_byDirect = pAssignDir->direct[i];

		//初始各玩家的分池值
		m_UserData[i].m_iPoolPoint = m_tagDeskCfg.dz.iPool;
	}
}

//来自服务器定庄家的消息，更新数据
void CClientData::SaveNTInfo(const LPCVOID lpData)
{
	const TMakeNTInfo* pNTInfo = (TMakeNTInfo*)lpData;

	//获得庄家位置
	m_byNTUser = pNTInfo->byUser;

	return;
}

//服务器发来起牌的方法，更新数据
void CClientData::SaveFetchMothod(const LPCVOID lpData)
{
	const TFetchMethod* pFetchMothod = (TFetchMethod*)lpData;

	//获得骰子值
	m_byFetchDice[0] = pFetchMothod->byDice[0];
	m_byFetchDice[1] = pFetchMothod->byDice[1];

	//获得起牌方向
	m_byFetchUser = pFetchMothod->byUser;

	//获得起牌的墩数
	m_iFetchPos = pFetchMothod->iPos;

	return;
}

//来自服务器的发手牌数据，进行更新
void CClientData::SaveHandCards(const LPCVOID lpData, BYTE nDeskStation)
{
	const TFetchHands* pFetchHands = (TFetchHands*)lpData;

	int iDeskStation = pFetchHands->iDeskStation;

	CString str;
	str.Format("wzmj:    SaveHandCards--->iCardNum -> %d", pFetchHands->iCardNum);
	OutputDebugString(str);

	//更新牌自己的手牌牌数据与数量
	::memcpy(m_UserData[iDeskStation].m_byHandCards,pFetchHands->byCards,pFetchHands->iCardNum);
	m_UserData[iDeskStation].m_iHandNums = pFetchHands->iCardNum;

	CUserData *p = &m_UserData[iDeskStation];
	for (int i = 0; i < p->m_iHandNums; i ++) {
		str.Format("wzmj:   SaveHandCards-> card->%d  fetchcard->%d i->%d   iDeskStation->%d", p->m_byHandCards[i], pFetchHands->byCards[i], i, iDeskStation);
		OutputDebugString(str);
	}

	return;
}

//来自服务器的跳牌数据，进行更新
void CClientData::SaveTiaoCards(const LPCVOID lpData, BYTE nDeskStation)
{
	const TTiaoCards* pTiaoCards = (TTiaoCards*)lpData;

	BYTE nMeStation = nDeskStation;

	CUserData* pUser = NULL;

	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		pUser = &m_UserData[i];
		
		if (pTiaoCards->tagFetch[i].val != 255)
		{
			//添加第一张跳牌
			pUser->m_byHandCards[pUser->m_iHandNums] = pTiaoCards->tagFetch[i].val;

			//更新手牌的数量
			pUser->m_iHandNums++;
		}
	}

	//庄家，则添加第二张手牌
	pUser = &m_UserData[m_byNTUser];

	//添加第一张跳牌
	pUser->m_byHandCards[pUser->m_iHandNums] = pTiaoCards->tagFetch[2].val;

	//更新手牌的数量
	pUser->m_iHandNums++;

	return;
}

//收服务器的为各玩家的补花数据，进行更新
void CClientData::SaveBuHuas(const LPCVOID lpData, BYTE nDeskStation)
{
	const TBuHuas* pBuHuas = (TBuHuas*)lpData;

	BYTE nMeStation = nDeskStation;

	CUserData* pUser = NULL;

	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		if (pBuHuas->iNum[i] > 0) //玩家i有花
		{
			pUser = &m_UserData[i];

			//更新玩家i的花牌表
			::memcpy(pUser->m_byHuaCards,pBuHuas->byHuaList[i],pBuHuas->iNum[i]);

			//更新玩家i的花牌数量
			pUser->m_iHuaNums = pBuHuas->iNum[i];
		}
	}

	pUser = &m_UserData[nMeStation];

	//如果自己有花,需要更新自己的手牌数据，
	//其他玩家没有手数据，没有更新意义
	if (pBuHuas->iNum[nMeStation] > 0)
	{
		//获取补牌，有花牌的牌去掉
		for (int i = 0; i < pBuHuas->iNum[nMeStation]; ++i)
		{
			for (int j = 0; j < pUser->m_iHandNums; ++j)
			{
				if (pUser->m_byHandCards[j] == pBuHuas->byHuaList[nMeStation][i])
				{
					pUser->m_byHandCards[j] = pBuHuas->byBuList[i];
					break;
				}
			}
		}
		
	}

	//排序手牌数据
	CGameAlgorithm::sort(pUser->m_byHandCards,pUser->m_iHandNums,false);

	return;
}

//服务器下发令牌，更新数据
void CClientData::SaveTokenInfo(const LPCVOID lpData)
{
	const TToken* pToken = (TToken*)lpData;

	CUserData* pUser = &m_UserData[pToken->byUser];

	//记录当前得令牌的玩家
	m_byTokenUser = pToken->byUser;

	if (pToken->bFetch && pToken->Fetcher.val != 0) //如果有抓牌
	{
		pUser->m_byHandCards[pUser->m_iHandNums] = pToken->Fetcher.val;
		pUser->m_iHandNums ++;
	}

	return;
}

//某玩家出牌了,更新数据
void CClientData::SaveOutCardInfo(const LPCVOID lpData, BYTE nDeskStation)
{
	const TOutCard* pOCInfo = (TOutCard*)lpData;

	CUserData* pUser = &m_UserData[pOCInfo->byUser];

	if (pOCInfo->byUser == nDeskStation)
	{

		CString str;
		str.Format("XHMJClient: server hands = ");
		for (int i = 0; i < pOCInfo->iHandNums; ++i)
		{
			str.Format("%s %02d",str,pOCInfo->byCards[i]);
		}
		OutputDebugString(str);
		
		str.Format("XHMJClient: before hands = ");
		for (int i = 0; i < pUser->m_iHandNums; ++i)
		{
			str.Format("%s %02d",str,pUser->m_byHandCards[i]);
		}
		OutputDebugString(str);

		//从自己的手牌数据中删除出掉的牌
		if (CGameAlgorithm::RemoveOneCard(pOCInfo->byCard,pUser->m_byHandCards,pUser->m_iHandNums))
		{
			pUser->m_iHandNums--;

			//重新排序牌数据
			CGameAlgorithm::sort(pUser->m_byHandCards,pUser->m_iHandNums,false);

			str.Format("XHMJClient:  after hands = ");
			for (int i = 0; i < pUser->m_iHandNums; ++i)
			{
				str.Format("%s %02d",str,pUser->m_byHandCards[i]);
			}
			OutputDebugString(str);

			for (int i = 0; i < pOCInfo->iHandNums; ++i)
			{
				if (pUser->m_byHandCards[i] != pOCInfo->byCards[i])
				{
					OutputDebugString("XHMJClient:手牌数据与服务器不同步！***********************************************************");
					break;
				}
			}
		}
	}
	else
	{
		//由于其他玩家没有实在的数据直接更新数量即可
		pUser->m_iHandNums--;
	}

	//更新pUser的出牌表的数据
	pUser->m_byOutedCards[pUser->m_iOutedNums++] = pOCInfo->byCard;

	return;
}

//我有动作，更新动作
void CClientData::SaveActNotify(const LPCVOID lpData, BYTE nDeskStation)
{
	const TActNotify* pActNotify = (TActNotify*)lpData;

	BYTE nMeStation = nDeskStation;

	//更新自己的动作信息
	if (pActNotify->info.usFlags > 0)
	{
		::memcpy(&m_UserData[nMeStation].m_tagActInfo,&pActNotify->info,sizeof(MjBase::TMjActInfo));
	}
	else
	{
		//空动作时清空自己的动作信息
		m_UserData[nMeStation].m_tagActInfo.Clear();
	}

	return;
}

//有玩家执行动作，更新数据
void CClientData::SaveActInfo(const LPCVOID lpData, BYTE nDeskStation)
{
	const TActInfo* pActInfo = (TActInfo*)lpData;

	CUserData* pUser = NULL;

	//有拦牌的出现，要更新拦牌及手牌数据
	if (TMjCGPNode::UNKNOWN != pActInfo->CGPNode.nType)
	{
		//得到动作发出者的数据
		pUser = &m_UserData[pActInfo->byUsers[0]];

		//除补杠动作，新动作是添加到栏牌表\
		  补杠动作是修改栏牌表
		if (TMjCGPNode::BuGang != pActInfo->CGPNode.nType)
		{
			pUser->m_lstCGP[pUser->m_iCGPNums++] = pActInfo->CGPNode;
		}
		else
		{
			pUser->m_lstCGP[pActInfo->CGPNode.nIdx] = pActInfo->CGPNode;
		}
		
		//更新手牌数量
		pUser->m_iHandNums = pActInfo->iHandNums;

		//更新手牌数据,只更新自己的,其他玩家是没有数据的，没更新必要
		if (nDeskStation == pActInfo->byUsers[0])
		{
			::memcpy(pUser->m_byHandCards,pActInfo->byHands,pActInfo->iHandNums);
		}

		//碰和杠动作者需要扣除出牌玩家出牌表一张牌
		if (TMjCGPNode::Peng == pActInfo->CGPNode.nType 
				|| TMjCGPNode::MingGang == pActInfo->CGPNode.nType
				|| TMjCGPNode::Chi == pActInfo->CGPNode.nType)
		{
			//得到最后出牌者的数据
			pUser = &m_UserData[pActInfo->CGPNode.from];

			pUser->m_iOutedNums--;
		}
	}

	//有听牌出现,更新手牌数据与出牌表
	if(MjActFlag::usTing == pActInfo->usActFlag)
	{
		;
	}

	//有胡牌出现,更新手牌数据
	if (MjActFlag::usHu == pActInfo->usActFlag)
	{
		BYTE nMeStation =nDeskStation;

		//得到自己的数据
		pUser = &m_UserData[nMeStation];

		//更新手牌数据
		::memcpy(pUser->m_byHandCards,pActInfo->byHands,pActInfo->iHandNums);

		//更新手牌数量
		pUser->m_iHandNums = pActInfo->iHandNums;
	}

	return;
}

//有玩家暗杠，要即时更新分数，更新数据
void CClientData::SaveAnGangResult(const LPCVOID lpData)
{
	const TAnGangResult* pAnGangResult = (TAnGangResult*)lpData;

	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		CUserData* pUser = &m_UserData[i];

		//修改分池值
		pUser->m_iPoolPoint = pAnGangResult->iPools[i];

		//修改玩家剩余的金币
		pUser->m_iLeftMoney = pAnGangResult->iLeftMoney[i];
	}

	return;
}

//收到回合结束消息动作，更新数据，这时各玩家之前保密的数据已发下来
void CClientData::SaveRoundFinish(const LPCVOID lpData)
{
	const TRoundFinish* pRoundFinish = (TRoundFinish*)lpData;

	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		CUserData* pUser = &m_UserData[i];

		//更新玩家的手牌
		::memcpy(pUser->m_byHandCards,pRoundFinish->byHandCards[i],pRoundFinish->iHandNums[i]);

		//得到手牌数量
		pUser->m_iHandNums = pRoundFinish->iHandNums[i];

		//得到结果
		pUser->m_nResult = pRoundFinish->nResult[i];

		//得到玩家胡的牌
		pUser->m_byHuCard = pRoundFinish->byHuCard[i];
	}

	return;
}

//收到结算消息后，更新数据
void CClientData::SaveRoundResult(const LPCVOID lpData)
{
	const TResult* pResult = (TResult*)lpData;

	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		CUserData* pUser = &m_UserData[i];

		//更新玩家剩余的金币
		pUser->m_iLeftMoney = pResult->iLeftMoney[i];

		//更新分池
		pUser->m_iPoolPoint = pResult->iPoolPoint[i];
	
	}

	return;
}

//恢复基本的游戏数据
void CClientData::RecurSaveBase(const LPCVOID lpData, BYTE nDeskStation)
{
	const TGSBase* pGSBase = (TGSBase*)lpData;

	BYTE nMeStation = nDeskStation;

	//还原桌面设置数据
	::memcpy(&m_tagDeskCfg,&pGSBase->tagDeskCfg,sizeof(TDeskCfg));

	//还原庄家位置数据
	m_byNTUser = pGSBase->byNTUser;

	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		//还原各玩家的坐位方向数据
		m_UserData[i].m_byDirect = pGSBase->direct[i];

		//还原获取各玩家的分池值数据
		m_UserData[i].m_iPoolPoint = pGSBase->iPoolPoint[i];

		//还原获取各玩家手上剩余的钱数据
		m_UserData[i].m_iLeftMoney = pGSBase->iLeftMoney[i];

		//还原各玩家手上的牌数量
		m_UserData[i].m_iHandNums = pGSBase->iHandNums[i];

		//还原各玩家花牌表数据，用其长度数据
		::memcpy(m_UserData[i].m_byHuaCards,pGSBase->byHuaCards[i],pGSBase->iHuaNums[i]);
		m_UserData[i].m_iHuaNums = pGSBase->iHuaNums[i];

	}

	//还原自己的牌数据
	::memcpy(m_UserData[nMeStation].m_byHandCards,pGSBase->byHandCards,\
		pGSBase->iHandNums[nMeStation]);

	return;
}

//恢复打牌时的游戏数据
void CClientData::RecurSavePlaying(const LPCVOID lpData, BYTE nDeskStation)
{
	const TGSPlaying* pPlaying = (TGSPlaying*)lpData;

	BYTE nMeStation = nDeskStation;

	//还原令牌得者数据
	m_byTokenUser = pPlaying->byTokenUser;

	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		//还原出牌列表数据,及其长度
		::memcpy(m_UserData[i].m_byOutedCards,pPlaying->byOutedCards[i],pPlaying->iOutedNums[i]);
		m_UserData[i].m_iOutedNums = pPlaying->iOutedNums[i];

		//还原拦牌表，及其长度
		::memcpy(m_UserData[i].m_lstCGP,pPlaying->CGPList[i],sizeof(MjBase::TMjCGPNode) * pPlaying->iCGPNums[i]);
		m_UserData[i].m_iCGPNums = pPlaying->iCGPNums[i];
	}

	//更新自己的动作信息
	if (pPlaying->ActInfo.usFlags > 0)
	{
		::memcpy(&m_UserData[nMeStation].m_tagActInfo,&pPlaying->ActInfo,sizeof(MjBase::TMjActInfo));
	}

	return;
}

/*----------------------------------------------------------------------------------------------------------------
	温州二人麻将 特有规则定义
------------------------------------------------------------------------------------------------------------------*/

bool CClientData::WzFengZiLocked(int nDeskStation, int iCaiShen)
{
	BYTE byHandCard[MJ_MAX_HANDCARD];
	memset(byHandCard, 255, sizeof(byHandCard));
	
	memcpy(byHandCard, m_UserData[nDeskStation].m_byHandCards, sizeof(m_UserData[nDeskStation].m_byHandCards));

	int FengZiGrid[7];
	memset(FengZiGrid, 0, sizeof(FengZiGrid));
	int iCardNumber = m_UserData[nDeskStation].m_iHandNums;
	for (int i = 0; i < iCardNumber; i ++)
	{
		if (byHandCard[i] == 31 && iCaiShen != 31)
		{
			FengZiGrid[0] ++;
		}
		else if (byHandCard[i] == 32 && iCaiShen != 32)
		{
			FengZiGrid[1] ++;
		}
		else if (byHandCard[i] == 33 && iCaiShen != 33)
		{
			FengZiGrid[2] ++;
		}
		else if (byHandCard[i] == 34 && iCaiShen != 34)
		{
			FengZiGrid[3] ++;
		}
		else if (byHandCard[i] == 35 && iCaiShen != 35)
		{
			FengZiGrid[4] ++;
		}
		else if (byHandCard[i] == 36 && iCaiShen != 36)
		{
			FengZiGrid[5] ++;
		}
		else if (byHandCard[i] == 37)
		{
			if (iCaiShen >= 31 && iCaiShen < 37)
				FengZiGrid[6] ++;
		}
	}

	if (FengZiGrid[0] == 1 || FengZiGrid[1] == 1 || FengZiGrid[2] == 1 ||
		FengZiGrid[3] == 1 || FengZiGrid[4] == 1 || FengZiGrid[5] == 1 || FengZiGrid[6] == 1)
	{
		return true;
	}

	return false;
}

bool CClientData::WzFengZiInside(int nDeskStation, int iCaiShen, int iFengZi)
{
	BYTE byHandCard[MJ_MAX_HANDCARD];
	memset(byHandCard, 255, sizeof(byHandCard));

	memcpy(byHandCard, m_UserData[nDeskStation].m_byHandCards, sizeof(m_UserData[nDeskStation].m_byHandCards));
	
	//去掉里面的财神 且 把白板换为财神
	int iCardNumber = m_UserData[nDeskStation].m_iHandNums;
	for (int i = 0; i < iCardNumber; i ++)
	{
		if (byHandCard[i] == (BYTE)iCaiShen)
			byHandCard[i] = 255;
		if (byHandCard[i] == 37)
			byHandCard[i] = (BYTE)iCaiShen;
	}

	CGameAlgorithm::sort(byHandCard, iCardNumber, false);
	for (int i = 0; i < iCardNumber; i ++)
	{
		if ((BYTE)iFengZi == byHandCard[i])
			return true;
	}

	return false;
}

bool CClientData::isWzFengZi(int iCard)
{
	if (iCard>=31 && iCard<=37)
		return true;
	return false;
}

bool CClientData::isWzFengZiEx(int iCard)
{
	if (iCard>=31 && iCard<=36)
		return true;
	return false;
}