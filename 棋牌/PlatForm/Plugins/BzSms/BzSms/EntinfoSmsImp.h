/********************************************************************
	created:	2012/04/30
	file base:	Sms
	file ext:	h
	author:		
	
	purpose:	entinfo.cn SMS通讯实现
*********************************************************************/
#ifndef __ENTINFO_IMP_H__
#define __ENTINFO_IMP_H__

#pragma once

#include <CGuid.h>
#include <initguid.h>

#include "SmsIf.h"
#include "EntinfoSmsImp.h"

using namespace WebService;

class CEntinfoSmsImp : public CWebServiceT<>,
				   public ISms
{
public:

	CEntinfoSmsImp(void);

	virtual ~CEntinfoSmsImp(void);

//接口实现
protected:

	//发送短信
	virtual HRESULT SendMessage(char* szPhoneNo,char* szMessage);

	//释放对象
	virtual void ReleaseObject();


};

#endif // __PJY_IMP_H__
