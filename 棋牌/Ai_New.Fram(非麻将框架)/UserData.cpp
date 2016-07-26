
#include "Stdafx.h"
#include "UserData.h"

//初始化设置
CUserData::CUserData(void)
{
	//玩家方向
	m_byDirect = MjDirect::Unknown;
}

//析构函数
CUserData::~CUserData(void)
{

}

//描述：初始化
void CUserData::Init()
{
	HalfInit();

	//玩家定方向时打的两骰子点数值
	m_byDirectPoint[0] = 0;
	m_byDirectPoint[1] = 0;

	//玩家是否完成打骰子定方位动画
	m_bAssignedDir = false;

	//玩家方向
	m_byDirect = MjDirect::Unknown;

	//游戏结果，指一局麻将的结果
	m_nResult = MjResult::Unknown;

	//玩家是否已断线
	m_bNetCut = false;

	//玩家信息
	m_pUserData = NULL;

	//玩家拥有的豹子数量
	m_iBaoziNums = 0;

	//每个玩家的分池值
	m_iPoolPoint = 0;

	//玩家的性别
	m_bBoy = false;
}

//半初始化
void CUserData::HalfInit()
{
	//自己的状态
	m_nState = UNKNOWN;

	//玩家是否完成打骰子定起牌方法动画
	m_bFetchMethodOK = false;

	//玩家前方的牌城数据
	::memset(m_byWallCards,255,MJ_MAX_WALLCARD);

	//玩家前方的牌城长度(一定是偶数)
	m_iWallSize = MJ_MAX_WALLCARD;

	//玩家能否抓牌
	m_bCanFetch = true;

	//玩家从牌城中的抓牌顺序，true为顺时针方向，false为逆时针方向
	m_bCWFetch = true;

	//用户的手牌
	::memset(m_byHandCards,0,sizeof(m_byHandCards));

	//用户手牌数量
	m_iHandNums = 0;

	//用户完成补花动画
	m_bBuHuaOK = false;

	//用户的花牌表
	::memset(m_byHuaCards,0,sizeof(m_byHuaCards));

	//用户的花牌数量，即花牌表长度
	m_iHuaNums = 0;

	//抓牌次数
	m_iFetchTimes = 0;

	//出牌锁
	m_bOutCardLock = true;

	//已出牌表
	::memset(m_byOutedCards,0,sizeof(m_byOutedCards));

	//已出的牌数
	m_iOutedNums = 0;

	//玩家的动作信息结构
	m_tagActInfo.Clear();

	//拦牌记录
	for (int i = 0; i < MJ_MAX_CGP; ++i)
	{
		m_lstCGP[i].Clear();
	}

	//玩家是否已听牌
	m_bIsTing = false;

	//拦牌数量
	m_iCGPNums = 0;

	//玩家胡的牌
	m_byHuCard = 255;

	//玩家胡的牌型表
	::memset(m_nHuTypes,TMjActInfo::ht_Unknown,MJ_MAX_HUTYPE);

	//玩家胡的牌型表长度
	m_iHuTypeNums = 0;

	//玩家的特殊牌型(如杠上开花,抢杠胡,杠冲等)
	m_nHuSpecType = TMjActInfo::hst_Unknown;

	//玩家本局的得失点数
	m_iInComePoint = 0;

	//玩家身上剩余的钱
	m_iLeftMoney = 0;

/*-测试数据-*/

	//如果该值有效(不等于255),则抓牌取强制使用此值
	m_byChFetch = 255;
}

//得到玩家定方向的骰子点数和
//@return 返回两骰子点数和
BYTE CUserData::GetDirectPoint()
{
	return (m_byDirectPoint[0] + m_byDirectPoint[1]);
}