#ifndef GAME_PROPERTY_ITEM_HEAD_FILE
#define GAME_PROPERTY_ITEM_HEAD_FILE

#pragma once

#include "GamePropertyHead.h"

//////////////////////////////////////////////////////////////////////////////////

//道具子项
class GAME_PROPERTY_CLASS CGamePropertyItem
{
	//友元定义
	friend class CGamePropertyManager;

	//变量定义
protected:
	tagPropertyInfo					m_PropertyInfo;						//道具信息
	tagPropertyAttrib				m_PropertyAttrib;					//道具属性

	//函数定义
public:
	//构造函数
	CGamePropertyItem();
	//析构函数
	virtual ~CGamePropertyItem();

	//重载函数
public:
	//图片信息
	virtual VOID GetImageInfo(tagPropertyImage & PropertyImage)=NULL;
	//提示信息
	virtual LPCTSTR GetNotifyMessage(LPCTSTR szSourceNickName,LPCTSTR szTargerNickName,WORD wItemCount,LPTSTR szResultMessage);

	//功能函数
public:
	//获取信息
	tagPropertyInfo * GetPropertyInfo() { return &m_PropertyInfo; }
	//获取属性
	tagPropertyAttrib * GetPropertyAttrib() { return & m_PropertyAttrib; }

	//功能函数
public:
	//使用道具
	VOID ShowBuyPropertyWnd(IGamePropertySink * pIGamePropertySink);
	//命令时间
	VOID OnEventPerformCommand(WORD wCommandID,VOID * pCmdData);
};

//////////////////////////////////////////////////////////////////////////////////
//礼物部分

//汽车礼物
class GAME_PROPERTY_CLASS CGamePropertyCar : public CGamePropertyItem
{
	//函数定义
public:
	//构造函数
	CGamePropertyCar();
	//析构函数
	virtual ~CGamePropertyCar();

	//重载函数
public:
	//图片信息
	virtual VOID GetImageInfo(tagPropertyImage & PropertyImage);
	//提示信息
	virtual LPCTSTR GetNotifyMessage(LPCTSTR szSourceNickName,LPCTSTR szTargerNickName,WORD wItemCount,LPTSTR szResultMessage);
};

//////////////////////////////////////////////////////////////////////////////////

//臭蛋礼物
class GAME_PROPERTY_CLASS CGamePropertyEgg : public CGamePropertyItem
{
	//函数定义
public:
	//构造函数
	CGamePropertyEgg();
	//析构函数
	virtual ~CGamePropertyEgg();

	//重载函数
public:
	//图片信息
	virtual VOID GetImageInfo(tagPropertyImage & PropertyImage);
	//提示信息
	virtual LPCTSTR GetNotifyMessage(LPCTSTR szSourceNickName,LPCTSTR szTargerNickName,WORD wItemCount,LPTSTR szResultMessage);
};

//////////////////////////////////////////////////////////////////////////////////

//鼓掌礼物
class GAME_PROPERTY_CLASS CGamePropertyClap : public CGamePropertyItem
{
	//函数定义
public:
	//构造函数
	CGamePropertyClap();
	//析构函数
	virtual ~CGamePropertyClap();

	//重载函数
public:
	//图片信息
	virtual VOID GetImageInfo(tagPropertyImage & PropertyImage);
	//提示信息
	virtual LPCTSTR GetNotifyMessage(LPCTSTR szSourceNickName,LPCTSTR szTargerNickName,WORD wItemCount,LPTSTR szResultMessage);
};

//////////////////////////////////////////////////////////////////////////////////

//香吻礼物
class GAME_PROPERTY_CLASS CGamePropertyKiss : public CGamePropertyItem
{
	//函数定义
public:
	//构造函数
	CGamePropertyKiss();
	//析构函数
	virtual ~CGamePropertyKiss();

	//重载函数
public:
	//图片信息
	virtual VOID GetImageInfo(tagPropertyImage & PropertyImage);
	//提示信息
	virtual LPCTSTR GetNotifyMessage(LPCTSTR szSourceNickName,LPCTSTR szTargerNickName,WORD wItemCount,LPTSTR szResultMessage);
};

//////////////////////////////////////////////////////////////////////////////////

//啤酒礼物
class GAME_PROPERTY_CLASS CGamePropertyBeer : public CGamePropertyItem
{
	//函数定义
public:
	//构造函数
	CGamePropertyBeer();
	//析构函数
	virtual ~CGamePropertyBeer();

	//重载函数
public:
	//图片信息
	virtual VOID GetImageInfo(tagPropertyImage & PropertyImage);
	//提示信息
	virtual LPCTSTR GetNotifyMessage(LPCTSTR szSourceNickName,LPCTSTR szTargerNickName,WORD wItemCount,LPTSTR szResultMessage);
};

//////////////////////////////////////////////////////////////////////////////////

//蛋糕礼物
class GAME_PROPERTY_CLASS CGamePropertyCake : public CGamePropertyItem
{
	//函数定义
public:
	//构造函数
	CGamePropertyCake();
	//析构函数
	virtual ~CGamePropertyCake();

	//重载函数
public:
	//图片信息
	virtual VOID GetImageInfo(tagPropertyImage & PropertyImage);
	//提示信息
	virtual LPCTSTR GetNotifyMessage(LPCTSTR szSourceNickName,LPCTSTR szTargerNickName,WORD wItemCount,LPTSTR szResultMessage);
};

//////////////////////////////////////////////////////////////////////////////////

//钻戒礼物
class GAME_PROPERTY_CLASS CGamePropertyRing : public CGamePropertyItem
{
	//函数定义
public:
	//构造函数
	CGamePropertyRing();
	//析构函数
	virtual ~CGamePropertyRing();

	//重载函数
public:
	//图片信息
	virtual VOID GetImageInfo(tagPropertyImage & PropertyImage);
	//提示信息
	virtual LPCTSTR GetNotifyMessage(LPCTSTR szSourceNickName,LPCTSTR szTargerNickName,WORD wItemCount,LPTSTR szResultMessage);
};

//////////////////////////////////////////////////////////////////////////////////

//暴打礼物
class GAME_PROPERTY_CLASS CGamePropertyBeat : public CGamePropertyItem
{
	//函数定义
public:
	//构造函数
	CGamePropertyBeat();
	//析构函数
	virtual ~CGamePropertyBeat();

	//重载函数
public:
	//图片信息
	virtual VOID GetImageInfo(tagPropertyImage & PropertyImage);
	//提示信息
	virtual LPCTSTR GetNotifyMessage(LPCTSTR szSourceNickName,LPCTSTR szTargerNickName,WORD wItemCount,LPTSTR szResultMessage);
};

//////////////////////////////////////////////////////////////////////////////////

//炸弹礼物
class GAME_PROPERTY_CLASS CGamePropertyBomb : public CGamePropertyItem
{
	//函数定义
public:
	//构造函数
	CGamePropertyBomb();
	//析构函数
	virtual ~CGamePropertyBomb();

	//重载函数
public:
	//图片信息
	virtual VOID GetImageInfo(tagPropertyImage & PropertyImage);
	//提示信息
	virtual LPCTSTR GetNotifyMessage(LPCTSTR szSourceNickName,LPCTSTR szTargerNickName,WORD wItemCount,LPTSTR szResultMessage);
};

//////////////////////////////////////////////////////////////////////////////////

//香烟礼物
class GAME_PROPERTY_CLASS CGamePropertySmoke : public CGamePropertyItem
{
	//函数定义
public:
	//构造函数
	CGamePropertySmoke();
	//析构函数
	virtual ~CGamePropertySmoke();

	//重载函数
public:
	//图片信息
	virtual VOID GetImageInfo(tagPropertyImage & PropertyImage);
	//提示信息
	virtual LPCTSTR GetNotifyMessage(LPCTSTR szSourceNickName,LPCTSTR szTargerNickName,WORD wItemCount,LPTSTR szResultMessage);
};

//////////////////////////////////////////////////////////////////////////////////

//别墅礼物
class GAME_PROPERTY_CLASS CGamePropertyVilla : public CGamePropertyItem
{
	//函数定义
public:
	//构造函数
	CGamePropertyVilla();
	//析构函数
	virtual ~CGamePropertyVilla();

	//重载函数
public:
	//图片信息
	virtual VOID GetImageInfo(tagPropertyImage & PropertyImage);
	//提示信息
	virtual LPCTSTR GetNotifyMessage(LPCTSTR szSourceNickName,LPCTSTR szTargerNickName,WORD wItemCount,LPTSTR szResultMessage);
};

//////////////////////////////////////////////////////////////////////////////////

//砖头礼物
class GAME_PROPERTY_CLASS CGamePropertyBrick : public CGamePropertyItem
{
	//函数定义
public:
	//构造函数
	CGamePropertyBrick();
	//析构函数
	virtual ~CGamePropertyBrick();

	//重载函数
public:
	//图片信息
	virtual VOID GetImageInfo(tagPropertyImage & PropertyImage);
	//提示信息
	virtual LPCTSTR GetNotifyMessage(LPCTSTR szSourceNickName,LPCTSTR szTargerNickName,WORD wItemCount,LPTSTR szResultMessage);
};

//////////////////////////////////////////////////////////////////////////////////

//鲜花礼物
class GAME_PROPERTY_CLASS CGamePropertyFlower : public CGamePropertyItem
{
	//函数定义
public:
	//构造函数
	CGamePropertyFlower();
	//析构函数
	virtual ~CGamePropertyFlower();

	//重载函数
public:
	//图片信息
	virtual VOID GetImageInfo(tagPropertyImage & PropertyImage);
	//提示信息
	virtual LPCTSTR GetNotifyMessage(LPCTSTR szSourceNickName,LPCTSTR szTargerNickName,WORD wItemCount,LPTSTR szResultMessage);
};

//////////////////////////////////////////////////////////////////////////////////
//道具部分

//2倍积分
class GAME_PROPERTY_CLASS CGamePropertyTwoScoreCard : public CGamePropertyItem
{
	//函数定义
public:
	//构造函数
	CGamePropertyTwoScoreCard();
	//析构函数
	virtual ~CGamePropertyTwoScoreCard();

	//重载函数
public:
	//图片信息
	virtual VOID GetImageInfo(tagPropertyImage & PropertyImage);
	//提示信息
	virtual LPCTSTR GetNotifyMessage(LPCTSTR szSourceNickName,LPCTSTR szTargerNickName,WORD wItemCount,LPTSTR szResultMessage);
};

//////////////////////////////////////////////////////////////////////////////////

//4倍积分
class GAME_PROPERTY_CLASS CGamePropertyFourScoreCard : public CGamePropertyItem
{
	//函数定义
public:
	//构造函数
	CGamePropertyFourScoreCard();
	//析构函数
	virtual ~CGamePropertyFourScoreCard();

	//重载函数
public:
	//图片信息
	virtual VOID GetImageInfo(tagPropertyImage & PropertyImage);
	//提示信息
	virtual LPCTSTR GetNotifyMessage(LPCTSTR szSourceNickName,LPCTSTR szTargerNickName,WORD wItemCount,LPTSTR szResultMessage);
};

//////////////////////////////////////////////////////////////////////////////////

//负分清零
class GAME_PROPERTY_CLASS CGamePropertyScoreClear : public CGamePropertyItem
{
	//函数定义
public:
	//构造函数
	CGamePropertyScoreClear();
	//析构函数
	virtual ~CGamePropertyScoreClear();

	//重载函数
public:
	//图片信息
	virtual VOID GetImageInfo(tagPropertyImage & PropertyImage);
	//提示信息
	virtual LPCTSTR GetNotifyMessage(LPCTSTR szSourceNickName,LPCTSTR szTargerNickName,WORD wItemCount,LPTSTR szResultMessage);
};

//////////////////////////////////////////////////////////////////////////////////

//逃跑清零
class GAME_PROPERTY_CLASS CGamePropertyEscapeClear : public CGamePropertyItem
{
	//函数定义
public:
	//构造函数
	CGamePropertyEscapeClear();
	//析构函数
	virtual ~CGamePropertyEscapeClear();

	//重载函数
public:
	//图片信息
	virtual VOID GetImageInfo(tagPropertyImage & PropertyImage);
	//提示信息
	virtual LPCTSTR GetNotifyMessage(LPCTSTR szSourceNickName,LPCTSTR szTargerNickName,WORD wItemCount,LPTSTR szResultMessage);
};

//////////////////////////////////////////////////////////////////////////////////

//小喇叭
class GAME_PROPERTY_CLASS CGamePropertyTrumpet  : public CGamePropertyItem
{
	//函数定义
public:
	//构造函数
	CGamePropertyTrumpet();
	//析构函数
	virtual ~CGamePropertyTrumpet();

	//重载函数
public:
	//图片信息
	virtual VOID GetImageInfo(tagPropertyImage & PropertyImage);
	//提示信息
	virtual LPCTSTR GetNotifyMessage(LPCTSTR szSourceNickName,LPCTSTR szTargerNickName,WORD wItemCount,LPTSTR szResultMessage);
};

//////////////////////////////////////////////////////////////////////////////////

//大喇叭
class GAME_PROPERTY_CLASS CGamePropertyTyphon  : public CGamePropertyItem
{
	//函数定义
public:
	//构造函数
	CGamePropertyTyphon();
	//析构函数
	virtual ~CGamePropertyTyphon();

	//重载函数
public:
	//图片信息
	virtual VOID GetImageInfo(tagPropertyImage & PropertyImage);
	//提示信息
	virtual LPCTSTR GetNotifyMessage(LPCTSTR szSourceNickName,LPCTSTR szTargerNickName,WORD wItemCount,LPTSTR szResultMessage);
};

//////////////////////////////////////////////////////////////////////////////////

//防踢卡
class GAME_PROPERTY_CLASS CGamePropertyGuardKickCard  : public CGamePropertyItem
{
	//函数定义
public:
	//构造函数
	CGamePropertyGuardKickCard();
	//析构函数
	virtual ~CGamePropertyGuardKickCard();

	//重载函数
public:
	//图片信息
	virtual VOID GetImageInfo(tagPropertyImage & PropertyImage);
	//提示信息
	virtual LPCTSTR GetNotifyMessage(LPCTSTR szSourceNickName,LPCTSTR szTargerNickName,WORD wItemCount,LPTSTR szResultMessage);
};

//////////////////////////////////////////////////////////////////////////////////

//护身符
class GAME_PROPERTY_CLASS CGamePropertyPossess  : public CGamePropertyItem
{
	//函数定义
public:
	//构造函数
	CGamePropertyPossess();
	//析构函数
	virtual ~CGamePropertyPossess();

	//重载函数
public:
	//图片信息
	virtual VOID GetImageInfo(tagPropertyImage & PropertyImage);
	//提示信息
	virtual LPCTSTR GetNotifyMessage(LPCTSTR szSourceNickName,LPCTSTR szTargerNickName,WORD wItemCount,LPTSTR szResultMessage);
};


//////////////////////////////////////////////////////////////////////////////////

//蓝钻卡
class GAME_PROPERTY_CLASS CGamePropertyBlueRingCard  : public CGamePropertyItem
{
	//函数定义
public:
	//构造函数
	CGamePropertyBlueRingCard();
	//析构函数
	virtual ~CGamePropertyBlueRingCard();

	//重载函数
public:
	//图片信息
	virtual VOID GetImageInfo(tagPropertyImage & PropertyImage);
	//提示信息
	virtual LPCTSTR GetNotifyMessage(LPCTSTR szSourceNickName,LPCTSTR szTargerNickName,WORD wItemCount,LPTSTR szResultMessage);
};

//////////////////////////////////////////////////////////////////////////////////

//黄钻卡
class GAME_PROPERTY_CLASS CGamePropertyYellowRingCard  : public CGamePropertyItem
{
	//函数定义
public:
	//构造函数
	CGamePropertyYellowRingCard();
	//析构函数
	virtual ~CGamePropertyYellowRingCard();

	//重载函数
public:
	//图片信息
	virtual VOID GetImageInfo(tagPropertyImage & PropertyImage);
	//提示信息
	virtual LPCTSTR GetNotifyMessage(LPCTSTR szSourceNickName,LPCTSTR szTargerNickName,WORD wItemCount,LPTSTR szResultMessage);
};

//////////////////////////////////////////////////////////////////////////////////

//白钻卡
class GAME_PROPERTY_CLASS CGamePropertyWhiteRingCard  : public CGamePropertyItem
{
	//函数定义
public:
	//构造函数
	CGamePropertyWhiteRingCard();
	//析构函数
	virtual ~CGamePropertyWhiteRingCard();

	//重载函数
public:
	//图片信息
	virtual VOID GetImageInfo(tagPropertyImage & PropertyImage);
	//提示信息
	virtual LPCTSTR GetNotifyMessage(LPCTSTR szSourceNickName,LPCTSTR szTargerNickName,WORD wItemCount,LPTSTR szResultMessage);
};

//////////////////////////////////////////////////////////////////////////////////

//红钻卡
class GAME_PROPERTY_CLASS CGamePropertyRedRingCard  : public CGamePropertyItem
{
	//函数定义
public:
	//构造函数
	CGamePropertyRedRingCard();
	//析构函数
	virtual ~CGamePropertyRedRingCard();

	//重载函数
public:
	//图片信息
	virtual VOID GetImageInfo(tagPropertyImage & PropertyImage);
	//提示信息
	virtual LPCTSTR GetNotifyMessage(LPCTSTR szSourceNickName,LPCTSTR szTargerNickName,WORD wItemCount,LPTSTR szResultMessage);
};


//////////////////////////////////////////////////////////////////////////////////

//Vip房卡
class GAME_PROPERTY_CLASS CGamePropertyVipRoomCard  : public CGamePropertyItem
{
	//函数定义
public:
	//构造函数
	CGamePropertyVipRoomCard();
	//析构函数
	virtual ~CGamePropertyVipRoomCard();

	//重载函数
public:
	//图片信息
	virtual VOID GetImageInfo(tagPropertyImage & PropertyImage);
	//提示信息
	virtual LPCTSTR GetNotifyMessage(LPCTSTR szSourceNickName,LPCTSTR szTargerNickName,WORD wItemCount,LPTSTR szResultMessage);
};

//////////////////////////////////////////////////////////////////////////////////


#endif