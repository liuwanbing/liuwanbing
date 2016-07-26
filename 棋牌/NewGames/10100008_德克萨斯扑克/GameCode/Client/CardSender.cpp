#include "Stdafx.h"
#include "CardSender.h"

#include "GameMachine.h"
#include "MiniGameEngine.h"


///名称：CCardSender
///描述：构造函数
CCardSender::CCardSender()
{
	//游戏主状态机指针
	m_pRootMachine = NULL;

	//游戏引擎指针
	m_pGameEngine = NULL;

	//初始已显示的牌
	::memset(m_byShowed,0,MAX_HANDCARDS_NUM);

	//发牌进度
	m_iSendingPos = 0;

	//记录当前是发手上的第几张牌
	m_iSendingIndex = 0;

	//发牌玩家队列
	::memset(m_SendCardQueue,255,sizeof(m_SendCardQueue));

}

///名称：~CCardSender
///描述：析构函数
CCardSender::~CCardSender()
{

}


//启动，初始数据用
bool CCardSender::StartUp(CGameMachine* pRootMachine,BYTE byGameStation,void* pGSData)
{
	//TGSSendingCard* pGSSendingCard = (TGSSendingCard*)pGSData;

	//记录游戏主状态机指针
	m_pRootMachine = pRootMachine;

	//记录游戏引擎指针
	m_pGameEngine = pRootMachine->m_pGameEngine;

	//载入玩家发牌队列数据
	LoadSendCardQueue();

	//当前发牌位置设成0
	m_iSendingPos = 0;

	//当前发牌张数为0
	m_iSendingIndex = 0;

// 	if (GS_SENDIND_CARD == byGameStation)
// 	{
// 		//重现发牌过程
// 		OnRecurSending(pGSData);
// 	}

	//通知视类进行绘画工作，由于发牌的过程是逐张发，需要定时控制 \
	  而第一个时间周期结束后，由GameEngine回调子Machine的处理函数OnTimerGap()
	if (m_pGameEngine)
	{
		if (0 == byGameStation)
		{
			m_pGameEngine->SetTimer(IDT_SEND_CARD_GAP,500,NULL);
		}
// 		else if (GS_SENDIND_CARD == byGameStation)
// 		{
// 			m_pGameEngine->OnRecurShowSendingCard(pGSSendingCard);
// 			m_pGameEngine->SetTimer(IDT_SEND_CARD_GAP,500,NULL);
// 		}
	}

	return true;
}

//关闭
bool CCardSender::ShutDown()
{
	//释放发牌机
	if (m_pGameEngine)
	{
		m_pGameEngine->KillTimer(IDT_SEND_CARD_GAP);
	}

	m_pRootMachine = NULL;
	m_pGameEngine = NULL;
	
	return true;
}

//发每张牌的时间间隔已到
bool CCardSender::OnTimerGap()
{
	if (!m_pRootMachine || !m_pGameEngine)
	{
		return false;
	}

	if (m_pRootMachine->m_byMeStation == m_SendCardQueue[m_iSendingPos])
	{
		//m_pGameEngine->SetTimer(IDT_SEND_MOVING_GAP,200,NULL);
	}

	//通知视类显示///////////////
	m_pGameEngine->OnPlayACardAnimate(m_SendCardQueue[m_iSendingPos],m_iSendingIndex);

	//通知音效类播放发牌声音///////////////
	m_pGameEngine->PlayBaseSound(DZPKSound::BT_SEND_BACK_CARD);

	m_iSendingPos++;
	if (m_SendCardQueue[m_iSendingPos] == 255)
	{
		m_iSendingPos = 0;
		m_iSendingIndex++;

		if (m_iSendingIndex == 2)
		{
			m_pGameEngine->KillTimer(IDT_SEND_CARD_GAP);

			//通知通信类进行工作
			m_pGameEngine->SendGameData(MDM_GM_GAME_NOTIFY,ASS_SEND_CARD_FINISH,0);

			return true;
		}
	}

	//通知音效类进行播放工作
	//m_pGameEngine->PlayBaseSound(DZPKSound::BT_SENDACARD);

	return true;
}

//发每张牌的运动时间间隔处理
bool CCardSender::OnTimerMovingGap()
{
	if (!m_pRootMachine || !m_pGameEngine)
	{
		return false;
	}

	//通知视类显示///////////////
	m_pGameEngine->KillTimer(IDT_SEND_MOVING_GAP);

	CString str;
	str.Format("dxy:m_iSendingIndex = %d",m_iSendingIndex);
	OutputDebugString(str);

	if (m_iSendingIndex < 2)
	{
		//m_pGameEngine->OnEndSendMeACard(m_iSendingIndex);
	}
	else
	{
		//m_pGameEngine->OnEndSendMeACard(1);
	}
	
	return true;
}

//获取已显示的牌
void CCardSender::GetShowedCards(BYTE byCard[])
{
	::memcpy(byCard,m_byShowed,m_iSendingPos);
	return;
}

//游戏桌基本状态重现
bool CCardSender::OnRecurSending(void* pGSBase)
{
	//TGSSendingCard* pGSSendingCard = (TGSSendingCard*)pGSBase;

	if (!m_pRootMachine || !m_pGameEngine)
	{
		return false;
	}

// 	if (pGSSendingCard == NULL)
// 	{
// 		return false;
// 	}

	//更新数据////


	//通知视类显示///////////////

	return true;
}

//载入玩家发牌队列数据
bool CCardSender::LoadSendCardQueue()
{
	if (!m_pRootMachine || !m_pGameEngine)
	{
		return false;
	}

	if (m_pRootMachine->m_byNTUser == 255)
	{
		return false;
	}

	int i = 0;
	BYTE byNextUser = 255;

	//从庄家开始
	m_SendCardQueue[i++] = m_pRootMachine->m_byNTUser;
	byNextUser = m_pRootMachine->GetNextUserStation(m_pRootMachine->m_byNTUser);

	while(byNextUser != m_pRootMachine->m_byNTUser)
	{
		m_SendCardQueue[i++] = byNextUser;
		byNextUser = m_pRootMachine->GetNextUserStation(byNextUser);
	}

	return true;
}