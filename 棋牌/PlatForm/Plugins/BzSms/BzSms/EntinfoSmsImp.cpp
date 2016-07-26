#include "StdAfx.h"
#include <stdio.h>

#include "EntinfoSmsImp.h"

//PJY SMS 通讯实现类//////////////////////////////////////////////////////////////////////////////////

CEntinfoSmsImp::CEntinfoSmsImp(void)
{
}

CEntinfoSmsImp::~CEntinfoSmsImp(void)
{
}

//接口实现////////////////////////////////////////////////////////////////////////////////////////////////////

HRESULT CEntinfoSmsImp::SendMessage(char* szPhoneNo,char* szMessage)
{
	CComBSTR bstPhoneNo(szPhoneNo);
	CComBSTR bstMessage(szMessage);
// 	CComBSTR bstSN("SDK-WSS-010-01736");
// 	CComBSTR bstPwd("0C6C7B645C7BC0153487CBB7C57B8ABC");
	CComBSTR bstSN("SDK-WSS-010-02914");
	CComBSTR bstPwd("25CCAFD1559AEA8B54086B215E22AF1E");
	CComBSTR bstExt("");
	CComBSTR bstDateTime("");
	CComBSTR bstRRID("");
	CComBSTR bstResult;

// 	SYSTEMTIME sys_t;
// 	char szTime[32] = {0};
// 	::GetLocalTime(&sys_t);
// 	_stprintf(szTime,"%02d:%02d:%02d",sys_t.wHour,sys_t.wMinute,sys_t.wSecond);
// 	CComBSTR bstDateTime(szTime);

// 	CString str;
// 	str.Format("DServer:CEntinfoSmsImp::SendMessagePhone = %s Message = %s",szPhoneNo,szMessage);
// 	OutputDebugString(str);

	HRESULT hr = CWebServiceT::mt(bstSN,bstPwd,bstPhoneNo,bstMessage,bstExt,bstDateTime,bstRRID,&bstResult);

	FILE *fp = NULL;
	fp = fopen("plugin_sms.log", "a");
	if (fp)
	{
		char szTimeStr[32];
		char szDateStr[32];
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);
		GetTimeFormatA(LOCALE_USER_DEFAULT,TIME_FORCE24HOURFORMAT,&sysTime,"HH:mm:ss",szTimeStr,32);
		GetDateFormatA(LOCALE_USER_DEFAULT,LOCALE_USE_CP_ACP,&sysTime,"yyyy-MM-dd",szDateStr,32);

		fprintf(fp,"mt====================================================================\n");
		fprintf(fp,"    datetime = %s %s\n",szDateStr,szTimeStr);
		fprintf(fp,"  -----------------------------------------------------------\n");
		fprintf(fp,"    param      SN  = %s\n",CW2A(bstSN.m_str));
		fprintf(fp,"    param      PWD = %s\n",CW2A(bstPwd.m_str));
		fprintf(fp,"    param  PhoneNo = %s\n",CW2A(bstPhoneNo.m_str));
		fprintf(fp,"    param      Msg = %s\n",CW2A(bstMessage.m_str));
		fprintf(fp,"    param      Ext = %s\n",CW2A(bstExt.m_str));
		fprintf(fp,"    param DateTime = %s\n",CW2A(bstDateTime.m_str));
		fprintf(fp,"    param     RRID = %s\n",CW2A(bstRRID.m_str));
		fprintf(fp,"\n");
		fprintf(fp,"    mt function return = 0x%08x\n",hr);
		fprintf(fp,"    output param Result = %s\n",CW2A(bstResult.m_str));
		fprintf(fp,"\n");

		fclose(fp);
	}

	


	return hr;
}

void CEntinfoSmsImp::ReleaseObject()
{
	OutputTraceStr("Sms: CEntinfoSmsImp::ReleaseObject");
	delete this;
}