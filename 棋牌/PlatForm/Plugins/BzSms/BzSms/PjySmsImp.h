/********************************************************************
	created:	2012/04/30
	file base:	Sms
	file ext:	h
	author:		
	
	purpose:	PJY SMS通讯实现
*********************************************************************/
#ifndef __PJY_IMP_H__
#define __PJY_IMP_H__

#pragma once

#include <CGuid.h>
#include <initguid.h>

#include "SmsIf.h"
#include "com_1885128pjy.h"

using namespace MessageUtil;

class CPjySmsImp : public CMessageUtilT<>,
				   public ISms
{
public:

	CPjySmsImp(void);

	virtual ~CPjySmsImp(void);

//接口实现
protected:

	//发送短信
	virtual HRESULT SendMessage(char* szPhoneNo,char* szMessage);

	//释放对象
	virtual void ReleaseObject();


};

#endif // __PJY_IMP_H__
