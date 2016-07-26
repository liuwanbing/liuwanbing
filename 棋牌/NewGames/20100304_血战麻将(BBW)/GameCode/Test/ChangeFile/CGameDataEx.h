#pragma once
//#include "../ChangeFile/UpgradeMessageEx.h"
#include "../GameMessage/CGameData.h"

///游戏数据管理类
class CGameDataEx: public CGameData
{
public:
	CGameDataEx(void);
	~CGameDataEx(void);
	
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
	void LoadRoomIni(int id);
	//加载牌数据
	virtual BYTE LoadPaiData(MjMenPaiData & menpai);

	void ClearBaseFen();//清空基础分信息

	///翻醒事件数据包
	tagFanDiFenEx  T_DiFen;
	//押注数据包
	tagYaZhuEx     T_YaZhu;

	//游戏底分
	int				m_iBaseFen;
	BYTE			m_byMaster;//桌主
	bool			m_bReBese[PLAY_COUNT];//是否回应了基础分设置
	
	
};
