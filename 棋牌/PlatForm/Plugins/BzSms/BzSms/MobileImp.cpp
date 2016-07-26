#include "StdAfx.h"

#include "MobileImp.h"

//PJY SMS 通讯实现类//////////////////////////////////////////////////////////////////////////////////

CMobileImp::CMobileImp(void)
{
}

CMobileImp::~CMobileImp(void)
{
}

//接口实现////////////////////////////////////////////////////////////////////////////////////////////////////

HRESULT CMobileImp::GetCodeInfo(char* szPhoneNo,char* szUserID,char* szResult)
{
	CComBSTR bstPhoneNo(szPhoneNo);
	CComBSTR bstUserID(szUserID);
	BSTR bstResult;

	HRESULT hr = CMobileCodeWST::getMobileCodeInfo(bstPhoneNo,bstUserID,&bstResult);
	if (S_OK == hr)
	{
		char* p = _com_util::ConvertBSTRToString((WCHAR*)bstResult);
		if (p)
		{
			::strcpy(szResult,p);
			delete[] p;
		}
	}

	return hr;
}

void CMobileImp::ReleaseObject()
{
	OutputTraceStr("Sms: CMobileImp::ReleaseObject");
	delete this;
}