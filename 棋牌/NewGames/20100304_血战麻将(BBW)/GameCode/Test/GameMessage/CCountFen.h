#pragma once

#include "..\ChangeFile\CGameDataEx.h"
#include "..\ChangeFile\CUserDataEx.h"
#include "..\ChangeFile\CCheckHuPaiEx.h"

class CSrvLogic;


#define MAX_SHADOW_COUNT  7  //JAdd-20090115
/////////////////糊牌类型结构//////////////////////////////////////

///算分类
class CCountFen
{
public:
	CCountFen(void);
	~CCountFen(void);

public:	

	CSrvLogic *pDesk;//父类（桌子）

	///是否一炮多响
	bool	bDuoXiang;
	///是否自摸
	bool	bZimo;
	///金币
	int		m_iMoney[4];
	///总分
	int		m_iZongFen[4];
	///糊牌分
	int		m_iHuFen[4];



public:
	///计算各个玩家的分数
	virtual void CountFen(tagHuPaiEx& verb,tagCountFenEx& CountFen);
	///计算杠分
	virtual void CountGangFen(int GangFen[]);
	///检测是否存在某种糊牌类型
	//virtual bool CheckHupaiType(BYTE type,BYTE hupai[]);

	void SetDeskPoint(CSrvLogic *desk);
};
