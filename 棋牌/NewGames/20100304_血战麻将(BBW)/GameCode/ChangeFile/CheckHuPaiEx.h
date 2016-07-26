#pragma once

#include "../GameMessage/CheckHuPai.h"
///事件处理类（重载）
class CheckHuPaiEx:public CheckHuPai
{
public:
	CheckHuPaiEx(void);
	~CheckHuPaiEx(void);
	
public:
	int   m_byTingMaxFan ;//听牌可能的最大番

public:
	
	///检测能否听牌
	virtual bool CanTing(BYTE station,BYTE CanOut[]);
	///计算听牌玩家根的数量
	int CountTingGen(BYTE pai[]);
    ///名称：CanHu
    ///描述：详细检测糊牌
    ///@param station 糊牌检测位置，hutype[] 能糊牌的类型，zimo 是否自摸
    ///@return true  能糊， false 不能
    virtual bool CanHu(BYTE station,BYTE lastpai,BYTE hutype[],bool zimo);
	///平糊检测
	virtual bool CheckPingHu(BYTE lastzhuapai=255,bool zimo=false,bool jing=false,bool CheckTing=false);

	///获取最大的组合牌型
	virtual bool  GetMaxCombination(bool zimo,bool jing,BYTE lastpai = 255);


    bool CheckJiangDui();

	///检测是否全幺牌
	bool CheckQuanYao();

	///计算玩家根的数量
	int CountGen();
	///计算七对中四张牌的数量
	int CountLongQi();
	///计算七对中四张牌的数量
	bool CheckHuGen(BYTE lastpai);

	//检测是否幺九七对
	bool CheckYaoJiuQiDui();



};
