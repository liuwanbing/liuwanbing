#pragma once

#include "../GameMessage/CCheckCPGAction.h"
///≥‘≈ˆ∏‹ºÏ≤‚¿‡(÷ÿ‘ÿ)
class CCheckCPGActionEx:public CCheckCPGAction
{
public:
	CCheckCPGActionEx(void);
	~CCheckCPGActionEx(void);
	
public:	
	
	
public:
	///ƒ‹∑Ò≥‘≈∆
	virtual bool CanChi(BYTE station,BYTE pai,tagCPGNotifyEx &ResultData);
	///ƒ‹∑Ò≈ˆ≈∆
	virtual bool CanPeng(BYTE station,BYTE pai,tagCPGNotifyEx &ResultData);
	///ƒ‹∑Ò∏‹≈∆
	virtual bool CanGang(BYTE station,BYTE pai,tagCPGNotifyEx &ResultData,bool me=false);


};
