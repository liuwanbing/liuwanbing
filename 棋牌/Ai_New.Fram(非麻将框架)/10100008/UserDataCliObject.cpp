
#include "StdAfx.h"
#include "UserDataCliObject.h"

///名称：CUserDataObject
///描述：初始化分数设置，胡牌类型等
///@param 
///@return 
CUserDataCliObject::CUserDataCliObject(void)
{	

}

///名称：析构函数
///描述：~CUserDataObject
///@param 
///@return 
CUserDataCliObject::~CUserDataCliObject(void)
{

}

///名称：Init
///描述：初始化
///@param 
///@return 
void CUserDataCliObject::Init()
{
	//初始自己扑克表
	::memset(m_byMyCards,0,sizeof(m_byMyCards));

	//用户手上扑克数目
	m_iCardsNum = 0;
}


///名称：GetCards
///描述：获得某个玩家手上的所有牌
///@param pCards取得的牌值放于此
///@return 
void CUserDataCliObject::GetCards(BYTE* pCards)
{
	::memcpy(pCards,m_byMyCards,m_iCardsNum);
	return;
}