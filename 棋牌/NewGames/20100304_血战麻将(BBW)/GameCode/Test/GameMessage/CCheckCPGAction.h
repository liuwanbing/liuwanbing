#pragma once

#include "..\ChangeFile\CGameDataEx.h"
#include "..\ChangeFile\CUserDataEx.h"
class CSrvLogic;

///吃碰杠检测类
class CCheckCPGAction
{
public:
	CCheckCPGAction(void);
	~CCheckCPGAction(void);
	
public:	
		CUserDataEx	*pUserData;//用户数据

		CSrvLogic *pDesk;//父类（桌子）
	
public:


	///能否吃牌
	virtual bool CanChi(BYTE station,BYTE pai,tagCPGNotifyEx &ResultData);
	///能否碰牌
	virtual bool CanPeng(BYTE station,BYTE pai,tagCPGNotifyEx &ResultData);
	///能否杠牌
	virtual bool CanGang(BYTE station,BYTE pai,tagCPGNotifyEx &ResultData,bool me=false);

	//void SetDataPoint(UserDataEx *userdata);

	void SetDeskPoint(CSrvLogic *desk);
};
