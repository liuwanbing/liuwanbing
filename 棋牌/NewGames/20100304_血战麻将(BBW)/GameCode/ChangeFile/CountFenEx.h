#pragma once

#include "CountFen.h"
///事件处理类（重载）
class CountUserFenEx:public CountUserFen
{
public:
	CountUserFenEx(void);
	~CountUserFenEx(void);
	
public:	
	
	int  m_iHuaZhu[PLAY_COUNT];//花猪分
	int  m_iChaJiao[PLAY_COUNT];//查叫分
	int  m_iZhuanYi[PLAY_COUNT];//呼叫转移分

public:
	virtual void CountFen(tagHuPaiEx& verb,tagCountFenEx& CountFen);
	///计算杠分
	virtual void CountGangFen(int GangFen[]);
};

//全局对象
//extern CountUserFenEx g_CountFen;