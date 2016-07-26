#pragma once

#include "../GameMessage/CCountFen.h"

///事件处理类（重载）
class CCountFenEx:public CCountFen
{
public:
	CCountFenEx(void);
	~CCountFenEx(void);
	
public:	
	
	
public:
	virtual void CountFen(tagHuPaiEx& verb,tagCountFenEx& CountFen);
	///计算杠分
	virtual void CountGangFen(int GangFen[]);
	///计算精牌张数
	virtual void CountJingNum();
	///计算精分
	virtual void CountJingFen(int JingFen[]);

	/////检测是否存在某种糊牌类型
	//virtual bool CheckHupaiType(BYTE type,BYTE hupai[]);

	//根据牌型获取胡牌基础分
	void GetHuPaiBaseFen(BYTE station,tagCountFenEx& CountFen,int fen[]);

	BYTE m_byWiner;//赢家位置


};
