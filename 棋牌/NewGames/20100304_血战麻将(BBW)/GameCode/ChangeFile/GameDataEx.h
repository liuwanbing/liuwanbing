#pragma once
//#include "../ChangeFile/UpgradeMessageEx.h"
#include "../GameMessage/GameData.h"

///游戏数据管理类
class GameDataEx: public GameData
{
public:
	GameDataEx(void);
	~GameDataEx(void);
	
public:	

public:
	///设置所有事件的后接事件
	virtual void SetThingNext();
	///设置某个事件的后接事件
	virtual void SetThingNext(BYTE id);
	//virtual 初始化数据
	virtual void InitData();
	///初始化数据
	virtual void LoadIni();

	///初始化房间特殊数据
	void	LoadRoomIni(int id);

	bool	m_bChengBao;//分承包制和家家有

	bool	m_bShowTax;//是否显示台费


    MJ_Setting &GetMjSetting();

	tagDingQueEx			T_DingQue;  //定缺事件

    ///糊牌牌事件
    tagHuPaiEx				T_HuPaiXZMJ[PLAY_COUNT];        // 四个玩家，各有各的胡牌事件
    ///算分事件
    tagCountFenEx			T_CountFenXZMJ[PLAY_COUNT];     // 四个玩家，各有各的算分事件

	BYTE					m_byFengDing;	//封顶配置默认3番
	int						m_iBaseFen;		//游戏基本分，默认为1
	bool					m_bForceFinish;	//是否强退结束版本


    MJ_Setting      m_MjSet;       // 麻将设置
	///麻将时间等待：时间到了服务器代为处理
	MJ_WaitTimeEx   m_MjWait;
private:

};

//全局对象
extern GameDataEx g_GameData;