
#include "StdAfx.h"
#include "UserDataSvrObject.h"

//初始化设置
CUserDataSvrObject::CUserDataSvrObject(void)
{	
	// 记录是否已全部确认代入
	m_bIn = false;
}

//析构函数
CUserDataSvrObject::~CUserDataSvrObject(void)
{

}

//描述：初始化
void CUserDataSvrObject::Init()
{
	HalfInit();

	//玩家是否已断线
	m_bNetCut = false;

	//用户当前可做的动作标记
	//m_byVerbFlag = 0L;

	// 记录是否已全部确认代入
	//m_bIn = false;

}

//半初始化
void CUserDataSvrObject::HalfInit()
{
	//自己的状态
	m_nState = UNKNOWN;

	//初始自己扑克表
	::memset(m_byMyCards,0,sizeof(m_byMyCards));

	//用户手上扑克数目
	m_iCardsNum = 0;

	//当前用户打出的扑克
	::memset(m_byOutCards,0,sizeof(m_byOutCards));

	//当前用户打出的扑克数量
	m_iOutCardsNum = 0;
	
	// 玩家是否发玩牌
	m_bSendCardFinish = false;

}

//获得某个玩家手上的所有牌
//@param pCards取得的牌值放于此
void CUserDataSvrObject::GetHandCards(BYTE* pCards)
{
	::memset(pCards,0,MAX_HANDCARDS_NUM);
	::memcpy(pCards,m_byMyCards,m_iCardsNum);
	return;
}

//获得玩家刚出的牌
//@param pCards取得的牌值放于此
void CUserDataSvrObject::GetOutCards(BYTE* pCards)
{
	::memset(pCards,0,MAX_HANDCARDS_NUM);
	::memcpy(pCards,m_byOutCards,m_iOutCardsNum);
	return;
}

/////名称：SetVerbs
/////描述：设置玩家的动作
/////@param pCards取得的牌值放于此
/////@return
//void CUserDataSvrObject::MakeVerbs(BYTE byFlag)
//{
//	m_byVerbFlag = byFlag;
//	return;
//}